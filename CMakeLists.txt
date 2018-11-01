cmake_minimum_required(VERSION 3.0.2)
 
PROJECT(MPMSim)

file(GLOB H_FILES *.h)
file(GLOB CPP_FILES *.cpp)
file(GLOB SHADER_FILES *glsl)
file(GLOB IMAGERSC *png)
file(GLOB QRC_FILES *.qrc)
file(GLOB ENGINE_FILES Engine/*h Engine/*cpp)
#file(GLOB LODEPNG_FILES Engine/LodePNG/lodepng.cpp Engine/LodePNG/lodepng.h)


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED QUIET)
find_path(GLM glm)
#find_path(ASSIMP Assimp)
# Include the current directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${GLM}/glm)# ${ASSIMP}/include)

qt5_add_resources(RESSrcs ${QRC_FILES})


add_executable(MPMSim WIN32 ${CPP_FILES} ${H_FILES} ${RESSrcs} ${IMAGERSC} ${SHADER_FILES} ${ENGINE_FILES})# ${LODEPNG_FILES})
#qt5_use_modules(SpringMassTreeSim Core Gui)
target_link_libraries(MPMSim Qt5::Widgets)# ${ASSIMP}/lib/assimp-vc140-mt.lib)

if(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    # Turn off debug console for release config
    set_target_properties(MPMSim PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
	
	# Sort these into folders/filters
	source_group("Engine" FILES ${ENGINE_FILES})
	#source_group("Engine\\LodePNG" FILES ${LODEPNG_FILES})
	source_group("Resources" FILES ${IMAGERSC})
	source_group("Resources\\Shaders" FILES ${SHADER_FILES})
  endif()
endif()