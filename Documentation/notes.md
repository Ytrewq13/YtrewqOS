# Notes from development

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
