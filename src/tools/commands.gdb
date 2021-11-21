target remote localhost:1234
tui enable
tui new-layout asm_regs {-horizontal asm 1 reg 1} 2 status 0 cmd 1
layout asm_regs
tui regs general
focus cmd
list
update
refresh
