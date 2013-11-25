#pragma once
#include "listdir.h"
#include <sys/stat.h>
#include "sync.h"

void recvFile(int socket, char *directory);
void sendFile(int socket, char *directory, FileInfo *file);
