#CMake 最低版本要求
cmake_minimum_required (VERSION 2.8)

#项目信息
project (temtimer)

#添加include目录和lib目录
set(INC_DIR include/)
#set(LINK_DIR lib/)
include_directories(${INC_DIR})
#link_directories(${LINK_DIR})
#link_libraries(***)

#支持gdb
set(CMAKE_BUIDLD_TYPE "DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#支持C++11和thread
#set(CMAKE_C_COMPILER g++)
#add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

#查找当前目录下所有源文件并将名称保存到 DIR_SRCS变量
aux_source_directory(. DIR_SRCS)

#指定生成目标
add_executable(Demo ${DIR_SRCS})
