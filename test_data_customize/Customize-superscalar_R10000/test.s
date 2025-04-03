.set noat
	.text
	.align	2
	.globl	__start
	.ent	__start
	.type	__start, @function
__start:
    addi $20, $0, 4
    
loop_start:
    addi $3, $3, 45
    addi $7, $7, 12
    addi $11, $11, 78
    addi $15, $15, 23

    addi $4, $4, 91
    addi $8, $8, 34
    addi $12, $12, 56
    addi $16, $16, 89

    andi $1, $1, 15
    ori $5, $5, 240
    addi $9, $9, 51
    andi $13, $13, 63

    addi $2, $2, 67
    addi $6, $6, 43
    addi $10, $10, 29
    addi $14, $14, 88

    ori $17, $17, 127
    addi $21, $21, 45
    addi $25, $25, 91
    andi $29, $29, 31

    addi $18, $18, 15
    ori $22, $22, 192
    andi $26, $26, 127
    addi $30, $30, 85

    sw $9, 0x4000($0)
    sw $3, 0x5000($0)
    sw $7, 0x6000($0)
    sw $1, 0x7000($0)

    ori $4, $4, 170
    addi $24, $24, 153
    andi $28, $28, 252
    addi $1, $1, 94

    beq $3, $7, label1
    bne $11, $4, label2
    beq $15, $8, label3
    bne $2, $6, label4

label1:
    addi $2, $2, 45
    addi $6, $6, 170
    ori $10, $10, 240
    andi $14, $14, 15

label2:
    addi $4, $4, 51
    addi $8, $8, 82
    ori $12, $12, 192
    andi $16, $16, 63

label3:
    addi $5, $5, 67
    addi $9, $9, 153
    andi $13, $13, 252
    ori $17, $17, 170

label4:
    addi $18, $18, 85
    addi $22, $22, 73
    ori $26, $26, 127
    andi $30, $30, 31

    addi $20, $20, -1
    bne $20, $0, loop_start
    nop
    nop

    lw $19, 0x4000($0)
    lw $23, 0x5000($0)
    lw $27, 0x6000($0)
    lw $31, 0x7000($0)

	.end	__start
	.size	__start, .-__start

