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

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PAGE_SIZE 4096

static int uffd = -1;
sigjmp_buf handlebuf;
sigjmp_buf sigbuf;

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

void* global_sig_context;
void handle_SIGBUS(int sig, siginfo_t *info, void *ucontext)
{
	static int fault_cnt = 0;
	struct uffdio_copy uffdio_copy;
	printf("SIGNAL: %d\n", sig);
	printf("signo: %d errno: %d code: %d\n", info->si_signo, info->si_errno, info->si_code);
	printf("ADDR: %p\n", info->si_addr);
	printf("UFFD: %d\n", uffd);
	if(sigsetjmp(sigbuf, 1) != 0) {
		return;
	}
	global_sig_context = ucontext;
	siglongjmp(handlebuf, 1);
	/*if(info->si_ptr != 0) {
		char page[PAGE_SIZE];
		memset(page, 'A' + fault_cnt % 20, PAGE_SIZE);
		copy_page(uffd, info->si_ptr, page);
		fault_cnt++;
	}*/
}

static void setup_sigbus_handler()
{
	struct sigaction sigbus_action;
	sigset_t empty_sig_set;
	sigemptyset(&empty_sig_set);
	sigbus_action.sa_sigaction = handle_SIGBUS;
	sigbus_action.sa_mask = empty_sig_set;
	sigbus_action.sa_flags = SA_SIGINFO;
	if(sigaction(SIGBUS, &sigbus_action, NULL) == -1) {
		errExit("signal");
	}
}

static int register_uffd()
{
	struct uffdio_api uffdio_api;
	int fd = syscall(__NR_userfaultfd, O_CLOEXEC);
	if(fd == -1) {
		errExit("userfaultfd");
	}

	uffdio_api.api = UFFD_API;
	uffdio_api.features = UFFD_FEATURE_SIGBUS | UFFD_FEATURE_MISSING_SHMEM;
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

int main(int argc, char* argv[])
{
	char* addr;
	int len = 10 * PAGE_SIZE;
	char page[PAGE_SIZE];

	setup_sigbus_handler();
	uffd = register_uffd();

	int shmem_fd = shm_open("USERFAULTFD/test/test", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(shmem_fd == -1) {
		errExit("shmem_open");
	}
	
	addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED) {
		errExit("mmap");
	}
	register_uffd_addr(addr, len, uffd);

	int l = 0xf;
	printf("SS ADDR: %p\n", ((char*)addr) + l);
		
	if (sigsetjmp(handlebuf, 1) != 0) {
		memset(page, 'A', PAGE_SIZE);
		copy_page(uffd, addr+l, page);
		setcontext(global_sig_context);
	}
	
	l = 0xf;
	printf("HERE1\n");
	for(int i = 0; i < 10; i++) {
		char c = addr[l];
		printf("%02X\n", c);
		l += 1024;
	}
	printf("HERE2\n");

	return 0;

	pid_t child = fork();
	printf("FORKED: %d\n", child);
	if(child == -1) {
		errExit("fork");
	}
	if(child) {
		printf("PAREND: %d\n", child);
		waitpid(child, NULL, 0);
	} else {
		uffd = register_uffd();
		register_uffd_addr(addr, len, uffd);
		sleep(1);
		printf("CHILD\n");
		l = 0xf;
		for(int i = 0; i < 10; i++) {
			char c = addr[l];
			printf("%02X\n", c);
			l += 1024;
		}
	}

	return 0;
}
