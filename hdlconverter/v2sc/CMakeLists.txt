# The cmake script to build the v2sc java project                      
# To use it, run the following:
# cd /path/to/project
# rm -rf build && mkdir build && cd build
# cmake ..
# make 
# to run java program, execute the following command
# java -jar Hello.jar

cmake_minimum_required(VERSION 3.10)
project(v2sc)

find_package(Java REQUIRED)
include(UseJava)

set(CMAKE_JAVA_EXECUTABLE v2sc)

add_jar(${PROJECT_NAME} 
         SOURCES @jfile.f 
         ENTRY_POINT hdl2SystemC
       )

get_target_property(_jarFile ${PROJECT_NAME} JAR_FILE)
get_target_property(_classDir ${PROJECT_NAME} CLASSDIR)
message(STATUS "Jar file ${_jarFile}")
message(STATUS "Class compiled to ${_classDir}")


