#include <dlfcn.h>
#include <stdbool.h>
#include "jenv.h"

#define LOG_TAG "jenv"
#define ANDROID_RUNTIME_DSO "libandroid_runtime.so"

typedef jint(*JNI_CreateJavaVM_t)(JavaVM **p_vm, JNIEnv **p_env, void *vm_args);

int init_java_env(JavaCTX *ctx, char **jvm_options, uint8_t jvm_nb_options) {
  JNI_CreateJavaVM_t JNI_CreateJVM;
  JniInvocationImpl* (*JniInvocationCreate)();
  bool (*JniInvocationInit)(JniInvocationImpl*, const char*);
  jint (*registerFrameworkNatives)(JNIEnv*);
  void* runtime_dso;

  ALOGV("[+] Initialize Java environment");

  if ((runtime_dso = dlopen(ANDROID_RUNTIME_DSO, RTLD_NOW)) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  if ((JniInvocationCreate = dlsym(runtime_dso, "JniInvocationCreate")) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  if ((JniInvocationInit = dlsym(runtime_dso, "JniInvocationInit")) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  if ((JNI_CreateJVM = (JNI_CreateJavaVM_t) dlsym(runtime_dso, "JNI_CreateJavaVM")) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  if ((registerFrameworkNatives = dlsym(runtime_dso, "registerFrameworkNatives")) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  ctx->invoc = JniInvocationCreate();
  JniInvocationInit(ctx->invoc, ANDROID_RUNTIME_DSO);

  JavaVMOption options[jvm_nb_options];

  for (int i = 0; i < jvm_nb_options; ++i)
    options[i].optionString = jvm_options[i];

  JavaVMInitArgs args;
  args.version = JNI_VERSION_1_6;
  args.nOptions = jvm_nb_options;
  args.options = options;
  args.ignoreUnrecognized = JNI_TRUE;

  jint status = JNI_CreateJVM(&ctx->vm, &ctx->env, &args);
  if (status == JNI_ERR) return JNI_ERR;

  ALOGD("[d] vm: %p, env: %p\n", ctx->vm, ctx->env);

  status = registerFrameworkNatives(ctx->env);
  if (status == JNI_ERR) return JNI_ERR;

  return JNI_OK;
}

int cleanup_java_env(JavaCTX *ctx) {
  void (*JniInvocationDestroy)(JniInvocationImpl*);
  void* runtime_dso;

  ALOGV("[+] Cleanup Java environment");

  if (ctx == NULL || ctx->vm == NULL) return JNI_ERR;

  if ((runtime_dso = dlopen(ANDROID_RUNTIME_DSO, RTLD_NOW)) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  if ((JniInvocationDestroy = dlsym(runtime_dso, "JniInvocationDestroy")) == NULL) {
    ALOGE("[!] %s\n", dlerror());
    return JNI_ERR;
  }

  (*ctx->vm)->DetachCurrentThread(ctx->vm);
  (*ctx->vm)->DestroyJavaVM(ctx->vm);
  JniInvocationDestroy(ctx->invoc);

  return JNI_OK;
}
