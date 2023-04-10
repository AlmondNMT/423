"""
File name: minesweeper.

Author: anonymous

Date: 4, 27

Assignment: Final project: Arcade
"""

import random
def moria(mordor, angmar, flags, score, wins, losses):
    """moria prints out an empty minefield, mordor{}, and asks the user if they want one of three 
    commands. Flag, unflag, or move. Flagging can replace a suspicious square with the letter F.
    Unflagging removes that F. Moving checks the answer key, (angmar{}). If angmar has "B" on that
    tile it ends the game. Otherwise, it replaces the mordor value with the angmar value and 
    reruns the program. It ends the game when the user has uncovered every tile."""
    a = 0
    while a < 7:
        b = 0
        rivendell = []
        while b < 7:
            s = str(mordor[b, a])
            s.replace("'", " ")
            rivendell.append(s)
            b =  b + 1
        print(rivendell)
        a = a + 1
    print("score:", score)
    print("Number of flags:", flags)
    lothlorien = input("Do you wish to FLAG, UNFLAG or MOVE?")
    if lothlorien == 'flag' or lothlorien == 'FLAG':
        if flags == 0:
            print("Error. You are all out of flags!")
            o = moria(mordor, angmar, flags, score, wins, losses)
            return o
        else:
            gondor = input("Enter in a coordinate from 1, 1 to 5, 5")
            gondor = gondor.split(",")
            if gondor[1][0] == " ":
                gondor[1] = gondor[1][1]
            mordor[int(gondor[0]), int(gondor[1])] = "F"
            flags = flags - 1
            h = moria(mordor, angmar, flags, score, wins, losses)
            return h
    elif lothlorien == 'unflag' or lothlorien == 'UNFLAG':
        arnor = input("Enter in a coordinate from 1, 1 to 5, 5")
        arnor = arnor.split(",")
        if arnor[1][0] == " ":
            arnor[1] = arnor[1][1]
        if mordor[int(arnor[0]), int(arnor[1])] == "F":
            mordor[int(arnor[0]), int(arnor[1])] = "0"
            flags = flags + 1
        else:
            print("Error, not a flagged square")
        g = moria(mordor, angmar, flags, score, wins, losses)
        return g
    elif lothlorien == 'move' or lothlorien == 'MOVE':
        rohan = input("Enter in a coordinate from 1, 1 to 5, 5")
        rohan = rohan.split(",")
        if rohan[1][0] == " ":
            rohan[1] = rohan[1][1]
        if mordor[int(rohan[0]), int(rohan[1])] == "F":
            print("Don't move to a flagged tile!!")
            c = moria(mordor, angmar, flags, score, wins, losses)
            return c
        if angmar[int(rohan[0]), int(rohan[1])] == "B":
            losses = losses + 1
            weathertop = [wins, losses, score]
            print("You stepped on a bomb! You lose!!")
            return weathertop
        else:
            if mordor[int(rohan[0]), int(rohan[1])] == "0":
                score = score + 2
            mordor[int(rohan[0]), int(rohan[1])] = angmar[int(rohan[0]), int(rohan[1])]
            if score == 44:
                print("Victory Achieved!!")
                wins = wins + 1
                fangorn = []
                fangorn.append(wins)
                fangorn.append(losses)
                fangorn.append(score)
                return(fangorn)
            else:
                f = moria(mordor, angmar, flags, score, wins, losses)
                return f
    else: 
        print("error, not a valid command")
        i = moria(mordor, angmar, flags, score, wins, losses)
        return i



