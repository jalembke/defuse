#include "libdefuse.h"
#include "glibc_ops.h"

__attribute__((constructor))
void start_up() {
	load_glibc_ops();
	load_mounts_void();
}
