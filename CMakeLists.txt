cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

project (GLINT)

if(MSVC)
  add_definitions(/DNOMINMAX)
endif()

if(UNIX)
    message(STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message(STATUS "Setting GLVND option.")
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package( glm CONFIG REQUIRED )
find_package( glfw3 CONFIG REQUIRED )
find_package( OpenGL REQUIRED )
find_package( assimp CONFIG REQUIRED )

include_directories( opengl )
if(APPLE)
	include_directories(/usr/local/include/)
	link_directories(/usr/local/lib/)
endif()

set(SHADER_PATH ${CMAKE_BINARY_DIR}/shader CACHE PATH "Path to shader directory")
set(MEDIA_PATH ${CMAKE_BINARY_DIR}/media CACHE PATH "Path to media directory")

add_subdirectory( opengl )
add_subdirectory( geometric_glint_aa )

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "geometric_glint_aa")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/media DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/geometric_glint_aa/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})