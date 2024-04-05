#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/event.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


int main()
{
	int p[2], pid;
	pipe(p);
	pid = fork();
	if (pid == 0)
	{
		dup2(p[0], 0);
		dup2(p[1], 1);
		write(p[1], "hello", 5);
		char *argv[] = {"cat", NULL};
		execve("change", argv, NULL);
	}
	else
	{
		// write(p[1], "Hello", 5);
		// close(p[1]);
		wait(NULL);
		char buf[10];
		read(p[0], buf, 10);
		printf("%s\n", buf);
		close(p[0]);
	}
}