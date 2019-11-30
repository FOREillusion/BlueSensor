# BlueSensor

Running on Arduino, this program can send sensor data to Raspberry Pi.

Built for BlueSense-NG. For previous versions, refer to legacy branch.

## Dependencies

These libraries are embedded in this project.

* serialpb (BufferProtocol.[h|cpp])
* protobuf generated files (sense.pb.[c|h])

You'll need to install the following dependencies yourself.

* nanopb (use vcpkg or other tools to install protoc plugin, and also install it in Arduino libraries folder)
* BME280 https://github.com/finitespace/BME280
* Python 2.7 and protobuf (required by nanopb generator plugin)

## Data Transmission

All packets are encapsuled in serialpb buffer protocol. For each packet,

The first byte indicates channel.

* nanopb packet begins with `0x00`, then follows raw nanopb (protobuf) data.
* PM2.5 sensor data begins with `0x01`, then follows raw serial data.
* log message begins with `0x02`, then follows the byte string.
