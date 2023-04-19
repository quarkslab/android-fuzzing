#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "jenv.h"

#define BUFFER_SIZE 256

extern jobject Java_qb_blogfuzz_NativeHelper_fuzzMeWrapper(JNIEnv*, jclass,
                                                           jbyteArray);

static JavaCTX ctx;

void fuzz_one_input(const uint8_t* buffer, size_t length) {
  jbyteArray jBuffer = (*ctx.env)->NewByteArray(ctx.env, length);
  (*ctx.env)->SetByteArrayRegion(ctx.env, jBuffer, 0, length,
                                 (const jbyte*)buffer);

  jclass wrapperClass = (*ctx.env)->FindClass(ctx.env, "qb/blogfuzz/Wrapper");
  jmethodID wrapperCtor =
      (*ctx.env)->GetMethodID(ctx.env, wrapperClass, "<init>", "([B)V");
  jobject objWrapper =
      (*ctx.env)->NewObject(ctx.env, wrapperClass, wrapperCtor, jBuffer);

  Java_qb_blogfuzz_NativeHelper_fuzzMeWrapper(ctx.env, NULL, objWrapper);

  (*ctx.env)->DeleteLocalRef(ctx.env, objWrapper);
  (*ctx.env)->DeleteLocalRef(ctx.env, jBuffer);
}

int main(void) {
  int status;
  const uint8_t buffer[BUFFER_SIZE];
  char* options = "-Djava.class.path=/data/local/tmp/mock.dex";

  ssize_t rlength = fread((void*)buffer, 1, BUFFER_SIZE, stdin);
  if (rlength == -1) return errno;

  if ((status = init_java_env(&ctx, &options, 1)) != 0) {
    return status;
  }

  fuzz_one_input(buffer, rlength);

  return 0;
}
