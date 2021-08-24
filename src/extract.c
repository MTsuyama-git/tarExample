#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <tardefs.h>
#include <readHeader.h>
#include <doHeader.h>
#include <binutils.h>

int main(int argc, char** argv) {
    int ret_code = 0;
    const char* prefix = NULL;
    struct stat __stat;

    if(argc < 2) {
        fprintf(stderr, "Usage %s input.tar (dest)\n", argv[0]);
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    fstat(fd, &__stat);
    if((__stat.st_mode & S_IFMT) != S_IFREG) {
        fprintf(stderr, "%s is not a file.\n", argv[1]);
        exit(1);
    }
    if(argc >= 3) {
        prefix = argv[2];
        mkdir(prefix, S_ISUID | S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR);
    }
    if(fd == -1) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        exit(1);
    }
    size_t end = lseek(fd, 0, SEEK_END);
    size_t pos = lseek(fd, 0, SEEK_SET);
    headerInfo* info = (headerInfo*)malloc(sizeof(headerInfo));;
    uint8_t buffer[1024];
    ret_code_e err;
    while(end - pos > 1024) {
        err = readHeader(info, fd);       
        pos = normalize(fd);
        if(err != SUCCESS) {
             continue;
        }
        info->data = (uint8_t*)malloc(sizeof(uint8_t)*info->size);
        read(fd, info->data, info->size);
        doHeader(info, prefix);
        pos = normalize(fd);
    }

end:
    doHeader(NULL, NULL);
    if(info->data) {
        free(info->data);
    }
    free(info);
    close(fd);
    return ret_code;
}


