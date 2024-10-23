#!/bin/sh

cd $(dirname $0)

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib
./MyRos2Channel ./cfg/my_ros2_channel_cfg.yaml
