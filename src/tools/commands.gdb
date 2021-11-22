target remote localhost:1234
tui enable
#tui new-layout asm_regs {-horizontal asm 2 regs 1} 2 status 0 cmd 1
tui new-layout asm_regs asm 2 regs 1 status 0 cmd 1
layout asm_regs
tui reg general
focus cmd
list
