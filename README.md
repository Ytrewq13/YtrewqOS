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
