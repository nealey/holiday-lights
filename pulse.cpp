#include "pulse.h"

#include <Arduino.h>

Pulse::Pulse(unsigned long period) {
  this->period = period;
  this->nextEventMillis = 0;
}

bool Pulse::Ticked() {
  return Ticked(millis());
}

bool Pulse::Ticked(unsigned long now) {
  if (now >= nextEventMillis) {
    Until(period, now);
    return true;
  }
  return false;
}

void Pulse::Until(unsigned long next, unsigned long now) {
  nextEventMillis = now + next;
}

void Pulse::Until(unsigned long next) {
  Until(next, millis());
}
