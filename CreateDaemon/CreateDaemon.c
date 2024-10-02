#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>
#include <signal.h>

#define MAX_FILES 1048576

pid_t pid_to_print = 0;

void HandleSignal(int signal) {
    if (signal == SIGURG) {
        printf("%d\n", pid_to_print);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    pid_t pid;
    int i;
    pid = fork();
    if (pid != 0) exit(EXIT_FAILURE);
    if (setsid() == -1) return EXIT_FAILURE;
    pid_to_print = pid;
    if (chdir("/") == -1) return EXIT_FAILURE;
    for (i = 0; i < MAX_FILES; i++) close(i);
    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
    if (signal(SIGURG, HandleSignal) == SIG_ERR) exit(EXIT_FAILURE);
    while (1) {};
    return 0;
}
