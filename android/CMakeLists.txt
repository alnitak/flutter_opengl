# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

# Project-level configuration.
set(PROJECT_NAME "flutter_opengl")
project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This value is used when generating builds using this plugin, so it must
# not be changed.
set(PLUGIN_NAME "flutter_opengl_plugin")

message(${ANDROID_ABI})


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.


add_library(
        # Sets the name of the library.
        ${PLUGIN_NAME}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ../src/ndk/ndk.cpp

        ../src/common.h
        ../src/ffi.cpp
        ../src/Renderer.cpp
        ../src/Shader.cpp
        ../src/uniformQueue.cpp
)
#set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections,--strip-debug -fvisibility=hidden -fuse-ld=lld")


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

			  # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

add_subdirectory(src/glm)
add_library(glm::glm ALIAS glm)

target_link_libraries(
        # Specifies the target library.
        ${PLUGIN_NAME}

       # Links the target library to the log library
       # included in the NDK.
       ${log-lib}

       android

       EGL
#       GLESv1_CM
#       GLESv2
       GLESv3
       glm
# GLESv3 not supported by many devices
)


include_directories( ../src )
include_directories( src/glm/ )
