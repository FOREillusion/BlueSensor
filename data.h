#ifndef ENVSENSOR_DATA_H
#define ENVSENSOR_DATA_H

#define DHTTYPE DHT22
#define DHTPIN 7

#define DATA_CMD_TEMPERATURE 97
#define DATA_CMD_DHT_TEMPERATURE_C 100
#define DATA_CMD_DHT_HUMIDITY 101
#define DATA_CMD_DHT_TEMPERATURE_F 102
#define DATA_CMD_DHT_HIC 103
#define DATA_CMD_DHT_HIF 104

#define DATA_CMD_PM 65
#define DATA_CMD_PM01 70
#define DATA_CMD_PM25 71
#define DATA_CMD_PM10 72

#define DATA_CMD_DEBUG 66
#define DATA_CMD_FAILED 98
#define DATA_CMD_CLEAR 67
#define DATA_CMD_FAILED_DHT 110
#define DATA_CMD_FAILED_PM 111

#define PACK_PAYLOAD_LENGTH 8
#define DATA_LENGTH 16

typedef unsigned char CMD;
typedef unsigned char SIZE;

union Payload {
  byte payload[PACK_PAYLOAD_LENGTH];
  int _int;
  long _long;
  float _float;
  double _double;
} Payload;

struct Packet {
  unsigned short message_id, response_id, command_id;
  union Payload payload;
  unsigned short verification;
} Packet;

union Data {
  struct Packet packet;
  char data[DATA_LENGTH];
} Data;

struct DHTData {
  float h, t, f, hif, hic;
} DHTData;

struct PMData {
  int pm01, pm25, pm10;
} PMData;

#endif
