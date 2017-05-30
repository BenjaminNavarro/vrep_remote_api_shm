cmake_minimum_required (VERSION 2.6)

project(VrepRemoteApiShm)

include_directories(include)
include_directories(remoteApi)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_BUILD_TYPE Debug)

file(
    GLOB
    server_side_src
    ${CMAKE_CURRENT_SOURCE_DIR}/v_repExtRemoteApi/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/scriptFunctionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/scriptFunctionDataItem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/v_repLib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/shared_memory.c
)

add_library(libv_repExtRemoteApi SHARED ${server_side_src})
set_target_properties(libv_repExtRemoteApi PROPERTIES PREFIX "")

file(
    GLOB
    client_side_src
    ${CMAKE_CURRENT_SOURCE_DIR}/remoteApi/*
    ${CMAKE_CURRENT_SOURCE_DIR}/common/shared_memory.c
)

add_library(remoteApi SHARED ${client_side_src})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(remoteApiDef WIN32)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(remoteApiDef __APPLE__)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(remoteApiDef __linux)
endif()
target_compile_definitions(remoteApi PRIVATE MAX_EXT_API_CONNECTIONS=255 NON_MATLAB_PARSING ${remoteApiDef})
if(${UNIX})
target_link_libraries(remoteApi rt)
endif()

file(
    GLOB
    test_shm_src
    ${CMAKE_CURRENT_SOURCE_DIR}/test/*
)
add_executable(test_shm ${test_shm_src})
target_link_libraries(test_shm remoteApi)
set_property(TARGET test_shm PROPERTY CXX_STANDARD 11)
