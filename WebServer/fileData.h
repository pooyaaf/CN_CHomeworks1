struct file_data
{
    int size;
    void *data;
};
// structure to hold the return code and the filepath to serve to client.
typedef struct
{
    int returncode;
    char *filename;
    int type; // GIF JPEG PDF etc.
    int content_length;
} httpRequest;
