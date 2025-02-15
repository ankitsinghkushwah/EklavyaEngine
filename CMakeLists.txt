cmake_minimum_required(VERSION 3.26.4)
project(EklavyaEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SUPPRESS_ALL_BUILD ON)


# Add an option to enable/disable AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer for memory error detection" OFF)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer is enabled")

    # Enable ASan for Xcode (Clang)
    if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif()

    # Enable ASan for Visual Studio (MSVC)
    if(WIN32 AND MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /fsanitize=address")
    endif()
else()
    message(STATUS "AddressSanitizer is disabled")
endif()

if(APPLE)
link_directories(${CMAKE_SOURCE_DIR}/Externals/libs/macos)
endif()

# Include directories
include_directories(EklavyaEngine
    ${CMAKE_SOURCE_DIR}/EklavyaEngine
    ${CMAKE_SOURCE_DIR}/Externals
    ${CMAKE_SOURCE_DIR}/Externals/include
    ${CMAKE_SOURCE_DIR}/Demos
)

# Add source and header files
file(GLOB_RECURSE SRC_FILES 
    "Demos/*.cpp"
    "EklavyaEngine/*.cpp"
    "Externals/*.c"
    "Externals/*.cpp"
)

file(GLOB_RECURSE HEADER_FILES
    "Demos/*.hpp"
    "Demos/*.h"
    "EklavyaEngine/*.hpp"
    "EklavyaEngine/*.h"
    "Externals/*.h"
    "Externals/*.hpp"
)

# Add the executable
add_executable(EklavyaEngine ${SRC_FILES} ${HEADER_FILES})

# Function to organize files into groups
function(organize_files_into_groups FILES)
    foreach(FILE ${FILES})
        file(RELATIVE_PATH FILE_DIR ${CMAKE_SOURCE_DIR} ${FILE})
        get_filename_component(FILE_DIR ${FILE_DIR} DIRECTORY)
        string(REPLACE "/" "\\" GROUP_NAME ${FILE_DIR})
        source_group(${GROUP_NAME} FILES ${FILE})
    endforeach()
endfunction()

# Function to copy resources
function(copy_resources RESOURCE_DIR DESTINATION_DIR)
    add_custom_command(
        TARGET EklavyaEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR}
        ${DESTINATION_DIR}
    )
endfunction()

add_compile_definitions(EKDEBUG)

# Platform-specific configurations
if(APPLE) 

#copy assets folder
    copy_resources(${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Debug/Resources)
    copy_resources(${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Release/Resources)
    copy_resources(${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Resources)

    target_link_libraries(EklavyaEngine 
        glfw3
        ${CMAKE_SOURCE_DIR}/Externals/libs/macos/libassimpd.5.3.0.dylib
        ${CMAKE_SOURCE_DIR}/Externals/libs/macos/libsfml-audio.2.6.1.dylib
        ${CMAKE_SOURCE_DIR}/Externals/libs/macos/libsfml-system.2.6.1.dylib
        "-framework IOKit"
        "-framework OpenGL"
        "-framework Cocoa"
        "${CMAKE_SOURCE_DIR}/Externals/libs/macos/frameworks/FLAC.framework"
        "${CMAKE_SOURCE_DIR}/Externals/libs/macos/frameworks/freetype.framework"
        "${CMAKE_SOURCE_DIR}/Externals/libs/macos/frameworks/ogg.framework"    
        "${CMAKE_SOURCE_DIR}/Externals/libs/macos/frameworks/vorbis.framework"
        "${CMAKE_SOURCE_DIR}/Externals/libs/macos/frameworks/vorbisenc.framework"
        "${CMAKE_SOURCE_DIR}/Externals/libs/macos/frameworks/vorbisfile.framework"   
    )
elseif(WIN32)

    add_compile_definitions(SFML_STATIC)
    #copy DLLs for build directories for DEBUG and RELEASe
    
    file(COPY ${CMAKE_SOURCE_DIR}/Externals/libs/win32/DLLs/assimp-vc143-mtd.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug)
    file(COPY ${CMAKE_SOURCE_DIR}/Externals/libs/win32/DLLs/openal32.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug)

    #copy assets
    copy_resources(
        ${CMAKE_SOURCE_DIR}/Resources  
        ${CMAKE_BINARY_DIR}/Debug/Resources  
    )
    copy_resources(
        ${CMAKE_SOURCE_DIR}/Resources  
        ${CMAKE_BINARY_DIR}/Release/Resources  
    )
    copy_resources(
        ${CMAKE_SOURCE_DIR}/Resources  
        ${CMAKE_BINARY_DIR}/Resources  
    )

    target_link_libraries(EklavyaEngine
        opengl32.lib
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/glfw3.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/assimp-vc143-mtd.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/openal32.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/flac.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/freetype.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/vorbis.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/vorbisenc.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/vorbisfile.lib"
        "${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/ogg.lib"
        $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/sfml-system-s-d.lib>
        $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/sfml-system-s.lib>
        $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/sfml-audio-s-d.lib>
        $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/Externals/libs/win32/sfml-audio/sfml-audio-s.lib>
    )

endif()

# Organize source and header files
organize_files_into_groups("${SRC_FILES}")
organize_files_into_groups("${HEADER_FILES}")


