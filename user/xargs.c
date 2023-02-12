#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void remwspace(char *string, int strlen)
{
    char *p;

    p = string;
    for(int i = 0; i < strlen; i++)
    {
        if (*p == ' ' || *p == '\n' || *p == '\r')
        {
            *p = '\0';
        }
        p++;
    }
}

int main(int argc, char *argv[])
{
    char *p;            // Temporary pointer to iterate through args array
    char **args;        // Array to store the arguments used with exec()
    int i;              // Index for the args array
    int pid;            // Store return value for fork()
    int readresult;     // Store return value for read()

    // Make sure that xargs was supplied at least one argument
    if (argc < 2)
    {
        printf("xargs: argument error; proper usage: \"xargs <command> [arguments]\"\n");
        exit(-1);
    }

    // Allocate memory for the args array
    args = malloc(sizeof(char *) * MAXARG);
    for (int i = 0; i < MAXARG; i++)
    {
        args[i] = malloc(sizeof(char) * 32);
    }

    // Copy the xargs arguments into the args array
    for (int i = 1; i < argc; i++)
    {
        strcpy(args[i - 1], argv[i]);
    }

    // Set the indexer and pointer to the array position after the last copied argument
    i = argc - 1;
    p = args[i];

    // Copy input from stdin into args, one word and line at a time
    readresult = 1;
    while (readresult > 0)
    {
        // Read 1 character into the current pointer position
        readresult = read(0, p, 1);

        // printf("%c\n", *p);

        // At the end of a word, null-terminate the argument and increment the args array
        if (strcmp(p, " ") == 0)
        {
            *p = '\0';
            p = args[++i];
            // printf("%s\n", args[i - 1]);
        }
        // At the end of a line, run exec on the current args and then reset the args array
        else if (strcmp(p, "\n") == 0 || strcmp(p, "\r") == 0 || readresult < 1)
        {
            // printf("newline/EOF reached\n");
            *p = '\0';

            remwspace(args[i], 32);

            // Temporarily remove the pointer that comes after the last argument as a means of
            // null-terminating the array
            p = args[++i];
            args[i] = 0x00;

            // Fork; the child should call exec(), and the parent should wait on the child
            pid = fork();
            if (pid > 0)
            {
                wait(0);
                // printf("parent resumed\n");
            }
            else
            {
                // printf("command = %s\n", args[0]);
                // printf("arg1 = %s\n", args[1]);
                // printf("arg2 = %s\n", args[2]);
                // printf("arg3 = %s\n", args[3]);
                // printf("child executing...\n");

                exec(args[0], args);
            }

            // Restore the array pointer that was removed and reset the args array
            args[i] = p;
            i = argc - 1;
            p = args[i];
        }
        // Increment the pointer position
        else
            p++;
    }

    // Free all the memory that was allocated
    for (int i = 0; i < MAXARG; i++)
    {
        free(args[i]);
    }
    free(args);

    exit(0);
}