#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHM_SIZE 1024

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Not enough arguments!");
        return EXIT_FAILURE;
    }
    key_t key;
    if ((key = ftok(argv[0], 1)) == -1) {
        printf("Cannot create the key!");
        return EXIT_FAILURE;
    }
    printf("%d\n", key);
    int shmid;
    if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
        printf("Cannot create the segment!");
        return EXIT_FAILURE;
    }
    int* data;
    if ((data = shmat(shmid, NULL, 0)) == (int*)(-1)) {
        printf("Cannot attach to data!");
        return EXIT_FAILURE;
    }
    key_t key1 = atoi(argv[1]);
    key_t key2 = atoi(argv[2]);
    int shmid1;
    if ((shmid1 = shmget(key1, SHM_SIZE, 0)) == -1) {
        printf("Cannot process segment1!");
        return EXIT_FAILURE;
    }
    int shmid2;
    if ((shmid2 = shmget(key2, SHM_SIZE, 0)) == -1) {
        printf("Cannot process segment2!");
        return EXIT_FAILURE;
    }
    int* data1;
    if ((data1 = shmat(shmid1, NULL, SHM_RDONLY)) == (int*)(-1)) {
        printf("Cannot attach to data1!");
        return EXIT_FAILURE;
    }
    int* data2;
    if ((data2 = shmat(shmid2, NULL, SHM_RDONLY)) == (int*)(-1)) {
        printf("Cannot attach to data1!");
        return EXIT_FAILURE;
    }
    size_t index;
    for (index = 0; index < 100; index++) {
        data[index] = data1[index] + data2[index];
    }
    if (shmdt(data) == -1) {
        printf("Cannot detach from data!");
        return EXIT_FAILURE;
    }
    if (shmdt(data1) == -1) {
        printf("Cannot detach from data1!");
        return EXIT_FAILURE;
    }
    if (shmdt(data2) == -1) {
        printf("Cannot detach from data2!");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
