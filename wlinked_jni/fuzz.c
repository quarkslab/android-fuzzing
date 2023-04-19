#include <errno.h>
#include <stdio.h>

#include "jenv.h"

#define BUFFER_SIZE 256

/* Target function */
extern jobject Java_qb_blogfuzz_NativeHelper_fuzzMeArray(JNIEnv *, jclass,
                                                         jbyteArray);

/* Java context */
static JavaCTX ctx;

/* Persistent loop */
void fuzz_one_input(const uint8_t *buffer, size_t length) {
  jbyteArray jBuffer = (*ctx.env)->NewByteArray(ctx.env, length);
  (*ctx.env)->SetByteArrayRegion(ctx.env, jBuffer, 0, length,
                                 (const jbyte *)buffer);

  Java_qb_blogfuzz_NativeHelper_fuzzMeArray(ctx.env, NULL, jBuffer);

  (*ctx.env)->DeleteLocalRef(ctx.env, jBuffer);
}

int main(void) {
  int status;
  const uint8_t buffer[BUFFER_SIZE];

  ssize_t rlength = fread((void *)buffer, 1, BUFFER_SIZE, stdin);
  if (rlength == -1) return errno;

  if ((status = init_java_env(&ctx, NULL, 0)) != 0) {
    return status;
  }

  fuzz_one_input(buffer, rlength);

  return 0;
}
