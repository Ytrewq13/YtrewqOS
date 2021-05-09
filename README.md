# YtrewqOS

My custom homebrew operating system

Currently this is only a bootloader written in AT&T assembly.

I Have plans to create the beginnings of a microkernel in C.

Name will likely change.

# Compiling

## Requirements

- CMake >= v3.20
- GNU assembler
- QEMU (x86_64) - for running the OS only

## Compilation process

```shell
mkdir build
cd build
cmake ..
make
```

Once the "kernel.bin" binary file has been compiled, you can run in using QEMU
by running the following command in the `build` directory.

```shell
make run
```
