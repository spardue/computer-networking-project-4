/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Stephen Pardue, Rikin Marfatia	
* PROJECT:	CS 3251 Project 2 - Professor Traynor
* DESCRIPTION:	Network Client Code
*
*////////////////////////////////////////////////////////////

#include "sync.h"

char * commands = "Enter one of these characters for a command:\tL, D, P, E\n"
"(L)ist:\tlist the files on the server\n"
"(D)iff:\tperform a diff of the files on the client vs the server\n"
"(P)ull:\tget files from the server that this client doesn't have\n"
"L(E)ave:\tterminate the session\n";

DirectoryInfo *getDiffDirectoryInfo(int clientSock) {
	DirectoryInfo *serverDir = recvDirectoryInfo(clientSock);
	DirectoryInfo *clientDir = calloc(1, sizeof(DirectoryInfo));
	DirectoryInfo *diffDir;
	

	if(listDirectory(clientDir, CLIENT_DIR) < 0)
	{
		printf("Error in initializing the DirectoryInfo Struct");
		return NULL;		
	}

	diffDir = DirectoryInfoMinus(serverDir, clientDir);
	return diffDir;
}

int handle_list(int clientSock){
	
	DirectoryInfo *dInfo = recvDirectoryInfo(clientSock);
	printf("List of files on Server: \n");
    printDirectoryInfo(dInfo);
    freeDirectoryInfo(dInfo);
	free(dInfo);
}

int handle_diff(int clientSock){
	DirectoryInfo *diffDir = getDiffDirectoryInfo(clientSock);
	printf("Files not owned locally: \n");
	printDirectoryInfo(diffDir);
	freeDirectoryInfo(diffDir);
	free(diffDir);
}

int handle_pull(int clientSock){
    DirectoryInfo *diffDir = getDiffDirectoryInfo(clientSock);
    FileInfo *file;
    int i;

    send(clientSock, &(diffDir->length), sizeof(int), 0);
    if (diffDir->length > 0) {
        LIST_FOREACH(file, &(diffDir->head), FileInfoEntry){
            send(clientSock, file, sizeof(FileInfo), 0);
            printf("Getting file: %s\n", file->name);
        }
        
        for (i = 0; i < diffDir->length; i++){
            recvFile(clientSock, CLIENT_DIR);
        }
        
     } else {
        printf("No files to sync!\n");
     }
    
    
   
    freeDirectoryInfo(diffDir);
	free(diffDir);
    return 0;
}

int handle_exit(int clientSock) {
	printf("Closing connection... \n");
    return close(clientSock);
}

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* The server address */
    
    int i;			    /* Counter Value */

    /* Create a new TCP socket*/
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSock < 0) {
        fprintf(stderr, "Yo bro, clientsock wasn't made\n");
        exit(1);
    }


    /* Construct the server address structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT);


    /* Establish connecction to the server */
    if (connect(clientSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        fprintf(stderr, "Could not establish a connection\n");
        exit(1);
    }
    printf("Connected to the server.\n");
    printf(commands);

    char buff;
    char input;
    
    while(1){
        printf("Enter L, D, P, or E: ");
        input = getc(stdin);
        getc(stdin); //for the newline character
        ssize_t numBytes = send(clientSock, &input, sizeof(char), 0);
        if (numBytes < 0) {
            fprintf(stderr, "send() failed\n");
            exit(1);
        }
        

		if(input == 'L')
		{
		        handle_list(clientSock);
		}
		else if(input == 'D')
		{
		        handle_diff(clientSock);
		}
		else if(input == 'P')
		{
		        handle_pull(clientSock);
		}
		else if(input == 'E')
		{
		    handle_exit(clientSock);
		    break;
		}
		printf("\n");

    }
    printf("Bye Bye\n");
    return 0;
}

