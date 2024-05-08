		lw		0		1		five	reg1 = 5
		lw		0		2		cnt		reg2 = 10
		lw		0		3		neg1	reg3 = -1
		lw		0		4		zero	reg4 = 0
		lw		0		0		zero	reg0 = 0
mult	add		0		1		0		reg1 = reg1 + reg1
		add		2		3		2		reg2 = reg2 - 1
		beq		2		4		done	if reg2 == 0 goto done
		beq		0		0		mult	goto mult(default)
done	halt							Should reg0(result) be 50 (5 * 10)
five	.fill	5
cnt		.fill	10
neg1	.fill	-1
zero	.fill	0
