#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int argc, char *argv[]) {

    key_t key = ftok("PATH", 123);

    int semid = atoi(argv[1]);  // argv[1] is str_semid, make it int again;

    struct sembuf ABdetails[2] = {
            {0, -1, SEM_UNDO},
            {1, -1, SEM_UNDO}
    };  // Decrease semaphores for A and B by 1

    struct sembuf Module = {3, 1, SEM_UNDO};

    while (1) {
        if (semop(semid, ABdetails, 2) == -1) {
            printf("Error with details A and B\n");
            exit(1);
        }
        printf("Module is produced\n");
        if (semop(semid, &Module, 1) == -1) {
            printf("Error with module\n");
            exit(1);
        }
    }
}