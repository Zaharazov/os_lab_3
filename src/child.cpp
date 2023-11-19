
#include <unistd.h> // for read() and write()
#include <string>
#include <semaphore.h>
#include <sys/mman.h>

int main(int argc, char** argv)
{
    FILE *f = fopen("output.txt", "w");
    fprintf(f, "The answer is: ");

    int mfile = open("memory_file.txt", O_RDWR, 0666);
    char* buffer = (char*)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mfile, 0);
    close(mfile);

    sem_t* sem = sem_open(argv[1], O_CREAT, 0777, 0);
    if (sem == SEM_FAILED)
    {
        perror("Couldn't open the semaphore!");
        return -1;
    }

    size_t i = 0;
    float divisible = 0;
    float number = 0;
    std::string snumber = "";
    bool notempty = 0;

    sem_wait(sem); // blocking the semaphore

    while(buffer[i] != -1)
    {
        if (buffer[i] != ' ' && buffer[i] != '\n')
        {
            snumber = snumber + buffer[i];
        }
        else if (buffer[i] == '\n')
        {
            number = std::stof(snumber);
            if (number == 0) 
            {
                fprintf(f, "div_by_zero");
                exit(-1);  
            }

            divisible /= number;
            fprintf(f, "%f ", divisible);
            divisible = 0;
            number = 0;
            snumber = "";
        }
        else if (buffer[i] == ' ')
        {
            number = std::stof(snumber);
            if (number == 0)
            {
                fprintf(f, "div_by_zero");
                exit(-1);
            } 

            if (divisible == 0)
                divisible = number;
            else
                divisible /= number;

            number = 0;
            snumber = "";
        }
        ++i;
    }

    sem_close(sem);
    munmap(buffer, 1024);

    return 0;
}
