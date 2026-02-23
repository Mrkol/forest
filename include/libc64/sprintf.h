#ifndef SPRINTF_H
#define SPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TARGET_PC
int sprintf(char* dst, const char* fmt, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif