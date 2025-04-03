.set noat
  .text
  .align 2
  .globl __start
  .ent __start
  .type __start, @function
__start:
Block_A:
   addi $1, $0, 1
   addi $2, $0, 2
   bne $1, $2, Block_D
Block_B:
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
   addi $8, $8, 8
Block_C:
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
   addi $9, $9, 9
Block_D:
   addi $3, $0, 10
   addi $4, $0, 11
   bne $3, $4, Block_F
Block_E:
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
   addi $17, $17, 17
Block_F:
   addi $18, $0, 18
   addi $19, $0, 18
   beq $18, $19, Block_H
Block_G:
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
   addi $25, $25, 25
Block_H:
   addi $27, $0, 27
   addi $28, $0, 28
   bne $27, $28, Block_J
Block_I:
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
   addi $2, $2, 2
Block_J:
   addi $3, $0, 3
   addi $4, $0, 4
   bne $3, $4, Block_L
Block_K:
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
   addi $10, $10, 10
Block_L:
   addi $18, $0, 18
   .end __start
   .size __start, .-__start
