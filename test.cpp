#include <stdio.h>
#include "morse.h"

// gcc -D test_main=main -o test morse.cpp test.cpp && ./test

int test_main(int argc, char *argv[]) {
  MorseEncoder enc = MorseEncoder("SOS ck ck ark");
  while (enc.Tick()) {
      if (enc.Transmitting) {
          printf("#");
      } else {
          printf(" ");
      }
  }
  printf("\n");
}
