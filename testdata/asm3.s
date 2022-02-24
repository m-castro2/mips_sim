.text
    lui $a0, 0x1001
    addi $t0, $0, 4
    
Loop:
    addi $t0, $t0, -1
    beq $t0, $0, Skip
    sw $t0, 0($a0)
    addi $a0, $a0, 4
    j Loop

Skip:
    addi $v0, $0, 10
    syscall
