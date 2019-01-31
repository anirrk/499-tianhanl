cmake_minimum_required(VERSION 3.3)

#Proto file
get_filename_component(service_layer_proto "../protos/service_layer.proto" ABSOLUTE)
get_filename_component(service_layer_proto_path "${service_layer_proto}" PATH)

# Generated sources
set(DIST_DIR "${CMAKE_CURRENT_BINARY_DIR}/dist")
file(MAKE_DIRECTORY ${DIST_DIR})
set(service_layer_proto_srcs "${DIST_DIR}/service_layer.pb.cc")
set(service_layer_proto_hdrs "${DIST_DIR}/service_layer.pb.h")
set(service_layer_grpc_srcs "${DIST_DIR}/service_layer.grpc.pb.cc")
set(service_layer_grpc_hdrs "${DIST_DIR}/service_layer.grpc.pb.h")
add_custom_command(
      OUTPUT "${service_layer_proto_srcs}" "${service_layer_proto_hdrs}" "${service_layer_grpc_srcs}" "${service_layer_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${DIST_DIR}"
        --cpp_out "${DIST_DIR}"
        -I "${service_layer_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${service_layer_proto}"
      DEPENDS ${service_layer_proto})

# Include generated *.pb.h files
include_directories("${DIST_DIR}")

add_library(store_adapter store_adapter.cc
    ${service_layer_proto_srcs}
    ${service_layer_grpc_srcs})
target_link_libraries(store_adapter
    ${_GRPC_GRPCPP_UNSECURE}
    ${_PROTOBUF_LIBPROTOBUF}
    key_value_store_client_sync)

add_executable(store_adapter_test store_adapter_test.cc
    ${service_layer_proto_srcs}
    ${service_layer_grpc_srcs})
target_link_libraries(store_adapter_test
    gtest
    gtest_main
    store_adapter)


add_executable(service_layer_server_sync service_layer_server_sync.cc
${service_layer_proto_srcs}
${service_layer_grpc_srcs})

target_link_libraries(service_layer_server_sync
${_GRPC_GRPCPP_UNSECURE}
${_PROTOBUF_PROTOC}
store_adapter)