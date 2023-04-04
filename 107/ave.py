def ave(rstri):
    august = list(rstri)
    print(august)
    #x = 0
    calig = []
    for x in august:
        if 88 <= ord(august[x]) <= 90:
            y = ord(august[x]) - 23
            calig.append(y)
        elif 120 <= ord(august[x]) <= 123:
            y = ord(august[x]) - 23
            calig.append(y)
        elif 65 <= ord(august[x]) <= 87:
            y = ord(august[x]) + 3
            calig.append(y)
        elif 97 <= ord(august[x]) <= 119:
            y = ord(august[x]) + 3
            calig.append(y)
        else:
            y = ord(august[x])
            calig.append(y)
    print(calig)

def main():
    sample_message = open("sample_message.txt", "r+")
    hadrian = ""
    print(sample_message.readline())
    
    #target.write("Secret message here")

if __name__ == "__main__":
    main()
