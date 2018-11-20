cmake_minimum_required(VERSION 3.0.2)
 
PROJECT(MPMSim)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles")
#message(WARNING "${CMAKE_SOURCE_DIR}/CMakeFiles/")

# Glob all the files together for use
file(GLOB H_FILES *.h)
file(GLOB CPP_FILES *.cpp)
file(GLOB SHADER_FILES Engine/Shaders/*glsl)
file(GLOB IMAGERSC *png)
file(GLOB ENGINE_FILES Engine/*h Engine/*cpp)

# Find all the packages and stuff
find_package(OpenGL REQUIRED)
find_path(GLM 
	NAMES "/glm" 
	PATHS ENV PATH)
find_path(GLEW 
	NAMES "/include/GL/glew.h"
	PATHS ENV PATH)
find_path(GLFW
	NAMES "/include/GLFW"
	PATHS ENV PATH)
find_path(EIGEN
	NAMES "/signature_of_eigen3_matrix_library"
	PATHS ENV PATH)

#include the directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${GLM}/glm ${GLEW}/include ${GLFW}/include ${EIGEN} ${OPENGL_INCLUDE_DIRS})

add_executable(MPMSim WIN32 ${CPP_FILES} ${H_FILES} ${RESSrcs} ${IMAGERSC} ${SHADER_FILES} ${ENGINE_FILES})

target_link_libraries(MPMSim 
	debug ${GLEW}/lib/Release/x64/glew32.lib optimized ${GLEW}/lib/Release/x64/glew32.lib
	debug ${GLFW}/lib/glfw3d.lib optimized ${GLFW}/lib/glfw3.lib
	debug ${OPENGL_LIBRARIES} optimized ${OPENGL_LIBRARIES})

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

# Add a build command to copy the shaders to the build directory post build
add_custom_command(
	TARGET MPMSim POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/Engine/Shaders
			${CMAKE_CURRENT_BINARY_DIR}/Shaders)
			
# Sort these into folders/filters
source_group("Engine" FILES ${ENGINE_FILES})
source_group("Resources" FILES ${IMAGERSC})
source_group("Engine\\Shaders" FILES ${SHADER_FILES})