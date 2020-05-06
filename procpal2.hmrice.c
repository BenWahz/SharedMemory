//
// Created by henry on 11/11/2019.
//
//This assignment was designed to run on a virtual machine that
// allowed other commands. This is why this seems like it wont compile.

//The code simulates the use of shared memory between a parent a child task.
//The child will read the string from the shared mem and determine if its a palindrome.
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define BUFFER_SIZE 32
#define SEM_1_NAME "/SEM1_hmrice"


#define BUFFER_SIZE 32
typedef enum {false, true} bool;
char *words[] = {"grandfather", "tunnel", "amanaplanacanalpanama",
                 "supervitamin", "vermont", "anna", "nowisthetimeforallgoodmen",
                 "ababa", "iwantamacbookkoobcamatnawi", "glenelg"};

int numWords = 10;
int main(int argc, char *argv[]) {
    int pid;
    int memid;
    int key = 35;
    char *ptr;
    char buffer[BUFFER_SIZE];
    char prev_buff[BUFFER_SIZE];
    int parentCheck = 0;
    int childCheck;
    sem_t *sem1;


    sem1 = sem_open(SEM_1_NAME, O_CREAT, 0666, 0);
    if (sem1 == NULL) {
        fprintf(stderr, "sem1_open() failed\n");
        return(8);
    }



    int i;
    for(i = 0; i < numWords; i++) {
        strcpy(buffer, words[i]);


        memid = shmget(key, BUFFER_SIZE, IPC_CREAT | 0666);
        if (memid < 0) {
            printf("shmget() failed\n");
            return (8);
        }

        pid = fork();
        if (pid < 0) {
            printf("fork failed\n");
            return (8);
        }

        if (pid > 0) {
            // this is the parent

            if(parentCheck == 0)
            {
                printf("I am the parent, and my pid is %d\n", getpid());
                parentCheck = 1;
            }

            ptr = (char *) shmat(memid, 0, 0);

            if (ptr == NULL) {
                printf("shmat() failed\n");
                return (8);
            }

            printf("(P) writing '%s' to the shared memory\n", buffer);
            strcpy(ptr, buffer);
            printf("(P) --- posting semaphore ---\n");
            sem_post(sem1);

            wait(NULL);



        } else {
            // this is the child
            pid = getpid();

            if (i < 1)
            {
                printf("I am the child, and my pid is %d\n", pid);
                //childCheck = 1;
            }

            ptr = (char *) shmat(memid, 0, 0);

            if (ptr == NULL) {
                printf("shmat() in child failed\n");
                return (8);
            }
            printf("(C) --- waiting for semaphore ---\n");
            sem_wait(sem1);
            sleep(1);
            printf("(C) --- recieved post from semaphore ---\n");

//            printf("Child will sleep for two seconds\n");
//            sleep(2);



            printf("(C) read this from the shared memory: '%s'\n", ptr);
            if(isPalindrome(buffer) == 1)
            {
                strcpy(prev_buff,buffer);
                strcpy(buffer, "yes");
                printf("(C) write this to the shared memory:  'yes'\n");
            }else if(isPalindrome(buffer) == 0)
            {
                strcpy(prev_buff,buffer);
                strcpy(buffer, "no");
                printf("(C) write this to the shared memory:  'no'\n");
            }else
            {
                printf("******  palindrome error\n");
            }

            if(strcmp(buffer, "yes") == 0)
            {
                printf("(P) %s is a palindrome!\n", prev_buff);
            }else if(strcmp(buffer,"no") == 0){
                printf("(P) %s is NOT a palindrome\n", prev_buff);
            }else
            {
                printf("*!*!*! read back error ---%s--- \n",buffer);
            }



            shmdt(ptr);
            return (0);
        }

    }
    shmdt(ptr);
    shmctl(memid, IPC_RMID, NULL);
    sem_unlink(SEM_1_NAME);
    return(0);
}

int isPalindrome(char str[])
{
    // Start from leftmost and rightmost corners of str
    int l = 0;
    int h = strlen(str) - 1;

    // Keep comparing characters while they are same
    while (h > l)
    {
        if (str[l++] != str[h--])
        {

            return 0;
        }
    }
    return 1;
}
