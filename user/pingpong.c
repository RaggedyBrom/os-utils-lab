#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int toChild[2], toParent[2];    // Arrays to hold the file descriptors for the two pipes
    int pid;
    int buf;

    // Establish the two pipes and make sure the creation was successful
    if (pipe(toChild) < 0 || pipe(toParent) < 0)
    {
        printf("Error creating pipes.");
        exit(-1);
    }

    write(toChild[0], &buf, 1);
    pid = fork();

    // Check for error creating the child process
    if (pid < 0)
    {
        printf("Error creating child process.");
        exit (-1);
    }
    // This only runs if the process is the child
    else if (pid == 0)
    {
        pid = getpid();
        read(toChild[1], &buf, 1);
        printf("%d: received ping\n", pid);

        write(toParent[0], &buf, 1);
        exit(0);
    }
    // This only runs if the process is the parent AND there was no error creating the child
    else
    {
        wait(0);

        pid = getpid();
        read(toParent[1], &buf, 1);
        printf("%d: received pong\n", pid);
    }
    exit(0);
}