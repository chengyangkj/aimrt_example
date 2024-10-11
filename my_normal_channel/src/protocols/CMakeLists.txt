set(PROTO_NAME "my_channel")

# 使用AimRT CMake工具，将protobuf生成代码，并生成一个名为${PROTO_NAME}_pb_gencode的target
add_protobuf_gencode_target_for_proto_path(
  TARGET_NAME ${PROTO_NAME}_pb_gencode
  PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR})
