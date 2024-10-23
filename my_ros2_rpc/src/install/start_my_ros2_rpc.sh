#!/bin/sh

cd $(dirname $0)

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib
./MyRos2Rpc ./cfg/my_ros2_rpc_cfg.yaml
