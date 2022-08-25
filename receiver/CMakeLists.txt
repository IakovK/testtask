cmake_minimum_required(VERSION 3.0)

project(receiver)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(receiver main.cpp receiver.cpp receiver.h packetbuffer.h)

target_include_directories(receiver PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(receiver PRIVATE ${Boost_LIBRARIES} Threads::Threads OpenSSL::SSL)
install(TARGETS receiver RUNTIME DESTINATION bin)
