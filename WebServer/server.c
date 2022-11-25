#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>       

#define PORT 18000

// structure to hold the return code and the filepath to serve to client.
typedef struct
{
    int returncode;
    char *filename;
} httpRequest;
// headers to send to clients
char *header200 = "HTTP/1.0 200 OK\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
char *header400 = "HTTP/1.0 400 Bad Request\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
char *header404 = "HTTP/1.0 404 Not Found\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
// ---- //
// send a message to a socket file descripter
int sendMessage(int fd, char *msg) {
    return write(fd, msg, strlen(msg));
}
// Extracts the filename needed from a GET request and adds public_html to the front of it
char * getFileName(char* msg)
{
    // Variable to store the filename in
    char * file;
    // Allocate some memory for the filename and check it went OK
    if( (file = malloc(sizeof(char) * strlen(msg))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to file in getFileName()\n");
        exit(EXIT_FAILURE);
    }
    
    // Get the filename from the header
    sscanf(msg, "GET %s HTTP/1.1", file);
    
    // Allocate some memory not in read only space to store "public_html"
    char *base;
    if( (base = malloc(sizeof(char) * (strlen(file) + 18))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to base in getFileName()\n");
        exit(EXIT_FAILURE);
    }
    
    char* ph = "public_html";
    
    // Copy public_html to the non read only memory
    strcpy(base, ph);
    
    // Append the filename after public_html
    strcat(base, file);
    
    // Free file as we now have the file name in base
    free(file);
    
    // Return public_html/filetheywant.html
    return base;
}

// get a message from the socket until a blank line is recieved
char *getMessage(int fd)
{

    // A file stream
    FILE *sstream;

    // Try to open the socket to the file stream and handle any failures
    if ((sstream = fdopen(fd, "r")) == NULL)
    {
        fprintf(stderr, "Error opening file descriptor in getMessage()\n");
        exit(EXIT_FAILURE);
    }

    // Size variable for passing to getline
    size_t size = 1;

    char *block;

    // Allocate some memory for block and check it went ok
    if ((block = malloc(sizeof(char) * size)) == NULL)
    {
        fprintf(stderr, "Error allocating memory to block in getMessage\n");
        exit(EXIT_FAILURE);
    }

    // Set block to null
    *block = '\0';

    // Allocate some memory for tmp and check it went ok
    char *tmp;
    if ((tmp = malloc(sizeof(char) * size)) == NULL)
    {
        fprintf(stderr, "Error allocating memory to tmp in getMessage\n");
        exit(EXIT_FAILURE);
    }
    // Set tmp to null
    *tmp = '\0';

    // Int to keep track of what getline returns
    int end;
    // Int to help use resize block
    int oldsize = 1;

    // While getline is still getting data
    while ((end = getline(&tmp, &size, sstream)) > 0)
    {
        // If the line its read is a caridge return and a new line were at the end of the header so break
        if (strcmp(tmp, "\r\n") == 0)
        {
            break;
        }

        // Resize block
        block = realloc(block, size + oldsize);
        // Set the value of oldsize to the current size of block
        oldsize += size;
        // Append the latest line we got to block
        strcat(block, tmp);
    }

    // Free tmp a we no longer need it
    free(tmp);

    // Return the header
    return block;
}

// parse a HTTP request and return an object with return code and filename
httpRequest parseRequest(char *msg)
{
    httpRequest ret;

    // A variable to store the name of the file they want
    char *filename;
    // Allocate some memory to filename and check it goes OK
    if ((filename = malloc(sizeof(char) * strlen(msg))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to filename in parseRequest()\n");
        exit(EXIT_FAILURE);
    }
    // Find out what page they want
    filename = getFileName(msg);

    // Check if its a directory traversal attack
    char *badstring = "..";
    char *test = strstr(filename, badstring);

    // Check if they asked for / and give them index.html
    int test2 = strcmp(filename, "public_html/");

    // Check if the page they want exists
    FILE *exists = fopen(filename, "r");

    // If the badstring is found in the filename
    if (test != NULL)
    {
        // Return a 400 header and 400.html
        ret.returncode = 400;
        ret.filename = "400.html";
    }

    // If they asked for / return index.html
    else if (test2 == 0)
    {
        ret.returncode = 200;
        ret.filename = "public_html/index.html";
    }

    // If they asked for a specific page and it exists because we opened it sucessfully return it
    else if (exists != NULL)
    {

        ret.returncode = 200;
        ret.filename = filename;
        // Close the file stream
        fclose(exists);
    }

    // If we get here the file they want doesn't exist so return a 404
    else
    {
        ret.returncode = 404;
        ret.filename = "404.html";
    }

    // Return the structure containing the details
    return ret;
}
int getHeaderSize(int fd, int returncode)
{
    // return match header size
    switch (returncode)
    {
    case 200:
        sendMessage(fd, header200);
        return strlen(header200);
        break;

    case 400:
        sendMessage(fd, header400);
        return strlen(header400);
        break;

    case 404:
        sendMessage(fd, header404);
        return strlen(header404);
        break;
    }
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

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Only this line has been changed. Everything is same.
    // char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        // Get the message from the file descriptor
        char *header = getMessage(new_socket);
        // Parse the request
        httpRequest details = parseRequest(header);
        // We done with header so we can free it!
        free(header);
        // Get header size
        int headersize;
        headersize = getHeaderSize(new_socket, details.returncode);
        // Print out the matched file
        int pagesize;
        pagesize = printFile(new_socket, details.filename);

        // char buffer[30000] = {0};
        // valread = read(new_socket, buffer, 30000);
        // printf("%s\n", buffer);
        // write(new_socket, hello, strlen(hello));
        // printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}