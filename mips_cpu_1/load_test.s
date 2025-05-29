.set noat
	.text
	.align	2
	.globl	__start
	.ent	__start
	.type	__start, @function
__start:
   addi $8, $0, 5
   sw $8, 0x7C0($30)
   lw $20, 0x7C0($30)

   .end __start
