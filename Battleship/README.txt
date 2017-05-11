
PLEASE NOTICE: when playing with graphics (i.e not using the -quiet flag), a sound is heard when the results
are shown. IT'S NOT A BUG - IT'S A FEATURE (really!).

*******************************
*   IMPORTANT CLATIFICATION   *
*******************************

our code convention for the board coordinates is:
x,y <--> row,col

for example, consider the following 3x3 board (dash = water):

|-B-|
|---|
|-b-|

if player A "attack" functions returns (3,2) it will hit the "b" in line 3 (and not the last dash in line 2).
we also use this convention when we apply "notifyOnAttackResults" for each player 
(i.e, the arguments would be row = 3, col = 2).

If any error occurrs, please consider this convention when testing our code.

Thanks :)