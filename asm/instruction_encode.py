from operand_encode import encodeOperand as op
import copy
import math

ZERO_OP = 'zero'
SINGLE_OP = 'single'
DUAL_OP = 'dual'
BRANCH_OP = 'branch'
DATA_OP = 'data'
DATA_OP_BYTE = 'data.b'

opc = {}
for i,v in enumerate(['MOV', 'ADD', 'SUB', 'MUL', 'AND', 'OR', 'XOR', 'SHRR']):
    opc[v.upper()] = {'type': DUAL_OP, 'opc': i}

for i,v in enumerate(['HLT', 'PUSH', 'POP', 'CALL', 'RET', 'INC', 'DEC', 'INV', 'TST', 'SHR', 'SHL', 'OUT', 'IN', 'SWAP']):
    opc[v.upper()] = {'type': SINGLE_OP, 'opc': i}

for i,v in enumerate(['JMP', 'jn', 'jge', 'jl', 'jne', 'jeq', 'jnc', 'jc', ]):
    opc[v.upper()] = {'type': BRANCH_OP, 'opc': i}
opc['JNZ'] = opc['JNE']
opc['JZ'] = opc['JEQ']
opc['DW'] = {'type': DATA_OP}
opc['DW.B'] = {'type': DATA_OP_BYTE}


# ----------------------------------------------------------------------------------------------------------------------
# Prep IO instructions
def prepIO(token:list, opc_type:dict):
    if len(token) is not 3:
        return None
    Ad, dst, X = op(token[1])
    port = int(token[2])
    if Ad is None:
        return None
    opc_type['ad'] = Ad
    opc_type['dst'] = dst
    opc_type['port'] = port
    if X:
        opc_type['payload'].append(X)
    opc_type['size'] = 1 + len(opc_type['payload'])
    return opc_type


# ----------------------------------------------------------------------------------------------------------------------
# Prep Instructions
# Extracts all information, like OP-Code, instruction type, operands and stores them into a dictionary
# Returns None on Failure
def prepInstruction(token:list):
    opc_id = token[0].upper()
    opc_type = opc.get(opc_id, None)

    if not opc_type:
        return None

    opc_type = copy.deepcopy(opc_type)
    opc_type['name'] = opc_id
    opc_type['payload'] = []

    if opc_id == 'IN' or opc_id == 'OUT':
        return prepIO(token, opc_type)

    if opc_id == 'HLT' or opc_id == 'RET':
        opc_type['ad'] = 0
        opc_type['dst'] = 0
        opc_type['port'] = 0
        opc_type['size'] = 1
        return opc_type

    if opc_type['type'] == DUAL_OP:
        if len(token) is not 3: # invalid
            return None
        Ad, dst, X = op(token[1])
        As, src, Y = op(token[2])
        if (Ad is None) or (As is None):
            return None
        opc_type['ad'] = Ad
        opc_type['dst'] = dst
        opc_type['as'] = As
        opc_type['src'] = src
        if X:
            opc_type['payload'].append(X)
        if Y:
            opc_type['payload'].append(Y)
        opc_type['size'] = 1 + len(opc_type['payload'])
        return opc_type

    if opc_type['type'] == SINGLE_OP:
        if len(token) is not 2:
            return None
        Ad, dst, X = op(token[1])
        if Ad is None:
            return None
        opc_type['ad'] = Ad
        opc_type['dst'] = dst
        opc_type['port'] = 0
        if X:
            opc_type['payload'].append(X)
        opc_type['size'] = 1 + len(opc_type['payload'])
        return opc_type

    if opc_type['type'] == BRANCH_OP:
        if len(token) is not 2:
            return None
        address = token[1].strip()
        opc_type['size'] = 1
        opc_type['destination'] = address
        return opc_type

    if opc_type['type'] == DATA_OP:
        for word in token[1:]:
            word = word.strip(",")
            if word[0] == '"' and word[-1] == '"':
                for c in word[1:-1]:
                    opc_type['payload'].append(ord(c))
            else:
                opc_type['payload'].append(int(word, 0))
        opc_type['size'] = len(opc_type['payload'])
        return opc_type

    if opc_type['type'] == DATA_OP_BYTE:

        last = None;
        
        def append_payload(c, last):
            if c > 0xFF:
                print("WARNING: Byte {0} DW greater than a byte. Discarding upper bits!".format(c))
                c = c & 0xFF
            if last == None:
                return c
            else:
                opc_type['payload'].append( (last << 8 ) + c )
                return None;

        for word in token[1:]:
            word = word.strip(",")
            if word[0] == '"' and word[-1] == '"':
                for c in word[1:-1]:
                    last = append_payload(ord(c), last)
            else:
                last = append_payload(int(word, 0), last)
        
        append_payload(0, last)

        opc_type['size'] = len(opc_type['payload'])
        return opc_type
    return None


# ----------------------------------------------------------------------------------------------------------------------
def encodeInstruction(instr:dict):
    if instr['type'] == DUAL_OP:
        encoded_instr = ["0b1{0:03b}{1:02b}{3:02b}{2:04b}{4:04b}".format(instr['opc'],
                                                      instr['ad'], instr['dst'],
                                                      instr['as'], instr['src'])]
        return encoded_instr + instr['payload']

    if instr['type'] == SINGLE_OP:
        encoded_instr = ["0b00{0:04b}{3:04b}{1:02b}{2:04b}".format(instr['opc'], instr['ad'], instr['dst'], instr['port'])]
        return encoded_instr + instr['payload']

    if instr['type'] == BRANCH_OP:
        addr = instr['destination'] - instr['instr_addr']
        encoded_instr = ["0b01{0:03b}{1:01b}{2:010b}".format(instr['opc'], 0, addr & 0x3FF)]
        return encoded_instr # + instr['payload']

    if instr['type'] == DATA_OP or DATA_OP_BYTE:
        return instr['payload']


examples = ["MOV R1, $DATA[R0]", "JZ 0xFFFF", "OUT R1, 0", "INC R1", "JMP $START", "DW \"Hello\" 0x20 \"World!\""]

if __name__ == '__main__':
    for instr in examples:
        res = prepInstruction(instr)
        print("{0:s}\t-> ".format(instr), end="")
        print(res)
