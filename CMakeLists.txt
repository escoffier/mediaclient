cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

project(client)

add_definitions(-DICE_CPP11_MAPPING)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

#set(Ice_SLICE_DIR "/usr/bin")

find_package(Ice 3.7.1 REQUIRED Ice )


if (Ice_FOUND)
message(${Ice_LIBRARIES})
endif()

set(_PROTOBUF_PROTOC slice2cpp) 
#set(_GRPC_CPP_PLUGIN_EXECUTABLE /usr/local/bin/grpc_cpp_plugin)
  # Proto file
  get_filename_component(hw_proto "${CMAKE_CURRENT_SOURCE_DIR}/ices/stream.ice" ABSOLUTE)
  message(STATUS "proto path: ${hw_proto}")
  get_filename_component(hw_proto_path "${hw_proto}" PATH)
  message(STATUS "hw_proto_path path: ${hw_proto_path}")
  
  # Generated sources
  set(hw_proto_srcs "${CMAKE_CURRENT_SOURCE_DIR}/stream.cpp")
  set(hw_proto_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/stream.h")
  #set(hw_grpc_srcs "${CMAKE_CURRENT_SOURCE_DIR}/HelloI.cpp")
  #set(hw_grpc_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/HelloI.h")
  
  add_custom_command(
        OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --output-dir "${CMAKE_CURRENT_SOURCE_DIR}"
        #ARGS --grpc_out "${CMAKE_CURRENT_SOURCE_DIR}"
          #--cpp_out "${CMAKE_CURRENT_SOURCE_DIR}"
          #-I "${hw_proto_path}"
          #--plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}" 
          "${hw_proto}"
        DEPENDS "${hw_proto}")
  
  add_custom_target(some_name ALL 
  DEPENDS "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}")
   # Include generated *.pb.h files
  set_source_files_properties("${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}" PROPERTIES GENERATED 1)

  #include_directories(./hksdk/include)
  #link_directories(/opt/CH_HCNetSDK_V5.2.7.4_build20170606_Linux64/lib /opt/CH_HCNetSDK_V5.2.7.4_build20170606_Linux64/lib/HCNetSDKCom)
  add_definitions(-DGLOG_NO_ABBREVIATED_SEVERITIES -DGOOGLE_GLOG_DLL_DECL= -D__x86_64__  -DCMAKE_BUILD_TYPE=Debug)


  #aux_source_directory(. SRCS)
  set(SOURCES
      ${CMAKE_SOURCE_DIR}/mediaclinet.cpp
      ${CMAKE_SOURCE_DIR}/stream.cpp)


  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
  link_libraries(  Glacier2++11 IceGrid++11 Ice++11 pthread dl glog libjrtp.a libjthread.a)
  add_executable(${PROJECT_NAME} ${SOURCES} )
