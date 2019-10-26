import sys
import re


def print_usage(exit_code=0):
    prog_name = sys.argv[0]

    print("Usage:")
    print("{} <binary> -- outputs disassembled file to stdout".format(prog_name))
    print("{} help -- prints this message".format(prog_name))
    exit(exit_code)


class Disassembler:
    def __init__(self, file):
        self.file = file
        self.ip = 0x100
        self.next_ip = 0x100

    def get_next_word(self):
        self.next_ip += 1
        line = self.file.readline()
        if line is "":
            return None
        return self.convert_to_binary(line.strip())

    def convert_to_binary(self, input):

        fmt = "{:016b}"

        def strip_matched(inp, pattern):
            match = re.search(pattern, inp)
            if match == None:
                return inp
            return inp[:match.start()]

        ## binary number
        if input.startswith('0b'):
            bin_num = strip_matched(input[2:], "[^01]")
            return fmt.format(int(bin_num,2))
        
        ## hex number
        if input.startswith('0x'):
            hexnum = strip_matched(input[2:].lower(), "[^a-f0-9]")
            return fmt.format(int(hexnum,16))

        # decimal number
        input = strip_matched(input, "\\D")
        return fmt.format(int(input))

    def disassemble(self):
        while True:
            self.ip = self.next_ip
            word = self.get_next_word()
            if word == None:
                break;
            if word[0] == '1':
                #print("dual opc:   {}".format(word))
                self.disasm_dual(word)
            elif word[:2] == '00':
                #print("single opc: {}".format(word))
                self.disasm_single(word)
            else:
                #print("branch opc: {}".format(word))
                self.disasm_branch(word)

    def format_hex(self, *words):
        prefix = "{:04X}: ".format(self.ip)
        for w in words:
            prefix += "{:04X} ".format(int(w,2))
        return "{:25s}".format(prefix[:-1])

    def decode_operand(self, instr, am, register):

        def format_reg(n):
            return {13: 'SP', 14: 'SR', 15: 'PC'}.get(n, "R{:02d}".format(n))

        if register == 14:
            if am == 0:
                return instr, 'SR'
            else:
                X = self.get_next_word()
                numX = int(X,2)
                instr.append(X)
                if am == 1:
                    return instr, '0x{:X}'.format(numX)
                if am == 2:
                    return instr, 'PC+0x{:X} ({:04X})'.format(numX, numX + self.ip-2)
                else:
                    return instr, '*0x{:X}'.format(numX)
        else:
            if am == 0:
                return instr, format_reg(register)
            elif am == 2:
                return instr, "*"+format_reg(register)
            else:
                X = self.get_next_word()
                numX = int(X,2)
                instr.append(X)
                reg = "0x{:X}+{}".format(numX, format_reg(register))
                if am == 3:
                    return instr, '*' + reg
                return instr, reg


    def disasm_dual(self, instr):
        opc   = int(instr[1:4], 2)
        r_ad  = int(instr[4:6], 2)
        r_as  = int(instr[6:8], 2)
        r_dst = int(instr[8:12], 2)
        r_src = int(instr[12:16], 2)

        instr = [instr]
        instr, dst = self.decode_operand(instr, r_ad, r_dst)
        instr, src = self.decode_operand(instr, r_as, r_src)

        op_codes = ['MOV', 'ADD', 'SUB', 'MUL', 'AND', 'OR', 'XOR', 'SHRR'] 
        print('{}; {} {}, {}'.format(self.format_hex(*instr), op_codes[opc], dst, src))

    def disasm_single(self, instr):
        opc   = int(instr[ 2:6 ], 2)
        port  = int(instr[ 6:10], 2)
        r_as  = int(instr[10:12], 2)
        r_src = int(instr[12:16], 2)

        op_codes = ['HLT', 'PUSH', 'POP', 'CALL', 'RET', 'INC', 'DEC', 'INV', 'TST', 'SHR', 'SHL', 'OUT', 'IN', 'SWAP']

        instr = [instr]

        if opc in [0, 4]: # hlt, ret
            print('{}; {}'.format(self.format_hex(*instr), op_codes[opc]))
            return

        instr, reg = self.decode_operand(instr, r_as, r_src)

        if opc == 11: # out
            print('{}; {} port 0x{:X}, {}'.format(self.format_hex(*instr), op_codes[opc], port, reg))
            return
        if opc == 12: # in
            print('{}; {} {}, port 0x{:X}'.format(self.format_hex(*instr), op_codes[opc], reg, port))
            return
        
        op_name = ""
        if opc >= len(op_codes):
            op_name = "???"
        else:
            op_name = op_codes[opc]

        print('{}; {} {}'.format(self.format_hex(*instr), op_name, reg))

    def disasm_branch(self, instr):
        condition = int(instr[2:5], 2)
        w         = int(instr[5:6], 2)
        offset    = int(instr[6:16], 2)
        if offset > 512:
            offset -= 1024 
        
        cond_opcodes = ['JMP', 'JN', 'JGE', 'JL', 'JNE/JNZ', 'JEQ/JZ', 'JNC', 'JC']

        if w == 0:
            print('{}; {} 0x{:X} ({:+d})'.format(self.format_hex(instr), cond_opcodes[condition], self.ip + offset, offset))
            return        
        X = self.get_next_word()
        numX = int(X,2)
        print('{}; {} 0x{:X}'.format(self.format_hex(instr, X), cond_opcodes[condition], numX))



if __name__ == "__main__":
    if len(sys.argv) != 2:
        print_usage(1)

    file_name = sys.argv[1]

    if file_name.lower() == 'help':
        print_usage()

    try:
        bin_file = open(file_name, 'r')
    except OSError as e:
        print("Could not open specified file '{}'".format(file_name))
        exit(2)
    else:
        with bin_file:
            Disassembler(bin_file).disassemble()


