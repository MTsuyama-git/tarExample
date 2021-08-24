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

static size_t parseSize(const char* s) {
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

static void dump(int fd, int sfid, size_t nbytes) {
    static char buffer[4096];
    while(nbytes > 0) {
        size_t len = read(sfid, buffer, ((nbytes < 4096) ? nbytes : 4096));
        if(len == 0) {
            fprintf(stderr, "failed to read\n");
            exit(1);
        }
        size_t written_len = write(fd, buffer, len);
        if(written_len != len || written_len == 0) {
            fprintf(stderr, "failed to write\n");
            exit(EXIT_FAILURE);
        }
        nbytes -= written_len;
    }
    return;
}

static void createLongNameSample(void) {
    struct stat __stat;
    mkdir("exceptional", S_ISUID | S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR);   
    mkdir("exceptional/qwertyuiopasdfghjklmnbvcxzqwertyuioplkjhgfdsazxcvbnmqwertyuioplkjhgfdqwertyuiopasdfghjklzxcvqwertyuioppoiuytre", S_ISUID | S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR);   
    if(stat("exceptional/qwertyuiopasdfghjklmnbvcxzqwertyuioplkjhgfdsazxcvbnmqwertyuioplkjhgfdqwertyuiopasdfghjklzxcvqwertyuioppoiuytre/hoge.txt", &__stat) != 0) {
        int fd = open("exceptional/qwertyuiopasdfghjklmnbvcxzqwertyuioplkjhgfdsazxcvbnmqwertyuioplkjhgfdqwertyuiopasdfghjklzxcvqwertyuioppoiuytre/hoge.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (fd == -1) {
            goto err;
        }
        write(fd, "hogehoge\n", 9);
        close(fd);
    }
    else {
        printf("exceptional/qwertyuiopasdfghjklmnbvcxzqwertyuioplkjhgfdsazxcvbnmqwertyuioplkjhgfdqwertyuiopasdfghjklzxcvqwertyuioppoiuytre/hoge.txt is already exists, skip.\n");
    }
    return;
err:
    fprintf(stderr, "Cannot create exceptional/qwertyuiopasdfghjklmnbvcxzqwertyuioplkjhgfdsazxcvbnmqwertyuioplkjhgfdqwertyuiopasdfghjklzxcvqwertyuioppoiuytre/hoge.txt\n");
    exit(EXIT_FAILURE);
}

static void createVarySizeSample(void) {
    struct stat __stat;
    int fid=open("/dev/random", O_RDONLY);
    char *f;
    int fd;
    if (fid == -1) {
        fprintf(stderr, "cannot open: %s\n", "/dev/random");
        exit(EXIT_FAILURE);
    }
    mkdir("sizes", S_ISUID | S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR);
    size_t written = 0;
    size_t nbytes = 256;
    if(stat("sizes/small", &__stat) != 0) {
        printf("Creating... sizes/small\n");
        fd = open("sizes/small", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (fd == -1) {
            f = "sizes/small";
            goto err;
        }
        dump(fd, fid, nbytes);
        close(fd);
        printf("Done.\n");
    }
    else {
        printf("sizes/small is already exists, skip\n");
    }

    if(stat("sizes/middle", &__stat) != 0) {
        printf("Creating... sizes/middle\n");
        fd = open("sizes/middle", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (fd == -1) {
        f = "sizes/middle";
        goto err;
        }
        nbytes = parseSize("256M");
        dump(fd, fid, nbytes);
        close(fd);
        printf("Done.\n");
    }
    else {
        printf("sizes/middle is already exists, skip\n");
    }

    if(stat("sizes/big", &__stat) != 0) {
        printf("Creating... sizes/big\n");
        fd = open("sizes/big", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (fd == -1) {
            f = "sizes/big";
            goto err;
        }
        nbytes = parseSize("1G");
        dump(fd, fid, nbytes);
        close(fd);    
        printf("Done.\n");
    }
    else {
        printf("sizes/big is already exists, skip\n");
    }
    close(fid);
    return;
err:
    fprintf(stderr, "cannot open: %s\n", f);
    exit(EXIT_FAILURE);
}

int main(int argc, const char** argv) {
    createVarySizeSample();
    createLongNameSample();
    return 0;

}

