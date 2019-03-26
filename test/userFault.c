#include <sys/types.h>
#include <stdio.h>
#include <linux/userfaultfd.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <string.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <ucontext.h>
#include <poll.h>
#include <pthread.h>

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PAGE_SIZE 4096

static int uffd = -1;

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
		fprintf(stderr, "UFFDIO_COPY unexpected copy %Ld\n",
			uffdio_copy.copy), exit(1);
	}
	printf("COPYPAGE E: %p\n", target);
	/*else {
		if(ioctl(ufd, UFFDIO_COPY, &uffdio_copy)) {
			errExit("RETRY UFFDIO_COPY error");
		}
	}*/
}

static int register_uffd()
{
	struct uffdio_api uffdio_api;
	int fd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if(fd == -1) {
		errExit("userfaultfd");
	}

	uffdio_api.api = UFFD_API;
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

void* fault_handler_thread(void* arg) 
{
    static struct uffd_msg msg;
    static int fault_cnt = 0;
	char page[PAGE_SIZE];
	struct uffdio_copy uffdio_copy;
	ssize_t nread;

	while(1) {

		struct pollfd pollfd;
   		int nready;
		pollfd.fd = uffd;
		pollfd.events = POLLIN;
		printf("\nfault_handler_thread():\n");
		printf("    poll start\n");
		nready = poll(&pollfd, 1, -1);
		if (nready == -1)
			errExit("poll");

		printf("\nfault_handler_thread():\n");
		printf("    poll() returns: nready = %d; POLLIN = %d; POLLERR = %d\n", nready, (pollfd.revents & POLLIN) != 0, (pollfd.revents & POLLERR) != 0);

		nread = read(uffd, &msg, sizeof(msg));
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

		printf("    UFFD_EVENT_PAGEFAULT event: ");
 		printf("flags = %llx; ", msg.arg.pagefault.flags);
		printf("address = %llx\n", msg.arg.pagefault.address);

		memset(page, 'A' + fault_cnt % 20, PAGE_SIZE);
 		fault_cnt++;
		copy_page(uffd, (void*)msg.arg.pagefault.address, page);
	}
}

int main(int argc, char* argv[])
{
	char* addr;
	pthread_t fault_thread;
	int len = 10 * PAGE_SIZE;
	char page[PAGE_SIZE];

	uffd = register_uffd();

	int shmem_fd = shm_open("USFSALTEST", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(shmem_fd == -1) {
		errExit("shmem_open");
	}
	ftruncate(shmem_fd, len);
	
	addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED) {
		errExit("mmap");
	}
	register_uffd_addr(addr, len, uffd);

	int pthread_ret = pthread_create(&fault_thread, NULL, fault_handler_thread, (void*)0);
	if (pthread_ret != 0) {
		errno = pthread_ret;
		errExit("pthread_create");
	}
		
	int l = 0xf;
	printf("HERE1\n");
	for(int i = 0; i < 10; i++) {
		char c = addr[l];
		printf("%02X\n", c);
		l += 1024;
	}
	printf("HERE2\n");

	return 0;
}
