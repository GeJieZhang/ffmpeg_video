//cpp.cpp
#include "change_code.h"
#include "test.h"

int Add(const char *str, const char *str2,JNIEnv *env)
{
    test *te = new test(env);
    te->main(str, str2);

    te = NULL;
    delete te;
    return 0;
}
