.set noat
	.text
	.align	2
	.globl	__start
	.ent	__start
	.type	__start, @function
__start:
   addi $8, $0, 1001
   sw $8, 24($1)
   addi $9, $9, 5
   #addi $9, $10, 1
   beq $9, $10, label1
   addi $10, $10, 3
   addi $8, $8, 1
   addi $9, $9, 2
label1:
   addi $10, $10, 4
   sll $11, $10, 2
   addi $12, $12, 8
	.end	__start
	.size	__start, .-__start
