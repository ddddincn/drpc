set(SRC_LIST
  ./rpcclient.cc
  ../user.pb.cc)
add_executable(client ${SRC_LIST})
target_link_libraries(client drpc protobuf)