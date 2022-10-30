#include "file.c"

int testReadBlocks() {
    int p;
    char ***buf;
    ReadBlocks(".", "testfile", &p, &buf);
    printf("\n---UNIT TEST----\n:get p:%d\n files:", p);
    for(int i=0;i<p+2;i++) {
        if(buf[i]!= NULL) {
            for(int j = 0;j<p-1;j++) {
                if(buf[i][j]) {
                    printf("\nfile %d:%s",i , buf[i]);
                } else {
                    printf("\nfile %d block %d:NULL", i, j);
                }
            }
        } else {
            printf("\nfile %d:NULL", i);
        }
    }
    return 0;
}

void testWriteBlocks() {
    int p = 3
    char ***buf = (char ***) malloc(sizeof(char **) * (p + 2));
    for(int i = 0;i<p+2;i++) {
        buf[i] = (char **)malloc(sizeof(char *) * (p-1));
        for(int j=0;j<p-1;j++) {
            buf[i][j] = malloc(sizeof(char) * 20);
            sprintf(buf[i][j], "disk%d,block%d", i, j);
        }
    }
    WriteBlocks(".", "writefile", buf, strlen(buf[0][0]), 3);
    return ;
}

int main() {
    // testReadBlocks();
    testWriteBlocks();
}