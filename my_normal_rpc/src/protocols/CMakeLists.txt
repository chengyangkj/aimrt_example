set(PROTO_NAME "my_rpc")

# 添加普通 protobuf 生成代码的target
add_protobuf_gencode_target_for_proto_path(
  TARGET_NAME ${PROTO_NAME}_pb_gencode
  PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 添加protobuf rpc生成代码的target 并依赖普通protobuf生成代码的target
add_protobuf_aimrt_rpc_gencode_target_for_proto_files(
  TARGET_NAME ${PROTO_NAME}_aimrt_rpc_gencode
  PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/my_rpc.proto
  GENCODE_PATH ${CMAKE_CURRENT_BINARY_DIR}
  DEP_PROTO_TARGETS ${PROTO_NAME}_pb_gencode)
