# Sample prog that implements the fibonacci sequence

MOV R0 10  #fib number to calculate
MOV R1 0   #f0
MOV R2 1   #f1

# Save for printing later
PUSH R0

# calculate the number iteratively
FIB:
	ADD R1, R2
	
	#swap values
	PUSH R1
	MOV R1, R2
	POP R2
	
	DEC R0
	JZ $END
	JMP $FIB

# print the result
END:
	PUSH $FIB_STR
	CALL $PRINT_STR
	INC SP
	
	POP R0
	OUT R0, 0
	
	PUSH $FIB_STR2
	CALL $PRINT_STR
	INC SP
	
	OUT R2, 0
	
	MOV PC 0xFFFF

	
# Print routine
PRINT_STR:
	# get argument from stack
	MOV R0 @1[SP] 
PRINT_STR_LOOP:
	MOV R1 @R0
	JZ $PRINT_STR_RET
	OUT R1, 1
	INC R0
	JMP $PRINT_STR_LOOP
PRINT_STR_RET:
	RET

	
# Data
FIB_STR:
	DW "Fibonacci number " 0
FIB_STR2:
	DW " is: " 0