"""
File name: arcade.py

Author: anonymous

Date: 4. 27

Assignment: Final project: Arcade
"""
import os
import random
import hangman as h
import minesweeper as m
def stats(scoresh, winsh, lossesh, scoresm, winsm, lossesm):
    """stats() prints the win/loss precentages, the high score, and the lowscore for both of the games.
    """
    Geronimo = open("Dustbucket_tavern_hall_o'_fame.txt", "w")
    Geronimo.write("Hangman")
    Geronimo.write("\n")
    Geronimo.write("-------------\n")

    gamesh = winsh + lossesh
    if gamesh == 0:
        Geronimo.write("No Games of hangman were played\n")
    else:
        Crazyhorse = "win %: " + str(int(winsh * 100/ gamesh)) + " %\n"
        Geronimo.write(Crazyhorse)
        Redcloud = "loss %: " + str(int(lossesh * 100 / gamesh)) + " %\n"
        Geronimo.write(Redcloud)
    if scoresh == []:
        Geronimo.write("No high score for this game.\n")
    else:
        t = 0
        highscore = scoresh[0]
        while t < len(scoresh):
            if int(scoresh[t]) - int(highscore) > 0:
                highscore = scoresh[t]
            t = t + 1
        Geronimo.write("Highscore =" + str(highscore) + "\n")
    if scoresh == []:
        Geronimo.write("No low score for this game\n")
    else:
        q = 0
        lowscore = scoresh[0]
        while q < len(scoresh):
            if int(lowscore) - int(scoresh[q]) > 0:
                lowscore = scoresh[q]
            q = q + 1
        Geronimo.write("Lowscore:" + str(lowscore) + "!!!!!!!!!\n")
    Geronimo.write("Minesweeper\n")
    Geronimo.write("------------\n")
    gamesm = winsm + lossesm
    if gamesm == 0:
        Geronimo.write("No Games of minesweeper were played\n")
    else:
        Geronimo.write("win %:" + str(int(winsm * 100/ gamesm)) + " %\n")
        Geronimo.write("loss %" + str(int(lossesm * 100 / gamesm)) + " %\n")
    if scoresm == []:
        Geronimo.write("No high score for this game.\n")
    else:
        t = 0
        highscorem = scoresm[0]
        while t < len(scoresm):
            if int(scoresm[t]) - int(highscorem) > 0:
                highscorem = scoresm[t]
            t = t + 1
        Geronimo.write("Highscore =" + str(highscorem) + "\n")
    if scoresm == []:
        Geronimo.write("No low score for this game\n")
    else:
        q = 0
        lowscorem = scoresm[0]
        while q < len(scoresm):
            if int(lowscorem) - int(scoresm[q]) > 0:
                lowscorem = scoresm[q]
            q = q + 1
        Geronimo.write("Lowscore:" + str(lowscorem) + "!!!!!!!!!\n")
    Geronimo.close()
    Geronimo = open("Dustbucket_tavern_hall_o'_fame.txt", "r")
    print(Geronimo.read())
    menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
    
    
def menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm):
    """menu() allows the user to acess the hangman and minesweeper files by running them, as well 
    as the stats, about the author, and exit functions. Stats runs the stats function, while About
    and Exit release a print statement."""
    print("  Menu  ")
    print("1) Hangman")
    print("2) Minesweeper")
    print("3) Stats")
    print("4) About")
    print("5) Exit")
    print("---------------")
    Oakley = input("Please pick an option:")
    if Oakley == "1":
        z = h.main()
        print(z)
        winsh = winsh + z[0]
        lossesh = lossesh + z[1]
        scoresh.append(z[2])
        j = input("Would you like to play a NEW GAME?")  
        while j == "NEW GAME" or j == "new game" or j == "YES" or j == "Yes" or j == "yes":
            y = h.main()
            winsh = winsh + y[0]
            lossesh = lossesh + y[1]
            scoresh.append(y[2])
            j = input("Would you like to play a NEW GAME?")
        menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
    elif Oakley == "2":
        w = m.main()
        winsm = winsm + w[0]
        lossesm = lossesm + w[1]
        scoresm.append(w[2])
        p = input("Would you like to play a new game?")
        while p == "NEW GAME" or p == "new game" or p == "YES" or p == "Yes" or p == "yes":
            v = m.main()
            winsm = winsm + v[0]
            lossesm = lossesm + v[1]
            scoresm.append(v[2])
            p = input("Would you like to play a new game?")
        menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
    elif Oakley == "3":
        stats(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
    elif Oakley == "4":
        print("Anonymous is a novice programmer, and a crime fighting crusader in the EEFU (Euphonium Extended Film Universe)")
        menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
    elif Oakley == "5":
        print("Travel well, cowpoke")
    else:
        print("Error. Invalid command")
        menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
def main():
    """main selects a random greeting and runs the menu program."""
    Greetings = []
    Billy = "Howdeedoo, Buckarro?"
    Greetings.append(Billy)
    Villa = "Welcome to the Dustbucket Tavern! Free games all around!"
    Greetings.append(Villa)
    Butch = "Well, well, well. If it ain't the good ol' king of the west!"
    Greetings.append(Butch)
    Wyatt = "This next one is on the house, y'all!"
    Greetings.append(Wyatt)
    Buffalobill = random.choice(Greetings)
    print(Buffalobill)
    winsh = 0
    lossesh = 0
    scoresh = []
    winsm = 0
    lossesm = 0
    scoresm = []
    menu(scoresh, winsh, lossesh, scoresm, winsm, lossesm)
main()
