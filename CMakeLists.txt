cmake_minimum_required(VERSION 2.8)
enable_testing()
project(web_crawler)
option(web_crawler_build_tests "Build all of web_crawler's unit tests." OFF)
set(SOURCE_FILES main/main.cpp)
add_executable(web_crawler ${SOURCE_FILES} main/URLFinder.h main/Curler.cpp main/Curler.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
endif (CURL_FOUND)

find_package (Threads)
add_subdirectory(unit_tests)

target_link_libraries (web_crawler ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -pthread /usr/local/lib/libgumbo.so)
