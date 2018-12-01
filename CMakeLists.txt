cmake_minimum_required(VERSION 3.5.0)
project(QuadSifter)
set(TARGET QuadSifter)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-g -O0 -std=c++17 ${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-literal-suffix")

find_package(OpenCV COMPONENTS core highgui video imgproc contrib REQUIRED)

include_directories(
    include
	${OpenCV_INCLUDE_DIRS}
)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# add the ROS executables
add_executable(${TARGET} ${SRC_FILES})
target_link_libraries(${TARGET} ${OpenCV_LIBS} "-lstdc++fs")
