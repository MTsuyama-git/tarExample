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
#include <binutils.h>

int main(int argc, char** argv) {
    int ret_code = 0;

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        exit(1);
    }

    size_t end = lseek(fd, 0, SEEK_END);
    printf("block_size:%lu\n", end/BLOCK_SIZE);
    size_t pos = lseek(fd, 0, SEEK_SET);
    headerInfo* info = (headerInfo*)malloc(sizeof(headerInfo));;
    uint8_t buffer[1024];
    ret_code_e err;
    while(end - pos > 1024) {
        err = readHeader(info, fd);       
        pos = normalize(fd);
        if(err != SUCCESS) {
            fprintf(stderr,"empty block\n");
            continue;
        }
        printf("pos: %lu/%lu\n", pos, end);
        dumpHeader(info);
        if(info->size > 0) {
            if(end - pos < info->size) {
                fprintf(stderr, "invalid end of file\n");
                ret_code = EXIT_FAILURE;
                goto end;
            }
            info->data = (uint8_t*)malloc(info->size);
            read(fd, info->data, info->size);
            printf("%s", info->data);
            pos = normalize(fd);
        }
    }

end:
    free(info);
    close(fd);
    return ret_code;
}


