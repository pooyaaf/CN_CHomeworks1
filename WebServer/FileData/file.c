#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "./file.h"

// Extracts the filename needed from a GET request and adds public_html to the front of it
char *getFileName(char *msg)
{
    // Variable to store the filename in
    char *file;
    // Allocate some memory for the filename and check it went OK
    if ((file = malloc(sizeof(char) * strlen(msg))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to file in getFileName()\n");
        exit(EXIT_FAILURE);
    }

    // Get the filename from the header
    sscanf(msg, "GET %s HTTP/1.1", file);

    // Allocate some memory not in read o   nly space to store "public_html"
    char *base;
    if ((base = malloc(sizeof(char) * (strlen(file) + 18))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to base in getFileName()\n");
        exit(EXIT_FAILURE);
    }

    char *ph = "public_html";

    // Copy public_html to the non read only memory
    strcpy(base, ph);

    // Append the filename after public_html
    strcat(base, file);

    // Free file as we now have the file name in base
    free(file);

    // Return public_html/filetheywant.html
    return base;
}
// print a file out to a socket file descriptor
int printFile(int fd, char *filename)
{

    /* Open the file filename and echo the contents from it to the file descriptor fd */

    // Attempt to open the file
    FILE *read;
    if ((read = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Error opening file in printFile()\n");
        exit(EXIT_FAILURE);
    }

    // Get the size of this file for printing out later on
    int totalsize;
    struct stat st;
    stat(filename, &st);
    totalsize = st.st_size;

    // Variable for getline to write the size of the line its currently printing to
    size_t size = 1;

    // Get some space to store each line of the file in temporarily
    char *temp;
    if ((temp = malloc(sizeof(char) * size)) == NULL)
    {
        fprintf(stderr, "Error allocating memory to temp in printFile()\n");
        exit(EXIT_FAILURE);
    }

    // Int to keep track of what getline returns
    int end;

    // While getline is still getting data
    while ((end = getline(&temp, &size, read)) > 0)
    {
        sendMessage(fd, temp);
    }

    // Final new line
    sendMessage(fd, "\n");

    // Free temp as we no longer need it
    free(temp);

    // Return how big the file we sent out was
    return totalsize;
}
