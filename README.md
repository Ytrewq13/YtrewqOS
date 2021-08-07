# YtrewqOS

My custom homebrew operating system for ARM 64 bit targets (specifically
Raspberry Pi 3 model B).

Currently contains a bootloader in ARMv8 assembly and the beginnings of a
kernel in C (along with a small library for interacting with the hardware).

Name will likely change.

# Compiling

## Requirements

- CMake >= v3.20
- LLVM (clang) >= 11.1.0
- QEMU (aarch64) - for running the OS only
- `gdb-multiarch` - for debugging the boot process

Make sure you *don't* have any of the following installed, as it can cause
problems with linker path resolution:
- ARM 64 GCC

## Compilation process

To compile the kernel, run the following commands inside the `build` directory.

```shell
cmake -DCMAKE_TOOLCHAIN_FILE="../src/tools/toolchain.cmake" ..
make
```

## To run the compiled kernel

Run the following command from inside the `build` directory (can be run before
compilation as it will compile the binary before running).

```shell
make run
```

# Contributing

Before working on a contribution:
- Make sure what you want to contribute isn't already part of the project.
- Check if any open pull requests have already addressed the issue/feature you
  want.

Before submitting a contribution:
- Make sure your code works: test it, thoroughly.
- Properly format your code according to the project's style guide (as
  specified in the provided clang-format style file).

## Applying clang-format to the project

You should use clang-format to autoformat the project files using the format
file for the project. This can be done from the top-level project directory
like so:

```shell
find src/ include/ -name *.c -or -name *.h | xargs -n1 clang-format --style=file -i
```

You can also run clang-format against one or more specific files if you didn't
change much:

```shell
clang-format --style=file -i FILE_PATH [ADDITIONAL_FILE_1 ...]
```
