#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    // The program should exit if the there are not exactly two arguments: the program name and the number of ticks to sleep.
    if (argc != 2)
    {
        printf("Incorrect argument formatting. Program usage: \"sleep [# of ticks to sleep]\"\n");
        exit(-1);
    }
    else
    {
        int ticks;
        ticks = atoi(argv[1]);      // Get the number of ticks from the second argument
        sleep(ticks);
        exit(0);
    }
}