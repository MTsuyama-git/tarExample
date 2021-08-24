#include <stdio.h>
#include <doHeader.h>
#include <binutils.h>
#include <tardefs.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

size_t parseSize(const char* s) {
    const char* p = s;
    size_t result = 0;
    while('0' <= *p && *p <= '9') {
        result *= 10;
        result += *p - '0';
        p++;
    }
    if(*p == 'K')
        result *= 1024;
    else if(*p == 'M')
        result *= 1024 * 1024;
    else if(*p == 'G')
        result *= 1024 * 1024 * 1024;
    return result;
}

int main(int argc, const char** argv) {
    size_t nbytes = 256;
    char buffer[4096];
    if(argc < 2) {
        fprintf(stderr, "Usage: %s dest (size)\n", argv[0]);
        exit(1);
    }
    if(argc >= 3) {
        nbytes = parseSize(argv[2]);
    }

    int fid=open("/dev/random", O_RDONLY);
    if (fid == -1) {
        fprintf(stderr, "cannot open: %s\n", "/dev/random");
        return fid;
    }
    size_t written = 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd == -1) {
        fprintf(stderr, "cannot open: %s\n", argv[1]);
        return fd;
    }
    while(nbytes > 0) {
        size_t len = read(fid, buffer, ((nbytes < 4096) ? nbytes : 4096));
        if(len == 0) {
            fprintf(stderr, "failed to read\n");
            exit(1);
        }
        size_t written_len = write(fd, buffer, len);
        if(written_len != len || written_len == 0) {
            fprintf(stderr, "failed to write\n");
            return 1;
        }
        nbytes -= written_len;
    }

    close(fd);
    close(fid);
    return 0;

}

