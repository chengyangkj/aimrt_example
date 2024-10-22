cmake_minimum_required(VERSION 3.26)
project(MyNormalChannel)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

# 添加三方库
include(cmake/GetAimRT.cmake)

# 添加对src/protocols文件夹的编译
add_subdirectory(src/protocols)

# 包含源文件
file(GLOB_RECURSE SOURCE_FILE ${PROJECT_SOURCE_DIR}/src/*.cc)

# 设置可执行程序的输出路径为bin目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILE})
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(std_msgs REQUIRED)

# 链接库
target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  aimrt::runtime::core 
  aimrt::interface::aimrt_module_cpp_interface 
  yaml-cpp::yaml-cpp 
  aimrt::interface::aimrt_module_protobuf_interface # 发布protobuf消息所需包含的接口
  my_channel_pb_gencode
  )

# 安装配置文件
add_custom_target(
  ${PROJECT_NAME}_build_all ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/install ${CMAKE_BINARY_DIR}/bin
  DEPENDS ${PROJECT_NAME})
