#include <SoftwareServo.h>

SoftwareServo *SoftwareServo::first;

#define NO_ANGLE (0xff)

SoftwareServo::SoftwareServo() : pin(0),angle(NO_ANGLE),pulse0(0),min16(34),max16(150),next(0)
{}

void SoftwareServo::setMinimumPulse(uint16_t t)
{
    min16 = t/16;
}

void SoftwareServo::setMaximumPulse(uint16_t t)
{
    max16 = t/16;
}

uint8_t SoftwareServo::attach(int pinArg)
{
    pin = pinArg;
    angle = NO_ANGLE;
    pulse0 = 0;
    next = first;
    first = this;
    digitalWrite(pin,0);
    pinMode(pin,OUTPUT);
    return 1;
}

void SoftwareServo::detach()
{
    for ( SoftwareServo **p = &first; *p != 0; p = &((*p)->next) ) {
	if ( *p == this) {
	    *p = this->next;
	    this->next = 0;
	    return;
	}
    }
}

void SoftwareServo::write(int angleArg)
{
    if ( angleArg < 0) angleArg = 0;
    if ( angleArg > 180) angleArg = 180;
    angle = angleArg;
    pulse0 = (min16*16L*clockCyclesPerMicrosecond() + (max16-min16)*(16L*clockCyclesPerMicrosecond())*angle/180L)/64L;
}

uint8_t SoftwareServo::read()
{
    return angle;
}

uint8_t SoftwareServo::attached()
{
    for ( SoftwareServo *p = first; p != 0; p = p->next ) {
	if ( p == this) return 1;
    }
    return 0;
}

void SoftwareServo::refresh()
{
    uint8_t count = 0, i = 0;
    uint16_t base = 0;
    SoftwareServo *p;
    static unsigned long lastRefresh = 0;
    unsigned long m = millis();

    if ( m >= lastRefresh && m < lastRefresh + 20) return;
    lastRefresh = m;

    for ( p = first; p != 0; p = p->next ) if ( p->pulse0) count++;
    if ( count == 0) return;

    SoftwareServo *s[count];
    for ( p = first; p != 0; p = p->next ) if ( p->pulse0) s[i++] = p;

    // bubblesort the SoftwareServos by pulse time, ascending order
    for(;;) {
	uint8_t moved = 0;
	for ( i = 1; i < count; i++) {
	    if ( s[i]->pulse0 < s[i-1]->pulse0) {
		SoftwareServo *t = s[i];
		s[i] = s[i-1];
		s[i-1] = t;
		moved = 1;
	    }
	}
	if ( !moved) break;
    }

    for ( i = 0; i < count; i++) digitalWrite( s[i]->pin, 1);

    uint8_t start = TCNT0;
    uint8_t now = start;
    uint8_t last = now;

    for ( i = 0; i < count; i++) {
	uint16_t go = start + s[i]->pulse0;

	for (;;) {
	    now = TCNT0;
	    if ( now < last) base += 256;
	    last = now;

	    if ( base+now > go) {
		digitalWrite( s[i]->pin,0);
		break;
	    }
	}
    }
}
