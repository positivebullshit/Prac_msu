#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>

void clear(int semid) {
    semctl(semid, IPC_RMID, 0);
    kill(0, SIGTERM);
}

int main(int argc, char *argv[]) {

    struct sembuf Product[2] = {
            {2, -1, SEM_UNDO},
            {3, -1, SEM_UNDO}
    };

    int n;  // number of products
    printf("Enter the number of products you want to get: ");
    scanf("%d", &n);

    key_t key = ftok("PATH", 123);

    int semid;
    // array of 4 semaphores - for detail A, B, C, Module
    if ((semid = semget(key, 4, 0666 | IPC_CREAT)) == -1) {
        printf("Can't get the array of semaphores\n");
        exit(1);
    }
    char str_semid[50];  // to make semid (char *) for executing by execl()
    sprintf(str_semid, "%d", semid);

    sigset(SIGINT, clear);

    if (!fork()) {
        execl("detail", "A", str_semid, NULL);
        perror("Detail A creation");
        exit(1);
    }

    if (!fork()) {
        execl("detail", "B", str_semid, NULL);
        perror("Detail B creation");
        exit(1);
    }

    if (!fork()) {
        execl("detail", "C", str_semid, NULL);
        perror("Detail C creation");
        exit(1);
    }

    if (!fork()) {
        execl("module", "Module", str_semid, NULL);
        perror("Module creation");
        exit(1);
    }

    for (int i = 1; i <= n; i++) {
        if (semop(semid, Product, 2) == -1) {
            printf("Error with product\n");
            exit(1);
        }
        printf("Product %d is produced\n\n", i);
    }

    clear(semid);

    return 0;
}
