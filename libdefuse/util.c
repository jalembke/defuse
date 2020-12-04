#define _GNU_SOURCE

#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

void* memdup(void* ptr, size_t size)
{
	void* ptr_cpy = malloc(size);
	memcpy(ptr_cpy, ptr, size);
	return ptr_cpy;
}

static void remove_path_string(char* p, const char* str)
{
	char* p_ptr = p;
	size_t str_len = strlen(str)-1;
	size_t p_len = strlen(p);
	while(p_ptr = strstr(p, str)) {
		memmove(p_ptr, p_ptr + str_len, p_len - (size_t)(p_ptr - p));
		p[p_len - (str_len)] = 0;
		p_len -= str_len;
	}
}

static void remove_dot_dot(char* p)
{
	char* str = "/../";
	char* p_ptr = p;
	size_t str_len = strlen(str)-1;
	size_t p_len = strlen(p);
	while(p_ptr = strstr(p, str)) {
		if (p_ptr == p) {
			return;
		}
		char* p_end = p_ptr + str_len;
		char* p_start = p_ptr - 1;
		while(p_start >= p && *p_start != '/') {
			p_start--;
		}
		size_t chars_to_remove = (size_t)(p_end - p_start);
		memmove(p_start, p_end, p_len - chars_to_remove);
		p[p_len - chars_to_remove] = 0;
		p_len -= chars_to_remove;
	}
}

/* given a relative path, calculate complete path assuming current directory */
char* resolve_path(const char *p)
{
	char path[PATH_MAX];
	char* path_ptr;
	size_t path_len;

	// Null or empty string
	if (p == NULL || p[0] == 0) {
		return NULL;
	}

	// if first character isn't "/", prepend current working dir
	if (p[0] != '/') {
		char *cwd = get_current_dir_name();
		size_t p_pos = strlen(cwd);
		path_ptr = path;
		strncpy(path, cwd, PATH_MAX);
		path_ptr += p_pos;
		strncpy(path_ptr, "/", PATH_MAX - p_pos);
		p_pos += 1;
		path_ptr += 1;
		strncpy(path_ptr, p, PATH_MAX - p_pos);
		free(cwd);
	} else {
		strncpy(path, p, PATH_MAX);
	}
	path_len = strlen(path);

	// Sanitize slashes and dots
	remove_path_string(path, "/./");
	remove_path_string(path, "//");
	remove_dot_dot(path);
	return strdup(path);
}
