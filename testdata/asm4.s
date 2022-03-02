.text
    addi $t4, $0, 0
    lui $a0, 0x1001
    addi $t0, $0, 4
    lui $t1, 0x0040
    addi $t1, $t1, 0x0010
    
Loop:
    addi $t0, $t0, -1
    beq $t0, $0, Skip
    sw $t0, 0($a0)
    addi $a0, $a0, 4
    jal Subroutine
    j Loop

Skip:
    addi $v0, $0, 10
    syscall
    
Subroutine:
    addi $t4, $t4, 1
    jr $ra
