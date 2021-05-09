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

To compile the kernel, run the following commands inside the `build` directory.

```shell
cmake ..
make
```

## To run the compiled kernel

Run the following command from inside the `build` directory (can be run before
compilation as it will compile the binary before running).

```shell
make run
```
