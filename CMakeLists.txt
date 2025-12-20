cmake_minimum_required(VERSION 4.0)
set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")
project(emerald_template CXX)

# Turn off optimization globally for all build types
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Wall -Wno-pointer-arith")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wno-implicit-int -Wno-pointer-arith -fpermissive")

set(CMAKE_C_STANDARD 99)

if(CMAKE_HOST_WIN32)
    set(MAKE_CMD wsl make)
else()
    set(MAKE_CMD make)
endif()

# Determine architecture extension
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_EXT "x64")
else()
    set(ARCH_EXT "x86")
endif()

## ## ## ## ## ## ## ## ## ## ## ##
## SETUP YOUR .spec AND DLL HERE ##
## ## ## ## ## ## ## ## ## ## ## ##
set(LOADER_SPEC "loader.spec")
set(DLL_PATH "crystal-palace/demo/test.${ARCH_EXT}.dll")

# Build libtcg (the loader example uses this)
add_custom_target(libtcg_make
        COMMAND ${MAKE_CMD}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tcg/libtcg
        COMMENT "Running make in tcg/libtcg"
)

# Build the demo dlls and .os to support default DLL and dprintf calls
add_custom_target(demo_make
        COMMAND ${MAKE_CMD}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/crystal-palace/demo
        COMMENT "Running make in crystal-palace/demo"
)

# Check if debug.h exists first to avoid errors
if(EXISTS "${CMAKE_SOURCE_DIR}/debug.h")
    file(SIZE "${CMAKE_SOURCE_DIR}/debug.h" DEBUG_H_SIZE)
else()
    set(DEBUG_H_SIZE 0)
endif()

# Check for debug.h header
if(DEBUG_H_SIZE LESS_EQUAL 10)
    message(WARNING "debug.h is empty or does not exist!")

    # Check for python version
    find_package(Python3 COMPONENTS Interpreter)

    if(Python3_Interpreter_FOUND)
        execute_process(
                # setup debug.h
                COMMAND ${Python3_EXECUTABLE} loader.py ${DLL_PATH}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    else()
        message(FATAL_ERROR "Python3 is required to generate debug.h but was not found.")
    endif()
else()
    # debug.h header already contains something
    message(STATUS "debug.h has content (size: ${DEBUG_H_SIZE} bytes)")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    # Create the output directory for the custom object
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/debug-dependencies/bin")

    # Build the debug binary
    add_custom_command(
            OUTPUT "${CMAKE_SOURCE_DIR}/debug-dependencies/bin/debug.${ARCH_EXT}.o"
            COMMAND ${CMAKE_C_COMPILER} -c debug.c -o bin/debug.${ARCH_EXT}.o
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/debug-dependencies"
            DEPENDS "${CMAKE_SOURCE_DIR}/debug-dependencies/debug.c"
            COMMENT "Compiling debug-dependencies/debug.c"
    )

    # Set the debug objects. Crystal Palace supports printf debugging, this setup
    # allows us to keep the same function calls in our code for .exe debugging as well
    set(DBG_OBJS
            ${CMAKE_SOURCE_DIR}/debug-dependencies/bin/debug.${ARCH_EXT}.o
    )
    set(TCG_OBJS
            ${CMAKE_SOURCE_DIR}/tcg/libtcg/bin/loaddll.${ARCH_EXT}.o
            ${CMAKE_SOURCE_DIR}/tcg/libtcg/bin/util.${ARCH_EXT}.o
            ${CMAKE_SOURCE_DIR}/tcg/libtcg/bin/resolve_eat.${ARCH_EXT}.o
    )

    # Mark as generated so CMake doesn't fail if they don't exist yet
    set_source_files_properties(${DBG_OBJS} ${TCG_OBJS} PROPERTIES GENERATED TRUE)

    add_executable(emerald_template main.cpp ${TCG_OBJS})

    # Set the entry point for the CRT
    target_link_options(emerald_template PRIVATE "-Wl,-e,WinMain -fpermissive")

    # Add -D_DEBUG
    target_compile_definitions(emerald_template PRIVATE _DEBUG)

else()
    add_library(emerald_template OBJECT main.cpp)

    # Disable optimizations specifically for emerald_template object library
    target_compile_options(emerald_template PRIVATE -O0)

    # Create the bin directory
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

    # Define the output filename based on architecture
    set(OUTPUT_OBJ "loader.${ARCH_EXT}.o")

    # Get the actual object file path from the build directory
    set(OBJ_FILE "${CMAKE_BINARY_DIR}/CMakeFiles/emerald_template.dir/main.cpp.obj")

    if(CMAKE_HOST_WIN32)
        # Convert Windows path to WSL path format
        string(REGEX REPLACE "^([A-Za-z]):" "/mnt/\\1" WSL_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
        string(REPLACE "\\" "/" WSL_SOURCE_DIR "${WSL_SOURCE_DIR}")
        # Make drive letter lowercase for WSL
        string(TOLOWER "${WSL_SOURCE_DIR}" WSL_SOURCE_DIR)

        # Create a custom target that copies and links - this becomes the main build target
        add_custom_target(build_loader ALL
                COMMAND ${CMAKE_COMMAND} -E copy "${OBJ_FILE}" "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_OBJ}"
                COMMAND wsl bash -lc "cd ${WSL_SOURCE_DIR} &&  ${WSL_SOURCE_DIR}/crystal-palace/link ${WSL_SOURCE_DIR}/${LOADER_SPEC} ${WSL_SOURCE_DIR}/bin/${OUTPUT_OBJ} ${WSL_SOURCE_DIR}/bin/loader.${ARCH_EXT}.o"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Copying object file and linking final loader"
                DEPENDS emerald_template
        )
    else()
        add_custom_target(build_loader ALL
                COMMAND ${CMAKE_COMMAND} -E copy "${OBJ_FILE}" "${CMAKE_SOURCE_DIR}/bin/${OUTPUT_OBJ}"
                COMMAND crystal-palace/link ${LOADER_SPEC} bin/${OUTPUT_OBJ} bin/loader.${ARCH_EXT}.o
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Copying object file and linking final loader"
                DEPENDS emerald_template
        )
    endif()
endif()

