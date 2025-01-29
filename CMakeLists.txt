cmake_minimum_required(VERSION 3.10)
project(OFEC)
find_package(Threads)
if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
else()
  set(CMAKE_CXX_FLAGS -pthread)
endif()

set(OFEC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DOFEC_DIR="${OFEC_DIR}")
include(SrcFiles.cmake)
add_executable(${CMAKE_PROJECT_NAME} ${OFEC_SOURCES})


if(CMAKE_COMPILER_IS_GNUCXX)
  # 检查GNU编译是否支持 -Wa,-mbig-obj 
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-Wa,-mbig-obj" GNU_BIG_OBJ_FLAG_ENABLE)
  message(STATUS GNU_BIG_OBJ_FLAG_ENABLE=${GNU_BIG_OBJ_FLAG_ENABLE})
endif()
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-z,stack-size=64000000") 
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_20)
include(ExtrLibs.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE  ${PROJECT_LIBRARIES})




