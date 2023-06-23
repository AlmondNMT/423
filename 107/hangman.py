"""
    File name: hangman.py

    Author: anonymous

    Date: 4, 27

    Assignment: Final project: Arcade.
    """



import random
def tombofhorrors(hickman, tharizdun):
    """tomb of horros sets up a blank word, lolth, and fills it up with askerisks. It sets the 
    score, a win counter and a loss counter to zero. It runs the mystery word, the blank word,
    and the counters through ravenloft. It returns the game result back to main."""
    strahd = list(hickman[0])
    print(strahd)
    lolth = ""
    z = 0
    while z < len(strahd):
        lolth = lolth + "*"
        z = z + 1
    score = 0
    win = 0
    loss = 0
    e = ravenloft(hickman, tharizdun, lolth, score, win, loss)
    return e

def ravenloft(hickman, tharizdun, lolth, score, win, loss):
    """Ravenloft checks the life counter. It ends the game if the life has reached zero and returns
    a list to combine the wins, losses, and the score. It also checks to see if the user has 
    uncovered the whole word (hickman), and it matches completely with the answer. If so, it returns a list
    combining the win counter, the loss counter and the score. It then asks the user to input a 
    letter. If the letter is anywhere in the word, if will increase the users score and replace
    the letter slot on lolth (as an *) with the letter guessed. It will do this for each instance
    of the letter occuring in the word. If no copies of the word are found in the answer, it lowers
    the life counter (tharizdun) by 1."""
    strahd = list(hickman[0])
    if tharizdun == 0:
        print("Game over! The Final answer was:", hickman[0])
        loss = loss + 1
        balrog = []
        balrog.append(win)
        balrog.append(loss)
        balrog.append(score)
        return balrog
    elif lolth == hickman[0]:
        print("You win!!", hickman[0], "You win!!")
        win = win + 1
        balrog = []
        balrog.append(win)
        balrog.append(loss)
        balrog.append(score)
        print(balrog)
        return balrog
        """catoblepas = input("Do you want to START a new game, or EXIT??")
        if catoblepas =! "start" and catoblepas =! "START" and catoblepas =! "exit" and catoblepas =! "EXIT":
            print("Error. Invald command.")
            catoblepas = input("Do you want to START a new game or EXIT?")
        if catoblepas == "start" or catoblepas == "START":
            main()
        elif catoblepas == "exit" or catoblepas == "EXIT":
            print("Thank you for playing hangman!")"""    
    else:
        print(lolth, "lives left:", tharizdun)
        print("score", score)
        beholder = input("guess a letter")
        y = 0
        demagorgon = 0
        while y < len(strahd):
            if strahd[y] == beholder:
                demagorgon = 1
                x = 0 
                lich = ""
                while x < len(strahd):
                    if x == y:
                        lich = lich + strahd[y]
                        x = x + 1
                    else:
                        lich = lich + "*"
                        x = x + 1
                w = 0 
                dungeon = list(lolth)
                dragon = list(lich)
                acerak = ""
                while w < len(strahd):
                    if dungeon[w] == dragon[w]:
                        acerak = acerak + dungeon[w]
                        w = w + 1
                    else:
                        if dungeon[w] == "*":
                            acerak = acerak + dragon[w]
                        else:
                            acerak = acerak + dungeon[w]
                        w = w + 1
                    lolth = acerak
                score = score + 2
                y = y + 1
            else:
                y = y + 1
        if demagorgon == 0:
            score = score - 2
            tharizdun = tharizdun - 1
            print("One life lost!")  
        l = ravenloft(hickman, tharizdun, lolth, score, win, loss) 
        return l

def main():
    """main asks the user for a difficulty, and chooses from 4 different word sets depending on the 
    user's choice. It then selects a random words and runs that word and a life counter through 
    tombofhorrors(). It returns the result of tombofhorrors."""
    difficulty = input("Choose your difficulty: easy, medium, hard, or MEGA-DETH: ")
    gygax = []
    hommlet = ["pleasing", "reasons", "frankenstein", "legionaire", "prestidigitation", "honolulu"]
    zuggtomy = ["astronomer", "albuquerque", "indubitably", "werewolf", "euphonium", "exceptional"]
    giants = ["metaphysics", "dracula", "dallas", "bean", "bass", "barracuda"]
    drow = ["jazz", "vortex", "mozambique", "igor", "pheonix", "odd", "fall"]
    if difficulty == "easy":
        gygax.append(random.choice(hommlet))
    elif difficulty == "medium":
        gygax.append(random.choice(zuggtomy))
    elif difficulty == "hard":
        gygax.append(random.choice(giants))
    elif difficulty == "MEGA-DETH":
        gygax.append(random.choice(drow))
    else:
        print("Error. Please pick a difficulty between easy, medium, hard, or MEGA-DETH.")
        main()
    u = tombofhorrors(gygax, 6)
    return u
main()
