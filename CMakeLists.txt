cmake_minimum_required(VERSION 3.8)

project(olive)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

# Create files lists
file(GLOB_RECURSE SRC_NETWORK "${CMAKE_CURRENT_SOURCE_DIR}/src/network/*.*")

# Add the executable
add_executable(olive olive.cpp ${SRC_NETWORK})

# Add source groups in project
source_group("network" FILES ${SRC_NETWORK})

# Add link libraries
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
target_link_libraries(olive LINK_PRIVATE Ws2_32 Iphlpapi)
endif()

