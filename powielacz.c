#include "semaphores.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <wait.h>
#include <assert.h>
#include <signal.h>

#define SEM_NAME "/semaphore"
#define FILE_NAME "numer.txt"

void at_end_removal()
{
    unlinkSem(SEM_NAME);
}

void signal_handler(int sig)
{
    printf("Received signal %d\n", sig);
    unlinkSem(SEM_NAME);
    exit(0);
}

int main(int argc, char *argv[]) {
    
    if (argc != 4)
    {
        printf("Wrong number of arguments\n");
        printf("Usage: %s <child_name> <num_processes> <num_sections>\n", argv[0]);
        exit(1);
    }
    
    // Utworzenie nowego semafora

    sem_t *semaphore = createSem(SEM_NAME);
    openSem(SEM_NAME);
    
    // Sprawdzenie i wypisanie wartości semafora
    
    int val = getSemValue(semaphore);
    printf("Semaphore address: %p, value: %d\n", (void *)semaphore, val);

    
    int num_processes = atoi(argv[2]);
    int num_sections = atoi(argv[3]);

    // Rejestracja funkcji atexit i ustawienie obsługi sygnało SIGINT
    
    if(atexit(at_end_removal)==-1)
    {
        perror("Failed to register atexit function");
        exit(1);
    }
    
    if(signal(SIGINT, signal_handler)==SIG_ERR)
    {
        perror("Failed to set signal handler");
        exit(1);
    }

    // Utworzenie pliku numer.txt i wpisanie do niego wartości 0
    
    FILE *file = fopen(FILE_NAME, "w");
    
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(1);
    }
    
    if(fprintf(file, "0") == -1)
    {
        perror("Failed to write to file");
        exit(1);
    }
    
    if(fclose(file) == EOF)
    {
        perror("Failed to close file");
        exit(1);
    }
    
    // Utworzenie procesów potomnych

    for (int i = 0; i < num_processes; i++)
    {
        pid_t pid = fork();
        
        if (pid == -1)
        {
            perror("Failed to fork");
            exit(1);
        }
        else if (pid == 0)
        {
            num_sections = num_sections-1;
            
            if(num_sections==0)
            {
                break;
            }

            if(execlp(argv[1], argv[1], num_sections, NULL)==-1)
            {
                perror("Failed to execute");
                exit(1);
            }
        }
        else 
        {
            if (wait(NULL) == -1)
            {
                perror("Failed to wait");
                exit(1);
            }
        }
    
    }   
    return 0;
}