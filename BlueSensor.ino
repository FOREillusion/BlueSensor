#include <AltSoftSerial.h>
#include "BufferProtocol.h"
#include <DFRobot_BME280.h>
#include <pb.h>
#include <pb_common.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "sense.pb.h"

#define TEMP 1
#define HUM 2
#define PA 3

#define CHANNEL_PROTOBUF 0
#define CHANNEL_SERIAL 1
#define CHANNEL_LOG 2

SerialProtocol sp;
BufferProtocol bp(&sp);
AltSoftSerial sSerial;

DFRobot_BME280 bme;

unsigned long lst_report_time;
void setup() {
  Serial.begin(9600);
  sSerial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  while (!Serial);
  if (!bme.begin()) {
    Serial.println("sensor not found!");
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

int write_message(BufferProtocol* bp, int32_t field, size_t which_data, int32_t d_i, float d_f) {
  uint8_t buffer[256];
  size_t message_length;
  SenseUpdate message = SenseUpdate_init_zero;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  message.field = field;
  message.which_data = which_data;
  if (which_data == SenseUpdate_data_int32_tag) {
    message.data.data_int32 = d_i;
  } else if (which_data == SenseUpdate_data_float_tag) {
    message.data.data_float = d_f;
  }
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

int write_int32(BufferProtocol* bp, int32_t field, int32_t data) {
  return write_message(bp, field, SenseUpdate_data_int32_tag, data, 0);
}

int write_float(BufferProtocol* bp, int32_t field, float data) {
  return write_message(bp, field, SenseUpdate_data_float_tag, 0, data);
}

void update_bme() {
  float temp, pa, hum;
  temp = bme.temperatureValue();
  pa = bme.pressureValue();
  hum = bme.humidityValue();
  write_log(&bp, "test");
  if (!isnan(temp)) write_float(&bp, TEMP, temp);
  if (!isnan(pa)) write_float(&bp, PA, pa);
  if (!isnan(hum)) write_float(&bp, HUM, hum);
}

void loop() {
  unsigned long current_time = millis();
  if (current_time - lst_report_time >= 1000) {
    update_bme();
    lst_report_time = current_time;
  }
  if (sSerial.available()) {
    bp.begin();
    begin_channel(&bp, CHANNEL_SERIAL);
    while (sSerial.available()) {
      bp.write(sSerial.read());
    }
    bp.end();
  }
}
