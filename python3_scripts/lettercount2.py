def lettercount(strink):
    ogl = list(strink)
    checker = []
    lettercounter = []
    thirdlist = []
    i = 0

    while i < len(ogl):
        l = 0
        while l < len(ogl):
            b = 0
            copycat = 0
            print(l)
            print(i)
            while b < len(ogl):
                if l < i:
                    b = len(ogl)
                    l = l + 1
                elif ogl[l] == ogl[i]:
                    l = l + 1
                    b = len(ogl)
                else:
                    if ogl[b] == ogl [l]:
                        copycat = 1
                        l = l + 1
                    else: b = b + 1
            if copycat == 0:
                lettercounter.append(ogl[l])
                l = l + 1
        checker.append(ogl[i])
        i = i + 1
    k = 0
    print(lettercounter)
    while k < len(lettercounter):
        j = 0
        m = 0
        while m < len(ogl):
            if lettercounter[k] == ogl[m]:
                j = j + 1
                m = m + 1
            else:
                m = m + 1
        k = k + 1
        thirdlist.append(j)
    print(thirdlist)
    fourth = {}
    c = 0
    """while c < len(lettercounter):
        fourth[]
        c = c + 1"""
def main():
    sillystringhasstrung = input("Say something:")
    lettercount(sillystringhasstrung)

main()