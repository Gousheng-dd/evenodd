#include"log.c"
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

#define PATH_SIZE 255

// 获取当前工作目录
void getCurrentDir(char* currentDir) {
    if (!getcwd(currentDir, PATH_SIZE)) {
        LogRecord(ERROR, "[getcwd] error");
    }
}

// 获取文件状态
void getFileStat(char* filePath, struct stat* fileStat) {
    if (stat(filePath, fileStat) != 0) {
        LogRecord(ERROR,"[stat] error, filePath=%s", filePath);
    }
}

// 检查数据文件夹是否存在，如果不存在则新建文件夹
int checkDisks(int p) {
    char currentDir[PATH_SIZE];
    getCurrentDir(currentDir);
    LogRecord(INFO,"[getCurrentDir] path=%s", currentDir);
    char *diskPath = malloc(strlen(currentDir) + 2);
    for (int i = 0;i < p + 2; i++) {
        // TODO check path under Linux system
        sprintf(diskPath, "%s\\disk_%d", currentDir, i);
        LogRecord(INFO,"[checkDisks] check path=%s", diskPath);
        if ((_access(diskPath, 0) != 0)) {
            if (mkdir(diskPath) == -1) {
                LogRecord(ERROR,"[checkDisks] mkdir error, path=%s", diskPath);
            }
        }
    }
}

// 数据编码并保存
int encode(char* fileContent, int p, int blockSize) {
    LogRecord(DEBUG, "[encode] blockSize=%d", blockSize);
    int byteSize = sizeof(char);
    int blockEleNum = blockSize / (p - 1);
    char*** fileMatrix = malloc(byteSize * blockSize * (p + 2));
    // 这里每一行是一个数据块 注意和图示的行列关系相反
    for (int i = 0;i < p;i++) {
        fileMatrix[i] = malloc(byteSize * blockSize);
        for (int j = 0;j < p - 1;j++) {
            fileMatrix[i][j] = malloc(byteSize * blockEleNum);  
            memcpy(fileMatrix[i][j], fileContent + i * blockSize + j * blockEleNum, blockEleNum);
        }
    }
    for (int i = p;i < p + 2;i++) {
        fileMatrix[i] = malloc(byteSize * blockSize);
        for (int j = 0;j < p - 1;j++) {
            fileMatrix[i][j] = malloc(byteSize * blockEleNum);
        }
    }
    
    for (int j = 0;j < p - 1;j++) {
        char P1 = 0;
        for (int k = 0;k < blockEleNum; k++) {
            for (int i = 0;i < p;i++) {
                P1 = P1 ^ fileMatrix[i][j][k];
            }  
            fileMatrix[p][j][k] = P1;
        }
    }
    for (int i = 0;i < p + 2;i++) {
        for (int j = 0;j < p - 1;j++) {
            LogRecord(DEBUG, "[encode] line %d col %d = %s", i,j, fileMatrix[i][j]);
        }
    }
    char *S = calloc(blockEleNum, byteSize);
    // calculate S
    for (int j = 0;j < p - 2; j++) {
        for (int i = p - 1;i >= 0; i--) {
            for (int k = 0;k < blockEleNum; k++) {
                S[k] = S[k] ^ fileMatrix[j][i][k];
            }
        }
    }
    // calculate each line
    for (int i = p - 2;i >= 0; i--) {
        for (int j = 0;j <= i; j++) {
            for (int k = 0;k < blockEleNum;k++) {
                fileMatrix[p + 1][j][k] = fileMatrix[p + 1][j][k] ^ S[k];
            }
        }
    }


    free(fileMatrix);
}

int readFile(char* filePath, int p) {
    struct stat fileDesc;
    getFileStat(filePath, &fileDesc);
    FILE *fp = fopen(filePath,"r");
    if (fp == NULL) {
        LogRecord(ERROR, "[fopen] error, filePath=%s", filePath);
    }
    char *fileContent = malloc(sizeof(char) * fileDesc.st_size);
    fread(fileContent, 1, fileDesc.st_size, fp);
    fclose(fp);
    // LogRecord(INFO,"[readFile] fileContent = %s", fileContent);
    int blockSize = (sizeof(char) * fileDesc.st_size / p) + 1;
    encode(fileContent, p, blockSize);
}

// for test only
int main() {
    readFile("D:\\Projects\\C\\evenodd\\evenodd.c", 5);
    LogFlush();
}
