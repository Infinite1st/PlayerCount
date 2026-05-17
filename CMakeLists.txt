cmake_minimum_required(VERSION 3.21)
project(PlayerCount VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

find_package(CommonLibSSE CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${headers} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PRIVATE ${SIMPLEINI_INCLUDE_DIRS})
