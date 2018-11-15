cmake_minimum_required(VERSION 3.0.2)
 
PROJECT(MPMSim)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles")
#message(WARNING "${CMAKE_SOURCE_DIR}/CMakeFiles/")

# Glob all the files together for use
file(GLOB H_FILES *.h)
file(GLOB CPP_FILES *.cpp)
file(GLOB SHADER_FILES *glsl)
file(GLOB IMAGERSC *png)
file(GLOB QRC_FILES *.qrc)
file(GLOB ENGINE_FILES Engine/*h Engine/*cpp)

# Find all the packages and stuff
find_package(Qt5Widgets REQUIRED QUIET)
find_path(GLM 
	NAMES "/glm" 
	PATHS ENV PATH)
find_package(OpenGL REQUIRED)
find_package(Eigen3 3.3 REQUIRED)
find_path(EigenTest REQUIRED)

# Tell CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
#include the directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${GLM}/glm ${OPENGL_INCLUDE_DIRS} ${EigenTest})

qt5_add_resources(RESSrcs ${QRC_FILES})

add_executable(MPMSim WIN32 ${CPP_FILES} ${H_FILES} ${RESSrcs} ${IMAGERSC} ${SHADER_FILES} ${ENGINE_FILES})

target_link_libraries(MPMSim 
	debug Qt5::Widgets Eigen3::Eigen optimized Qt5::Widgets Eigen3::Eigen
	debug ${OPENGL_LIBRARIES} Eigen3::Eigen optimized ${OPENGL_LIBRARIES} Eigen3::Eigen)

if(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    # Turn off debug console for release config
    set_target_properties(MPMSim PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
  endif()
endif()

# Sort these into folders/filters
source_group("Engine" FILES ${ENGINE_FILES})
source_group("Resources" FILES ${IMAGERSC})
source_group("Resources\\Shaders" FILES ${SHADER_FILES})