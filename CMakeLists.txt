cmake_minimum_required(VERSION 3.21)

set(TOOL_PATH "MTools")
set(HV_PATH "Script/libhv_static")
set(LUA_PATH "Script/liblua_static")
set(SURFACE "surface")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -Os -Wall -s   -mllvm -sub   -mllvm -fla -mllvm -sobf ")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -Os -Wall -Wextra -s  -mllvm -sub  -mllvm -fla -mllvm -sobf ")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/arm/${ANDROID_ABI}/) #设置可执行文件的输出目录
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/arm/${ANDROID_ABI}/) #设置库文件的输出目录

add_subdirectory(surface)

include_directories(

        ${SURFACE}/include/
        ${SURFACE}/include/ImGui
        ${SURFACE}/include/Android_touch
        ${SURFACE}/include/Android_shm
        ${SURFACE}/include/native_surface
        ${SURFACE}/include/Android_draw
        src/include
        ${TOOL_PATH}/include
        ${HV_PATH}/include
        ${LUA_PATH}/include
)

set(
        SORCES
        ${TOOL_PATH}/ByteUtil.cpp
        ${TOOL_PATH}/DataDec.cpp
        ${TOOL_PATH}/DataEnc.cpp
        ${TOOL_PATH}/GameTools.cpp
        ${TOOL_PATH}/MemoryTools.cpp
        ${TOOL_PATH}/SocketTools.cpp
        ${TOOL_PATH}/TimeTools.cpp
        ${TOOL_PATH}/UDPClient.cpp
        ${TOOL_PATH}/UDPServer.cpp
        ${TOOL_PATH}/TCPClient.cpp
        ${TOOL_PATH}/TCPServer.cpp
        ${TOOL_PATH}/BinCCation.cpp
        ${TOOL_PATH}/abiUtil.cpp
        ${TOOL_PATH}/IOUtils.cpp
)

add_library(hv_static STATIC IMPORTED)
set_target_properties(hv_static PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${HV_PATH}/${ANDROID_ABI}/libhv_static.a)

add_library(lua_static STATIC IMPORTED)
set_target_properties(lua_static PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${LUA_PATH}/${ANDROID_ABI}/liblua_static.a)

add_executable(fpsGame ${SORCES} src/fpsGame.cpp)
add_executable(mobaGame ${SORCES} src/mobaGame.cpp)

add_executable(serach ${SORCES} tools/serach.cpp)
add_executable(arkGame ${SORCES} src/arkGame.cpp)
add_executable(ue4Game ${SORCES} src/ue4Game_4_23.cpp)
add_executable(autoClick ${SORCES} tools/autoClick.cpp)

#add_executable(recv ${SORCES} tools/recv.cpp)
#add_executable(tt ${SORCES} tools/test.cpp)

target_link_libraries(fpsGame log lua_static hv_static)
target_link_libraries(mobaGame log lua_static hv_static)
target_link_libraries(arkGame log NativeSurface)
target_link_libraries(ue4Game log NativeSurface)
