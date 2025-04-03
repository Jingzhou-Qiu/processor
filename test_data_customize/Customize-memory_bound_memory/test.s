.set noat
    .text
    .align    2
    .globl    __start
    .ent    __start
    .type    __start, @function
__start:
    addi    $5, $0, 10
    
outer_loop:
    addi    $1, $0, 0x7F8
    sll     $1, $1, 3
    addi    $2, $0, 32
    addi    $3, $0, 1
    addi    $4, $0, 0x800
    sll     $4, $4, 3
    
inner_loop:
    sw      $3, 0($1)
    sll     $3, $3, 1
    addi    $3, $3, 7
    add     $1, $1, $4
    addi    $2, $2, -1
    bne     $2, $0, inner_loop
    addi    $5, $5, -1
    bne     $5, $0, outer_loop
    
    addi    $1, $0, 0x17F8
    sll     $1, $1, 3
    lw      $6, 0($1)
    
endlabel:
    .end    __start
    .size    __start, .-__start
