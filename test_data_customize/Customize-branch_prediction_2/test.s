.set noat
	.text
	.align	2
	.globl	__start
	.ent	__start
	.type	__start, @function
__start:
   addi $8, $0, 5
   addi $9, $0, 10
   addi $10, $0, 15
   addi $11, $0, 20
   addi $30, $0, 0x4000
   sll $30, $30, 2
   addi $31, $30, 0
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $8, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   addi $30, $30, 0x1000
   sw $9, 0x7C0($30)
   lw $20, 0x7C0($31)
   lw $21, 0x17C0($31)
   beq $20, $21, branch1
   sw $9, 0x8C0($30)
   addi $12, $0, 1
   addi $13, $0, 2
   addi $14, $0, 3
   add $15, $12, $13
   add $16, $13, $14
   sub $17, $15, $12
   add $18, $16, $17
branch1:
   addi $22, $0, 15
   addi $23, $0, 25
   bne $22, $23, branch2
   addi $12, $0, 4
   addi $13, $0, 5
   add $14, $12, $13
   sub $15, $14, $12
   add $16, $15, $13
   addi $17, $0, 6
   add $18, $16, $17
branch2:
   lw $24, 0x27C0($31)
   addi $25, $0, 15
   bne $24, $25, branch3
   addi $12, $0, 7
   addi $13, $0, 8
   add $14, $12, $13
   sub $15, $14, $12
   add $16, $15, $13
   addi $17, $0, 9
   add $18, $16, $17
branch3:
   addi $22, $0, 20
   addi $23, $0, 20
   beq $22, $23, branch4
   addi $12, $0, 10
   addi $13, $0, 11
   add $14, $12, $13
   sub $15, $14, $12
   add $16, $15, $13
   addi $17, $0, 12
   add $18, $16, $17
branch4:
   lw $24, 0x37C0($31)
   addi $25, $0, 20
   bne $24, $25, branch5
   addi $12, $0, 13
   addi $13, $0, 14
   add $14, $12, $13
   sub $15, $14, $12
   add $16, $15, $13
   addi $17, $0, 15
   add $18, $16, $17
branch5:
   lw $22, 0x47C0($31)
   lw $23, 0x7C0($30)
   beq $22, $23, branch6
   sw $24, 0x9C0($30)
   addi $12, $0, 16
   addi $13, $0, 17
   add $14, $12, $13
   sub $15, $14, $12
   add $16, $15, $13
   addi $17, $0, 18
   add $18, $16, $17
branch6:
   lw $24, 0x67C0($31)
   addi $25, $0, 5
   beq $24, $25, branch7
   addi $12, $0, 19
   addi $13, $0, 20
   add $14, $12, $13
   sub $15, $14, $12
   add $16, $15, $13
   addi $17, $0, 21
   add $18, $16, $17
branch7:
	.end	__start
	.size	__start, .-__start
