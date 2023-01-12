lab  	lw  	0 	1 	2
     	lw  	0 	1 	flve
The  	lw  	0 	1 	1
     	lw  	0 	1 	lab
     	sw  	0 	1 	lab
     	sw  	0 	1 	live
     	sw  	0 	1 	flve
     	sw  	1 	5 	live
     	sw  	2 	1 	live
     	sw  	2 	1 	The
     	beq 	2 	1 	5
    	beq 	2 	1 	-5
     	beq 	0 	1 	live
     	beq 	0 	1 	flve
    	noop 	2
     	noop
     	halt 	
live 	.fill 	5
flve 	.fill 	-1