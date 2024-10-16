include(FetchContent)

message(STATUS "get aimrt ...")

set(aimrt_REPOSITORY_URL "https://github.com/AimRT/AimRT")

FetchContent_Declare(aimrt GIT_REPOSITORY ${aimrt_REPOSITORY_URL} GIT_TAG v0.8.2)

FetchContent_GetProperties(aimrt)

if(NOT aimrt_POPULATED)
  set(AIMRT_BUILD_TESTS
    OFF
    CACHE BOOL "")

  FetchContent_MakeAvailable(aimrt)
endif()
