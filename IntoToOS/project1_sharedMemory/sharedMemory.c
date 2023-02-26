/*
* Author: Jack Cornette - 113509534
* Username: JackCornette
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
    int value;
} shared_mem;
shared_mem *count;

/***  Each child process executes one of these functions all on the same variable ***/

int process1(int val) {
    while(val < 100000) {
        val += 1;
    }
    return val;
}

int process2(int val) {
    while(val < 200000) {
        val += 1;
    }
    return val;
}
int process3(int val) {
    while(val < 300000) {
        val += 1;
    }
    return val;
}

int process4(int val) {
    while(val < 500000) {
        val += 1;
    }
    return val;
}

int main() {
    
    int shmid, parentid, pid1, pid2, pid3, pid4, ID, status;
    char *shmadd;
    shmadd= (char *) 0;
    
    // create an IPC key
    key_t key = ftok("sharedMemory", 1);
    // create shared memory of size equal to one int value, -1 if failed to created mem, s
    if ((shmid= shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0){
        perror("shmget");
        exit (1);
    }
    // connect to a shared memory segment by returning a pointer to count, -1 for failure to connect
    if ((count = (shared_mem*) shmat(shmid, shmadd, 0)) == 
    (shared_mem*) -1) {
        perror("shmat");
        exit (0);
        
    }
    count->value = 0;
    parentid = getpid();
    
    // create the first child
    if((pid1 = fork()) == 0) { // parent creats a child
        count->value = process1(count->value);
        printf("From Process 1: counter = %d\n", count->value);
        exit(0); // succesfully exit process
    }
    else if((pid2 = fork()) == 0)  { // parent creats a second child
        count->value = process2(count->value); 
        printf("From Process 2: counter = %d\n", count->value);
        exit(0); // succesfully exit process
    }
    else if((pid3 = fork()) == 0) { // parent creats a third child
        count->value = process3(count->value); 
        printf("From Process 3: counter = %d\n", count->value);
        exit(0); // succesfully exit process
    }
    else if((pid4 = fork()) == 0) { // parent creats a forth child
        count->value = process4(count->value); 
        printf("From Process 4: counter = %d\n", count->value);
        exit(0); // succesfully exit process
    }

    waitpid(pid1, NULL, 0); // parent waits for first child to finish
    
    waitpid(pid2, NULL, 0); // parent waits for second child to finish
    
    waitpid(pid3, NULL, 0); // parent waits for third child to finish
    
    waitpid(pid4, NULL, 0); // parent waits for forh child to finish
    printf("Child with pid %d has just exited.\n", pid1);
    printf("Child with pid %d has just exited.\n", pid2);
    printf("Child with pid %d has just exited.\n", pid3);
    printf("Child with pid %d has just exited.\n", pid4);
    // detach shared memory space from process address space
    if (shmdt(count) == -1)    {
        perror ("shmdt");
        exit (-1);
    }
    shmctl(shmid, IPC_RMID, NULL); // deallocate shared memory

    return 0;
}