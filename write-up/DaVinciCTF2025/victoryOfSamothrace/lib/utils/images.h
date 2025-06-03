#ifndef IMAGES_H
#define IMAGES_H

#ifdef ARDUINO_ARCH_ESP32
  // ESP32 uses a different pgmspace.h location
  #include <pgmspace.h>
#else
  // STM32 uses AVR compatibility
  #include <avr/pgmspace.h>
#endif

extern const unsigned char PROGMEM dvid_logo[1024];
extern const unsigned char bufferAnimation[30][1024];

#endif