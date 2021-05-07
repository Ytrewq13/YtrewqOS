cmake_minimum_required(VERSION 3.13)

# Name of the project
project(YtrewqOS C)

enable_language(ASM-ATT)

# Set the name of the executable target
set(BINNAME kernel.bin)
set(BOOTLOADER boot.o)

set(THREADS_PREFER_PTHREAD_FLAG ON)

# The `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED)

#find_package(Threads REQUIRED) # pthread
#find_package(GIF REQUIRED) # lib_gif

# pkg_check_modules(<PREFIX> [REQUIRED] [QUIET] <MODULE> [<MODULE>]*)
# This call creates a special `PkgConfig::<PREFIX>` variable
pkg_check_modules(MODS REQUIRED IMPORTED_TARGET
    )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include local directories
include_directories(include) # "<project root>/include/"

set(SOURCES "src/kernel.c")
set(BOOTSOURCE "src/boot.s")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/src/tools/linker.ld")

# Create the targets
add_executable(${BINNAME} ${SOURCES})
add_library(${BOOTLOADER} OBJECT ${BOOTSOURCE})

set_target_properties(${BINNAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
target_link_options(${BINNAME} PRIVATE -T${LINKER_SCRIPT})

add_dependencies(${BINNAME} ${BOOTLOADER})

# This both links and includes the required libraries
# TODO: Get the linker script working to produce the flat binary
target_link_libraries(${BINNAME} PUBLIC)

# This is so we get a compile commands JSON file (used by ALE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
