		lw		0		0		zero		reg0 = 0 (temporary)		
		lw		0		1		ten			reg1 = 10
		lw		0		2		eleven		reg2 = 11
		lw		0		3		zero		reg3 = 0
		lw		0		4		zero		reg4 = 0
swap	add		1		3		3			reg3 = reg1 + reg3
		add		2		4		4			reg4 = reg2 + reg4
		add		0		3		2			reg2 = reg3 + reg0
		add		0		4		1			reg1 = reg4 + reg0
		halt								reg1 should be 11 and reg2 should be 10					
zero	.fill	0
ten		.fill	10
eleven	.fill	11
