	lw 	0 	1 	one
 	lw 	0 	1 	two
	lw	0	1	three
	lw	0	1	four
	add	1	0	2
 	nor 	1 	2 	3
 	add 	3 	1 	4
 	add 	2 	1	1
 	nor 	4 	1 	5
 	beq 	2 	2 	sth
 	beq 	2 	4 	end
sth 	sw 	0 	4 	two
 	noop
	noop
 	add 	5 	6 	7
end	lw	0	1	two
 	halt
one 	.fill 	1
two	.fill	2
three	.fill 	3
four	.fill	4
