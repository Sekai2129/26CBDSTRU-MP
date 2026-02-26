#include <stdlib.h>
#include <stdio.h>

#define GRID_SIZE 9
#define C_MIN 1
#define C_MAX 3
#define N_MAX 16
#define OVER_VAL 20

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
