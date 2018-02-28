# Toy Virtual Machine

This is work in progress and by no means complete or comprehensive tested.  
I'm shure there are a lot of bugs in the code.

This file gives a brief overview of the architecture and workings of the VM

**Things that are planned:**  
1. ~~Assembler support~~
2. Verilog implementation (minus the multiplication, probably)


## Overview

The VM consists of
- `128kB` of memory (64k instructions)  
- `27 Core instructions` (+ one halt instruction)
- `16 Registers` (13 General Purpose, 3 Special: stack pointer, status register, program counter)
- `7 addressing modes`
- `port mapped io` with 16 different ports

# Assembler
Documentation will be added soon...

# Architecture
## Memory
Memory consists of 65536 16-bit words (128kB).  
Execution starts at 0x100, the stack is initialized to 0xEEEE.  
The program stops when Address `0xFFFF` is reached or a `HLT` instruction is executed

## Register
The VM consists of 16 Registers.  
`R0 through R12` are general purpose.  
`R12` may also be addressed with `SB` and used as stack base.  
Register `13` is the stack pointer register (`SP`).  
Register `14` is the status register (`SR`).  
Register `15` is the program counter (`PC`).

## Address Modes
The address mode bits (as/ad for source/destination respectively) select the address mode used. `n` is the selected register (src/dst bits).
In some cases an extra word, following the instruction word is used (`X`).  

    | am   | addressing | type     
    --------------------------------------------
    | 0b00 | Rn         | register 
    --------------------------------------------
    | 0b01 | X+Rn       | indexed / array
    --------------------------------------------
    | 0b10 | MEM[Rn]    | dereferenced register
    --------------------------------------------
    | 0b11 | MEM[X+Rn]  | dereferenced index
    --------------------------------------------

When `n` is set to `SR`, the following table is used:

    | am   | addressing | type     
    --------------------------------------------
    | 0b00 | SR         | status register
    --------------------------------------------
    | 0b10 | X          | immediate 
    --------------------------------------------
    | 0b01 | PC+X       | relative
    --------------------------------------------
    | 0b11 | MEM[X]     | indirect
    --------------------------------------------

## Instructions
There are three different instruction types:
- Single Operand instructions with one operand 
- Dual Operand instructions with two operands
- Branching Instructions

### Dual OP
Dual Operand instruction operate on two register. These instructions have the following layout:  

    -----------------------------------------------------------------
    | F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    -----------------------------------------------------------------
    | 1 |    opc    |  ad   |  as   |      dst      |      src      |
    -----------------------------------------------------------------

The following instructions are supported:  
    `MOV`, `ADD`, `SUB`, `MUL`, `AND`, `OR`, `XOR`, `SHRR`

### Single OP
Single Operand instructions operan only on one register.  
The layout is as follows:

    -----------------------------------------------------------------
    | F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    -----------------------------------------------------------------
    | 0 | 0 |      opc      |     port      |  as   |      src      |
    -----------------------------------------------------------------

The following instructions are supported:

`HLT`,
`PUSH`,
`POP`, 
`CALL`, 
`RET`, 
`INC`, 
`DEC`, 
`INV`, 
`TST`, 
`SHR`, 
`SHL`, 
`OUT`, 
`IN`,

### Branching Instructions 
These are used to branch to different parts of the code and use the status register to check the conditions.  
The address can either be a signed 10bit offset or an absolute address.

The following format is used:

    -----------------------------------------------------------------
    | F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    -----------------------------------------------------------------
    | 0 | 1 | condition | w |        10bit signed offset            |
    -----------------------------------------------------------------

If w is set, the offset will be ignored and the following word will be used as absolute address.

The following branch operations are possible:

`JMP`, 
`JN`, 
`JGE`, 
`JL`, 
`JNE` / `JNZ`, 
`JEQ` / `JZ`, 
`JNC`, 
`JC`, 

## Input / Output

The VM features port mapped io with the instructions `IN` and `OUT`.  
Up to 16 ports may be used. Port 0 is the stdin/stdout port.

