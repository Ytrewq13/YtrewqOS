cmake_minimum_required(VERSION 3.13)

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.")

# Name of the project
project(YtrewqOS C)

enable_language(ASM)

set(OBJCOPY_BIN aarch64-none-elf-objcopy)
set(OBJDUMP_BIN aarch64-none-elf-objdump)
set(NM_BIN aarch64-none-elf-nm)

set(QEMU_BIN qemu-system-aarch64)
set(QEMU_ARGS -machine raspi3b)

set(DISASS_ARGS -b binary -m aarch64 -D)

set(SECTOR_SIZE 4096)

# TODO: pick a better name for the executable - it isn't a kernel
set(DISKIMG kernel.img)
set(BINNAME kernel.bin)
set(ELFNAME kernel.elf)
#set(KERNEL kernel.o)
set(BOOTLOADER boot.o)
#set(BOOT32LOADER boot32.o)
#set(BOOT64LOADER boot64.o)
set(IMGSCRIPT createimg.sh)
#set(DEBUG_SYMBOLS_FILE debug.sym) # Used by bochs


#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/src/tools/toolchain.cmake")

# The `pkg_check_modules` function is created with this call
#find_package(PkgConfig REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

# Include local directories
include_directories(include) # "<project root>/include/"

#set(SOURCES "src/kernel/kernel.c")
set(BOOTSOURCE "src/boot/boot.S")
#set(BOOT32SOURCE "src/boot/boot32.S")
#set(BOOT64SOURCE "src/boot/boot64.S")
set(DEBUG_DIR "${CMAKE_SOURCE_DIR}/src/debug")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/src/tools")
set(LINKER_SCRIPT "${TOOLS_DIR}/linker.ld") # The linker script
#set(LINK_SCRIPT "${TOOLS_DIR}/link.sh") # The script that runs the linker
set(GDB_SCRIPTFILE "${DEBUG_DIR}/remote-debug.gdb")
set(GDB_DEBUG_SCRIPT "${TOOLS_DIR}/gdb-debug.sh")
set(IMGSCRIPT_SRC "${TOOLS_DIR}/createimg.sh")

# Create the targets
add_executable(${ELFNAME})
#add_library(${KERNEL} OBJECT ${SOURCES})
add_library(${BOOTLOADER} OBJECT ${BOOTSOURCE})
#add_library(${BOOT32LOADER} OBJECT ${BOOT32SOURCE})

add_custom_target(bin DEPENDS ${BINNAME}) # For creating only the bin (manually)
add_custom_target(run ${QEMU_BIN} ${QEMU_ARGS} -drive format=raw,file=${DISKIMG}
    DEPENDS ${DISKIMG})
add_custom_target(debug
    ${GDB_DEBUG_SCRIPT} ${BINNAME} ${ELFNAME} ${GDB_SCRIPTFILE}
    SOURCES ${GDB_DEBUG_SCRIPT} ${GDB_SCRIPTFILE}
    DEPENDS ${BINNAME} ${ELFNAME})
add_custom_target(distclean rm -rf ${CMAKE_BINARY_DIR}/*)
add_custom_target(disassemble
    ${OBJDUMP_BIN} ${DISASS_ARGS} ${BINNAME} |
    less
    DEPENDS ${BINNAME})

# Commands which assist in creating the target
add_custom_command(OUTPUT ${BINNAME}
    COMMAND ${OBJCOPY_BIN} -O binary ${ELFNAME} ${BINNAME}
    DEPENDS ${ELFNAME})
add_custom_command(OUTPUT ${DISKIMG}
    COMMAND ${CMAKE_BINARY_DIR}/${IMGSCRIPT} ${BINNAME} ${DISKIMG}
    DEPENDS ${IMGSCRIPT} ${BINNAME})

# Put this back if we will want the symbol file.
#add_custom_command(OUTPUT ${DEBUG_SYMBOLS_FILE}
#    COMMAND ${NM_BIN} ${ELFNAME} | grep -i ' T ' |
#            cut -d' ' -f1,3 > ${DEBUG_SYMBOLS_FILE}
#    DEPENDS ${ELFNAME})
add_custom_command(OUTPUT ${IMGSCRIPT}
    COMMAND sed 's/@SECTOR_SIZE@/${SECTOR_SIZE}/g' ${IMGSCRIPT_SRC} > ${IMGSCRIPT}
    COMMAND chmod --reference=${IMGSCRIPT_SRC} ${IMGSCRIPT}
    DEPENDS ${IMGSCRIPT_SRC})

set_target_properties(${ELFNAME} PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")
target_link_options(${ELFNAME} PRIVATE -nostartfiles -T${LINKER_SCRIPT})

# Compilation targets
add_dependencies(${ELFNAME} ${BOOTLOADER})

# This both links and includes the required libraries
target_link_libraries(${ELFNAME} PUBLIC ${BOOTLOADER})

# This is so we get a compile commands JSON file (used by ALE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
