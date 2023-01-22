# The Flutter tooling requires that developers have a version of Visual Studio
# installed that includes CMake 3.14 or later. You should not increase this
# version, as doing so will cause the plugin to fail to compile for some
# customers of the plugin.
cmake_minimum_required(VERSION 3.14)

# Project-level configuration.
set(PROJECT_NAME "flutter_opengl")
project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(POWERVR ${CMAKE_CURRENT_LIST_DIR}/Native_SDK)
set(GLEW ${CMAKE_CURRENT_LIST_DIR}/glew)
set(GLM ${CMAKE_CURRENT_LIST_DIR}/glm)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "flutter_opengl_plugin")

# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "flutter_opengl_plugin.cpp"
  "flutter_opengl_plugin.h"
  "fl_my_texture_gl.cc"
  
  ../src/common.h
  ../src/ffi.cpp
  ../src/Renderer.cpp
  ../src/Shader.cpp
  ../src/uniformQueue.cpp
)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  "include/flutter_opengl/flutter_opengl_plugin_c_api.h"
  "flutter_opengl_plugin_c_api.cpp"
  ${PLUGIN_SOURCES}
)

# Apply a standard set of build settings that are configured in the
# application-level CMakeLists.txt. This can be removed for plugins that want
# full control over build settings.
apply_standard_settings(${PLUGIN_NAME})

# Symbols are hidden by default to reduce the chance of accidental conflicts
# between plugins. This should not be removed; any symbols that should be
# exported should be explicitly exported with the FLUTTER_PLUGIN_EXPORT macro.
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# don't treat warnings as compile errors in this plugin. Itt will be removed
target_compile_options(${PLUGIN_NAME} PRIVATE "/WX-")

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(${PLUGIN_NAME} PUBLIC 
  "${POWERVR}/include"
  "${GLM}"
  "${GLEW}/include"
  )

find_package(OpenGL REQUIRED)
target_link_libraries(${PLUGIN_NAME} PRIVATE 
  flutter 
  flutter_wrapper_plugin
  OpenGL::GL
  "${POWERVR}/lib/Windows_x86_64/libEGL.lib"
  "${POWERVR}/lib/Windows_x86_64/libGLESv2.lib"
  "${GLEW}/lib/Release/x64/glew32.lib"
  "${GLEW}/lib/Release/x64/glew32s.lib"
  )


# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(flutter_opengl_bundled_libraries
  ""
  # "${POWERVR}/lib/Windows_x86_64/libEGL.lib"
  "${POWERVR}/lib/Windows_x86_64/libEGL.dll"
  # "${POWERVR}/lib/Windows_x86_64/libGLESv2.lib"
  "${POWERVR}/lib/Windows_x86_64/libGLESv2.dll"
  "${GLEW}/bin/Release/x64/glew32.dll"
  "${GLEW}/lib/Release/x64/glew32.lib"
  "${GLEW}/lib/Release/x64/glew32s.lib"
  PARENT_SCOPE
)
