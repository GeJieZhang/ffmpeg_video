//
// Created by Administrator on 2019/5/5 0005.
//

#include <jni.h>

#ifndef MYFFMPEGAPPLICATION_TEST_H
#define MYFFMPEGAPPLICATION_TEST_H

#endif //MYFFMPEGAPPLICATION_TEST_H

class test{
public:

    test(JNIEnv *env2);
    ~test();

    void main(const char *str, const char *str2);

    void init(const char *str,const char *str2);

public:
    JNIEnv *env;
};