cmake_minimum_required(VERSION 3.18)

project(mitsubishi_fx_tcp_plc)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "-std=c++17 ")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>") # 针对 C 编译器
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>") # 针对 C++ 编译器
if(MSVC)
    add_definitions("-DUNICODE" "-D_UNICODE") # Windows API 使用 unicode-16
endif()


if(MSVC)
    # 为 MSVC 添加 /arch:SSE2 选项
    add_compile_options(/arch:SSE2)
endif()
############################################################
# Should be used in the TOP LEVEL of CMakeLists.txt file.
# Set the install root path as the cmake binary dir
############################################################
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  message(STATUS "Setting default CMAKE_INSTALL_PREFIX path to ${CMAKE_BINARY_DIR}/install")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE STRING "The path to use for make install" FORCE)
endif()

# Setting Path for install.
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin/)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib/)
# set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include/)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# ##########################################################
# # Setting path for 3rd party libraries
# ##########################################################
if (NOT DEFINED THIRD_PARTY_LIBRARY_DIR)
    # Check if the environment variable is set
    if (DEFINED ENV{THIRD_PARTY_LIBRARY_DIR})
        set(THIRD_PARTY_LIBRARY_DIR $ENV{THIRD_PARTY_LIBRARY_DIR} CACHE PATH "Path to 3rd party libraries")
    else()
        set(THIRD_PARTY_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../../3rdParty CACHE PATH "Path to 3rd party libraries")
    endif()
else ()
    set(THIRD_PARTY_LIBRARY_DIR ${THIRD_PARTY_LIBRARY_DIR} CACHE PATH "Path to 3rd party libraries")
endif()
message(STATUS "THIRD_PARTY_LIBRARY_DIR = ${THIRD_PARTY_LIBRARY_DIR}")


set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configurations" FORCE)
# 默认设置编译选项
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Set postfix for debug file.
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Set debug library postfix")


message("PROJECT_NAME: "${PROJECT_NAME} )
###########################################################
# Add EXE test target
############################################################
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/tcpclient.cpp
    # Add header files is for IDE
    #include/${PROJECT_NAME}/test_comm_module_all.h
)

# Add include dir for current target COMMON
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_executable(tcpclient src/tcpclient.cpp)
target_include_directories(tcpclient PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
