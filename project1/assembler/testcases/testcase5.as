start   lw      	0       	1       three    	reg1 = 3 
        lw      	0       	5       dAddr    	reg5 = address of the double function
        jalr    	5       	6               	Call the double function, store return address in reg6
		noop										reg2 should be 6
        halt                            
double  add     	1       	1       	2       reg2 = reg1 + reg1
        jalr    	6       	0                	Return to the address stored in reg6
dAddr	.fill   		double      
three   .fill   		3           
