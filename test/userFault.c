#include <sys/types.h>
#include <stdio.h>
#include <linux/userfaultfd.h>
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

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PAGE_SIZE 4096

static int uffd = -1;
sigjmp_buf handlebuf;
sigjmp_buf sigbuf;

static void copy_page(int ufd, void* target, void* page)
{
	struct uffdio_copy uffdio_copy;
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
	/*else {
		if(ioctl(ufd, UFFDIO_COPY, &uffdio_copy)) {
			errExit("RETRY UFFDIO_COPY error");
		}
	}*/
}

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
	siglongjmp(handlebuf, 1);
	/*if(info->si_ptr != 0) {
		char page[PAGE_SIZE];
		memset(page, 'A' + fault_cnt % 20, PAGE_SIZE);
		copy_page(uffd, info->si_ptr, page);
		fault_cnt++;
	}*/
}

int main(int argc, char* argv[])
{
	struct sigaction sigbus_action;
	sigset_t empty_sig_set;

	char* addr;
	struct uffdio_api uffdio_api;
	struct uffdio_register uffdio_register;
	int len = 10 * sysconf(_SC_PAGE_SIZE);

	sigemptyset(&empty_sig_set);
	sigbus_action.sa_sigaction = handle_SIGBUS;
	sigbus_action.sa_mask = empty_sig_set;
	sigbus_action.sa_flags = SA_SIGINFO;
	if(sigaction(SIGBUS, &sigbus_action, NULL) == -1) {
		errExit("signal");
	}

	uffd = syscall(__NR_userfaultfd, O_CLOEXEC);
	if(uffd == -1) {
		errExit("userfaultfd");
	}

	uffdio_api.api = UFFD_API;
	uffdio_api.features = UFFD_FEATURE_SIGBUS;
	if (ioctl(uffd, UFFDIO_API, &uffdio_api) == -1) {
		errExit("ioctl-UFFDIO_API");
	}

	addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED) {
		errExit("mmap");
	}

	uffdio_register.range.start = (unsigned long) addr;
	uffdio_register.range.len = len;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1) {
		errExit("ioctl-UFFDIO_REGISTER");
	}
	
	int l = 0xf;
	printf("SS ADDR: %p\n", ((char*)addr) + l);

	if (sigsetjmp(handlebuf, 1) != 0) {
		char page[PAGE_SIZE];
		memset(page, 'A', PAGE_SIZE);
		copy_page(uffd, ((char*)addr) + l, page);
		siglongjmp(sigbuf, 1);
	}

	printf("HERE\n");

	for(int i = 0; i < 10; i++) {
		char c = addr[l];
		printf("%02X\n", c);
		l += 1024;
	}

	return 0;
}
