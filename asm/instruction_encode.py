from operand_encode import encodeOperand as op
import copy
import num_convert

SINGLE_OP = 'single'
DUAL_OP = 'dual'
BRANCH_OP = 'branch'
DATA_OP = 'data'

opc = {}
for i,v in enumerate(['MOV', 'ADD', 'SUB', 'MUL', 'AND', 'OR', 'XOR', 'SHRR']):
    opc[v.upper()] = {'type': DUAL_OP, 'opc': i}

for i,v in enumerate(['HLT', 'PUSH', 'POP', 'CALL', 'RET', 'INC', 'DEC', 'INV', 'TST', 'SHR', 'SHL', 'OUT', 'IN']):
    opc[v.upper()] = {'type': SINGLE_OP, 'opc': i}

for i,v in enumerate(['JMP', 'jn', 'jge', 'jl', 'jne', 'jeq', 'jnc', 'jc', ]):
    opc[v.upper()] = {'type': BRANCH_OP, 'opc': i}
opc['JNZ'] = opc['JNE']
opc['JZ'] = opc['JEQ']
opc['DW'] = {'type': DATA_OP}


# ----------------------------------------------------------------------------------------------------------------------
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
def prepInstruction(token:str):
    token = token.split()
    opc_id = token[0].upper()
    opc_type = opc.get(opc_id, None)

    if not opc_type:
        return None

    opc_type = copy.deepcopy(opc_type)
    opc_type['name'] = opc_id
    opc_type['payload'] = []

    if opc_id == 'IN' or opc_id == 'OUT':
        return prepIO(token, opc_type)

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
        opc_type['size'] = '1'
        opc_type['destination'] = address
        return opc_type

    if opc_type['type'] == DATA_OP:
        for word in token[1:]:
            word = word.strip(",")
            if word[0] == '"' and word[-1] == '"':
                for c in word[1:-1]:
                    opc_type['payload'].append(ord(c))
            else:
                opc_type['payload'].append(num_convert.toInt(word))
        opc_type['size'] = len(opc_type['payload'])
        return opc_type
    return None


# ----------------------------------------------------------------------------------------------------------------------
def encodeInstruction(instr:dict, lables:dict):
    if instr['type'] == 'dual':
        encoded_instr = [int("1{0:03b}{1:02b}{3:02b}{2:04b}{4:04b}".format(instr['opc'],
                                                      instr['ad'], instr['dst'],
                                                      instr['as'], instr['src']))]
        return encoded_instr + instr['payload']

    if instr['type'] == 'single':
        encoded_instr = "00{0:04b}{3:04b}{1:02b}{2:04b}".format(instr['opc'], instr['as'], instr['src'], instr['port'])
        return encoded_instr + instr['payload']

    if instr['type'] == 'branch':
        encoded_instr = "01{0:03b}{1:01b}{2:010b}".format(instr['opc'], instr['w'], instr['offset'])
        return encoded_instr + instr['payload']


examples = ["MOV R1, $DATA[R0]", "JZ 0xFFFF", "OUT R1, 0", "INC R1", "JMP $START", "DW \"Hello\" 0x20 \"World!\""]

if __name__ == '__main__':
    for instr in examples:
        res = prepInstruction(instr)
        print("{0:s}\t-> ".format(instr), end="")
        print(res)
