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

#define DATA_BUFFER_SIZE 2048

static char strBuffer[256];
static uint8_t dataBuffer[DATA_BUFFER_SIZE];

static size_t parseOctal(char* str);
static void dump(void*, ssize_t);
static const char* flag2Str(typeflag_e);
static char* parseAsOctal(int);
static char* parseAsOctal12(size_t);
static const char* parseAsTypeFlag(typeflag_e);
static size_t calcCheckSum(uint8_t*);
static char* parseAsCheckSum(size_t);

void dumpHeader(headerInfo* info) {
    printf("name:%s\n", info->name);
    printf("mode:%s\n", parseAsOctal(info->mode));
    printf("uid:%d\n", info->uid);
    printf("gid:%d\n", info->gid);
    printf("size:%lu\n", info->size);
    printf("mtime:%lu\n", info->mtime);
    printf("checksum:");
    dump(info->checksum, CHECKSUM_LEN);
    printf("typeflag:%s\n", flag2Str(info->typeflag));
    printf("linkname:%s\n", info->linkname);
    printf("magic:%s\n", info->magic);
    if(memcmp(info->magic, "ustar", 5) == 0) {
        goto ustar;
    }
    goto end;
ustar:
    printf("version:");
    dump(info->version, VERSION_LEN);
    printf("uname:%s\n", info->uname);
    printf("gname:%s\n", info->gname);
    printf("devmajor:");
    dump(info->devmajor, DEVMAJOR_LEN);
    printf("devminor:");
    dump(info->devminor, DEVMINOR_LEN);
    printf("prefix:");
    dump(info->prefix, PREFIX_LEN);
end:
    return;

}

ret_code_e writeFooter(int fd) {
    memset(dataBuffer, 0, 2048);
    write(fd, dataBuffer, 2048);
    /* for(int i = 0; i < 4; ++i) { */
    /* } */
    return SUCCESS;
}

ret_code_e writeBody(int fd, int __fd) {
    size_t readlen;
    while((readlen = read(__fd, dataBuffer, DATA_BUFFER_SIZE)) > 0) {
        write(fd, dataBuffer, readlen);
    }

    // padding
    size_t pos = ftell0(fd);
    size_t remain = (FLOOR(pos, BLOCK_SIZE) * BLOCK_SIZE) - pos;
    memset(dataBuffer, 0, 512);
    write(fd, dataBuffer, remain);
}

ret_code_e writeHeader(int fd, headerInfo* info) {
    ret_code_e ret = SUCCESS;
    size_t dataSize;
    memset(dataBuffer,0, 512);
    memcpy(dataBuffer, info->name, NAME_LEN);
    memcpy(dataBuffer + NAME_LEN, parseAsOctal(info->mode), MODE_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN, parseAsOctal(info->uid), UID_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN, parseAsOctal(info->gid), GID_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN, parseAsOctal12(info->size), SIZE_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN, parseAsOctal12(info->mtime), MTIME_LEN);
    // skip checksum
    memset(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN, 0x20, CHECKSUM_LEN);

    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN, parseAsTypeFlag(info->typeflag), TYPEFLAG_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN , info->linkname, LINKNAME_LEN);
    if(strcmp(info->magic, "ustar")) {
        goto calc;

    }
    info->magic[5] = 0x20;
    info->magic[6] = 0x20;
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN , info->magic, MAGIC_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN + MAGIC_LEN, info->version, VERSION_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN + MAGIC_LEN + VERSION_LEN, info->uname, UNAME_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN + MAGIC_LEN + VERSION_LEN + UNAME_LEN, info->gname, GNAME_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN + MAGIC_LEN + VERSION_LEN + UNAME_LEN + GNAME_LEN, info->devmajor, DEVMAJOR_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN + MAGIC_LEN + VERSION_LEN + UNAME_LEN + GNAME_LEN + DEVMAJOR_LEN, info->devminor, DEVMINOR_LEN);
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN + CHECKSUM_LEN + TYPEFLAG_LEN + LINKNAME_LEN + MAGIC_LEN + VERSION_LEN + UNAME_LEN + GNAME_LEN + DEVMAJOR_LEN + DEVMINOR_LEN, info->prefix, PREFIX_LEN);
calc:
    // add checksum
    memcpy(dataBuffer + NAME_LEN + MODE_LEN + UID_LEN + GID_LEN + SIZE_LEN + MTIME_LEN, parseAsCheckSum(calcCheckSum(dataBuffer)), CHECKSUM_LEN);
    write(fd, dataBuffer, 512);
    /* dataSize = info->size; */
    /* uint8_t *ptr = info->data; */
    /* while(dataSize > 0) { */
    /*     memset(dataBuffer, 0, 512); */
    /*     memcpy(dataBuffer, ptr, ((512 < dataSize)? 512 : dataSize)); */
    /*     size_t written = write(fd, dataBuffer, ((512 < dataSize)? 512 : dataSize)); */
    /*     if(written == 0) { */
    /*         fprintf(stderr, "failed to write\n"); */
    /*         exit(1); */
    /*     } */
    /*     dataSize -= written; */
    /* } */
    return ret;
}

