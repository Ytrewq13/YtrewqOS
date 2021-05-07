cmake_minimum_required(VERSION 3.10)

# Name of the project
project(YtrewqOS C)

# Set the name of the executable target
set(BINNAME YtrewqOS)

set(THREADS_PREFER_PTHREAD_FLAG ON)

# The `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED)

#find_package(Threads REQUIRED) # pthread
#find_package(GIF REQUIRED) # lib_gif

# pkg_check_modules(<PREFIX> [REQUIRED] [QUIET] <MODULE> [<MODULE>]*)
# This call creates a special `PkgConfig::<PREFIX>` variable
pkg_check_modules(MODS REQUIRED IMPORTED_TARGET
#    OpenCL
#    fontconfig
#    freetype2
#    imlib2
#    libexif
#    libwebp
#    x11
#    xft
    )

# Use `$ pkgconf --list-all | grep -i <MODULE>` to see if <MODULE> can be loaded
# using pkg-config (the pkg_check_modules() function). If it isn't then you will
# have to use find_package() with the corresponding Find<MODULE> cmake module,
# or (if there isn't one) you will have to hardcode the cflags and libraries for
# linking the executable against.

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include local directories
include_directories(include) # "<project root>/include/"

# File globbing is not recommended
#file(GLOB SOURCES "src/*.c")

# You should do this instead
set(SOURCES
    "src/kernel.c"
    )

add_executable(${BINNAME} ${SOURCES})

# This both links and includes the required libraries
# TODO: Figure out how to compile a flat binary using CMake
# TODO: How to use a linker script?
target_link_libraries(${BINNAME} PUBLIC
    PkgConfig::MODS # Modules from pkg_check_modules()
    )

# This is so we get a compile commands JSON file (used by ALE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
