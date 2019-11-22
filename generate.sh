#!/bin/bash

PROTOC_PATH=$VCPKG_ROOT/installed/$VCPKG_DEFAULT_TRIPLET/tools/protobuf/protoc
NANOPB_PLUGIN=$(find $VCPKG_ROOT | grep protoc-gen-nanopb | head -n 1)
$PROTOC_PATH --plugin=$NANOPB_PLUGIN --nanopb_out=. --python_out=. -I . sense.proto
