#ifndef ENVSENSOR_PM_H
#define ENVSENSOR_PM_H

#include <Arduino.h>

#define PM_BUFFER 32
#define PM_BUFFER_SIZE (PM_BUFFER * 2)
#define PM_SERIAL_BAUDRATE 9600

struct pm_DI {
  int pm01, pm25, pm10;
  HardwareSerial* serial;
  byte buffer[PM_BUFFER_SIZE];
};

char pm_checkValue(byte *buffer, int len);
int pm_transmitPM01(byte *buffer);
int pm_transmitPM25(byte *buffer);
int pm_transmitPM10(byte *buffer);
void pm_initialize(struct pm_DI* storage, HardwareSerial* serial);
int pm_refresh(struct pm_DI* storage);

#endif
