#ifndef SoftwareServo_h
#define SoftwareServo_h

#include "Arduino.h"
#include <inttypes.h>

class SoftwareServo
{
  private:
    uint8_t pin;
    uint8_t angle;       // in degrees
    uint16_t pulse0;     // pulse width in TCNT0 counts
    uint8_t min16;       // minimum pulse, 16uS units  (default is 34)
    uint8_t max16;       // maximum pulse, 16uS units, 0-4ms range (default is 150)
    class SoftwareServo *next;
    static SoftwareServo* first;
  public:
    SoftwareServo();
    uint8_t attach(int);
    void detach();
    void write(int);         // specify the angle in degrees, 0 to 180
    uint8_t read();
    uint8_t attached();
    void setMinimumPulse(uint16_t);
    void setMaximumPulse(uint16_t);
    static void refresh();
};

#endif
