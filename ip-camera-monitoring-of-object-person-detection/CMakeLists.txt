# Set the CMake minimum version
cmake_minimum_required(VERSION 3.5)

# Declare the Project version and the Language that the project is written in
project(objectDetectCpp VERSION 0.0.1)

# Find and add the OpenCV dependencies
find_package( OpenCV REQUIRED )

 # This is equal to QMAKE_CXX_FLAGS += -std=c++0x
set(CMAKE_CXX_STANDARD 11)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Global Install Directories
include(GNUInstallDirs)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/object.cpp)

set(UTILS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../common/exutils.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${UTILS_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common/)
target_link_libraries(${PROJECT_NAME}  PUBLIC  ${OpenCV_LIBS})
