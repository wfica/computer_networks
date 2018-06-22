import re;

def mis():
    f = open('kuba', 'r')
    for i, line in enumerate(f):
        x, y = re.findall(r'([0-9a-fA-F]+)-([0-9a-fA-F]+)', line)[0]
        yield  int(y, base=16)  - int(x, base=16), i 


print( *sorted(list(mis())), sep="\n")