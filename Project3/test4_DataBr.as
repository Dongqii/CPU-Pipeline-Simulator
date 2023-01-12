	lw	0	1	one
	lw	0	2	two
	nor 	2 	2 	2	
	nor 	5 	5 	5
	nor 	2 	5 	3
	beq 	3 	0 	true
	add 	1 	2 	5
	beq 	0 	0 	end
true 	lw 	0 	1 	one
end 	halt
two 	.fill 	2
one 	.fill 	1
three	.fill	3
