# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)
# 定义项目名称
project(simple_server)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
# 确保项目必须使用指定的 C++ 标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找并引入 OpenSSL 库，该库为必须依赖
find_package(OpenSSL REQUIRED)

# 添加头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/HttpServer/include
    ${PROJECT_SOURCE_DIR}/WebApps/CloudStoreServer/include
    /usr/include/mysql-cppconn-8
    /usr/include/mysql
    ${OPENSSL_INCLUDE_DIR}
)

# 查找必要的库
find_library(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconn mysqlcppconn8
    PATHS /usr/lib /usr/lib64 /usr/local/lib
)

# 递归查找 HttpServer 目录下的所有 .cpp 源文件
file(GLOB_RECURSE HTTP_SERVER_SRC
    "${PROJECT_SOURCE_DIR}/HttpServer/src/*.cpp"
)

# 递归查找 CloudStoreServer 目录下的所有 .cpp 源文件
file(GLOB_RECURSE GOMOKU_SERVER_SRC
    "${PROJECT_SOURCE_DIR}/WebApps/CloudStoreServer/src/*.cpp"
)

# 设置主程序源文件路径
set(MAIN_SRC "${PROJECT_SOURCE_DIR}/WebApps/CloudStoreServer/src/main.cpp")

# 如果主程序文件不存在，创建一个
if(NOT EXISTS ${MAIN_SRC})
    file(WRITE ${MAIN_SRC}
"#include \"../include/CloudStoreServer.h\"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        CloudStoreServer server;
        server.start();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << \"Error: \" << e.what() << std::endl;
        return 1;
    }
}
")
endif()

# 创建名为 simple_server 的可执行文件，并添加源文件
add_executable(simple_server
    ${MAIN_SRC}
    ${HTTP_SERVER_SRC}
    ${GOMOKU_SERVER_SRC}
)

# 将可执行文件与必要的库进行链接
target_link_libraries(simple_server
    pthread       # 线程库
    muduo_net     # muduo 网络库
    muduo_base    # muduo 基础库
    mysqlcppconn  # MySQL C++ 连接器库
    mysqlclient   # MySQL 客户端库
    ssl           # OpenSSL 安全套接层库
    crypto        # OpenSSL 加密库
)

# 打印调试信息
message(STATUS "Include directories:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "  ${dir}")
endforeach()

message(STATUS "Main source file: ${MAIN_SRC}")
message(STATUS "HTTP Server sources: ${HTTP_SERVER_SRC}")
# 打印 CloudStoreServer 服务器的源文件路径
message(STATUS "CloudStore Server sources: ${GOMOKU_SERVER_SRC}")
