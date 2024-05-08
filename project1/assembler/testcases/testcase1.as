        .fill   	5            First number
        .fill   	3            Second number
        .fill   	0            Memory location to store the result
start   lw      	0       	1       	0        reg1 = 5 
        lw      	0       	2       	1        reg2 = 3 
        add     	1       	2       	3        reg3 = reg1 + reg2 
        sw      	0       	3       	2        Store the result (reg3) in memory at location 2
        halt                             
