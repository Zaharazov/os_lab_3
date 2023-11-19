#include <unistd.h> // for fork() and pipe
#include <string>
#include <semaphore.h>
#include <sys/mman.h>

int main()
{
    int mfile = open("memory_file.txt", O_RDWR | O_CREAT, 0666); // file descriptor has been created
    if (mfile == -1)
    {
        perror("Couldn't open the file!");
        return -1;
    }
    ftruncate(mfile, 1024); // file reduction to 1024 bytes

    char* buffer = (char*)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mfile, 0); // creating a memory map
    if (buffer == MAP_FAILED)
    {
        perror("Failed to create the buffer!");
        return -1;
    }
    close(mfile); // closing a file

    sem_t* sem = sem_open("semaf", O_CREAT, 0777, 0); // creating a semaphore
    if (sem == SEM_FAILED)
    {
        perror("Couldn't open the semaphore!");
        return -1;
    }

    int id = fork(); // create a process

    if (id == -1) // error
    {
        return -1;
    }
    else if (id == 0) // child
    {
        execlp("./child", "./child", "semaf", NULL); // launching a file for execution
        return 0;
    }
    else // parent (id > 0)
    {
        char c;
        c = getchar();
        size_t ind = 0;
        
        while (c != EOF)
        {
            buffer[ind++] = c;
            c = getchar();
        }
        buffer[ind] = c;

        sem_post(sem); // unlocking the semaphore
        sem_close(sem); // closing the semaphore

        munmap(buffer, 1024); // cleaning a memory map
        
        int status;
        waitpid(0, &status, 0); // awaiting process status change

        if (status != 0) return -1;
        
    }
    return 0;
}
