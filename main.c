#include <stdlib.h>
#include <stdio.h>

#define GRID_SIZE 9
#define C_MIN 1
#define C_MAX 3
#define N_MAX 16
#define OVER_VAL 20

// -- Applicable Sets --

struct Position {
    int a;
    int b;
};

struct GameState {
    
    // V variables (0 = false, 1 = true)
    int good;
    int go;
    int start;
    int over;
    int found;
    
    // N variable
    int val;
    
    int R[GRID_SIZE]; // red occupied positions
    int B[GRID_SIZE]; // blue occupied positions
    int S[GRID_SIZE]; // seen positions
    int T[GRID_SIZE]; // expanded positions
    
};

struct GameState initGame() {
    struct GameState game;
    int i;

    game.good  = 0;
    game.go    = 1;
    game.start = 1;
    game.found = 0;
    game.val   = 0;
    game.over  = 0;
    
    for (i = 0; i < GRID_SIZE; i++) {
        game.R[i] = 0;
        game.B[i] = 0;
        game.S[i] = 0;
        game.T[i] = 0;
    }
    
    printf("Game Initialized\n");

    return game;
}

// -- Helper Functions --

int posToIndex(struct Position pos) {
    return (pos.a - 1) * 3 + (pos.b - 1);
}

int isValidPos(struct Position pos) {
    int validA, validB, result;
    validA = (pos.a >= C_MIN && pos.a <= C_MAX);
    validB = (pos.b >= C_MIN && pos.b <= C_MAX);
    result = (validA && validB);
    return result;
}

int countSet(int set[]) {
    int i, count;
    count = 0;
    for (i = 0; i < GRID_SIZE; i++) {
        if (set[i] == 1)
            count = count + 1;
    }
    return count;
}

int countF(struct GameState game) {
    int i, count;
    count = 0;
    for (i = 0; i < GRID_SIZE; i++) {
        if (game.R[i] == 0 && game.B[i] == 0)
            count = count + 1;
    }
    return count;
}

int checkOver(struct GameState game) {
    int fCount, rCount, bCount, oneSideGone, result;
    fCount = countF(game);
    rCount = countSet(game.R);
    bCount = countSet(game.B);
    oneSideGone = (!game.start && ((rCount > 0 && bCount == 0) || (rCount == 0 && bCount > 0)));
    result = (fCount == 3 || game.val >= OVER_VAL || oneSideGone);
    return result;
}

// forward declaration so Replace can call Expand
struct GameState expand(struct GameState game, struct Position pos);

/*
   replace

   Purpose: Core capture and claim logic. On each player's turn,
            checks if the position is occupied by the opponent
            (capture), occupied by themselves (found trigger),
            or free (claim). Then updates S and T accordingly,
            calling Expand if the position has been visited twice.

   Important variables:
   - idx:       array index of pos
   - game.found: flag set to 1 when an occupied position is hit,
                 used to decide whether to update S and T
*/
struct GameState replace(struct GameState game, struct Position pos) {
    int idx;
    idx = posToIndex(pos);
    game.found = 0;

    if (game.go == 1) {
        // Red's turn: capture Blue, acknowledge own, or claim free
        if (game.B[idx] == 1) {
            game.B[idx] = 0;
            game.found  = 1;
        } else if (game.R[idx] == 1) {
            game.found = 1;
        } else {
            game.R[idx] = 1;
        }
    } else {
        // Blue's turn: capture Red, acknowledge own, or claim free
        if (game.R[idx] == 1) {
            game.R[idx] = 0;
            game.found  = 1;
        } else if (game.B[idx] == 1) {
            game.found = 1;
        } else {
            game.B[idx] = 1;
        }
    }

    // first visit: add to S and reset found
    if (game.found == 1 && game.S[idx] == 0) {
        game.S[idx] = 1;
        game.found  = 0;
    // second visit: add to T and trigger Expand
    } else if (game.found == 1 && game.S[idx] == 1 && game.T[idx] == 0) {
        game.T[idx] = 1;
        game        = expand(game, pos);
    }

    return game;
}

/*
   expand

   Purpose: Triggered when a position is visited a second time.
            Removes the current position then spreads the Replace
            effect to its cardinal neighbors. Red expands upward,
            Blue expands downward. Both always expand left and right.
            Neighbors outside M = C x C are skipped.

   Important variables:
   - u, d, k, r: the four cardinal neighbors of pos
                 u = up, d = down, k = left, r = right
*/
struct GameState expand(struct GameState game, struct Position pos) {
    struct Position u, d, k, r;

    // compute all four cardinal neighbors
    u.a = pos.a - 1; u.b = pos.b;
    d.a = pos.a + 1; d.b = pos.b;
    k.a = pos.a;     k.b = pos.b - 1;
    r.a = pos.a;     r.b = pos.b + 1;

    // step 1: remove current position from all sets
    game = removePiece(game, pos);

    // step 2: conditional neighbor - Red goes up, Blue goes down
    if (game.go == 1) {
        if (isValidPos(u))
            game = replace(game, u);
    } else {
        if (isValidPos(d))
            game = replace(game, d);
    }

    // step 3: always replace left and right if within bounds
    if (isValidPos(k))
        game = replace(game, k);
    if (isValidPos(r))
        game = replace(game, r);

    return game;
}