static size_t calcCheckSum(uint8_t* data) {
    size_t result = 0;
    for(size_t i = 0; i < 512; ++i) {
        result += data[i];
    }
    return result;
}

static char* parseAsCheckSum(size_t checkSum) {
    for(int i = 0; i < 6; ++i) {
        strBuffer[i] = '0' + ((checkSum >> ((5-i) * 3)) & 0x7);
    }
    strBuffer[6] = 0x00;
    strBuffer[7] = 0x20;
    return strBuffer;
}

static char* parseAsOctal(int mode) {
    for(int i = 0; i < 7; ++i) {
        strBuffer[i] = '0' + ((mode >> ((6-i) * 3)) & 0x7);
    }
    strBuffer[7] = 0;
    return strBuffer;
}

static char* parseAsOctal12(size_t size) {
    for(int i = 0; i < 11; ++i) {
        strBuffer[i] = '0' + ((size >> ((10-i) * 3)) & 0x7);
    }
    strBuffer[11] = 0;
    return strBuffer;
}


ret_code_e readHeader(headerInfo* info, int fd) {
    static ret_code_e ret_code;
    static uint8_t buffer[256];
    static size_t len;

    ret_code = SUCCESS;
    memset(buffer, 0, sizeof(buffer));
    if(info->data) {
        free(info->data);
        info->data = NULL;
    }
    initHeaderInfo(info);
    len = read(fd, buffer, NAME_LEN);
    if(len < NAME_LEN) {
        ret_code = NAME_ERROR;
        goto end;
    }
    if(strlen(buffer) == 0) {
        // empty block
        ret_code = EMPTY_BLOCK;
        goto end;
    }
    memcpy(info->name, buffer, len);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, MODE_LEN);
    if(len < MODE_LEN) {
        ret_code = MODE_ERROR;
        goto end;
    }
    info->mode = parseOctal(buffer);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, UID_LEN);
    if(len < UID_LEN) {
        ret_code = UID_ERROR;
        goto end;
    }
    info->uid = parseOctal(buffer);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, GID_LEN);
    if(len < GID_LEN) {
        ret_code = GID_ERROR;
        goto end;
    }
    info->gid = parseOctal(buffer);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, SIZE_LEN);
    if(len < SIZE_LEN) {
        ret_code = SIZE_ERROR;
        goto end;
    }
    info->size = parseOctal(buffer);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, MTIME_LEN);
    if(len < MTIME_LEN) {
        ret_code = MTIME_ERROR;
        goto end;
    }
    info->mtime = parseOctal(buffer);


    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, CHECKSUM_LEN);
    if(len < CHECKSUM_LEN) {
        ret_code = CHECKSUM_READ_ERROR;
        goto end;
    }
    memcpy(info->checksum, buffer, len);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, TYPEFLAG_LEN);
    if(len < TYPEFLAG_LEN) {
        ret_code = TYPEFLAG_ERROR;
        goto end;
    }
    if('0' <= buffer[0] && buffer[0] <= '7') 
        info->typeflag = buffer[0] - '0';
    else if(buffer[0] == 'D')
        info->typeflag = DIRECTORY_ENTRY;
    else if(buffer[0] == 'K')
        info->typeflag = LONG_LINK_NAME;
    else if(buffer[0] == 'L')
        info->typeflag = LONG_PATH_NAME;
    else if(buffer[0] == 'M')
        info->typeflag = MULTIPLE;
    else if(buffer[0] == 'N')
        info->typeflag = NAME;
    else if(buffer[0] == 'S')
        info->typeflag = SPARSE;
    else if(buffer[0] == 'V')
        info->typeflag = VALUE;
    else
        info->typeflag = UNKNOWN;
    

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, LINKNAME_LEN);
    if(len < LINKNAME_LEN) {
        ret_code = LINKNAME_ERROR;
        goto end;
    }
    memcpy(info->linkname, buffer, len);

    size_t offset = lseek(fd, 0, SEEK_CUR);
    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, MAGIC_LEN);
    if(len < MAGIC_LEN) {
        ret_code = MAGIC_ERROR;
        goto end;
    }
    memcpy(info->magic, buffer, len);
    if(memcmp(info->magic, "ustar", 5) != 0) {
        lseek(fd, offset, SEEK_SET);
        goto end;
    }

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, VERSION_LEN);
    if(len < VERSION_LEN) {
        ret_code = VERSION_ERROR;
        goto end;
    }
    memcpy(info->version, buffer, len);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, UNAME_LEN);
    if(len < UNAME_LEN) {
        ret_code = UNAME_ERROR;
        goto end;
    }
    memcpy(info->uname, buffer, len);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, GNAME_LEN);
    if(len < GNAME_LEN) {
        ret_code = GNAME_ERROR;
        goto end;
    }
    memcpy(info->gname, buffer, len);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, DEVMAJOR_LEN);
    if(len < DEVMAJOR_LEN) {
        ret_code = DEVMAJOR_ERROR;
        goto end;
    }
    memcpy(info->devmajor, buffer, len);


    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, DEVMINOR_LEN);
    if(len < DEVMINOR_LEN) {
        ret_code = DEVMINOR_ERROR;
        goto end;
    }
    memcpy(info->devminor, buffer, len);

    memset(buffer, 0, sizeof(buffer));
    len = read(fd, buffer, PREFIX_LEN);
    if(len < PREFIX_LEN) {
        ret_code = PREFIX_ERROR;
        goto end;
    }
    memcpy(info->prefix, buffer, len);

