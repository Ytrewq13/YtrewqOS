cmake_minimum_required(VERSION 3.13)

# Name of the project
project(YtrewqOS C)

enable_language(ASM)

# TODO: pick a better name for the executable - it isn't a kernel
set(DISKIMG kernel.img)
set(BINNAME kernel.bin)
set(ELFNAME kernel.elf)
set(KERNEL kernel.o)
set(BOOTLOADER boot.o)
set(BOOT32LOADER boot32.o)
set(BOOT64LOADER boot64.o)
set(BOCHSRC bochsrc)
set(IMGSCRIPT createimg.sh)
set(DEBUG_SYMBOLS_FILE debug.sym)

#set(THREADS_PREFER_PTHREAD_FLAG ON)

# The `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_ASM_FLAGS "${ASM-FLAGS} -g")
else()
    set(CMAKE_ASM_FLAGS "${ASM-FLAGS}")
endif()

# Include local directories
include_directories(include) # "<project root>/include/"

set(SOURCES "src/kernel/kernel.c")
set(BOOTSOURCE "src/boot/boot.s")
set(BOOT32SOURCE "src/boot/boot32.s")
set(BOOT64SOURCE "src/boot/boot64.s")
set(DEBUG_DIR "${CMAKE_SOURCE_DIR}/src/debug")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/src/tools")
set(LINKER_SCRIPT "${TOOLS_DIR}/linker.ld") # The linker script
set(LINK_SCRIPT "${TOOLS_DIR}/link.sh") # The script that runs the linker
set(GDB_SCRIPTFILE "${DEBUG_DIR}/remote-debug.gdb")
set(BOCHSRC_BLANK "${DEBUG_DIR}/bochsrc")
set(GDB_DEBUG_SCRIPT "${TOOLS_DIR}/gdb-debug.sh")
set(IMGSCRIPT_SRC "${TOOLS_DIR}/createimg.sh")

set(IMG_CYLINDERS 203)
set(IMG_HEADS 16)
set(IMG_SECTORS 63)
set(SECTOR_SIZE 512)

# Create the targets
add_custom_target(${BINNAME} objcopy -O binary ${ELFNAME} ${BINNAME}
    BYPRODUCTS ${BINNAME})
add_executable(${ELFNAME})
add_library(${KERNEL} OBJECT ${SOURCES})
add_library(${BOOTLOADER} OBJECT ${BOOTSOURCE})
add_library(${BOOT32LOADER} OBJECT ${BOOT32SOURCE})
add_custom_target(run qemu-system-x86_64 -drive format=raw,file=${BINNAME})
add_custom_target(debug
    ${GDB_DEBUG_SCRIPT} ${BINNAME} ${ELFNAME} ${GDB_SCRIPTFILE}
    SOURCES ${GDB_DEBUG_SCRIPT} ${GDB_SCRIPTFILE})
add_custom_target(debug-bochs bochs -q -f ${BOCHSRC}
    DEPENDS ${DEBUG_SYMBOLS_FILE} ${BOCHSRC} ${DISKIMG})
add_custom_target(distclean rm -rf ${CMAKE_BINARY_DIR}/*)
add_custom_target(disassemble
    objdump -D -Mi8086,addr16,data16 -b binary -mi386 ${BINNAME} |
    less)

add_custom_command(OUTPUT ${DISKIMG}
    COMMAND ${CMAKE_BINARY_DIR}/${IMGSCRIPT} ${BINNAME} ${DISKIMG}
    DEPENDS ${IMGSCRIPT} ${BINNAME})

add_custom_command(OUTPUT ${BOCHSRC}
    COMMAND sed 's/@DISKIMG@/${DISKIMG}/g\;
            s/@DEBUG_SYMBOLS_FILE@/${DEBUG_SYMBOLS_FILE}/g'
            ${BOCHSRC_BLANK} > ${BOCHSRC})
add_custom_command(OUTPUT ${DEBUG_SYMBOLS_FILE}
    COMMAND nm ${ELFNAME} | grep -i ' T ' |
            cut -d' ' -f1,3 > ${DEBUG_SYMBOLS_FILE}
    DEPENDS ${ELFNAME})
add_custom_command(OUTPUT ${IMGSCRIPT}
    COMMAND sed 's/@SECTOR_SIZE@/${SECTOR_SIZE}/g' ${IMGSCRIPT_SRC} > ${IMGSCRIPT}
    COMMAND chmod --reference=${IMGSCRIPT_SRC} ${IMGSCRIPT}
    DEPENDS ${IMGSCRIPT_SRC})

set_target_properties(${ELFNAME} PROPERTIES
    LINK_DEPENDS "${LINKER_SCRIPT}"
    RULE_LAUNCH_LINK "${LINK_SCRIPT} <OBJECTS>")
target_link_options(${ELFNAME} PRIVATE -T${LINKER_SCRIPT})

set_property(DIRECTORY ${CMAKE_BINARY_DIR}
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES bx_enh_dbg.ini)

# Compilation targets
add_dependencies(${ELFNAME} ${BOOTLOADER} ${BOOT32LOADER} ${KERNEL})
add_dependencies(${BINNAME} ${ELFNAME})

# Running & debugging targets
add_dependencies(run ${BINNAME})
add_dependencies(debug ${BINNAME} ${ELFNAME})
add_dependencies(disassemble ${BINNAME})

# This both links and includes the required libraries
target_link_libraries(${ELFNAME} PUBLIC ${BOOTLOADER} ${BOOT32LOADER} ${KERNEL})

# This is so we get a compile commands JSON file (used by ALE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
