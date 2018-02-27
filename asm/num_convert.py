
def toInt(token:str):
    if token.startswith("0x"):
        return int(token[2:], 16)
    if token.startswith("0b"):
        return int(token[2:], 2)
    int(token)
