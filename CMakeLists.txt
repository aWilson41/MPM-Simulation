cmake_minimum_required(VERSION 3.0.2)
 
PROJECT(MPMSim)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles")
#message(WARNING "${CMAKE_SOURCE_DIR}/CMakeFiles/")

# Glob all the files together for use
file(GLOB MAIN_FILES *.h *.cpp)

# Find the packages
add_subdirectory(GLEngine)

add_executable(MPMSim WIN32 ${MAIN_FILES})
target_include_directories(MPMSim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(MPMSim PUBLIC GLEngine)
add_definitions(-DUSEEIGEN)

if(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    # Turn off debug console for release config
    set_target_properties(MPMSim PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(MPMSim PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
	# Enable parallel build for msvc
	add_definitions(/MP)
  endif()
endif()