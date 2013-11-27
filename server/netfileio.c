#include "netfileio.h"



void recvFile(int socket, char *directory){
    FILE *file;
    FileInfo f;
    int size = 0;
    int total;
    int got = 0;
    int toWrite;
    char *buff;
    
    chdir(directory);
    if (recv(socket, &f, sizeof(FileInfo), 0) < 0){
        fprintf(stderr, "Error recieving the FileInfo in recvFile\n");
        exit(-1);
    }
    if (recv(socket, &size, sizeof(int), 0) < 0){
        fprintf(stderr, "Error recieving the size of the file in recvFile\n");
        exit(-1);
    }
    fprintf("size: \n", size);
 
    buff = malloc(RCVBUFSIZE*sizeof(char));
    if (buff == NULL){
        fprintf(stderr, "Error allocating memory for the file in recvFile\n");
        exit(-1);
    }
    file = fopen(f.name, "wb");
    printf("f.name: %s\n", f.name);
    if (file == NULL){
        fprintf(stderr, "could not open the file for writing... in recvFile\n");
        exit(-1);
    }
    total = size;
    printf("total : %d\n", total);
    while (total > 0) {
        memset(buff, 0, RCVBUFSIZE);
        
        if (total < RCVBUFSIZE) {
            toWrite = total;
        } else {
            toWrite = RCVBUFSIZE;
        }
        total-=toWrite;
        //printf("To write: %d\n", toWrite);
        
        
        int read;
        if ((read = recv(socket, buff, toWrite*sizeof(char), 0)) < 0){
            fprintf(stderr, "Error recieving the file in recvFile\n");
            exit(-1);
        }
        //printf("read %d, toWrite %d\n", read, toWrite);
        if (fwrite(buff, sizeof(char), toWrite, file) != sizeof(char)*toWrite){
            fprintf(stderr, "error writing to file in recvFile\n");
            exit(-1);
        }
        got+=RCVBUFSIZE;
     }
     
     printf("bye...\n");
        
    
    
    fclose(file);
    free(buff);
    chdir("..");
}


void sendFile(int socket, char *directory, FileInfo *file) {
    chdir(directory);
    struct stat st;
    FILE * f;
    char * buff;
    
    int total;
    int sent = 0;
    int toRead;
    int i;
    
    printf("HI: %s\n", file->name);    
    stat(file->name, &st);

    
    char buff2 = 'a';
    
    for (i = 0; i < 255; i++){
        send(socket, &(file->name[i]), 1, 0);
    }
    
    //send(socket, file, sizeof(FileInfo), 0);
    
    //send the size
    int32_t size = st.st_size;
    printf("name: %s, size: %d\n", file->name, size);
    //size = htonl(size);
    printf("name: %s, size: %d\n", file->name, size);
    send(socket, &st.st_size, sizeof(int32_t), 0);
    
    
    buff = malloc(RCVBUFSIZE*sizeof(char));
    if (buff == NULL){
        fprintf(stderr, "Error allocating memory for the file in recvFile\n");
        exit(-1);
    }
    
    f = fopen(file->name, "rb");
    if (f == NULL){
        fprintf(stderr, "error opening the file for reading in sendFile\n");
        exit(1);
    }
    total = size;
    while (total > 0) {
        //printf("total: %d\n", total);
        memset(buff, 0, RCVBUFSIZE);
        
        if (total < RCVBUFSIZE) {
            toRead = total;
        } else {
            toRead = RCVBUFSIZE;
        }
        total-=toRead;
        //printf("toRead: %d\n", toRead);
        
        if (fread(buff, sizeof(char), toRead, f) != sizeof(char)*toRead){
            fprintf(stderr, "error reading the file in sendFile\n");
            exit(1);
        }
        
        int sent = send(socket, buff, sizeof(char)*toRead, 0);
        
        if (sent != toRead) {
            printf("sent: %d\n", sent);
        }
        if (sent != sizeof(char)*toRead){
            fprintf(stderr, "error sending the buff in sendFile\n");
            exit(1);
        }
    }
    
    fclose(f);
    free(buff);
    chdir("..");
}


