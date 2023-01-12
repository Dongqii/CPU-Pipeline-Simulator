 	lw 	0 	1 	one
  	lw 	0 	2 	two
	lw	0	3	three
  	beq 	2 	3 	1
	beq	1	2	1
  	beq 	3 	3 	after
after  	add 	1 	2 	4
 	lw 	0 	4 	two
 	add 	4 	5 	6
 	add 	4 	5 	6
 	beq 	5 	6 	1
 	halt
one 	.fill 	1
two 	.fill 	2
three	.fill	3
