cmake_minimum_required(VERSION 3.10)

project(PhoneBook CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_router.proto graph.proto transport_catalogue.proto svg.proto map_renderer.proto)

set(TC_FILES domain.h domain.cpp main.cpp   json.h json.cpp json_builder.h json_builder.cpp transport_router.h transport_router.cpp geo.h geo.cpp
json_reader.h json_reader.cpp map_renderer.h map_renderer.cpp request_handler.h request_handler.cpp serialization.h serialization.cpp svg.h svg.cpp transport_catalogue.h
transport_catalogue.cpp transport_router.proto transport_catalogue.proto svg.proto map_renderer.proto graph.proto)

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${TC_FILES})
target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
