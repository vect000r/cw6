#include "semaphores.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Wrong number of arguments in wykluczanie\n");
        printf("Usage: <semaphore_name > <text_file> <num_sections>");
        exit(1);
    }
    
    int num_sections = atoi(argv[3]);
    char buffer1[10];
    char buffer2[10];
    ssize_t bytesRead;
    
    // Sekcja prywatna
    
    sem_t *sem = openSem(argv[1]);
    int value = getSemValue(sem);
    printf("Child process %d occupied semaphore address: %p, value: %d\n",getpid(), (void *)sem, value);
    semWait(sem);

    // Sekcja krytyczna
    for(int i = 0; i < num_sections; i++)
    {
        sleep(rand() % 5);
        value = getSemValue(sem);
        printf("    Child process %d occupied semaphore address: %p, value: %d, critical section number: %d\n",getpid(), (void *)sem, value, i);
        
        int file = open(argv[2], O_RDONLY);

        if (file == -1)
        {
            perror("    Failed to open file");
            exit(1);
        }
        
        bytesRead = read(file, buffer1, sizeof(buffer1));

        if (bytesRead == -1)
        {
            perror("    Failed to read from file");
            exit(1);
        }
        
        int number = atoi(buffer1);
       
        if (close(file) == EOF)
        {
            perror("    Failed to close file");
            exit(1);
        }
        
        sleep(rand() % 5);
        number = number + 1;
        snprintf(buffer2, sizeof(buffer2),"%d", number);

        file = open(argv[2], O_WRONLY);

        if (file == -1)
        {
            perror("    Failed to open file");
            exit(1);
        }
    
        if (write(file, buffer2, strlen(buffer2)) == -1)
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
    }
    // Koniec sekcji krytycznej

    value = getSemValue(sem);
    printf("Child process %d occupied semaphore address: %p, value: %d\n",getpid(), (void *)sem, value);
    closeSem(sem);
    exit(0);    

    return 0;
}
