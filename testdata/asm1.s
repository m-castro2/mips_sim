.text
    lui $9, 0x1000
    addi $8, $0, 25
Loop:
    sll $9, $8, 2
    add $t1, $t1, $s0
    sw $8, 0($9)
    sw $8, 4($9)
    addi $8, $8, -1
    bne $8, $0, Loop

    addi $2, $0, 10
    syscall
