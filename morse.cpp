#include "morse.h"

struct MorseSign {
  // Length of this sign. If Length is 0, Bits is the duration of a pause.
  uint8_t Length;
  uint8_t Bits;
};

const struct MorseSign GetMorseSign(char c) {
  switch (c) {
    case 3:  // ETX - End of Text
      return (struct MorseSign){8, 0b01010101};
    case 4:  // EOT - End of Transmission
      return (struct MorseSign){0, MORSE_PAUSE_TRANSMISSION};
    case ' ':
      return (struct MorseSign){0, MORSE_PAUSE_WORD};
    case '0':
      return (struct MorseSign){5, 0b11111};
    case '1':
      return (struct MorseSign){5, 0b01111};
    case '2':
      return (struct MorseSign){5, 0b00111};
    case '3':
      return (struct MorseSign){5, 0b00011};
    case '4':
      return (struct MorseSign){5, 0b00001};
    case 5:
      return (struct MorseSign){5, 0b00000};
    case 6:
      return (struct MorseSign){5, 0b10000};
    case 7:
      return (struct MorseSign){5, 0b11000};
    case 8:
      return (struct MorseSign){5, 0b11100};
    case 9:
      return (struct MorseSign){5, 0b11110};
    case 'a':
    case 'A':
      return (struct MorseSign){2, 0b01};
    case 'b':
    case 'B':
      return (struct MorseSign){4, 0b1000};
    case 'c':
    case 'C':
      return (struct MorseSign){4, 0b1010};
    case 'd':
    case 'D':
      return (struct MorseSign){3, 0b100};
    case 'e':
    case 'E':
      return (struct MorseSign){1, 0b0};
    case 'f':
    case 'F':
      return (struct MorseSign){4, 0b0010};
    case 'g':
    case 'G':
      return (struct MorseSign){3, 0b110};
    case 'h':
    case 'H':
      return (struct MorseSign){4, 0b0000};
    case 'i':
    case 'I':
      return (struct MorseSign){2, 0b00};
    case 'j':
    case 'J':
      return (struct MorseSign){4, 0b0111};
    case 'k':
    case 'K':
      return (struct MorseSign){3, 0b101};
    case 'l':
    case 'L':
      return (struct MorseSign){4, 0b0100};
    case 'm':
    case 'M':
      return (struct MorseSign){2, 0b11};
    case 'n':
    case 'N':
      return (struct MorseSign){2, 0b10};
    case 'o':
    case 'O':
      return (struct MorseSign){3, 0b111};
    case 'p':
    case 'P':
      return (struct MorseSign){4, 0b0110};
    case 'q':
    case 'Q':
      return (struct MorseSign){4, 0b1101};
    case 'r':
    case 'R':
      return (struct MorseSign){3, 0b010};
    case 's':
    case 'S':
      return (struct MorseSign){3, 0b000};
    case 't':
    case 'T':
      return (struct MorseSign){1, 0b1};
    case 'u':
    case 'U':
      return (struct MorseSign){3, 0b001};
    case 'v':
    case 'V':
      return (struct MorseSign){4, 0b0001};
    case 'w':
    case 'W':
      return (struct MorseSign){3, 0b011};
    case 'x':
    case 'X':
      return (struct MorseSign){4, 0b1001};
    case 'y':
    case 'Y':
      return (struct MorseSign){4, 0b1011};
    case 'z':
    case 'Z':
      return (struct MorseSign){4, 0b1100};
    default:
      return (struct MorseSign){0, 0};
  }
}

MorseEncoder::MorseEncoder() {
  SetText("");
}
MorseEncoder::MorseEncoder(const char *s) {
  SetText(s);
}

void MorseEncoder::SetText(const char *s) {
  p = s;
  bit = 0;
  ticksLeft = 0;
  Transmitting = false;
}

void MorseEncoder::Quiet(int ticks) {
  Transmitting = false;
  ticksLeft = ticks;
}

bool MorseEncoder::Tick() {
  --ticksLeft;
  if (ticksLeft > 0) {
    return true;
  }

  // We're out of ticks

  if (!p || !*p) {
    return false;
  }

  // If we were just transmitting, we have to stop for at least one dit
  if (Transmitting) {
    Transmitting = false;
    ticksLeft = MORSE_DIT;
    return true;
  }

  const struct MorseSign sign = GetMorseSign(*p);
  if (sign.Length == 0) {
    // Pause
    Transmitting = false;
    ticksLeft = sign.Bits - (MORSE_PAUSE_LETTER - MORSE_DIT);
    ++p;
    bit = 0;
  } else if (bit == sign.Length) {
    // All done with that sign!
    Transmitting = false;
    ticksLeft = MORSE_PAUSE_LETTER;
    ++p;
    bit = 0;
  } else {
    // Sign
    uint8_t bitMask = 1 << (sign.Length - bit - 1);
    Transmitting = true;
    if (sign.Bits & bitMask) {
      ticksLeft = MORSE_DAH;
    } else {
      ticksLeft = MORSE_DIT;
    }
    ++bit;
  }

  return true;
}
