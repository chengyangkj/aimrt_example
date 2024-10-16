include(FetchContent)

message(STATUS "get aimrt ...")

set(aimrt_REPOSITORY_URL "https://github.com/AimRT/AimRT" CACHE STRING "")

FetchContent_Declare(aimrt GIT_REPOSITORY ${aimrt_REPOSITORY_URL} GIT_TAG v0.8.2)

FetchContent_GetProperties(aimrt)

if(NOT aimrt_POPULATED)
    set(AIMRT_BUILD_TESTS
        OFF
        CACHE BOOL "")
    set(AIMRT_BUILD_EXAMPLES
        OFF
        CACHE BOOL "")
    set(AIMRT_BUILD_DOCUMENT
        OFF
        CACHE BOOL "")

    set(AIMRT_BUILD_CLI_TOOLS
        OFF
        CACHE BOOL "")
    set(AIMRT_BUILD_PYTHON_RUNTIME
        OFF
        CACHE BOOL "")

    set(AIMRT_BUILD_RUNTIME
        ON
        CACHE BOOL "")
    set(AIMRT_USE_FMT_LIB
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_WITH_PROTOBUF
        ON
        CACHE BOOL "")
    set(AIMRT_USE_LOCAL_PROTOC_COMPILER
        OFF
        CACHE BOOL "")
    set(AIMRT_USE_PROTOC_PYTHON_PLUGIN
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_WITH_ROS2
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_ROS2_PLUGIN
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_NET_PLUGIN
        ON
        CACHE BOOL "")

    set(AIMRT_BUILD_SM_PLUGIN
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_LCM_PLUGIN
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_MQTT_PLUGIN
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_RECORD_PLAYBACK_PLUGIN
        OFF
        CACHE BOOL "")
    set(AIMRT_BUILD_TIME_MANIPULATOR_PLUGIN
        ON
        CACHE BOOL "")
    set(AIMRT_BUILD_PARAMETER_PLUGIN
        ON
        CACHE BOOL "")

    FetchContent_MakeAvailable(aimrt)
endif()