end:
    return ret_code;
}

static size_t parseOctal(char* str) {
    size_t result = 0;
    while(*str) {
        result *= 8;
        result += (*str - '0');
        str++;
    }
    return result;
}

static void dump(void* buf, ssize_t len) {
    char* c = (char*)buf;

    for(ssize_t i = 0; i < len; ++i) {
        printf("%02X ", c[i]);
    }
    printf("\n");
    return;
}

static const char* flag2Str(typeflag_e flag) {
    if(flag == COMMON) {
        return "common";
    }
    else if(flag == HARD_LINK) {
        return "hard link";
    }
    else if(flag == SYM_LINK) {
        return "symbolic link";
    }
    else if(flag == CHAR_DEV_NODE) {
        return "Character Type Device Node";
    }
    else if(flag == BLOCK_DEV_NODE) {
        return "Block Type Device Node";
    }
    else if(flag == DIRECTORY) {
        return "Directory";
    }
    else if(flag == FIFO) {
        return "FIFO";
    }
    else if(flag == RESERVED) {
        return "Reserved";
    }
    else if(flag == DIRECTORY_ENTRY) {
        return "Directory Entry";
    }
    else if(flag == LONG_LINK_NAME) {
        return "Long Link Name";
    }
    else if(flag == LONG_PATH_NAME) {
        return "Long Path Name";
    }
    else if(flag == MULTIPLE) {
        return "Multiple";
    }
    else if(flag == NAME) {
        return "Name";
    }
    else if(flag == SPARSE) {
        return "Sparse";
    }
    else if(flag == VALUE) {
        return "Value";
    }
    else {
        return "";
    }
}

static const char* parseAsTypeFlag(typeflag_e flag) {
    if(flag == COMMON) {
        return "0";
    }
    else if(flag == HARD_LINK) {
        return "1";
    }
    else if(flag == SYM_LINK) {
        return "2";
    }
    else if(flag == CHAR_DEV_NODE) {
        return "3";
    }
    else if(flag == BLOCK_DEV_NODE) {
        return "4";
    }
    else if(flag == DIRECTORY) {
        return "5";
    }
    else if(flag == FIFO) {
        return "6";
    }
    else if(flag == RESERVED) {
        return "7";
    }
    else if(flag == DIRECTORY_ENTRY) {
        return "D";
    }
    else if(flag == LONG_LINK_NAME) {
        return "K";
    }
    else if(flag == LONG_PATH_NAME) {
        return "L";
    }
    else if(flag == MULTIPLE) {
        return "M";
    }
    else if(flag == NAME) {
        return "N";
    }
    else if(flag == SPARSE) {
        return "S";
    }
    else if(flag == VALUE) {
        return "V";
    }
    else {
        return "U";
    }
}
