cmake_minimum_required(VERSION 3.13)

# Name of the project
project(YtrewqOS C)

enable_language(ASM-ATT)

# Set the name of the executable target
set(BINNAME kernel.bin)
set(KERNEL kernel.o)
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

set(SOURCES "src/kernel/kernel.c")
set(BOOTSOURCE "src/boot/boot.s")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/src/tools")
set(LINKER_SCRIPT "${TOOLS_DIR}/linker.ld")
set(RUNNER_SCRIPT "${TOOLS_DIR}/run")

# Create the targets
add_executable(${BINNAME})
add_library(${KERNEL} OBJECT ${SOURCES})
add_library(${BOOTLOADER} OBJECT ${BOOTSOURCE})
add_custom_target(run qemu-system-x86_64 -drive format=raw,file=${BINNAME})
# TODO: should check that binary_dir is safe to delete?
add_custom_target(distclean rm -rf ${CMAKE_BINARY_DIR}/*)

set_target_properties(${BINNAME} PROPERTIES
    LINK_DEPENDS "${LINKER_SCRIPT}"
    RULE_LAUNCH_LINK "${CMAKE_SOURCE_DIR}/src/tools/link.sh <OBJECTS>")
target_link_options(${BINNAME} PRIVATE -T${LINKER_SCRIPT})

add_dependencies(${BINNAME} ${BOOTLOADER} ${KERNEL})
add_dependencies(run ${BINNAME})

# This both links and includes the required libraries
target_link_libraries(${BINNAME} PUBLIC ${BOOTLOADER} ${KERNEL})

# This is so we get a compile commands JSON file (used by ALE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
