# 概述

该项目是Linux下基于zookeeper的drpc远程调用框架。Rpc Server向Zookeeper Server注册服务，Rpc Client通过Zookeeper Server获取与Rpc Server通讯方式进行远程调用。

# 开发环境

* Ubuntu 22.04
* gcc version 11.4.0
* zookeeper 3.4.10

# 构建项目

**环境依赖**

* [protobuf 3.21.11](https://github.com/protocolbuffers/protobuf)
* [muduo 2.0.2](https://github.com/chenshuo/muduo)
* [zookeeper 3.4.10](https://github.com/apache/zookeeper)
* [spdlog 1.14.1](https://github.com/gabime/spdlog)

**build**

确保安装所有依赖后执行

````
git clone https://github.com/ddddincn/drpc.git
./build.sh
````

