#ifndef	__MD5_H
#define	__MD5_H

#include <string>

std::string md5(const std::string &str);

typedef void * MD5CTX;
typedef unsigned char MD5RESULT[16];

MD5CTX InitMD5();
bool UpdateMD5(MD5CTX ctx, const void * buf, size_t size);
bool FinalMD5(MD5CTX ctx, MD5RESULT & result);

std::string MD5ToStr(MD5RESULT & result);
#endif
