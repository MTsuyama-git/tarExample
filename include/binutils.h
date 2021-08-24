#pragma once
#ifdef __cplusplus
extern "C"{
#endif

    size_t normalize(int fd);
    size_t remain(int fd);
    size_t ftell0(int fd);
    void   initHeaderInfo(headerInfo*);
    void copyHeaderInfo(headerInfo* dest, const headerInfo* info);

#ifdef __cplusplus
}
#endif
