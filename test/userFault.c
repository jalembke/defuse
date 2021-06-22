#include <sys/types.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <string.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <poll.h>
#include <pthread.h>

#include "userfaultfd.h"
#include "util.h"

#define READ_PAGES 32
#define PAGE_SIZE 4096
#define BUFFER_SIZE (READ_PAGES * PAGE_SIZE)

struct userfaultControlBlock {
	int uffd;
	int backend_fd;
	int shmem_fd;
	void* address;
	int space_size;
};
	
static volatile char c;
static int fault_count;

static void copy_page(int ufd, void* target, void* page)
{
	struct uffdio_copy uffdio_copy;
	
	//printf("COPYPAGE I: %p\n", target);
	uffdio_copy.dst = (unsigned long)target & ~(PAGE_SIZE - 1);
	uffdio_copy.src = (unsigned long)page;
	uffdio_copy.len = PAGE_SIZE;
	uffdio_copy.mode = 0;
	uffdio_copy.copy = 0;
	if (ioctl(ufd, UFFDIO_COPY, &uffdio_copy)) {
		/* real retval in ufdio_copy.copy */
		if (uffdio_copy.copy != -EEXIST)
			errExit("UFFDIO_COPY error");
	} else if (uffdio_copy.copy != PAGE_SIZE) {
		fprintf(stderr, "UFFDIO_COPY unexpected copy %" PRIu64 "\n",
			uffdio_copy.copy), exit(1);
	}
	// printf("COPYPAGE E: %p\n", target);
	/*else {
		if(ioctl(ufd, UFFDIO_COPY, &uffdio_copy)) {
			errExit("RETRY UFFDIO_COPY error");
		}
	}*/
}

static void copy_pages(struct userfaultControlBlock* ucb, void* target, void* buffer, uint64_t buffer_size)
{
	uint64_t bytes_remaining = ucb->space_size - (uint64_t)target - (uint64_t)ucb->address;
	int bytes_to_copy = bytes_remaining < buffer_size ? bytes_remaining : buffer_size;
	int pages_to_copy = ((bytes_to_copy + PAGE_SIZE-1) & ~(PAGE_SIZE-1)) / PAGE_SIZE;
	// printf("%X %X\n", bytes_to_copy, pages_to_copy);
	for(int i = 0; i < pages_to_copy; i++) {
		copy_page(ucb->uffd, ((char*)target) + i*PAGE_SIZE, ((char*)buffer) + i*PAGE_SIZE);
	}
}

static int register_uffd()
{
	struct uffdio_api uffdio_api;
	int fd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if(fd == -1) {
		errExit("userfaultfd");
	}

	uffdio_api.api = UFFD_API;
	//uffdio_api.features = UFFD_FEATURE_MISSING_SHMEM | UFFD_FEATURE_EVENT_UNMAP;
	uffdio_api.features = UFFD_FEATURE_MISSING_SHMEM;
	if (ioctl(fd, UFFDIO_API, &uffdio_api) == -1) {
		errExit("ioctl-UFFDIO_API");
	}
	return fd;
}

static void register_uffd_addr(void* addr, size_t len, int fd)
{
	struct uffdio_register uffdio_register;
	uffdio_register.range.start = (unsigned long) addr;
	uffdio_register.range.len = len;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(fd, UFFDIO_REGISTER, &uffdio_register) == -1) {
		errExit("ioctl-UFFDIO_REGISTER");
	}
}

static void unregister_uffd_addr(void* addr, size_t len, int fd)
{
	struct uffdio_range uffdio_range;
	uffdio_range.start = (unsigned long) addr;
	uffdio_range.len = len;
	if (ioctl(fd, UFFDIO_UNREGISTER, &uffdio_range) == -1) {
		errExit("ioctl-UFFDIO_UNREGISTER");
	}
}

