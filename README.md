# BlueSensor

Running on Arduino, this program can send sensor data to Raspberry Pi.

For the next generation of BlueSense. For legacy version, refer to legacy branch.

## Dependencies

These libraries are embedded in this project.

* serialpb (BufferProtocol.[h|cpp])
* protobuf generated files (sense.pb.[c|h])

You need to install the following dependencies yourself.

* nanopb (use vcpkg or other tools to install protoc plugin, and also install it in Arduino libraries folder)
* DFRobot_BME280 (install it in Arduino libraries folder)
* Python 2.7 and protobuf (required by nanopb generator plugin)

## Data Transmission

All packets are encapsuled in serialpb buffer protocol. For each packet,

* nanopb packet begins with `0x00`, then follows raw nanopb (protobuf) data.
* log message begins with `0x01`, then follows the byte string.
