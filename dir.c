#include"log.c"
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

#define PATH_SIZE 255

void getCurrentDir(char* currentDir) {
    if (!getcwd(currentDir, PATH_SIZE)) {
        LogRecord(ERROR, "[getcwd] error");
    }
}

int CheckDisks(int p) {
    char currentDir[PATH_SIZE];
    getCurrentDir(currentDir);
    LogRecord(INFO,"[getCurrentDir] path=%s", currentDir);
    char *diskPath = malloc(strlen(currentDir) + 2);
    for (int i = 0;i < p + 2; i++) {
        // TODO check path under Linux system
        sprintf(diskPath, "%s\\disk_%d", currentDir, i);
        LogRecord(INFO,"[CheckDisks] check path=%s", diskPath);
        if ((_access(diskPath, 0) != 0)) {
            if (mkdir(diskPath) == -1) {
                LogRecord(ERROR,"[CheckDisks] mkdir error, path=%s", diskPath);
            }
        }
    }
}