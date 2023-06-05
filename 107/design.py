Design plans:

These programs are all independent of each other. It is best to tackle them seperately. 

Hangman can be done with most of the string and list manipulation that we've learned before. 
We will need to import random to "import a random word from a set of words". 
I am planning on using recursion since we will need to reprint the message, score, and life totals.
These are all changing variables that will be listed as parameters. They will be redefined inside 
the main recursion function.
The screen should be able to check by character whether the input letter is inside the answer word. 
The answer key and a blank entry (with **** instead of letters) will both also be alterable parameters.
The blank entry will redefine itself based on the answer key. 

Minesweeper will behave similarly, but with boards. We will need to reprint the board every guess.
Each minesweeper board will need the coordinates (var. 1) and the tile (var. 2). I will have 
two board sets, a blank board, and an answer key board. 
The answer key will need to be randomly generated. I will need to import random.
Dictionaries will be preferable to lists to keep track of the entries.
Every time a tile is "revealed", the dictionary entry will redefine itself based on the answer key.
Print the tiles as a set of lists, row by row, column by column. 

The stats scoreboard will need to keep track of three variables, scores, losses, and wins. 
Victory precentage will be "wins/total games", "losses/total games"
High score and low score can be found with maxiumum and minimum from lab 3. Keep the scores in a 
list. .append each new score. 

About and Exit can be text files. 
