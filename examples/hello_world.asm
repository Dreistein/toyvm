# This is a sample program that is supposed to print "Hello World!"


MOV R0, $DATA

START:
	MOV R1, @R0		# fetch char
    JZ $END			# if char is 0, halt
    OUT R1, 1		# else print char
	
    INC R0			# increment address
    JMP $START		# repeat
END:
    HLT

DATA:
    DW "Hello World!" 10 0

