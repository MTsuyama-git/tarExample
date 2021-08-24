#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <tardefs.h>
#include <binutils.h>
#include <tardefs.h>

size_t ftell0(int fd) {
    return lseek(fd, 0, SEEK_CUR);
}

size_t normalize(int fd) {
    size_t pos = ftell0(fd);
    size_t remain = (FLOOR(pos, BLOCK_SIZE) * BLOCK_SIZE) - pos;
    return lseek(fd, remain, SEEK_CUR);
}


size_t remain(int fd) {
    size_t pos = ftell0(fd);
    size_t result = lseek(fd, 0, SEEK_END) - pos;
    lseek(fd, pos, SEEK_SET);
    return result;
}

void initHeaderInfo(headerInfo* info) {
    if(info->data != NULL) {
        free(info->data);
        info->data = NULL;
    }
    memset(info->name, 0, NAME_LEN+1);
    info->mode = 0;
    info->uid = 0;
    info->gid = 0;
    info->size = 0;
    info->mtime = 0;
    memset(info->checksum, 0, CHECKSUM_LEN+1);
    info->typeflag = UNKNOWN;
    memset(info->linkname, 0, LINKNAME_LEN+1);
    memset(info->magic, 0, MAGIC_LEN+1);
    info->version[0] = 0x20;
    info->version[1] = 0x00;
    info->version[2] = 0x00;
    memset(info->uname, 0, UNAME_LEN+1);
    memset(info->gname, 0, GNAME_LEN+1);
    memset(info->devmajor, 0, DEVMAJOR_LEN+1);
    memset(info->devminor, 0, DEVMINOR_LEN+1);
    memset(info->prefix, 0, PREFIX_LEN+1);
}


void copyHeaderInfo(headerInfo* dest, const headerInfo* info) {
    initHeaderInfo(dest);
    if(info->data != NULL) {
        dest->data = (uint8_t*)malloc(sizeof(uint8_t) * info->size);
        memcpy(dest->data, info->data, info->size);
    }
    memcpy(dest->name, info->name, NAME_LEN+1);
    dest->mode = info->mode;
    dest->uid = 0;
    dest->gid = 0;
    dest->size = 0;
    dest->mtime = 0;
    memcpy(dest->checksum, info->checksum, CHECKSUM_LEN+1);
    dest->typeflag = info->typeflag;
    memcpy(dest->linkname, info->linkname, LINKNAME_LEN+1);
    memcpy(dest->magic, info->magic, MAGIC_LEN+1);
    memcpy(dest->version, info->version, VERSION_LEN+1);
    memcpy(dest->uname, info->uname, UNAME_LEN+1);
    memcpy(dest->gname, info->gname, GNAME_LEN+1);
    memcpy(dest->devmajor, info->devmajor, DEVMAJOR_LEN+1);
    memcpy(dest->devminor, info->devminor, DEVMINOR_LEN+1);
    memcpy(dest->prefix, info->prefix, PREFIX_LEN+1);
}
