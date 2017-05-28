#include "pm.h"

int pm_transmit(byte* buffer, int startpos) {
    return ((((int)buffer[startpos]) << 8) + (int)buffer[startpos + 1]);
}

int pm_transmitPM01(byte *buffer){
    return pm_transmit(buffer, 4);
}

int pm_transmitPM25(byte *buffer) {
    return pm_transmit(buffer, 6);
}

int pm_transmitPM10(byte *buffer) {
    return pm_transmit(buffer, 8);
}

char pm_checkValue(byte *buffer, int len) {
    unsigned long recvSum = 0;
    char i = 0;
    for(i = 0; i < len - 2; i++) {
        recvSum = recvSum + (long)buffer[i];
    }
    if(recvSum == ((((long)buffer[len - 2]) << 8) + (long)buffer[len - 1])) {
        return 1;
    }
    return 0;
}

void pm_initialize(struct pm_DI* DI, HardwareSerial* serial) {
    serial->begin(PM_SERIAL_BAUDRATE);
    DI->serial = serial;
    DI->pm01 = DI->pm25 = DI->pm10 = -1;
}

int pm_refresh(struct pm_DI* DI) {
  int __length = DI->serial->available();
  int __i = 0;
  if(__length >= PM_BUFFER) {
    DI->serial->readBytes(DI->buffer, min(PM_BUFFER_SIZE, __length));
    for(__i = 0; __i < PM_BUFFER; __i++) {
      if(DI->buffer[__i] == 0x42 && DI->buffer[__i + 1] == 0x4d) {
        if(pm_checkValue(DI->buffer + __i, PM_BUFFER)) {
          DI->pm01 = pm_transmitPM01(DI->buffer + __i);
          DI->pm25 = pm_transmitPM25(DI->buffer + __i);
          DI->pm10 = pm_transmitPM10(DI->buffer + __i);
        }
      }
    }
    return 0;
  } else {
    return 1;
  }
}
