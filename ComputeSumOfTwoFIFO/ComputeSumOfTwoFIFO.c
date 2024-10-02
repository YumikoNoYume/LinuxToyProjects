#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_NUM_SIZE 100

int GetNumberFromInput(int fd, int* sum) {
    char buf[MAX_NUM_SIZE + 1];
    int bytes = read(fd, buf, MAX_NUM_SIZE);
    if (bytes <= 0) return -1;
    buf[bytes] = '\0';
    *sum += atoi(buf);
    return 0;
}

int main() {
    int f1 = open("./in1", O_RDONLY | O_NONBLOCK);
    int f2 = open("./in2", O_RDONLY | O_NONBLOCK);
    if (!(f1 && f2)) {
        printf("Error with opening pipes\n");
        return EXIT_FAILURE;
    }
    fd_set read_set, watch_set;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 700000;
    int sum = 0;
    FD_ZERO(&watch_set);
    FD_SET(f1, &watch_set);
    FD_SET(f2, &watch_set);
    while (FD_ISSET(f1, &watch_set) || FD_ISSET(f2, &watch_set)) {
        read_set = watch_set;
        int result = select(f2 + 1, &read_set, NULL, NULL, &tv);
        if (result == -1) return EXIT_FAILURE;
        if (result == 0) break;
        if (FD_ISSET(f1, &read_set)) {
            if (GetNumberFromInput(f1, &sum) == -1) {
                FD_CLR(f1, &watch_set);
                close(f1);
                f1 = 0;
            }
        }
        if (FD_ISSET(f2, &read_set)) {
            if (GetNumberFromInput(f2, &sum) == -1) {
                FD_CLR(f2, &watch_set);
                close(f2);
                f2 = 0;
            }
        }
    }
    printf("%d\n", sum);
    return EXIT_SUCCESS;
}