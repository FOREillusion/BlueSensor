#include <AltSoftSerial.h>
#include <BME280I2C.h>
#include <pb.h>
#include <pb_common.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <Wire.h>
#include "BufferProtocol.h"
#include "sense.pb.h"

#define TEMP 1
#define HUM 2
#define PA 3

#define CHANNEL_PROTOBUF 0
#define CHANNEL_SERIAL 1
#define CHANNEL_LOG 2

SerialProtocol sp;
BufferProtocol bp(&sp);

BME280I2C::Settings bme_setting(
  BME280::OSR_X1, 
  BME280::OSR_X1, 
  BME280::OSR_X1, 
  BME280::Mode_Normal, 
  BME280::StandbyTime_1000ms,
  BME280::Filter_16,
  BME280::SpiEnable_False,
  0x77);
BME280I2C bme(bme_setting);

void write_log(BufferProtocol* bp, const char* log);

unsigned long lst_report_time;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  while (!Serial);
  write_log(&bp, "[info] serial connected");
  Wire.begin();
  if (!bme.begin()) {
    write_log(&bp, "[error] bme280 sensor not found!");
    while (1);
  }
  digitalWrite(13, LOW);
  lst_report_time = 0;
}

void write(BufferProtocol* bp, unsigned char* buf, size_t buf_size) {
  for (int i = 0; i < buf_size; i++) {
    bp->write(buf[i]);
  }
}

void begin_channel(BufferProtocol* bp, unsigned char channel_id) {
  bp->write(channel_id);
}

void write_log(BufferProtocol* bp, const char* log) {
  bp->begin();
  begin_channel(bp, CHANNEL_LOG);
  write(bp, log, strlen(log));
  bp->end();
}

int write_data(BufferProtocol* bp, int32_t field, float data) {
  uint8_t buffer[SenseUpdate_size];
  size_t message_length;
  SenseUpdate message = SenseUpdate_init_default;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, SenseUpdate_size);
  message.field = field;
  message.data = data;
  if (!pb_encode(&stream, SenseUpdate_fields, &message)) {
    write_log(bp, PB_GET_ERROR(&stream));
    return -1;
  }
  message_length = stream.bytes_written;
  bp->begin();
  begin_channel(bp, CHANNEL_PROTOBUF);
  write(bp, buffer, message_length);
  bp->end();
  return message_length;
}

void update_bme() {
  float temp, pa, hum;
  bme.read(pa, temp, hum);
  if (!isnan(temp)) write_data(&bp, TEMP, temp);
  if (!isnan(pa)) write_data(&bp, PA, pa);
  if (!isnan(hum)) write_data(&bp, HUM, hum);
}

void loop() {
  unsigned long current_time = millis();
  if (current_time - lst_report_time >= 1000) {
    update_bme();
    lst_report_time = current_time;
  }
  if (Serial2.available()) {
    bp.begin();
    begin_channel(&bp, CHANNEL_SERIAL);
    while (Serial2.available()) {
      bp.write(Serial2.read());
    }
    bp.end();
  }
}
