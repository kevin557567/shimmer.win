#ifndef _BASE64_H_
#define _BASE64_H_

#pragma once

namespace Util {
namespace BASE64 {

char* base64_encode(const char *data, int dlen);
char* base64_encode(const char *data);
char* base64_decode(const char *bdata, int bdlen);
char* base64_decode(const char *bdata);
char* base64_decode(const char *bdata, int *outbdlen);

// 新增的base64编解码函数
int ToBase64(void* pSrc,int nSrcLen, char* strBase64, int* nBase64Len);
int FromBase64(const char* strSrc, int nSrcLen, void* pDest, int* nDestLen);
char* ToUnicodeAndBase64(char* source, int* destlen);
char* FromUnicodeAndBase64(char* source, int* destlen);


} // end namespace BASE64
} // end namespace Util

#endif
