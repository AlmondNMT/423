def simplediff(nermal, arlene):
    nermal = nermal.split("\n")
    arlene = arlene.split("\n")
    c = 0
    while c < len(nermal):
        a = 0
        t = nermal[c]
        meow = arlene[c]
        if len(t) != len(meow):
            print(len(t), len(meow))
            print("There is a difference in", c, "c", a)
            print(nermal[c], "---", arlene[c])
            a = len(t)
        while a < len(t) and a < len(meow):
            if t[a] == meow[a]:
                a = a + 1
            else:
                print("There is a difference in", c, "c", a)
                print(nermal[c], "---", arlene[c])
                a = len(t)

        c = c + 1
def main():
    garfield = ""
    with open("file1.txt", "r") as sample_message:
        odie = sample_message.readline()
        while odie != "":
            garfield = garfield + odie
            odie = sample_message.readline()
    jon = str(garfield)
    print(jon)
    hobbes = ""
    with open("file2.txt", "r") as sample_2:
        mr_bun = sample_2.readline()
        while mr_bun != "":
            hobbes = hobbes + mr_bun
            mr_bun = sample_2.readline()
    calvin = str(hobbes)
    simplediff(jon, calvin)

main()
