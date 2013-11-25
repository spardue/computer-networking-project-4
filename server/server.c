/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:   Stephen Pardue, Rikin Marfatia	
* PROJECT:	CS 3251 Project 2 - Professor Traynor
* DESCRIPTION:	Network Server Code
*
*////////////////////////////////////////////////////////////

#include "sync.h"

void sendDirectoryInfo(int clientSock) {
	DirectoryInfo dInfo;
	FileInfo *file;
	int i;
	memset(&dInfo, 0, sizeof(DirectoryInfo));
	listDirectory(&dInfo, SERVER_DIR);     // populates the dInfo struct

    
    
    //send the lenght of the dirInfo
    send(clientSock, &dInfo.length, sizeof(int), 0);
    LIST_FOREACH(file, &(dInfo.head), FileInfoEntry){
        send(clientSock, file, sizeof(FileInfo), 0);
    }
    
    freeDirectoryInfo(&dInfo);
}


int handle_list(int clientSock, struct sockaddr_in clientAddr){
	printf("Sending List to: %s\n", inet_ntoa(clientAddr.sin_addr));
	FILE *fp = fopen(LOG_FILE, "a");
	fprintf(fp, "Sending List to: %s\n", inet_ntoa(clientAddr.sin_addr));
	fclose(fp);
	
    sendDirectoryInfo(clientSock);
	return 0;
	
}

int handle_diff(int clientSock, struct sockaddr_in clientAddr){
	printf("Sending Diff to: %s\n", inet_ntoa(clientAddr.sin_addr));
	FILE *fp = fopen(LOG_FILE, "a");
	fprintf(fp, "Sending Diff to: %s\n", inet_ntoa(clientAddr.sin_addr));
	fclose(fp);

    sendDirectoryInfo(clientSock);
    return 0;
}

int handle_pull(int clientSock, struct sockaddr_in clientAddr){
	printf("Sending Files to: %s\n", inet_ntoa(clientAddr.sin_addr));
    DirectoryInfo *diffDir;
    FileInfo *file;

	FILE *fp = fopen(LOG_FILE, "a");
	fprintf(fp, "Sending Files to: %s\n", inet_ntoa(clientAddr.sin_addr));
	fclose(fp);
	
    sendDirectoryInfo(clientSock);
    diffDir = recvDirectoryInfo(clientSock);
    
    if (diffDir->length > 0){
        LIST_FOREACH(file, &(diffDir->head), FileInfoEntry){
            sendFile(clientSock, SERVER_DIR, file);
        }
    } else {
        printf("No files to sync!\n");
    }
    
    freeDirectoryInfo(diffDir);
	free(diffDir);
    return 0;
}

int handle_exit(int clientSock) {
    printf("Client terminated the connection.\n");
    return 0;
}


void *handleClient(SockAndAddr *sa) {

    char buff; 
    char toSend;
    int clientSock = sa->clientSock; // *((int *)clientSocket);
	struct sockaddr_in clientAddr = sa->clientAddr;

	printf("Request from: %s\n", inet_ntoa(clientAddr.sin_addr));
    
    while (1) {

        if (recv(clientSock, &buff, sizeof(char), 0) < 0){
            fprintf(stderr, "Yo, can't recv dat data bro\n");
            exit(1);
        }

        switch(buff) {
            case 'L':
                handle_list(clientSock, clientAddr);
                break;
            case 'D':
                handle_diff(clientSock, clientAddr);
                break;
            case 'P':
                handle_pull(clientSock, clientAddr);
                break;
            case 'E':
                handle_exit(clientSock);
                break;
        }
        if (buff == 'E') {
            break;
        }
       
    }
    close(clientSock);
    free(sa); //i guess?
    pthread_exit(NULL);
}

/* The main function */
int main(int argc, char *argv[])
{

    int serverSock;				/* Server Socket */
    //int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort = SERVER_PORT;		/* Server port */
    unsigned int clntLen = sizeof(changeServAddr);			/* Length of address data struct */

    //system("mkdir theProject"); ---- we can make a new directory and set it to that directory, or we can add choice.

    /* Create new TCP Socket for incoming requests*/
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock < 0) {
        fprintf(stderr, "could not create server socket\n");
        exit(1);
    }

    int on = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* Construct local address structure*/
    memset(&changeServAddr, 0, clntLen);
    changeServAddr.sin_family = AF_INET;
    changeServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    changeServAddr.sin_port = htons(changeServPort);
    
    /* Bind to local address structure */
    if (bind(serverSock, (struct sockaddr *) &changeServAddr, clntLen) < 0){
        fprintf(stderr, "could not bind to local address");
        exit(1);
    }

    /* Listen for incoming connections */
    printf("Listening for incoming connections.\n");
    if (listen(serverSock, MAXPENDING) < 0) {
        fprintf(stderr, "listen() failed\n");
        exit(1);
    }

    int * clientSock;
	FILE *fp;
	fp = fopen(LOG_FILE, "a");
	fprintf(fp, "SERVER LOGS\n");
	fclose(fp);
    /* Loop server forever*/
    while(1)
    {

        /* Accept incoming connection */
        memset(&changeClntAddr, 0, clntLen);
        clientSock = (int *) malloc(sizeof(int));
        *clientSock = accept(serverSock, (struct sockaddr *) &changeClntAddr, &clntLen);
        if (*clientSock < 0){
            continue;
            //fprintf(stderr, "error accepting the client socket\n");
            //exit(1);
        }
		SockAndAddr *tuple = (SockAndAddr *) malloc(sizeof(SockAndAddr));
		tuple->clientAddr = changeClntAddr;
		tuple->clientSock = *clientSock;
        pthread_t pth;
        printf("Handling a new client.\n");
        pthread_create((pthread_t *) malloc(sizeof(pthread_t)), NULL, handleClient,tuple);
    }
}


                                                       

