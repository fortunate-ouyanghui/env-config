cmake_minimum_required(VERSION 3.18)
project(socket_project)

# ======== 路径配置 ========
#set(变量名 变量初始值 声明为cache缓存变量 类型：路径型 描述)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "头文件目录")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "源文件目录")
set(BIN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin" CACHE PATH "可执行文件输出目录")

# ======== 打印路径信息 ========
# 可选类型有：WARNING FATAL_ERROR STATUS
message(STATUS "项目根目录：${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "头文件目录：${INCLUDE_DIR}")
message(STATUS "源文件目录：${SRC_DIR}")
message(STATUS "输出目录：${BIN_OUTPUT_DIR}")

# ======== 编译配置 ========
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ======== 包含头文件路径 ========
add_library(global INTERFACE)  # 声明纯接口目标（无实际代码）
target_include_directories(
    global
    INTERFACE
    ${INCLUDE_DIR}
)

# ======== 可执行文件配置 ========
add_executable(cli
    ${SRC_DIR}/client.cpp
)

add_executable(srv
    ${SRC_DIR}/server.cpp
)

# ======== 统一设置输出目录 ========
set_target_properties(cli srv PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
)

# ========== 依赖配置 ==========
# 假设 lib 目录下有自定义静态库（如 libmylib.a）
# target_link_libraries(cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmylib.a)
# target_link_libraries(srv PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmylib.a)
target_link_libraries(cli PRIVATE global)  # 客户端目标未链接接口库
target_link_libraries(srv PRIVATE global)  # 服务端目标未链接接口库


# ========== 编译信息提示 ==========
message(STATUS "======== 配置信息 ========")
message(STATUS "编译配置总结：")
message(STATUS "  - 头文件路径: ${INCLUDE_DIR}")
message(STATUS "  - 生成目标: cli（客户端）、srv（服务端）")
message(STATUS "  - 输出位置: ${BIN_OUTPUT_DIR}/cli 和 ${BIN_OUTPUT_DIR}/srv")
message(STATUS "======== 配置信息 ========")
