include(FetchContent)

message(STATUS "get aimrt ...")

FetchContent_Declare(aimrt GIT_REPOSITORY https://gitee.com/robot-os/AimRT GIT_TAG v0.8.2)

FetchContent_GetProperties(aimrt)

if(NOT aimrt_POPULATED)
  set(AIMRT_BUILD_TESTS
    OFF
    CACHE BOOL "")
  FetchContent_MakeAvailable(aimrt)
endif()
