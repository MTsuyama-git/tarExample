#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include <tardefs.h>
#include <readHeader.h>
#include <doHeader.h>
#include <binutils.h>

struct dirList {
    char** buffer;
    size_t bufferSize;
    size_t used;
};

void setStatInfo(headerInfo* info, struct stat);

void initDirList(struct dirList* lst) {
    lst->buffer = NULL;
    lst->bufferSize = 0;
    lst->used = 0;
}

void addDirList(struct dirList* dir, const char* path) {
    if(dir == NULL)
        return;
    if(dir->bufferSize == dir->used) {
        size_t newBufferSize = dir->bufferSize + 1024;
        char** buffer = (char**)malloc(sizeof(char*) * newBufferSize);
        for(size_t i = 0; i < dir->used; ++i) {
            buffer[i] = dir->buffer[i];
        }
        free(dir->buffer);
        dir->buffer = buffer;
    }
    dir->buffer[dir->used] = (char*)calloc(strlen(path)+1, sizeof(char));
    memcpy(dir->buffer[dir->used++], path, strlen(path));
}

static uint8_t dataBuffer[512];
static void writeLink(int fd, const char* path) {
    static headerInfo info;
    info.data = NULL;
    initHeaderInfo(&info);
    info.uid = 0;
    info.gid = 0;
    memcpy(info.name, "././@LongLink", sizeof("././@LongLink"));
    info.typeflag = LONG_PATH_NAME;
    info.mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    info.size = strlen(path) + 1;
    info.mtime = 0;

    memcpy(info.uname, "root", sizeof("root"));
    memcpy(info.gname, "root", sizeof("root"));
    
    writeHeader(fd, &info);
    write(fd, path, strlen(path)+1);

    // padding
    size_t pos = ftell0(fd);
    size_t remain = (FLOOR(pos, BLOCK_SIZE) * BLOCK_SIZE) - pos;
    memset(dataBuffer, 0, 512);
    write(fd, dataBuffer, remain);
    return;
}

void searchDir(int fd, const char* path) {
    DIR* dir;
    int cfd;
    static char fullPath[2048];
    struct dirent *dp;
    static struct stat __stat;
    struct dirList dirs;
    headerInfo info;
    dir = opendir(path);
    if(dir == NULL) {
        return;
    }

    if(strlen(path) >= 99) {
        // create Long Link
        writeLink(fd, path);
    }

    info.data = NULL;
    stat(path, &__stat);
    initHeaderInfo(&info);
    initDirList(&dirs);
    info.typeflag = DIRECTORY;
    setStatInfo(&info, __stat);
    memcpy(info.magic, "ustar", 5);
    sprintf(fullPath, "%s/", path);
    int c = fullPath[100];
    fullPath[100] = 0;
    memcpy(info.name, fullPath, strlen(fullPath));
    fullPath[100] = c;
    writeHeader(fd, &info);

    dp = readdir(dir);
    while(dp != NULL) {
        if(strcmp(".", dp->d_name) == 0 || strcmp("..", dp->d_name) == 0) {
            dp = readdir(dir);
            continue;
        }
        sprintf(fullPath, "%s/%s", path, dp->d_name);
        if(strlen(fullPath) >= 100) {
            writeLink(fd, fullPath);
        }
        stat(fullPath, &__stat);
        /* printf("%s\n", fullPath); */
        if((__stat.st_mode & S_IFMT) == S_IFDIR) {
            addDirList(&dirs, fullPath);
        }
        else if((__stat.st_mode & S_IFMT) == S_IFREG) {
            initHeaderInfo(&info);
            info.typeflag = COMMON;
            setStatInfo(&info, __stat);
            memcpy(info.magic, "ustar", 5);
            fullPath[100] = 0;
            memcpy(info.name, fullPath, strlen(fullPath));
            fullPath[100] = c;
            cfd = open(fullPath, O_RDONLY);
            info.size = remain(cfd);
            /* printf("size:%lu\n", info.size); */
            writeHeader(fd, &info);
            writeBody(fd, cfd);
            close(cfd);
        }
        dp = readdir(dir);
    }
    if(dir != NULL) {closedir(dir);}
    for(size_t i = 0; i < dirs.used; ++i) {
        searchDir(fd, dirs.buffer[i]);
        free(dirs.buffer[i]);
    }
    free(dirs.buffer);
    
    return;
}

int main(int argc, char** argv) {
    int ret_code = 0;
    int fd, cfd;
    struct stat __stat;
    headerInfo info;
    memset(&info, 0, sizeof(headerInfo));
    initHeaderInfo(&info);
    if(argc < 3) {
        fprintf(stderr, "Usage %s output.tar input\n", argv[0]);
        exit(1);
    }
    fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if(fd == -1) {
        fprintf(stderr, "Cannot create %s\n", argv[1]);
        exit(1);
    }
    stat(argv[2], &__stat);
    setStatInfo(&info, __stat);
    memcpy(info.magic, "ustar", 5);
    memcpy(info.name, argv[2], strlen(argv[2]));
    if((__stat.st_mode & S_IFMT) == S_IFDIR) {
        /* memcpy(info.mode, "0000755", 8); */
        searchDir(fd, argv[2]);
    }
    else if((__stat.st_mode & S_IFMT) == S_IFREG) {
        info.typeflag = COMMON;
        cfd = open(argv[2], O_RDONLY);
        info.size = lseek(cfd, 0, SEEK_END);
        lseek(cfd, 0, SEEK_SET);
        writeHeader(fd, &info);
        writeBody(fd, cfd);
        close(cfd);
    }
end:
    writeFooter(fd);
    close(fd);
    return ret_code;
}

/* static int octToDec(int octal) { */
/*     int result = 0; */
/*     while(octal > 0) { */
/*         result *= 8; */
/*         result += octal % 10; */
/*         octal /= 10; */
/*     } */
/*     return result; */
/* } */

void setStatInfo(headerInfo* info, struct stat __stat) {
    struct passwd* pwd;
    struct group * gwd;

    info->uid = __stat.st_uid;
    info->gid = __stat.st_gid;
    info->mode = (__stat.st_mode & (S_IRWXU | S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRGRP | S_IWGRP | S_IXGRP | S_IRWXO | S_IROTH | S_IWOTH | S_IXOTH));
    info->size = 0;
    info->mtime = __stat.st_mtime;
    pwd = getpwuid(info->uid);
    if(pwd != NULL){
        strcpy(info->uname, pwd->pw_name);
    }
    gwd = getgrgid(info->gid);
    if(gwd != NULL) {
        strcpy(info->gname, gwd->gr_name);
    }
    return;
}
