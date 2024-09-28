#!/bin/bash

set -e

cd `pwd`/proto &&
  protoc header.proto --cpp_out=. &&
  mv header.pb.h ../src &&
  mv header.pb.cc ../src &&
  cd ..

cd `pwd`/example &&
  protoc user.proto --cpp_out=. &&
  cd ..

mkdir `pwd`/build && 
  cd `pwd`/build &&
  cmake .. &&
  make
