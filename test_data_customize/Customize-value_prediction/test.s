.set noat
   .text
   .align	2
   .globl	__start
   .ent	__start
   .type	__start, @function
__start:
    addi $5, $0, 2

    addi $30, $0, 0x4000
    sll $30, $30, 2
    addi $31, $30, 0
outer_loop_sw:
    addi $8, $0, 0x1000
    addi $4, $0, 8
inner_loop_sw:
    sw $8, 0x7C0($30)
    add $30, $30, $8
    addi $8, $8, 0x1000
    addi $4, $4, -1
    bne $4, $0, inner_loop_sw
    addi $5, $5, -1
    bne $5, $0, outer_loop_sw

    addi $4, $0, 7
    lw $9, 0x7C0($31)
    add $11, $31, $9
    addi $4, $4, -1
    lw $9, 0x7C0($11)
    add $12, $11, $9
    addi $4, $4, -1
    lw $9, 0x7C0($12)
    add $13, $12, $9
    addi $4, $4, -1
    lw $9, 0x7C0($13)
    add $14, $12, $9
    .end	__start
    .size	__start, .-__start
