This program is an arcade that allows the user to play the games of
minesweeper and hangman.  The program greets you and introduces the main
menu. Users are expected to give a single number command, 1 - 5.

1 activates the game hangman. Users are invited to pick a difficulty level.
Difficulty levels must be typed exactly "easy", "medium", "hard", or
"MEGA-DETH".  Users are then given a panel with asterisks representing a
lowercase word (propernouns are included!)  Users should input a lowercase
letter.

2 activates the game of minesweeper. To play, users are given three
commands.  Typing in "move" or "MOVE" allows the user to reveal the
tile. Numbers indicate how many bombs are adjascent (horizontally,
vertically, or diagonally) to the tile. An underscore, '_', indicates that
there are zero bombs next to the tile.  Typing in "flag" or "FLAG" allows
the user to flag any bomb tiles, replacing the square with the letter
"F". The user can only do this three times.  Unflagging allows the user to
remove a flag from any tile marked "F". This can be done by typing "unflag"
or "Unflagging".

For all of these commands, users are asked to punch in coordinates on a 5 by
5 field.  Users must punch in their first coordinate, 1 - 5, a comma and
then their second coordinate, any number between 1 - 5. The first coordinate
selects the column, and the second selects the row. For example, a command
of "3, 2" will find the tile 3 across and 2 down.

After finishing a game, the machine will ask the users if they want to play
again.  If the user types in "Yes", "YES", "yes", "NEW GAME", or "new game",
the game will reset.  Otherwise, it will return the user to the main menu.

Typing in 3 acesses the stats panel, showing the high score, low score, and
win/loss precentages for that session of play.

Typing in 4 will acess special info about the developer.

Typing in 5 will finish the program. 
