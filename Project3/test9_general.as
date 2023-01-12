     	add 1 3 5
     	add 1 0 1
lab1 	nor 4 3 5
     	sw  0 0 lab1
     	nor 2 3 4
     	sw  3 1 -2
     	lw  0 1 2
     	lw  4 1 -2
lab2 	lw  4 1 lab3
     	lw  1 1 lab1
     	lw  0 2 lab4
lab4 	beq 2 1 5
     	beq 2 1 -5
nop  	beq 1 1 lab2
     	beq 0 1 lab1
     	sw  3 0 nop
     	sw  0 1 lab3
lab3 	noop
     	noop
     	halt
one	.fill	1
two	.fill	2
three	.fill	3

