MAIN:
    PUSH @$WORD
    CALL $POPCOUNT
    POP R0
    OUT R0 0
    HLT

WORD: DW 0x1234

POPCOUNT:
    PUSH SB         #108
    MOV R0 @2[SP]   #109
    MOV R1 0xFFFF   #10B
INC:
    INC R1          #10D
SHIFT:
    SHR R0          #10E
    JC $INC #if carry is set, increment #10F
    JNZ $SHIFT # if number is still not zero, shift again #110
    MOV @2[SP] R1 #111
    POP SB #113
    RET #114