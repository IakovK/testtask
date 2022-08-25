cmake_minimum_required(VERSION 3.0)

project(sender)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(sender main.cpp sender.cpp sender.h packetgenerator.cpp packetgenerator.h)

target_include_directories(sender PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(sender PRIVATE Boost::system Threads::Threads OpenSSL::SSL)
install(TARGETS sender RUNTIME DESTINATION bin)
