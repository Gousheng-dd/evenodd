#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "log.c"

#define PATH_NAME_LEN 100

/// @brief read file from emulated disks
/// @param path emulated disks path
/// @param fileName 待读取文件名
/// @param p 保存根据包含 fileName 的 disks 个数推断出的质数 p
/// @return 指向读取结果 char[p][block_size] 的指针
int ReadBlocks(char *path, char* fileName, int *p, char ***buf) {
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
                    *buf = (char **)malloc(sizeof(char *) * (*p+2));
                }
                // read file
                sprintf(fullPath, "%s/%s", fullPath, fName->d_name);
                stat(fullPath, &stBuf);
                (*buf)[diskIdx] = (char *)malloc(sizeof(char) * stBuf.st_size);
                fp = fopen(fullPath, "r");
                long long readLen = fread((*buf)[diskIdx], sizeof(char), stBuf.st_size, fp);
                if(!readLen) {
                    LogRecord(WARN, "open file %s error\n", fullPath);
                    free((*buf)[diskIdx]);
                    (*buf)[diskIdx] = NULL;
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
int WriteBlocks(char *path, char* fileName, char **buf, long long bufSize, int diskNum) {
    for(int i=0;i<diskNum;i++) {
        char diskPath[PATH_NAME_LEN];
        sprintf(diskPath, "%s/disk_%d/%s_%d", path, i, fileName, diskNum-2);
        FILE *fp = fopen(diskPath, "w");
        if(fp == NULL) {
            LogRecord(ERROR, "open file %s error", diskPath);
            return -1;
        }
        long long cnt = fwrite(buf[i], 1, bufSize, fp);
        LogRecord(INFO, "write %lld bytes to file %s", cnt, diskPath);
        fclose(fp);
    }
    return 0;
}
