#!/bin/bash

cd bin

for ((i = 1; i <= 10000; i++))
do
   ./example/client -f../example/rpcclient/config.conf
done