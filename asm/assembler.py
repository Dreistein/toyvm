import io
import sys
from instruction_encode import prepInstruction, encodeInstruction


def printUsage():
    print("Usage: assembler <input> <output>")
    print("This version only supports 10-bit signed jump offsets!")


# parses the input string into tokens
# breaks on whitespaces but leaves content in quotes intact
def tokenize(token: str):
    out = []                    # tokens
    quote = token.find('"')     # location of next quote
    inQuote = False             # is the current part quoted?
    while quote is not -1:
        if inQuote:
            out.append('"' + token[0:quote+1])  # append whole quoted string (with quotes)
        else:
            out += token[0:quote].split()       # split non-quoted string into tokens
        token = token[quote + 1:].lstrip().lstrip(',')  # '" , something...' -> 'something...'
        quote = token.find('"')
        inQuote = not inQuote
    return out + token.split()


# prints an error message and the line on which the error occurred
def errOnLine(msg:str, lnum:int):
    print("="*80)
    print("ERROR: {0:s}".format(msg))
    print('Line {0:d}: {1:s}'.format(lnum, input_lines[lnum-1]))
    exit(1)


# replaces the label with the address
def replaceLabel(label, lbl_table:dict, lineno:int):
    # not a label, already an address / value
    if type(label) is not str:
        return label
    # also not a label, could be something like '3456'
    if label[0] is not '$':
        return int(label, 0)

    label = label[1:] # trim the '$' off
    if label not in lbl_table:
        errOnLine('Not declared label "{0:s}"'.format(label), lineno)
    return lbl_table[label]

# ------------------------------------- ASSEMBLER ----------------------------------------------------------------------

if len(sys.argv) < 3:
    printUsage()
    exit(1)

print("opening file {0:s}".format(sys.argv[1]))
input_file = io.open(sys.argv[1], mode="r", encoding="utf-8")
print("writing to file {0:s}".format(sys.argv[2]))
output_file = io.open(sys.argv[2], mode="w", encoding="utf-8")


input_lines = []            # contains all input lines
instructions = []           # contains the prepped instructions
labels = {}                 # contains all known labels
instr_offset = 0x100        # address of instruction (starting at 0x100)


print("First Pass: Parsing and prepping...")

# fist pass: parse lines, prep instructions and labels
for linenum, line in enumerate(input_file.readlines()):
    input_lines.append(line)
    line = line.split('#', maxsplit=1)[0].strip()  # removes comments from line

    if line == "":
        continue

    # if the line is prepended by a label, add to labels with current instruction address
    labelPos = line.find(':')
    if labelPos is not -1:
        label = line[0:labelPos]            # extract label part
        line = line[labelPos+1:].lstrip()   # rest of line minus whitespaces

        if not label.isidentifier():
            errOnLine("Syntax Error: Not a valid identifier!", len(input_lines))

        # check if label already exists
        if label in labels:
            errOnLine('Label "{0:s}" already defined!'.format(label), len(input_lines))
        # add to labels
        labels[label] = instr_offset

    if line == "":
        continue

    # decode instruction and operands and calculate size
    instr = prepInstruction(tokenize(line))
    if not instr:
        errOnLine("Failed to prepare instruction!", len(input_lines))
    instr['line_number'] = len(input_lines)    # line number for error handling

    instr['instr_addr'] = instr_offset  # address of instructions for branch calculations
    instr_offset += instr['size']

    instructions.append(instr)

print("OK")
print("Second Pass: Replacing labels and writing to output...")

# second pass, replace labels and encode instructions
for instr in instructions:
    if 'dst' in instr:
        instr['dst'] = replaceLabel(instr['dst'], labels, instr['line_number'])
    if 'src' in instr:
        instr['src'] = replaceLabel(instr['src'], labels, instr['line_number'])
    if 'destination' in instr:
        instr['destination'] = replaceLabel(instr['destination'], labels, instr['line_number'])
    for i, val in enumerate(instr['payload']):
        instr['payload'][i] = replaceLabel(val, labels, instr['line_number'])

    for line in encodeInstruction(instr):
        output_file.write(str(line))
        output_file.write("\n")

print("All done.")

# close open files
input_file.close()
output_file.close()
