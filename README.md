# YtrewqOS

My custom homebrew operating system for ARM 64 bit targets (specifically
Raspberry Pi 3 model B).

Currently this is only a bootloader written ARMv8 assembly.

I Have plans to create the beginnings of a microkernel in C.

Name will likely change.

# Compiling

## Requirements

- CMake >= v3.20
- GNU assembler (`aarch-none-elf-gcc-bin`)
- QEMU (aarch64) - for running the OS only

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
