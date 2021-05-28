cmake_minimum_required(VERSION 3.13)

# Name of the project
project(YtrewqOS C)

enable_language(ASM-ATT)

# Set the name of the executable target
set(BINNAME kernel.bin)
set(ELFNAME kernel.elf)
set(KERNEL kernel.o)
set(BOOTLOADER boot.o)
set(BOOT32LOADER boot32.o)

set(THREADS_PREFER_PTHREAD_FLAG ON)

# The `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED)

#find_package(Threads REQUIRED) # pthread
#find_package(GIF REQUIRED) # lib_gif

# pkg_check_modules(<PREFIX> [REQUIRED] [QUIET] <MODULE> [<MODULE>]*)
# This call creates a special `PkgConfig::<PREFIX>` variable
#pkg_check_modules(MODS REQUIRED IMPORTED_TARGET)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_ASM-ATT_FLAGS "${ASM-ATTFLAGS} -g")
else()
    set(CMAKE_ASM_FLAGS "${ASM-ATTFLAGS}")
endif()

# Include local directories
include_directories(include) # "<project root>/include/"

set(SOURCES "src/kernel/kernel.c")
set(BOOTSOURCE "src/boot/boot.s")
set(BOOT32SOURCE "src/boot/boot32.s")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/src/tools")
set(LINKER_SCRIPT "${TOOLS_DIR}/linker.ld")
set(GDB_COMMANDFILE "${CMAKE_SOURCE_DIR}/src/debug/remote-debug.gdb")

# Create the targets
add_custom_target(${BINNAME} objcopy -O binary ${ELFNAME} ${BINNAME}
    BYPRODUCTS ${BINNAME})
add_executable(${ELFNAME})
add_library(${KERNEL} OBJECT ${SOURCES})
add_library(${BOOTLOADER} OBJECT ${BOOTSOURCE})
add_library(${BOOT32LOADER} OBJECT ${BOOT32SOURCE})
add_custom_target(run qemu-system-x86_64 -drive format=raw,file=${BINNAME})
add_custom_target(debug qemu-system-x86_64 -s -S -drive format=raw,file=${BINNAME} & gdb ${ELFNAME} -command=${GDB_COMMANDFILE})
add_custom_target(distclean rm -rf ${CMAKE_BINARY_DIR}/*)
add_custom_target(disassemble objdump -D -Mi8086,addr16,data16 -b binary -mi386 ${BINNAME} | less)

set_target_properties(${ELFNAME} PROPERTIES
    LINK_DEPENDS "${LINKER_SCRIPT}"
    RULE_LAUNCH_LINK "${CMAKE_SOURCE_DIR}/src/tools/link.sh <OBJECTS>")
target_link_options(${ELFNAME} PRIVATE -T${LINKER_SCRIPT})

add_dependencies(${ELFNAME} ${BOOTLOADER} ${BOOT32LOADER} ${KERNEL})
add_dependencies(${BINNAME} ${ELFNAME})
add_dependencies(run ${BINNAME})
add_dependencies(debug ${BINNAME} ${ELFNAME})
add_dependencies(disassemble ${BINNAME})

# This both links and includes the required libraries
target_link_libraries(${ELFNAME} PUBLIC ${BOOTLOADER} ${BOOT32LOADER} ${KERNEL})

# This is so we get a compile commands JSON file (used by ALE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