/*
   update

   Purpose: Called during the movement phase when a player selects
            their own piece. Resets good to false, then checks if
            the position needs to be added to S. If pos was already
            in S and not yet in T, triggers Expand.

   Important variables:
   - idx:       array index of pos
   - game.good: reset to 0 at start, flipped to 1 if pos was
                not yet in S (meaning a new visit was recorded)
*/
struct GameState update(struct GameState game, struct Position pos) {
    int idx;
    idx       = posToIndex(pos);
    game.good = 0;

    // pos not yet seen: add to S and mark good as true
    if (game.S[idx] == 0) {
        game.S[idx] = 1;
        game.good   = !game.good; // flips 0 to 1
    }

    // pos already in S but not expanded: add to T and trigger Expand
    if (game.good == 0 && game.S[idx] == 1 && game.T[idx] == 0) {
        game.T[idx] = 1;
        game        = expand(game, pos);
    }

    return game;
}


/*
   nextPlayerMove

   Purpose: Main entry point for each player's turn. Handles both
            the placement phase (start=1) and movement phase
            (start=0). Ends placement when both R and B have one
            piece each. Switches turns and increments val if the
            move was valid (good=1).

   Important variables:
   - idx:        array index of pos
   - game.good:  set to 1 if move was valid, used to confirm turn
   - game.start: flipped to 0 when |R|=1 and |B|=1
   - game.go:    toggled after each valid move
   - game.val:   incremented after each valid move
*/
struct GameState nextPlayerMove(struct GameState game, struct Position pos) {
    int idx;
    idx = posToIndex(pos);

    if (!game.over && game.start && game.go) {
        // placement phase: Red places piece
        game.R[idx] = 1;
        game.S[idx] = 1;
        game.good   = 1;
    } else if (!game.over && game.start && !game.go) {
        // placement phase: Blue places piece
        game.B[idx] = 1;
        game.S[idx] = 1;
        game.good   = 1;
    } else if (!game.over && !game.start &&
               ((game.go && game.R[idx] == 1) ||
                (!game.go && game.B[idx] == 1))) {
        // movement phase: player selects their own piece
        game = update(game, pos);
        game.good = 1;
    }

    // end placement phase when both sides have exactly one piece
    if (game.start && countSet(game.R) == 1 && countSet(game.B) == 1)
        game.start = 0;

    // valid move: flip good, switch turns, increment val
    if (!game.over && game.good) {
        game.good = !game.good;  // reset to 0
        game.go   = !game.go;    // switch player
        game.val  = game.val + 1;
    }

    // recheck over condition after every move
    game.over = checkOver(game);

    return game;
}

/*
   gameOver

   Purpose: Determines and prints the final result of the game
            by comparing the sizes of R and B. Called once the
            over condition becomes true.

   Important variables:
   - rCount: number of Red occupied positions
   - bCount: number of Blue occupied positions
*/
void gameOver(struct GameState game) {
    int rCount, bCount;
    rCount = countSet(game.R);
    bCount = countSet(game.B);

    printf("\n========== GAME OVER ==========\n");
    printf("Turns played : %d\n", game.val);
    printf("Red  pieces  : %d\n", rCount);
    printf("Blue pieces  : %d\n", bCount);

    // determine winner based on piece count
    if (rCount > bCount)
        printf("RESULT: R wins!\n");
    else if (bCount > rCount)
        printf("RESULT: B wins!\n");
    else
        printf("RESULT: Draw!\n");

    printf("================================\n");
}

// -- Display Function --

/*
   printBoard

   Purpose: Prints the current state of the 3x3 board along
            with game state info. R = Red, B = Blue, . = Free.

   Important variables:
   - a, b: row and col used to iterate over the grid
   - idx:  array index of current cell
*/
void printBoard(struct GameState game) {
    int a, b, idx;

    printf("\n  Board (val=%d | %s | start=%d)\n",
           game.val,
           game.go ? "Red's turn" : "Blue's turn",
           game.start);
    printf("  +---+---+---+\n");

    for (a = 1; a <= C_MAX; a++) {
        printf("  |");
        for (b = 1; b <= C_MAX; b++) {
            struct Position p;
            p.a = a;
            p.b = b;
            idx = posToIndex(p);
            if (game.R[idx] == 1)
                printf(" R |");
            else if (game.B[idx] == 1)
                printf(" B |");
            else
                printf(" . |");
        }
        printf("  row %d\n", a);
        printf("  +---+---+---+\n");
    }

    printf("   col 1   2   3\n");
    printf("  |R|=%d  |B|=%d  |F|=%d\n",
           countSet(game.R), countSet(game.B), countF(game));
}

// -- Game Initialization -- 

struct GameState initGame() {
    struct GameState game;
    int i;
    game.good  = 0;
    game.go    = 1;
    game.start = 1;
    game.found = 0;
    game.val   = 0;
    game.over  = 0;

    for (i = 0; i < GRID_SIZE; i++) {
        game.R[i] = 0;
        game.B[i] = 0;
        game.S[i] = 0;
        game.T[i] = 0;
    }

    printf("Game Initialized\n");
    return game;
}

int main(int argc, const char * argv[]) {

    struct GameState game;
        
    game = initGame();
        
    // Confirm initialization     printf("Game initialized.\n");
    printf("good  = %d\n", game.good);
    printf("go    = %d\n", game.go);
    printf("start = %d\n", game.start);
    printf("found = %d\n", game.found);
    printf("over  = %d\n", game.over);
    printf("val   = %d\n", game.val);
    
    return EXIT_SUCCESS;
}
