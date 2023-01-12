 	lw 	1 	2 	3
 	sw 	2 	3 	1
 	lw 	3 	4 	5
 	beq 	1 	2 	3
 	noop
 	add 	5 	4 	5
 	noop
 	noop
 	add 	5 	5 	5
 	lw 	1 	2 	3
 	sw 	2 	3 	1
 	lw 	3 	4 	5
 	beq 	0 	0 	1
 	noop
 	sw 	4 	1 	1
 	lw 	3 	4 	5	
 	noop
 	noop
 	sw 	4 	1 	1
	beq	1	1	end
end 	halt
one	.fill	1
two	.fill	2
three	.fill	3
