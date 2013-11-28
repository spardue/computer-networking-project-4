#################################################################
##
## FILE:	Makefile
## PROJECT:	CS 3251 Project 1 - Professor Traynor
## DESCRIPTION: Compile Project 1
##
#################################################################

CC=gcc

OS := $(shell uname -s)
# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: client server 

client: client.c
	$(CC) listdir.c listdir.h sync.h netfileio.c netfileio.h client.c -o client -lcrypto -lssl
server: server.c
	$(CC) -pthread sync.h listdir.c listdir.h netfileio.c netfileio.h server.c `xml2-config --cflags` -lxml2 xmlparser.c xmlparser.h  -o server -lcrypto -lssl 

clean:
	    rm -f client server *.o

