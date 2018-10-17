
_START:
    PUSH $MAT_RES
    PUSH $MAT_B
    PUSH $MAT_A
    PUSH @$P_SIZE
    PUSH @$N_SIZE
    PUSH @$M_SIZE
    CALL  $MUL_MAT

    MOV R0 0
    MOV R1 $MAT_RES
    MOV R2 @$N_SIZE # n*m for max index
    MUL R2 @$P_SIZE
    ADD R2 $MAT_RES
    OUTP_LOOP:
        OUT @R1 0
        INC R1
        INC R0
        MOV R3 @$P_SIZE
        SUB R3 R0
        JNZ $AFTER_ROW_END_CHECK
        MOV R4 @$LF
        OUT R4 1
        MOV R0 0
        AFTER_ROW_END_CHECK:
        MOV R3 R2
        SUB R3 R1
        JNZ $OUTP_LOOP
    MOV R0 $MAT_RES
    HLT
# end _START


# calculates C = A x B
# expects m n p a_addr b_addr o_addr on stack
MUL_MAT:
    # prolog
    PUSH SB
    MOV SB SP
    PUSH SB

    # stack pointer to general purpose
    # register for easy addressing
    MOV R0 SB
    #@2[R0] # m
    #@3[R0] # n
    #@4[R0] # p
    #@5[R0] # a_addr
    #@6[R0] # b_addr
    #@7[R0] # o_addr

    # save st base
    # we are using r12


    MOV R3 0    # tmpAdd = 0
    MOV R2 0    # i = 0 to N
    OUTER_LOOP:
        # move values to registers
        MOV R5 @5[R0]   # A_addr[tmpAdd]
        ADD R5 R3
        MOV R6 @7[R0]   # O_addr[tmpAdd]
        ADD R6 R3
        CALL $MUL_ROW
        MOV R3 R3 # ------------------------------
        # update loop variables
        ADD R3 @3[R0] # tmpAdd += N
        INC R2
        MOV R1 @3[R0]
        SUB R1 R2
        JNZ $OUTER_LOOP
    #jmp $MUL_MAT_EPI
    MOV PC $MUL_MAT_EPI

    # expects R5 = Arow R6 = outrow
    MUL_ROW:
        MOV R4 0 # i=0 to P
        MUL_ROW_LOOP:
            # move values to registers
            MOV R10 R5      # ARow
            MOV R11 @6[R0]  # MAT_B[i]
            ADD R11 R4
            MOV R12 R6  # OUT[i]
            ADD R12 R4
            CALL $MUL_CELL

            # increment and test loop
            INC R4
            MOV R12 @4[R0]
            SUB R12 R4
            JNZ $MUL_ROW_LOOP
        RET


    # expects a_ind=R10, b_ind=R11, out=R12
    MUL_CELL:
        MOV @R12, 0     # out=0
        MOV R9,   0     # i=0 to M
        MUL_CELL_LOOP:
            MOV R8 R10
            ADD R8 R9
            MOV R8 @R8
            MUL R8, @R11
            ADD @R12, R8
            ADD R11, @4[R0]
            INC R9
            MOV R7, @4[R0]
            SUB R7 R9
            JNZ $MUL_CELL_LOOP
        RET

MUL_MAT_EPI:
    # epilog
    POP SB
    MOV SP SB
    POP SB
    RET
# end MUL_MAT


N_SIZE:
    DW 2
M_SIZE:
    DW 2
P_SIZE:
    DW 2

MAT_A: # size N x M
    DW 1 2
    DW 3 4
MAT_B: # size M x P
    DW 4 3
    DW 2 1
MAT_RES:
    DW 0 0
    DW 0 0
LF:
DW 10