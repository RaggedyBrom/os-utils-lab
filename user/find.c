#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// Recursively checks or directory at "path" for files that match the name "fname"
void find(char *path, char *fname)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // Open a descriptor for the file at path
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // Grab the stats for the descriptor
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Switch based on if the descriptor is a device, file, or directory
    switch (st.type)
    {
    case T_DEVICE:
    // If the descriptor is for a file we check if it's a match
    case T_FILE:
        if (strcmp(path, fname) == 0)
        {
            printf("%s", path);
        }
        break;
    // If the descriptor is for a directory we need to search its contents
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: path too long\n");
            break;
        }
        
        // Copy the path into a buffer and set a pointer to the end of it
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        
        // Read dirents from the directory file until it ends
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;

            // Move the name of the dirent to the end of the path buffer
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // Check the stats for file corresponding to the dirent
            if (stat(buf, &st) < 0)
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }

            // If the dirent is a directory and not "." or "..", recursively call find() on it
            if (st.type == T_DIR)
            {
                if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
                {
                    find(buf, fname);
                }
            }
            // If the dirent is a file, check if it's a match
            else if (strcmp(de.name, fname) == 0)
            {
                printf("%s\n", buf);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "find: argument error; correct usage: \"find [start path] <filename>\"\n");
        exit(-1);
    }

    find(argv[1], argv[2]);
    exit(0);
}