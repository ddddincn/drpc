set(SRC_LIST ./rpcserver.cc ../user.pb.cc)
add_executable(server ${SRC_LIST})
target_link_libraries(server drpc protobuf)