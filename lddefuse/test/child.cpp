#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <string>
#include <iostream>

std::string get_path()
{
	char buffer[PATH_MAX + 1] = {0};
	if(-1 == readlink("/proc/self/exe", buffer, sizeof(buffer))) {
		fprintf(stderr, "Failed to read /proc/self/exe: %s\n", strerror(errno));
		exit(1);
	}
	std::string exepath(buffer);
	return exepath.substr(0, exepath.find_last_of("\\/"));
}

int main(int argv, char* argc[])
{
	std::string exePath = get_path();
	std::cout << exePath << std::endl;
	return 0;
}
