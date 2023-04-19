#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#define BUFFER_SIZE 256

/* Target function */
extern void _Z6fuzzMePKai(const uint8_t*, uint64_t);


/* Persistent loop */
void fuzz_one_input(const uint8_t *buf, int len) {
  _Z6fuzzMePKai(buf, len);
}

int main(void) {
  const uint8_t buffer[BUFFER_SIZE];

  ssize_t rlength = fread((void*) buffer, 1, BUFFER_SIZE, stdin);
  if (rlength == -1)
    return errno;

  fuzz_one_input(buffer, rlength);

  return 0;
}
