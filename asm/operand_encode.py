import re

# ----------------------------------------------------------------------------------------------------------------------

ptn = re.compile(r"^(?P<modifier>[@+-])?(?P<base>PC|SR|SP|SB|R\d{1,2}|\$[A-Za-z_]+|\d+)(?:\[(?P<index>R\d{1,2}|SP|PC|BP)\])?$")
register = {"R0": 0, "R1": 1, "R2": 2, "R3": 3, "R4": 4, "R5": 5, "R6": 6, "R7": 7,
         "R8": 8, "R9": 9, "R10": 10, "R11": 11, "R12": 12, "SB": 12, "SP": 13, "SR": 14, "PC": 15}


def encodeOperand(token:str):
    token = token.strip().strip(',')
    match = ptn.match(token)
    if not match:
        return [None]*3

    mod = match.group("modifier")
    base = match.group("base")
    index = match.group("index")

    am = 0
    base = register.get(base, base)
    index = register.get(index, index)

    if (mod or index) and (base == "SR"):  # that combination is not possible
        return [None]*3

    if mod == "@":
        am |= 0b10  # set deref bit

    if index is not None:  # ADDR[REG]
        if type(base) == int:  #cannot address two registers. the base address has to be a value or label
            return [None]*3
        am |= 0b01
        return am, index, base

    if mod == '+' or mod == '-':  #+-VAL --> PC+VAL
        if type(base) == int:
            return [None]*3  # this combination is not possible. relative mode only works with value offsets
        return 0b10, register['SR'], mod+base

    if type(base) == str: # X
        return am | 0b01, register['SR'], base

    # Reg
    return am, base, None

# ----------------------------------------------------------------------------------------------------------------------

if __name__ == '__main__':
    examples = ["R1", "500[R1]", "$lbl[R1]", "@R1", "@500[R1]", "@$lbl[R1]", "+500",
                "-500", "500", "+$lbl", "-$lbl", "$lbl", "@500", "@$lbl"]

    for v in examples:
        print(v, end=":\t")
        print(encodeOperand(v))