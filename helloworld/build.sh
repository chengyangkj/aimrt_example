#!/bin/sh

#进入至当前目录
cd $(dirname $0)

#执行cmake 并指明build目录名
cmake -B build \
    -Dboost_DOWNLOAD_URL=https://gitee.com/AimRT/boost/releases/download/1.82.0/boost-1.82.0.tar.xz \
    -Dasio_DOWNLOAD_URL=https://gitee.com/AimRT/asio/repository/archive/asio-1-30-2.zip \
    -Dfmt_DOWNLOAD_URL=https://gitee.com/AimRT/fmt/repository/archive/10.2.1.zip \
    -Dgflags_DOWNLOAD_URL=https://gitee.com/mirrors/gflags/repository/archive/v2.2.2.zip \
    -Dgoogletest_DOWNLOAD_URL=https://gitee.com/mirrors/googletest/repository/archive/v1.13.0.zip \
    -Dprotobuf_DOWNLOAD_URL=https://gitee.com/mirrors/protobufsource/repository/archive/v3.21.12.zip \
    -Dyaml-cpp_DOWNLOAD_URL=https://gitee.com/mirrors/yaml-cpp/repository/archive/0.8.0.zip \
    -Djsoncpp_DOWNLOAD_URL=https://gitee.com/AimRT/jsoncpp/repository/archive/1.9.6.zip \
    -Dsqlite_DOWNLOAD_URL=https://gitee.com/AimRT/sqlite/releases/download/amalgamation-3420000/sqlite-amalgamation-3420000.zip \
    -Dstdexec_DOWNLOAD_URL=https://gitee.com/AimRT/stdexec/repository/archive/nvhpc-23.09.rc4.zip \
    -Dlibunifex_DOWNLOAD_URL=https://gitee.com/AimRT/libunifex/repository/archive/591ec09e7d51858ad05be979d4034574215f5971.zip \
    -Dtbb_DOWNLOAD_URL=https://gitee.com/mirrors/tbb/repository/archive/v2021.13.0.zip \
    -Dpybind11_DOWNLOAD_URL=https://gitee.com/mirrors/pybind11/repository/archive/v2.13.1.zip \
    -Dpaho_mqtt_c_DOWNLOAD_URL=https://gitee.com/mirrors/paho.mqtt.c/repository/archive/v1.3.13.zip \
    -Dnlohmann_json_DOWNLOAD_URL=https://gitee.com/AimRT/json/repository/archive/v3.11.3.zip \
    -Dopentelemetry_proto_DOWNLOAD_URL=https://gitee.com/AimRT/opentelemetry-proto/repository/archive/v1.3.2.zip \
    -Dopentelemetry_cpp_DOWNLOAD_URL=https://gitee.com/AimRT/opentelemetry-cpp/repository/archive/v1.16.1.zip \
    -Dzenohc_DOWNLOAD_URL=https://gitee.com/AimRT/zenoh-c/repository/archive/1.0.0.6.zip \
    -Dnghttp2_DOWNLOAD_URL=https://gitee.com/mirrors/nghttp2/repository/archive/v1.62.1.zip \
    -Diceoryx_DOWNLOAD_URL=https://gitee.com/mirrors/iceoryx/repository/archive/v2.0.6.zip \
    -Dcpptoml_DOWNLOAD_URL=https://gitee.com/AimRT/cpptoml/repository/archive/v0.1.0.zip \
    $@

#判断cmake是否成功
if [ $? -ne 0 ]; then
    echo "cmake failed"
    exit 1
fi

# 以系统最大的核心数来编译
cd build || exit 1
make -j$(nproc)
