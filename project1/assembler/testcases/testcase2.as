        .fill   	8            First number
        .fill   	3            Second number
        .fill   	0            Memory location to store the result
start   lw      	0       	1       	0        reg1 = 8 
        lw      	0       	2       	1        reg2 = 3 
        nor     	1       	2       	3        reg3 = ~(reg1 | reg2)
        nor     	3       	3       	3        reg3 = ~(reg3 | reg3) - Double nor to simulate NOT operation
        add     	3       	2       	3        reg3 = reg3 + reg2 = -reg1 + reg2 
        sw      	0       	3       	2        Store the result (reg3) in memory at location 2 (Should store 5)
        halt                             Stop the program
