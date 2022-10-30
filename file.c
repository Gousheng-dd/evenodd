#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "log.c"

typedef unsigned long long uint64;

#define PATH_NAME_LEN 100

/// @brief read file from emulated disks
/// @param path emulated disks path
/// @param fileName 待读取文件名
/// @param p 保存根据包含 fileName 的 disks 个数推断出的质数 p
/// @return 指向读取结果 char[p][block_size] 的指针
int ReadBlocks(char *path, char* fileName, int *p, char ****buf) {
    DIR *dp, *diskp;
    FILE *fp;
    struct dirent *dirName, *fName;
    struct stat stBuf;

    dp = opendir(path);
    if(!dp) {
        LogRecord(ERROR, "open directory %s error\n", path);
        return -1;
    }
    // get p and read file from disks
    *p = 0;
    while(dirName = readdir(dp)){
        if(strncmp(dirName->d_name, "disk_", 5)) {
            continue;
        }
        char fullPath[PATH_NAME_LEN];
        int diskIdx;
        sscanf(dirName->d_name, "disk_%d", &diskIdx);
        sprintf(fullPath, "%s/%s", path, dirName->d_name);
        diskp = opendir(fullPath);
        if(!dp) {
            LogRecord(ERROR, "open emulated disk directory %s error\n", fullPath);
            continue;
        }
        // travese disk_i file dictory to find "fileName"
        while(fName = readdir(diskp)) {
            if(!strncmp(fName->d_name, fileName, strlen(fileName))) {
                if(*p == 0) {
                    // get p from filename
                    char *pos = strrchr(fName->d_name, '_');
                    *p = atoi(pos+1);
                    *buf = (char ***)malloc(sizeof(char **) * (*p+2));
                }
                // read file
                sprintf(fullPath, "%s/%s", fullPath, fName->d_name);
                stat(fullPath, &stBuf);
                uint64 blockSize = stBuf.st_size / (p - 1) + 1;
                (*buf)[diskIdx] = (char **)malloc(sizeof(char*) * (p-1));
                fp = fopen(fullPath, "r");
                if(!fp) {
                    LogRecord(WARN, "open file %s error\n", fullPath);
                    free((*buf)[diskIdx]);
                    (*buf)[diskIdx] = NULL;
                    break;
                }
                for(int i = 0;i < p - 2; i++) {
                    (*buf)[diskIdx][i] = (char *)malloc(sizeof(char) * blockSize);
                    uint64 readLen = fread((*buf)[diskIdx][i], sizeof(char), blockSize, fp);
                    if(!readLen) {
                        LogRecord(WARN, "expect to read %lld bytes from %s for block %d, but read 0 byte\n", readLen, fullPath, i);
                        free((*buf)[diskIdx][i]);
                        (*buf)[diskIdx][i] = NULL;
                    } else {
                        LogRecord(INFO, "read %lld bytes form %s for block %d\n", readLen, fullPath, i);
                    }
                }
                // calculate fread ElementCount for block p-1
                (*buf)[diskIdx][i] = (char *)malloc(sizeof(char) * blockSize);
                memset((*buf)[diskIdx][i], 0, sizeof(char) * blockSize);
                uint64 readLen = fread((*buf)[diskIdx][p-2], sizeof(char), stBuf.st_size - blockSize * (p-2), fp);
                if(!readLen) {
                    LogRecord(WARN, "expect to read %lld bytes from %s for block %d, but read 0 byte\n", stBuf.st_size - blockSize * (p-2), fullPath, p-2);
                    free((*buf)[diskIdx][p-2]);
                    (*buf)[diskIdx][p-2] = NULL;
                } else {
                    LogRecord(INFO, "read %lld bytes form %s for block %d\n", readLen, fullPath, i);
                }
                break;
            }
        }
    }
    return 0;
}


/// @brief 
/// @param path 
/// @param fileName 
/// @param buf 
/// @param bufSize 
/// @param diskNum 
/// @return 
int WriteBlocks(char *path, char* fileName, char ***buf, uint64 blockSize, int diskNum) {
    for(int i=0;i<diskNum;i++) {
        char diskPath[PATH_NAME_LEN];
        sprintf(diskPath, "%s/disk_%d/%s_%d", path, i, fileName, diskNum-2);
        FILE *fp = fopen(diskPath, "w");
        if(fp == NULL) {
            LogRecord(ERROR, "open file %s error", diskPath);
            return -1;
        }
        for(int i= 0;i< diskNum - 2;i++) {
            uint64 cnt = fwrite(buf[i], 1, blockSize, fp);
            LogRecord(INFO, "write %lld bytes to file %s", cnt, diskPath);
        }
        fclose(fp);
    }
    return 0;
}
