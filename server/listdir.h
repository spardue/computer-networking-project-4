#pragma once
#include <dirent.h>
#include <openssl/md5.h>
#include <sys/queue.h>
#define MAX_BUFF 512
#define NAME_MAX 256

struct FileInfo {
	char name[NAME_MAX];
	char checksum[MD5_DIGEST_LENGTH];
	LIST_ENTRY(FileInfo) FileInfoEntry;
};

typedef struct FileInfo FileInfo;

//LIST_HEAD(FileInfoList, FileInfo);

typedef struct {
    LIST_HEAD(FileInfoList, FileInfo) head;
    int length;
} DirectoryInfo;