void* fault_handler_thread(void* arg) 
{
    static struct uffd_msg msg;
    static int fault_cnt = 0;
	//char* buffer = (char*)malloc(BUFFER_SIZE);
	char buffer[BUFFER_SIZE];
	struct uffdio_copy uffdio_copy;
	ssize_t nread;
	struct userfaultControlBlock* ucb = (struct userfaultControlBlock*)arg;
	fault_count = 0;

	while(1) {

		struct pollfd pollfd;
   		int nready;
		pollfd.fd = ucb->uffd;
		pollfd.events = POLLIN;
		nready = poll(&pollfd, 1, -1);
		if (nready == -1)
			errExit("poll");

		nread = read(ucb->uffd, &msg, sizeof(msg));
  		if (nread == 0) {
			printf("EOF on userfaultfd!\n");
			exit(EXIT_FAILURE);
		}

		if (nread == -1)
			errExit("read");

		if (msg.event != UFFD_EVENT_PAGEFAULT) {
			fprintf(stderr, "Unexpected event on userfaultfd\n");
			exit(EXIT_FAILURE);
		}

		if(msg.arg.pagefault.address < (uint64_t)ucb->address) {
			errExit("invalid fault address");
		}

		uint64_t offset = ( msg.arg.pagefault.address - (uint64_t)ucb->address);

		// printf("PF: %016lX %" PRIu64 "\n", msg.arg.pagefault.address, offset);

		ssize_t bytes_read = pread(ucb->backend_fd, buffer, BUFFER_SIZE, offset);
		if(bytes_read == -1) {
			errExit("fault read");
		}
		fault_count++;
		if(bytes_read < BUFFER_SIZE) {
			memset(((char*)buffer) + bytes_read, 0, BUFFER_SIZE-bytes_read);
		}

		copy_pages(ucb, (void*)msg.arg.pagefault.address, buffer, BUFFER_SIZE);
		//printf("%d\n", fault_count);
	}
}

static inline void* setup_paging_manager(const char* file, struct userfaultControlBlock* ucb)
{
	int bfd = open(file, O_RDONLY);
	off_t file_size = get_file_size(bfd);
	if(bfd == -1) {
		errExit("open backend");
	}
	int uffd = register_uffd();
	if(uffd == -1) {
		errExit("uffd register");
	}
	int shmem_fd = shm_open("USFSALTEST", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(shmem_fd == -1) {
		errExit("shmem_open");
	}
	ftruncate(shmem_fd, 0);
	ftruncate(shmem_fd, file_size);

	void* addr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if(addr == MAP_FAILED) {
		errExit("mmap");
	}

	register_uffd_addr(addr, file_size, uffd);
	ucb->uffd = uffd;
	ucb->shmem_fd = shmem_fd;
	ucb->backend_fd = bfd;
	ucb->address = addr;
	ucb->space_size = file_size;

	return addr;
}

static inline void end_paging_manager(struct userfaultControlBlock* ucb)
{
	munmap(ucb->address, ucb->space_size);
	unregister_uffd_addr(ucb->address, ucb->space_size, ucb->uffd);
	close(ucb->shmem_fd);
	close(ucb->backend_fd);
	close(ucb->uffd);
}

int main(int argc, char* argv[])
{
	struct userfaultControlBlock ucb;
	pthread_t fault_thread;

	char* addr = setup_paging_manager(argv[1], &ucb);

	int pthread_ret = pthread_create(&fault_thread, NULL, fault_handler_thread, (void*)&ucb);
	if (pthread_ret != 0) {
		errno = pthread_ret;
		errExit("pthread_create");
	}

	uint64_t start_time = get_time();
	int l = 0xf;
	while(l < ucb.space_size) {
		if(argv[2][0] == 'r')
			c = addr[l];
		else
			addr[l] = 100;
		//printf("%02X\n", c);
		l += 1024;
	}
	uint64_t end_time = get_time();

	// printf("%d %" PRIu64 "\n", fault_count, end_time - start_time);
	printf("%" PRIu64 "\n", end_time - start_time);
	end_paging_manager(&ucb);

	return 0;
}
