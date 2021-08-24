#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
#include <sys/types.h>

#define NAME_LEN (100)
#define MODE_LEN (8)
#define GID_LEN (8)
#define UID_LEN (8)
#define SIZE_LEN (12)
#define MTIME_LEN (12)
#define CHECKSUM_LEN (8)
#define TYPEFLAG_LEN (1)
#define LINKNAME_LEN (100)
#define MAGIC_LEN (6)
#define VERSION_LEN (2)
#define UNAME_LEN (32)
#define GNAME_LEN (32)
#define DEVMAJOR_LEN (8)
#define DEVMINOR_LEN (8)
#define PREFIX_LEN (155)
#define HEADER_SIZE (512)

#define BLOCK_SIZE (512)


#define MIN(a, b)\
    (((a) <= (b)) ? a : b)

#define FLOOR(a, b)\
    ((a) / (b) + (((a)%(b))? 1 : 0))

typedef enum _ret_code {
    SUCCESS = 0,
    EMPTY_BLOCK,
    NAME_ERROR,
    MODE_ERROR,
    UID_ERROR,
    GID_ERROR,
    SIZE_ERROR,
    MTIME_ERROR,
    CHECKSUM_READ_ERROR,
    CHECKSUM_ERROR,
    TYPEFLAG_ERROR,
    LINKNAME_ERROR,
    MAGIC_ERROR,
    VERSION_ERROR,
    UNAME_ERROR,
    GNAME_ERROR,
    DEVMAJOR_ERROR,
    DEVMINOR_ERROR,
    PREFIX_ERROR,
    FILEOPEN_ERROR,
    UNKNOWN_TYPE_FLAG,
} ret_code_e;

    typedef enum _typeflag {
        COMMON = 0,
        HARD_LINK = 1,
        SYM_LINK = 2,
        CHAR_DEV_NODE = 3,
        BLOCK_DEV_NODE = 4,
        DIRECTORY = 5,
        FIFO = 6,
        RESERVED = 7,
        DIRECTORY_ENTRY = 8,
        LONG_LINK_NAME = 9,
        LONG_PATH_NAME = 10,
        MULTIPLE = 11,
        NAME = 12, // DEPRECATED
        SPARSE = 13,
        VALUE = 14, // IGNORED
        UNKNOWN = 15
    } typeflag_e;

typedef struct _headerInfo {
    char name[NAME_LEN+1];
    int mode;
    int uid;
    int gid;
    size_t size;
    size_t mtime;
    uint8_t checksum[CHECKSUM_LEN+1];
    typeflag_e typeflag;
    char linkname[LINKNAME_LEN+1];
    char magic[MAGIC_LEN+1];
    uint8_t version[VERSION_LEN+1];
    char uname[UNAME_LEN+1];
    char gname[GNAME_LEN+1];
    char devmajor[DEVMAJOR_LEN+1];
    char devminor[DEVMINOR_LEN+1];
    char prefix[PREFIX_LEN+1];
    uint8_t* data;
}headerInfo;

#ifdef __cplusplus
}
#endif
