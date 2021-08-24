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

static headerInfo __info = {0};
static int __fd = 0;

static char  pathName[2048];
static char  fileName[4096];

static void initInfo(void);
static void copyHeader(headerInfo* info);

ret_code_e doHeader(headerInfo* info, const char* prefix) {
    ret_code_e ret_code = SUCCESS;
    if(info == NULL) {
        if(__fd != -1) {
            close(__fd);
        }
        return ret_code;
    }
    
    if(__info.typeflag == LONG_PATH_NAME || __info.typeflag == LONG_LINK_NAME) {
        memcpy(pathName, __info.data, 2048);
    }
    else if(__info.typeflag == COMMON && info->typeflag == MULTIPLE) {
        memcpy(pathName, __info.name, NAME_LEN);
    }
    else {
        if(__info.typeflag == COMMON) {
            close(__fd);
        }
        memcpy(pathName, info->name, NAME_LEN);
        if(__info.typeflag != MULTIPLE) {
            initInfo();
        }
    }
    if(info->typeflag == COMMON) {
        if(prefix != NULL) {
            sprintf(fileName, "%s/%s", prefix, pathName);
        }
        else {
            sprintf(fileName, "%s", pathName);
        }
        __fd = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if(__fd == -1) {
            fprintf(stderr, "cannot open: %s\n", fileName);
            ret_code = FILEOPEN_ERROR;
            exit(1);
        }
        if(info->size && info->data) {
            write(__fd, info->data, info->size);
        }
        copyHeader(info);
    }
    else if(info->typeflag == HARD_LINK) {

    }
    else if(info->typeflag == SYM_LINK) {

    }
    else if(info->typeflag == CHAR_DEV_NODE) {

    }
    else if(info->typeflag == BLOCK_DEV_NODE) {

    }
    else if(info->typeflag == DIRECTORY) {
        if(prefix != NULL) {
            sprintf(fileName, "%s/%s", prefix, pathName);
        }
        else {
            sprintf(fileName, "%s", pathName);
        }
        mkdir(fileName, S_ISUID | S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR);
    }
    else if(info->typeflag == FIFO) {

    }
    else if(info->typeflag == RESERVED) {

    }
    else if(info->typeflag == DIRECTORY_ENTRY) {

    }
    else if(info->typeflag == LONG_LINK_NAME) {
        copyHeader(info);
    }
    else if(info->typeflag == LONG_PATH_NAME) {
        copyHeader(info);
    }
    else if(info->typeflag == MULTIPLE) {
        if(info->size && info->data) {
            write(__fd, info->data, info->size);
        }
    }
    else if(info->typeflag == NAME) {

    }
    else if(info->typeflag == SPARSE) {

    }
    else if(info->typeflag == VALUE) {

    }
    else {
        ret_code = UNKNOWN_TYPE_FLAG;
    }    
    return ret_code;
}


static void initInfo(void) {
    initHeaderInfo(&__info);
}


static void copyHeader(headerInfo* info) {
    initInfo();
    copyHeaderInfo(&__info, info);
}

