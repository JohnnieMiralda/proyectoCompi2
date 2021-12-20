.data

.globl main
.text
main: 
addiu $sp, $sp, -12

sw $ra, 0($sp)

li $t0, 5

 sw $t0 , 4($sp)

li $t0, 8

 sw $t0 , 8($sp)

lw $t0, 4($sp)

li $t1, 2

div $t0, $t1
mfhi $t0

li $t1, 0

beq $t0, $t1, label2
addi $t0, $zero, 0
 j finalLabel3
label2:
addi $t0, $zero, 1
finalLabel3:

beqz $t0, else0


j endif1
else0: 
endif1 :

lw $t0, 8($sp)

li $t1, 4

div $t0, $t1
mfhi $t0

li $t1, 0

beq $t0, $t1, label5
addi $t0, $zero, 0
 j finalLabel6
label5:
addi $t0, $zero, 1
finalLabel6:

beqz $t0, endif4


endif4 :

lw $ra, 0($sp)
addiu $sp, $sp, 12
jr $ra

