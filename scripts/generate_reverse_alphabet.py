ALPHABET="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
FMT="    ['{}'] = {},"

print("{")

for i in range(len(ALPHABET)):
    print(FMT.format(ALPHABET[i], i))

print("}")