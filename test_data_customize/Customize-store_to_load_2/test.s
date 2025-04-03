.set noat
	.text
	.align	2
	.globl	__start
	.ent	__start
	.type	__start, @function
__start:
    addi $31, $0, 10
loop:
    addi $1, $0, 100
    addi $2, $0, 12768
    addi $4, $0, 12768
    addi $6, $0, 12768
    addi $8, $0, 12768
    sw $1, 0($2)
    addi $1, $1, 50
    addi $7, $7, 200
    addi $9, $9, 100
    xor $3, $7, $9
    addi $10, $10, 250
    sw $1, 0($2)
    lw $3, 0($4)
    lw $5, 0($6)
    lw $7, 0($8)
    add $9, $3, $7
    addi $31, $31, -1
    bne $31, $0, loop
	.end	__start
	.size	__start, .-__start
