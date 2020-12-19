#pragma once
#include "durations.h"

class Pulse {
public:
  Pulse(unsigned long period);
  
  /** Ticked tells you if a period has elapsed. */
  bool Ticked();
  bool Ticked(unsigned long now);

  /** Until sets the duration of the next period. */
  void Until(unsigned long next);
  void Until(unsigned long next, unsigned long now);

  unsigned long period;
  unsigned long nextEventMillis;
};
