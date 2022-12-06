#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "FileData/file.h"

#define PORT 18000
struct file_data *filedata; // load MIME FILES into this var

// structure to hold the return code and the filepath to serve to client.
typedef struct
{
    int returncode;
    char *filename;
    int type; // GIF JPEG PDF etc.
    int content_length;
} httpRequest;

struct file_data
{
    int size;
    void *data;
};

// headers to send to clients
char *header200 = "HTTP/1.0 200 OK\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
char *header400 = "HTTP/1.0 400 Bad Request\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
char *header404 = "HTTP/1.0 404 Not Found\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
// char *headerJPEG = "HTTP/1.1 200 OK\nConnection: close\nContent-Type: image/jpeg\nContent-Length: 244480\n\n";
// char *headerGIF = "HTTP/1.0 200 Ok\nServer: CS241Serv v0.1\nContent-Type: image/gif\n\n";
// char *headerMP3 = "HTTP/1.0 200 Ok\nServer: CS241Serv v0.1\nContent-Type: audio/mpeg\n\n";
// char *headerPDF = "HTTP/1.0 200 Ok\nServer: CS241Serv v0.1\nContent-Type: application/pdf\n\n";

// ---- // MIME SEND
int send_response(int fd, char *header, char *content_type, void *body, int content_length)
{
    const int max_response_size = 262144;
    char response[max_response_size];
    int response_length;
    time_t t = time(NULL);
    struct tm *local_time = localtime(&t);
    char *timestamp = asctime(local_time);

    response_length = sprintf(response, "%s\nDate %s Connection: close\nContent-Type: %s\nContent-Length: %d\n\n", header, timestamp, content_type, content_length);
    memcpy(response + response_length, body, content_length);
    response_length += content_length;

    // Send it all!
    int rv = send(fd, response, response_length, 0);
    if (rv < 0)
    {
        perror("send");
    }

    return rv;
}

struct file_data *file_load(char *filename)
{
    char *buffer, *p;
    struct stat buf;
    int bytes_read, bytes_remaining, total_bytes = 0;

    // Get the file size
    if (stat(filename, &buf) == -1)
    {
        return NULL;
    }

    // Make sure it's a regular file
    if (!(buf.st_mode & S_IFREG))
    {
        return NULL;
    }

    // Open the file for reading
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        return NULL;
    }

    // Allocate that many bytes
    bytes_remaining = buf.st_size;
    p = buffer = malloc(bytes_remaining);

    if (buffer == NULL)
    {
        return NULL;
    }

    // Read in the entire file
    while (bytes_read = fread(p, 1, bytes_remaining, fp), bytes_read != 0 && bytes_remaining > 0)
    {
        if (bytes_read == -1)
        {
            free(buffer);
            return NULL;
        }

        bytes_remaining -= bytes_read;
        p += bytes_read;
        total_bytes += bytes_read;
    }

    // Allocate the file data struct
    struct file_data *filedata = malloc(sizeof *filedata);

    if (filedata == NULL)
    {
        free(buffer);
        return NULL;
    }

    filedata->data = buffer;
    filedata->size = total_bytes;

    return filedata;
}

//-----//
// send a message to a socket file descripter
int sendMessage(int fd, char *msg)
{
    return write(fd, msg, strlen(msg));
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
    filedata = file_load(filename);
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
        //    char *ext = strrchr(filename, '.');
        //    if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0)
        if (strchr(filename, 'jpg') != NULL || strchr(filename, 'jpeg') != NULL)
            ret.type = 1; // JPEG ( self-made var )
        if (strchr(filename, 'gif') != NULL || strchr(filename, 'GIF') != NULL)
            ret.type = 2; // GIF ( self-made var )
        if (strchr(filename, 'mp3') != NULL || strchr(filename, 'MP3') != NULL)
            ret.type = 3; // MP3 ( self-made var )
        if (strchr(filename, 'pdf') != NULL || strchr(filename, 'PDF') != NULL)
            ret.type = 4; // MP3 ( self-made var )
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
int getHeaderSize(int fd, int returncode, int type)
{
    // return match header size
    switch (returncode)
    {
    case 200:
        if (type == 1)
        {
            send_response(fd, "HTTP/1.1 200 OK", "image/jpg", filedata->data, filedata->size);
        }

        else if (type == 2)
        {
            send_response(fd, "HTTP/1.1 200 OK", "image/gif", filedata->data, filedata->size);
        }
        else if (type == 3)
        {
            send_response(fd, "HTTP/1.1 200 OK", "audio/mpeg", filedata->data, filedata->size);
        }
        else if (type == 4)
        {
            send_response(fd, "HTTP/1.1 200 OK", "application/pdf", filedata->data, filedata->size);
        }
        else
        {
            sendMessage(fd, header200);
            return strlen(header200);
        }

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
        headersize = getHeaderSize(new_socket, details.returncode, details.type);
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