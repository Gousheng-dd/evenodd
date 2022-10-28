#include "file.c"

int testReadBlocks() {
    int p;
    char **buf;
    ReadBlocks(".", "testfile", &p, &buf);
    printf("\n---UNIT TEST----\n:get p:%d\n files:", p);
    for(int i=0;i<p+2;i++) {
        if(buf[i]!= NULL) {
            printf("\nfile %d:%s",i , buf[i]);
        } else {
            printf("\nfile %d:NULL", i);
        }
    }
    return 0;
}

void testWriteBlocks() {
    char **buf = (char **) malloc(3);
    buf[0] = "change!disk0\0";
    buf[1] = "change!disk1\0";
    buf[2] = "change!disk2\0";
    WriteBlocks(".", "writefile", buf, strlen(buf[0]), 3);
    return ;
}

int main() {
    // testReadBlocks();
    testWriteBlocks();
}