#include "semaphores.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <wait.h>
#include <signal.h>
#include <string.h>

#define SEM_NAME "/semaphore"
#define FILE_NAME "numer.txt"

void at_end_removal()
{
    unlinkSem(SEM_NAME);
}

void signal_handler(int sig)
{
    printf("Received signal %d\n", sig);
    if (remove(FILE_NAME) == -1)
    {
        perror("Failed to remove file");
        exit(1);
    }    
    // usunięcie semafora w tej funkcji odbywa sie za pomocą exit, ponieważ już w funkcji atexit jest wywołane usunięcie semafora
    exit(0);
}

int main(int argc, char *argv[]) {
    
    if (argc != 4)
    {
        printf("Wrong number of arguments in powielacz\n");
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
    
    int file = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    
    if (file == -1)
    {
        perror("Failed to open file");
        exit(1);
    }
    
    char buffer1[10];
    int number = 0;
    snprintf(buffer1, sizeof(buffer1),"%d", number);

    if(write(file, buffer1, strlen(buffer1)) == -1)
    {
        perror("Failed to write to file");
        exit(1);
    }
    
    if(close(file) == EOF)
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

            if(execlp(argv[1], argv[1], "/semaphore", "numer.txt", argv[3], NULL)==-1)
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
    
    file = open(FILE_NAME, O_RDONLY);

    if (file == -1)
    {
        perror("Failed to open file");
        exit(1);
    }
    
    char buffer2[10];
    ssize_t bytesRead = read(file, buffer2, sizeof(buffer2));

    if (bytesRead == -1)
    {
        perror("Failed to read from file");
        exit(1);
    }

    if (close(file) == EOF)
    {
        perror("Failed to close file");
        exit(1);
    }
    
    if (remove(FILE_NAME) == -1)
    {
        perror("Failed to remove file");
        exit(1);
    }
    
    int number_after = atoi(buffer2);

    if (number_after == num_processes * num_sections)
    {
        printf("Correct value in file\n");
    }
    else
    {
        printf("Incorrect value in file\n");
    }
    
    return 0;
}