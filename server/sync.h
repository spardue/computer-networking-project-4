#pragma once
#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>	    /* for OpenSSL EVP digest libraries/SHA256 */
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "listdir.h"
#include "netfileio.h"
#include "xmlparser.h"

/* Constants */
#define RCVBUFSIZE 1024		    /* The receive buffer size */
#define SNDBUFSIZE 1		    /* The send buffer size */
#define MDLEN 32

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1337

#define CLIENT_DIR "./clientFiles"
#define SERVER_DIR "./theProject"
#define LOG_FILE "./LOG.txt"

#define MAXPENDING 5
#define BUFSIZE 25

typedef struct {
	struct sockaddr_in clientAddr;
	int clientSock;
} SockAndAddr;
