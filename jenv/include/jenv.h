#include <android/log.h>
#include <jni.h>

/* JniInvocationImpl struct (from libnativehelper/JniInvocation.c) */
typedef struct JniInvocation {
    const char* jni_provider_library_name;
    void* jni_provider_library;
    jint (*JNI_GetDefaultJavaVMInitArgs)(void*);
    jint (*JNI_CreateJavaVM)(JavaVM**, JNIEnv**, void*);
    jint (*JNI_GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);
} JniInvocationImpl;

/* Log facilities */
#define ALOGV(...) \
  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/* CTX */
typedef struct JavaContext {
  JavaVM* vm;
  JNIEnv* env;
  JniInvocationImpl* invoc;
} JavaCTX;

/* API */
int init_java_env(JavaCTX*, char**, uint8_t);
int cleanup_java_env(JavaCTX*);
