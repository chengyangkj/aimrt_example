#!/bin/sh

#进入至当前目录
cd $(dirname $0)

#执行cmake 并指明build目录名
cmake -B build \
    -Dboost_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/boost-1.82.0.tar.xz \
    -Dpybind11_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/pybind11-2.13.1.tar.gz \
    -Dpaho_mqtt_c_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/paho.mqtt.c-1.3.13.tar.gz \
    -Dfmt_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/fmt-10.2.1.tar.gz -Dgflags_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/gflags-2.2.2.tar.gz \
    -Dgoogletest_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/googletest-1.13.0.tar.gz \
    -Dprotobuf_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/protobuf-3.21.12.tar.gz \
    -Dyaml-cpp_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/yaml-cpp-0.8.0.tar.gz \
    -Djsoncpp_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/jsoncpp-1.9.5.tar.gz \
    -Dlibunifex_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/libunifex-591ec09e7d51858ad05be979d4034574215f5971.tar.gz \
    -Dtbb_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/oneTBB-2021.13.0.tar.gz \
    -Dsqlite_DOWNLOAD_URL=https://gitee.com/chengyangkj/aimrt_thirdparty/releases/download/v0.8.2/sqlite-amalgamation-3420000.zip
$@

#判断cmake是否成功
if [ $? -ne 0 ]; then
    echo "cmake failed"
    exit 1
fi

# 以系统最大的核心数来编译
cd build || exit 1
make -j$(nproc)
