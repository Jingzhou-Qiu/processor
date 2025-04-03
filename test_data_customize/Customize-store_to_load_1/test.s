.set noat
	.text
	.align	2
	.globl	__start
	.ent	__start
	.type	__start, @function
__start:
    addi $1, $0, 100
    addi $2, $0, 12768
    addi $4, $0, 12768
    sw $1, 0($2)
    addi $1, $1, 50
    addi $7, $7, 200
    addi $1, $1, 25
    addi $8, $8, 150
    addi $9, $9, 100
    addi $1, $1, 75
    addi $10, $10, 250
    addi $6, $6, 125
    sw $1, 0($2)
    lw $3, 0($4)
    add $5, $3, $6
	.end	__start
	.size	__start, .-__start
