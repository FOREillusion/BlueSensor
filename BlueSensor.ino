#include <DHT.h>

#include "pm.h"
#include "data.h"

const unsigned long WAIT_TIME = 1000;
long last_sleep = 0;

DHT dht(DHTPIN, DHTTYPE);

struct pm_DI pmDI;

void setup()
{
  Serial.begin(9600);
  pm_initialize(&pmDI, &Serial3);
  dht.begin();
}

int getDHTData(struct DHTData *data) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    return 0;
  }
  
  float hif = dht.computeHeatIndex(f, h);

  float hic = dht.computeHeatIndex(t, h, false);

  data->h = h; data->t = t; data->f = f; data->hif = hif; data->hic = hic;

  return 1;
}

int getPMData(struct PMData *data) {
  if (pmDI.pm01 == -1 || pmDI.pm25 == -1 || pmDI.pm10 == -1) {
    return 0;
  }

  data->pm01 = pmDI.pm01;
  data->pm25 = pmDI.pm25;
  data->pm10 = pmDI.pm10;
  pmDI.pm01 = pmDI.pm25 = pmDI.pm10 = -1;
  
  return 1;
}

void serialEvent3() {
  while(!pm_refresh(&pmDI));
}

struct DHTData dhtData;
struct PMData pmData;
union Data data;

int writeMessage(HardwareSerial *serial, union Data* data) {
  serial->write((char*)data, DATA_LENGTH);
  return 0;
}

int writeAvailable(HardwareSerial *serial) {
  memset(data.data, 0, sizeof(data.data));
  writeMessage(serial, &data);
  return 0;
}

unsigned short message_id = 0;

int writeCommand(HardwareSerial *serial, unsigned short response_id, unsigned short command_id, union Payload *payload) {
  data.packet.message_id = message_id;
  data.packet.response_id = response_id;
  data.packet.command_id = command_id;
  memcpy(data.packet.payload.payload, payload->payload, PACK_PAYLOAD_LENGTH);
  unsigned int __verification = 0;
  for (int i = 0; i < DATA_LENGTH - 2; i++) {
    __verification = ((unsigned int)(__verification + (unsigned char)data.data[i])) % 65536;
  }
  data.packet.verification = (unsigned short)__verification;
  writeMessage(serial, &data);
  ++message_id;
  return 0;
}

int writePM(HardwareSerial *serial, unsigned short response_id = 0) {
  union Payload __payload;
  __payload._long = pmData.pm01;
  writeCommand(serial, response_id, DATA_CMD_PM01, &__payload);
  __payload._long = pmData.pm10;
  writeCommand(serial, response_id, DATA_CMD_PM10, &__payload);
  __payload._long = pmData.pm25;
  writeCommand(serial, response_id, DATA_CMD_PM25, &__payload);
}

int writeDHT(HardwareSerial *serial, unsigned short response_id = 0) {
  union Payload __payload;
  __payload._float = dhtData.t;
  writeCommand(serial, response_id, DATA_CMD_DHT_TEMPERATURE_C, &__payload);
  __payload._float = dhtData.f;
  writeCommand(serial, response_id, DATA_CMD_DHT_TEMPERATURE_F, &__payload);
  __payload._float = dhtData.h;
  writeCommand(serial, response_id, DATA_CMD_DHT_HUMIDITY, &__payload);
  __payload._float = dhtData.hic;
  writeCommand(serial, response_id, DATA_CMD_DHT_HIC, &__payload);
  __payload._float = dhtData.hif;
  writeCommand(serial, response_id, DATA_CMD_DHT_HIF, &__payload);
}

int writeDHTFail(HardwareSerial *serial, unsigned short response_id = 0) {
  union Payload __payload;
  __payload._int = 0;
  writeCommand(serial, response_id, DATA_CMD_FAILED_DHT, &__payload);
}

int writePMFail(HardwareSerial *serial, unsigned short response_id = 0) {
  union Payload __payload;
  __payload._int = 0;
  writeCommand(serial, response_id, DATA_CMD_FAILED_PM, &__payload);
}

int __count = 0;

void loop() {
  
  if (millis() - last_sleep >= WAIT_TIME) {
    last_sleep = millis();
    if ((++__count) >= 3) {
      if (getDHTData(&dhtData)) {
        writeDHT(&Serial);
      } else {
        writeDHTFail(&Serial);
      }
      if (getPMData(&pmData)) {
        writePM(&Serial);
      } else {
        writePMFail(&Serial);
      }
      __count = 0;
    }
    writeAvailable(&Serial);
  }
}

