#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int main() 
{ 
    pid_t pid1, pid2, pid3;
    
	int fd1[2];
	int fd2[2]; 

	if (pipe(fd1)==-1) 
	{ 
		perror("bad pipe1");
        exit(1);
	} 

    pid1 = fork();
    if(pid1 == -1) {
        perror("fork1 error");
        exit(1);
    }
    else if(pid1 == 0) {
        dup2(fd1[1], 1);
        close(fd1[0]);
        close(fd1[1]);

        char *argv1[] = {"ps", "aux", NULL};
            execv("/bin/ps", argv1);

        perror("ps aux execute doesn't work");
        exit(1);
    }

    if(pipe(fd2)==-1) {
        perror("bad pipe2");
        exit(1);
    }

    pid2 = fork();
    if(pid2 == -1) {
        perror("fork2 error");
        exit(1);
    }
    else if(pid2 == 0) {
        dup2(fd1[0], 0);
        dup2(fd2[1], 1);
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        char *argv2[] = {"sort", "-nrk", "3,3", NULL};
            execv("/bin/sort", argv2);

        perror("sort execute doesn't work");
        exit(1);
    }

    close(fd1[0]);
    close(fd1[1]);

    pid3 = fork();
    if (pid3 == -1) 
    {
        perror("fork3 error");
        exit(1);
    } 
    else if (pid3 == 0) 
    {
        dup2(fd2[0], 0);
        close(fd2[0]);
        close(fd2[1]);
        char *argv3[] = {"head", "-5", NULL};
            execv("/bin/head", argv3);
        perror("head execute doesn't work");
        exit(1);
    }
    exit(0);
}
