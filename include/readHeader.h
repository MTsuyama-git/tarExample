#pragma once

#ifdef __cplusplus
extern "C"{
#endif

void dumpHeader(headerInfo* info);
ret_code_e readHeader(headerInfo* info, int fd);
    ret_code_e writeHeader(int fd, headerInfo* info);
    ret_code_e writeBody(int, int);
    ret_code_e writeFooter(int);

#ifdef __cplusplus
}
#endif
