#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
	chdir("~/Documents");
	mkdir("NewDirectory", 0700);
	ls("NewDirectory");
	return 0;
}