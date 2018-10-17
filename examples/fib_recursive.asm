# Sample prog that implements the fibonacci sequence

MOV R0 10
PUSH R0
CALL $FIB
POP R0
OUT R0, 0
MOV PC 0xFFFF

DW 0 #also hlt instruction, safety first

# Recursive fibonacci implementation
FIB:
	# prolog
	PUSH SB
	MOV SB SP
	
	# get argument
	MOV R0 SB
	ADD R0 2
	MOV R1 @R0
	
	# if arg == 0 or arg == 1, return 1
	JZ $FIB_RET1
	DEC R1
	JZ $FIB_RET1
	
	# save registers
	PUSH R0
	PUSH R1
	PUSH R1 # save argument (arg-1 because of dec)
	
	CALL $FIB # call first time
	POP R2
	POP R1
	POP R0
	
	# R1 = arg-2
	DEC R1
	
	# save registers
	PUSH R0	
	PUSH R2
	PUSH R1
	
	CALL $FIB
	POP R2
	POP R1
	POP R0
	
	ADD R1 R2
	MOV @R0 R1
	
	JMP $FIB_EPI
FIB_RET1:	
	MOV @R0 1
	
FIB_EPI:
	MOV SP SB
	POP SB
	RET
# END FIBONACCI