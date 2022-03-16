# Interim report notes

## 2021-10

* There are lots more areas of work than I anticipated (e.g. eMMC driver,
  SD driver, block devices, etc. all required before I can start work on the
  file system). I need to reorganise the Gantt chart (and probably make my
  aims more realistic).
  * Threads / multithreading won't happen.
  * Init system & service manager won't be completed (maybe I can make an init
    system?).
  * `man` won't be implemented.
  * Memory management & process control will be ported / "inspired" by other
    projects (`malloc()` is too complicated & I can't create my own process
    control method).
* The shell is going to have to wait until I implement several other parts of
  the kernel. I need an ELF loader before I'll be able to make the shell its
  own program.
  * Debug shell isn't needed (I can use `GDB` / print statements for
    debugging).

## 2021-11-21

* Exceptions result in a trap going to $0x200$, not the address $0x8$ in a
  small exception vector table. I think there is a system register that
  controls the exception vector table start address for each EL.
* *Variadic* functions use the scalar / vector / floating - point registers
  (`q0` - `q31`), so they need `CPTR_EL2.FPEN` bits to be set.

# File System

## FAT

* [FAT specification](https://wiki.osdev.org/FAT)
* [ExFAT official specification](https://docs.microsoft.com/en-us/windows/win32/fileio/exfat-specification)

# Executable file formats

* ELF vs PE
  * ELF is better documented
  * PE has too many Windows-specific details
* Raw binary?
  * Everything statically compiled
  * Larger binary sizes (not really important)
  * Easy to extract from statically linked ELF binaries (`objcopy`)
    * Tools exist to convert dynamically linked ELF binaries into statically
    linked ones (at least for Linux)
    * All binaries would be compiled from source, so just compile statically to
    start with
  * Implement raw binary loading first (i.e. a stopgap solution until ELF is
  supported in a future update)

# 2022-03-10 supervisor update

## My progress

* Implemented the base for reading `dirent`s from the SD card (or any `exfat`
file system)
  * `dirent` structure was taken from one of the rpi bootloader projects I
  copied code from

## Plans for the project

* I will start writing the dissertation this weekend
  * Copy the structure of the document from the interim report
  * I will work on it for a few hours every week (Friday afternoons)
* I will use the FS code to implement a `getdents` syscall
* I will write a basic "shell" which will run as part of the kernel
  * Not truly a shell, just a command-line interface to functions which will
  implement some basic programs (e.g. `ls`, `cat`, `tree`, `pwd`, `cd`, maybe
  `ed`)
* Maybe try to implement a binary file loader (or port an ELF loader?)
  * Then move the shell functions into their own executables
  * Create an executable shell which handles user interaction and calling
  executables (create a `/bin/` directory)

## What I won't implement (and I'll explain why not in the dissertation)

* Virtual memory
  * Memory "ownership"
  * Page tables & MMU
* Context switching
  * Timer interrupts
  * Multi-tasking
* File ownership
  * Exfat doesn't support this anyway

## Notes

* Bootstrapping through various stages of OS dev
  * Batch processing OSes (skipped)
  * Single-tasking OS with built-in shell and utilities
  * Single-tasking OS with binary shell and utilities (binary loader built-in
  to kernel)
  * Single-tasking OS with static ELF shell and utilities (static ELF loader as
  raw binary, binary loader in kernel)
  * Multi-tasking OS (probably won't reach this stage)
* Rename dissertation - historically informed approach to OS dev
  * Maybe use this as a subtitle
