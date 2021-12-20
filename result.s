.data
string0: .asciiz"a = "

.globl main
.text
main: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

li $t0, 1

 sw $t0 , 4($sp)

li $t0, 3

 sw $t0 , 8($sp)

lw $t0, 4($sp)

addi $t0 , $t0 , 1 
sw $t0,4($sp)

lw $t0, 4($sp)

li $t1, 1

add $t0, $t0, $t1

sw $t0 , 4($sp)


la $a0, string0
li $v0, 4
syscall

lw $t0, 4($sp)

move $a0, $t0
li $v0, 1
syscall

lw $ra, 0($sp)
addiu $sp, $sp, 12
jr $ra

