#include <jni.h>
#include <string>


#include "android/log.h"


extern "C" {

#include "ffmpeg.h"
}




extern "C"
JNIEXPORT jint JNICALL
Java_com_m4399_ffmpeg_1cmd_FFmpegKit_run(JNIEnv *env, jclass type, jobjectArray commands) {

    int argc = env->GetArrayLength(commands);
    char *argv[argc];

    __android_log_print(ANDROID_LOG_ERROR, "Kit", "argc %d\n", argc);
    int i;
    for (i = 0; i < argc; i++) {
        jstring js = (jstring) env->GetObjectArrayElement(commands, i);
        argv[i] = (char *) env->GetStringUTFChars(js, 0);
        __android_log_print(ANDROID_LOG_ERROR, "Kit", "argv %s\n", argv[i]);
    }


    return run(argc, argv);
    //return 0;
}