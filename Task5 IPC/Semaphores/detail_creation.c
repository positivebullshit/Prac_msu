#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main (int argc, char *argv[]) {

    key_t key = ftok("PATH", 123);

    int semid = atoi(argv[1]);  // argv[1] is str_semid, make it int again;

    int creation_time[3] = {2, 3, 4};  // creation time for each type of detail

    // argv[0][0] is 'A' or 'B' or 'C' so (argv[0][0] - 'A') is the sem_num
    struct sembuf Detail = {argv[0][0] - 'A', 1, SEM_UNDO};

    while (1) {
        sleep(creation_time[argv[0][0] - 'A']);
        if (semop(semid, &Detail, 1) == -1) {
            printf("Error with creation detail %s\n", argv[0]);
            exit(1);
        }
        printf("Detail %s is produced\n", argv[0]);
    }
}