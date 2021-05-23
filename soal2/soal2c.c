#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

    int pid;
    int fd1[2];
    int fd2[2];

    if (pipe(fd1) == -1) {
        perror("bad pipe1");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("bad fork1");
        exit(1);
    } 
    else if (pid == 0) {
        dup2(fd1[1], 1);

        close(fd1[0]);
        close(fd1[1]);

        char *argv[] = {"ps", "aux", NULL};
            execv("/bin/ps", argv);
        perror("ps aux execute doesnt work");
        _exit(1);
    }

    if (pipe(fd2) == -1) {
        perror("bad pipe2");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("bad fork2");
        exit(1);
    } 
    else if (pid == 0) {
        dup2(fd1[0], 0);
        dup2(fd2[1], 1);

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
 
        char *argv[] = {"sort", "-nrk", "3.3", NULL};
            execv("/usr/bin/sort", argv);
        perror("sort execute doesnt work");
        _exit(1);
    }

    close(fd1[0]);
    close(fd1[1]);

    if ((pid = fork()) == -1) {
        perror("bad fork3");
        exit(1);
    } 
    else if (pid == 0) {
        dup2(fd2[0], 0);

        close(fd2[0]);
        close(fd2[1]);

        char *argv[] = {"head", "-5", NULL};
            execv("/usr/bin/head", argv);

        perror("head execute doesnt work");
        _exit(1);
    }

}
