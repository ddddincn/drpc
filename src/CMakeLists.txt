set(SRC_LIST
    application.cc
    provider.cc
    config.cc
    header.pb.cc
    channel.cc
    controller.cc
    zkclient.cc
)

add_library(drpc SHARED ${SRC_LIST})

target_link_libraries(drpc muduo_net muduo_base zookeeper_mt)