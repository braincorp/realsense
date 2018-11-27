cmake_minimum_required(VERSION 2.8.3)
project(realsense2_camera)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(catkin REQUIRED COMPONENTS
    message_generation
    roscpp
    sensor_msgs
    std_msgs
    nodelet
    cv_bridge
    image_transport
    tf
    dynamic_reconfigure
    diagnostic_updater
    )

find_package(OpenMP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Setup pybind11 and catch2
add_subdirectory(${CMAKE_SOURCE_DIR}/../../dep/pybind11 pybind11)

# Setup python includes and libs (needed for pybind)
find_package(PythonLibs 2.7 REQUIRED)

find_package(realsense2)
if(NOT realsense2_FOUND)
    message(FATAL_ERROR "\n\n Intel RealSense SDK 2.0 is missing, please install it from https://github.com/IntelRealSense/librealsense/releases\n\n")
endif()

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    message(STATUS "Create Release Build.")
    set(CMAKE_CXX_FLAGS "-O2 ${CMAKE_CXX_FLAGS}")
else()
    message(STATUS "Create Debug Build.")
endif()

set(CMAKE_CXX_FLAGS "-fPIE -fPIC -std=c++11 -D_FORTIFY_SOURCE=2 -fstack-protector -Wformat -Wformat-security -Wall ${CMAKE_CXX_FLAGS}")

add_message_files(
    FILES
    IMUInfo.msg
    Extrinsics.msg
    )

generate_messages(
    DEPENDENCIES
    sensor_msgs
    std_msgs
    )

include_directories(
    include
    ${catkin_INCLUDE_DIRS}
    )

# Generate dynamic reconfigure options from .cfg files
generate_dynamic_reconfigure_options(
  cfg/base_d400_params.cfg
  cfg/sr300_params.cfg
  cfg/rs415_params.cfg
  cfg/rs435_params.cfg
)

# RealSense ROS Node
catkin_package(
    LIBRARIES ${PROJECT_NAME}
    CATKIN_DEPENDS message_runtime roscpp sensor_msgs std_msgs librealsense2
    nodelet
    cv_bridge
    image_transport
    dynamic_reconfigure
    )

add_library(${PROJECT_NAME}
    include/constants.h
    include/realsense_node_factory.h
    include/base_realsense_node.h
    include/rs415_node.h
    include/rs435_node.h
    include/sr300_node.h
    src/realsense_node_factory.cpp
    src/base_realsense_node.cpp
    src/rs415_node.cpp
    src/rs435_node.cpp
    src/sr300_node.cpp
    )
set(REALSENSE_TEST_TARGET ${PROJECT_NAME}_tests)
add_library(${REALSENSE_TEST_TARGET} src/python_module.cpp)
target_link_libraries(${REALSENSE_TEST_TARGET} PRIVATE ${PROJECT_NAME} ${PYTHON_LIBRARIES})
target_include_directories(${REALSENSE_TEST_TARGET} PRIVATE ${PYBIND11_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS} ./include)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_generate_messages_cpp)
add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_gencfg)


target_link_libraries(${PROJECT_NAME}
    realsense2
    ${catkin_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    )

# Install nodelet 
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    )

# Install header files
install(DIRECTORY include/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
    )

# Install launch files
install(DIRECTORY launch/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
    )

# Install rviz files
install(DIRECTORY rviz/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/rviz
    )

# Install xml files
install(FILES nodelet_plugins.xml
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
    )

# Install realsense test target where it can be picked up by python
install(TARGETS ${REALSENSE_TEST_TARGET} 
        LIBRARY DESTINATION "/usr/lib/python2.7/dist-packages/brain"
        COMPONENT shining_software
        PERMISSIONS OWNER_READ
                    OWNER_WRITE
                    OWNER_EXECUTE
                    GROUP_READ
                    GROUP_EXECUTE
                    WORLD_READ
                    WORLD_EXECUTE
    )
