#!/bin/bash

mkdir -p ./third_party
cd ./third_party
wget https://github.com/ETLCPP/etl/archive/refs/tags/20.19.0.tar.gz
tar -zxvf 20.19.0.tar.gz
rm 20.19.0.tar.gz
cd ..
