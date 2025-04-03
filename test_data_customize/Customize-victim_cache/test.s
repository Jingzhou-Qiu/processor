.set noat
   .text
   .align	2
   .globl	__start
   .ent	__start
   .type	__start, @function
__start:
    addi $5, $0, 10
outer_loop:
    addi $8, $0, 5
    addi $9, $0, 10
    addi $30, $0, 0x4000
    sll $30, $30, 2
    addi $31, $30, 0
    addi $4, $0, 9
    addi $6, $0, 0x800
    sll $6, $6, 4
inner_loop:
    sw $8, 0x7C0($30)
    add $30, $30, $6
    addi $4, $4, -1
    bne $4, $0, inner_loop
    addi $5, $5, -1
    bne $5, $0, outer_loop

    .end	__start
    .size	__start, .-__start
