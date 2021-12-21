.data
string0: .asciiz" \na = "
string1: .asciiz" \nb = "
string2: .asciiz" \na+b = "
string3: .asciiz" \na-b = "
string4: .asciiz" \na*b = "
string5: .asciiz" \na/b = "
string6: .asciiz" \n b%a = "
string7: .asciiz" \na>b = "
string8: .asciiz" \na<b = "
string9: .asciiz" \na>=b = "
string10: .asciiz" \na<=b = "
string11: .asciiz" \nc==b = "
string14: .asciiz" \na==b = "
string17: .asciiz" \ntrue&&true = "

.globl main
.text
main: 
addiu $sp, $sp, -20

sw $ra, 0($sp)

li $t0, 1

 sw $t0 , 4($sp)

li $t0, 3

 sw $t0 , 8($sp)

li $t0, 3

 sw $t0 , 12($sp)

li $t0 , 0
 sw $t0 , 16($sp)


la $a0, string0
li $v0, 4
syscall

lw $t0, 4($sp)

move $a0, $t0
li $v0, 1
syscall


la $a0, string1
li $v0, 4
syscall

lw $t0, 8($sp)

move $a0, $t0
li $v0, 1
syscall


la $a0, string2
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

add $t0, $t0, $t1

move $a0, $t0
li $v0, 1
syscall


la $a0, string3
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

sub $t0, $t0, $t1

move $a0, $t0
li $v0, 1
syscall


la $a0, string4
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

mult $t0, $t1
mflo $t0

move $a0, $t0
li $v0, 1
syscall


la $a0, string5
li $v0, 4
syscall

lw $t0, 8($sp)

lw $t1, 4($sp)

div $t0, $t1
mflo $t0

move $a0, $t0
li $v0, 1
syscall


la $a0, string6
li $v0, 4
syscall

lw $t0, 8($sp)

lw $t1, 4($sp)

div $t0, $t1
mfhi $t0

move $a0, $t0
li $v0, 1
syscall


la $a0, string7
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

sgt $t0, $t0, $t1

move $a0, $t0
li $v0, 1
syscall


la $a0, string8
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

slt $t0, $t0, $t1

move $a0, $t0
li $v0, 1
syscall


la $a0, string9
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

sge $t0, $t0, $t1

move $a0, $t0
li $v0, 1
syscall


la $a0, string10
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

sle $t0, $t0, $t1

move $a0, $t0
li $v0, 1
syscall


la $a0, string11
li $v0, 4
syscall

lw $t0, 12($sp)

lw $t1, 8($sp)

beq $t0, $t1, label12
addi $t0, $zero, 0
 j finalLabel13
label12:
addi $t0, $zero, 1
finalLabel13:

move $a0, $t0
li $v0, 1
syscall


la $a0, string14
li $v0, 4
syscall

lw $t0, 4($sp)

lw $t1, 8($sp)

beq $t0, $t1, label15
addi $t0, $zero, 0
 j finalLabel16
label15:
addi $t0, $zero, 1
finalLabel16:

move $a0, $t0
li $v0, 1
syscall


la $a0, string17
li $v0, 4
syscall

li $t0, 1

li $t1, 1

addi $t0, $zero, 0
beq $t0, $t0, finalLabel19
beq $t1, $t0, finalLabel19
label18:
addi $t0, $zero, 1
finalLabel19:

sw $t0 , 16($sp)

lw $t0, 16($sp)


lw $ra, 0($sp)
addiu $sp, $sp, 20
jr $ra