def main():
    """main sets up two dictionaries, mordor{} and angmar{}. It sets up a third dictionary, Bree{} and 
    gives bree random entries from the bomb field, Dagorlad. Once that is done, it installs bomb tiles,
    blank tiles, and border tiles into angmar. It then runs a while loop to check and see if the blank
    tiles border a bomb square horizontally, vertically and then diagonally, running each tile through
    a while loop. Every time a bomb is adjascent, it replaces the blank with a number or it sets the 
    number to one higher than before. It then runs moria to begin the game. """
    dagorlad = ["B", "_", "_", "_", "_", "_", "_", "_", "_", "_", "B", "_", "_", "_", "_", "B", "_", "_", "_", "_", "_", "_", "_", "_", "_"]
    mordor = {}
    mordor[0, 0] = "*"
    mordor[1, 0] = "*"
    mordor[2, 0] = "*"
    mordor[3, 0] = "*"
    mordor[4, 0] = "*"
    mordor[5, 0] = "*"
    mordor[6, 0] = "*"
    mordor[0, 1] = "|"
    mordor[1, 1] = "0"
    mordor[2, 1] = "0"
    mordor[3, 1] = "0"
    mordor[4, 1] = "0"
    mordor[5, 1] = "0"
    mordor[6, 1] = "|"
    mordor[0, 2] = "|"
    mordor[1, 2] = "0"
    mordor[2, 2] = "0"
    mordor[3, 2] = "0"
    mordor[4, 2] = "0"
    mordor[5, 2] = "0"
    mordor[6, 2] = "|"
    mordor[0, 3] = "|"
    mordor[1, 3] = "0"
    mordor[2, 3] = "0"
    mordor[3, 3] = "0"
    mordor[4, 3] = "0"
    mordor[5, 3] = "0"
    mordor[6, 3] = "|"
    mordor[0, 4] = "|"
    mordor[1, 4] = "0"
    mordor[2, 4] = "0"
    mordor[3, 4] = "0"
    mordor[4, 4] = "0"
    mordor[5, 4] = "0"
    mordor[6, 4] = "|"
    mordor[0, 5] = "|"
    mordor[1, 5] = "0"
    mordor[2, 5] = "0"
    mordor[3, 5] = "0"
    mordor[4, 5] = "0"
    mordor[5, 5] = "0"
    mordor[6, 5] = "|"
    mordor[0, 6] = "*"
    mordor[1, 6] = "*"
    mordor[2, 6] = "*"
    mordor[3, 6] = "*"
    mordor[4, 6] = "*"
    mordor[5, 6] = "*"
    mordor[6, 6] = "*"
    hobbiton = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    osgiliath = 0
    bree = {}
    while osgiliath < 25:
        buckland = random.choice(hobbiton)
        bree[osgiliath] = dagorlad[buckland - 1]
        hobbiton.remove(buckland)
        osgiliath = osgiliath + 1
    angmar = {}
    angmar[0, 0] = "-"
    angmar[1, 0] = "-"
    angmar[2, 0] = "-"
    angmar[3, 0] = "-"
    angmar[4, 0] = "-"
    angmar[5, 0] = "-"
    angmar[6, 0] = "-"
    angmar[0, 1] = "|"
    angmar[1, 1] = bree[0]
    angmar[2, 1] = bree[1]
    angmar[3, 1] = bree[2]
    angmar[4, 1] = bree[3]
    angmar[5, 1] = bree[4]
    angmar[6, 1] = "|"
    angmar[0, 2] = "|"
    angmar[1, 2] = bree[5]
    angmar[2, 2] = bree[6]
    angmar[3, 2] = bree[7]
    angmar[4, 2] = bree[8]
    angmar[5, 2] = bree[9]
    angmar[6, 2] = "|"
    angmar[0, 3] = "|"
    angmar[1, 3] = bree[10]
    angmar[2, 3] = bree[11]
    angmar[3, 3] = bree[12]
    angmar[4, 3] = bree[13]
    angmar[5, 3] = bree[14]
    angmar[6, 3] = "|"
    angmar[0, 4] = "|"
    angmar[1, 4] = bree[15]
    angmar[2, 4] = bree[16]
    angmar[3, 4] = bree[17]
    angmar[4, 4] = bree[18]
    angmar[5, 4] = bree[19]
    angmar[6, 4] = "|"
    angmar[0, 5] = "|"
    angmar[1, 5] = bree[20]
    angmar[2, 5] = bree[21]
    angmar[3, 5] = bree[22]
    angmar[4, 5] = bree[23]
    angmar[5, 5] = bree[24]
    angmar[6, 5] = "|"
    angmar[0, 6] = "-"
    angmar[1, 6] = "-"
    angmar[2, 6] = "-"
    angmar[3, 6] = "-"
    angmar[4, 6] = "-"
    angmar[5, 6] = "-"
    angmar[6, 6] = "-"
    x = 1
    while x < 6:
        y = 1
        while y < 6:
            if angmar[x, y] == "B":
                y = y + 1
            else:
                if angmar[x, y + 1] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x, y - 1] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x + 1, y] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x - 1, y] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x + 1, y + 1] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x + 1, y - 1] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x - 1, y + 1] == "B":
                    if angmar[x, y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                if angmar[x - 1, y - 1] == "B":
                    if angmar[x , y] == "_":
                        angmar[x, y] = 1
                    else:
                        angmar[x, y] = angmar[x, y] + 1
                y = y + 1
        x = x + 1
    k = moria(mordor, angmar, 3, 0, 0, 0)
    return k
if "__name__" == "__main__":
    main()
