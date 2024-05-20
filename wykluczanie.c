#include "semaphores.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SEM_NAME "/semaphore"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong number of arguments\n");
        printf("Usage: <process_name> <num_sections>");
        exit(1);
    }
    
    int num_sections = atoi(argv[1]);

    // Sekcja prywatna
    
    sem_t *sem = openSem(SEM_NAME);
    int value = getSemValue(sem);
    printf("Child process %d occupied semaphore address: %p, value: %d\n",getpid(), (void *)sem, value);
    semWait(sem);

    // Sekcja krytyczna

    sleep(rand() % 5);
    value = getSemValue(sem);
    printf("    Child process %d occupied semaphore address: %p, value: %d, critical section number: %d\n",getpid(), (void *)sem, value, num_sections);
        
    int file = open("numer.txt", O_RDONLY);

    if (file == -1)
    {
        perror("    Failed to open file");
        exit(1);
    }
        
    int number;
    char* buffer = (char*)calloc(1,sizeof(char));


    if (read(file, buffer, 1) == EOF)
    {
        perror("    Failed to read from file");
        exit(1);
    }

    if (close(file) == EOF)
    {
        perror("    Failed to close file");
        exit(1);
    }
        
    sleep(rand() % 5);
    number = number + 1;

    file = open("numer.txt", O_WRONLY);

    if (file == -1)
    {
        perror("    Failed to open file");
        exit(1);
    }

    if (write(file, &number, sizeof(number)) == -1)
    {
        perror("    Failed to write to file");
        exit(1);
    }

    if (close(file) == EOF)
    {
        perror("    Failed to close file");
        exit(1);
    }

    printf("    Child process %d incremented number to: %d\n", getpid(), number);
    semPost(sem);

    // Koniec sekcji krytycznej

    value = getSemValue(sem);
    printf("Child process %d occupied semaphore address: %p, value: %d\n",getpid(), (void *)sem, value);
    closeSem(sem);
    exit(0);    

return 0;
}