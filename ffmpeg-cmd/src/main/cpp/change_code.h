//cpp.h
#ifndef __CPP_H__
#define __CPP_H__


#include <jni.h>



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int Add(const char *str, const char *str2,JNIEnv *env);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
