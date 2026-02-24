# 26CBDSTRU-MP
Diwa, Kimura, Obando - CBDSTRU Machine Project 

General Description / Assumption: 
The machine project is a 3x3 'territory' turn based game where two players will place one 'piece' for each of their turns. 

// let R be Red and B be Blue for easy digestion
// ** - means contradiction or confusion

MP BIBLE:

Applicable Sets:
Set C = {1,2,3} - Set of positibe numbers than are less than 4. Shall be used as VALID coordinated for the grid of the program (no 0 or less nor 4 and up)
Set N = {1,2,3,..., 16} ** 
Set M = CxC - This should act as the "board", which contains every possible pair from Set C. This contains 9 positions: (1,1), (1,2), (1,3), (2,1), (2,2), (2,3), (3,1), (3,2), (3,3).
Set V = {true, false} - boolean values (flag), based on our coverage so far we'll use {1,0}

System Variables: 
// all are assumptions based on variable names and are Not explicitly stated in the instructions 
good (V) - flag to track whether a player's move is valid and was successfully executed. If true, then proceed to switch turns. If false, turn should not advance
go (V) - flag to track whose turn it is. If true, let R go. If false, let B go. 
start (V) - flag to check whether game is on placement phase or movement phase.
over (V) - flag to indicate that the game is 'over'
found (V) - flag to be used inside function Replace to check if a position is already occupied. 
val (N) - turn counter, game ends when it reaches 20 **
R,B,S,T,F (subsets of M) - Collection of grid positions where R holds Red's position, B holds Blue's, S tracks the position last "seen" (visited during Replace), T trackers positions that have been "expanded". 

System Facts:
// generally the conditions that the program must always check
F... - the free spots that aren't occupied by either player. We count how many 'M' that are not in R and B.
over... - the game will be over if ANY of the THREE conditions are true. 1.) if there are only 3 free spots that remain, 2.) if 20 turns have been played, 3.) if there is a 'wipe-out' where ONE player loses all of their pieces during the movement phase

System Initiatlization:
good = false
go = true
start = true
found = flase
val = 0
R,B,S,T = empty 

System States and Behavior: 
Remove(pos) — cleans up a position. If it's Red's turn, remove pos from R. If it's Blue's turn, remove pos from B. Then always remove pos from both S and T regardless. This is called internally by Expand, not directly by the player.

Replace(pos) — which is the most complex function. It first resets found = false. Then depending on whose turn it is, it either captures an opponent's piece (removes it from their set and sets found=true), acknowledges stepping on your own piece (found=true but no set change), or claims a new free position (adds to your set, found stays false). After all that, if found ended up true, it checks S and T to decide whether to just log the position in S, or trigger a full Expand.

Expand(pos) — a cascading function triggered when a position is visited twice (in S but not yet in T). It computes the four neighbors of pos (up, down, left, right), calls Remove on pos itself, then calls Replace on the conditional neighbor (up if Red's turn, down if Blue's turn), and always calls Replace on left and right. Neighbors outside the 3×3 grid are simply skipped.

Update(pos) — which is called during the movement phase when a player selects their own piece. It resets good to false, then if pos is not yet in S it adds it and flips good to true. If after that good is still false (meaning pos was already in S) and it's not in T yet, it adds pos to T and calls Expand.

NextPlayerMove(pos) — the main entry point for each turn. During placement phase it adds the piece to the right set. During movement phase it calls Update. After processing, if both R and B have exactly 1 piece, placement ends. If good ended up true, it flips good back to false, switches go, and increments val.

GameOver() — Simply compares |R| and |B|. Whoever has more pieces wins. Equal sizes is a draw.


