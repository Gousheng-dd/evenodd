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
int ReadFile(char *path, char* fileName, int *p, char **buf) {
    DIR *dp;
    struct dirent *dirName;
    struct stat buf;

    dp = opendir(path);
    if(!dp) {
        LogRecord(ERROR, "open directory %s error\n", path);
        return NULL;
    }
    while(dirName = readdir(dp)){
        if(strncmp(dirName->d_name, "disk_", 5)) {
            continue;
        }
        
    }
}


/// @brief 
/// @param path 
/// @param fileName 
/// @param buf 
/// @param bufSize 
/// @param p 
/// @return 
int WriteFile(char *path, char* fileName, char **buf, long long bufSize, int p) {
    for(int i=0;i<p;i++) {
        char diskPath[PATH_NAME_LEN];
        getDiskPath(path, fileName, i, diskPath);
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

void getDiskPath(char *path, char* fileName, int idx, char *diskPath) {
    strcpy(diskPath, path);
    strcat(diskPath, "/disk_");
    char diskIdx[10];
    itoa(idx, 10, diskIdx);
    strcat(diskPath, diskIdx);
    strcat(diskPath, fileName);
    return 0;
}
