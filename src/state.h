#ifndef CROSSWORD_STATE_H
#define CROSSWORD_STATE_H

#include "puzzle.h"
#include "solution.h"
#include "pack.h"

struct cursor {
    uint8_t row;
    uint8_t col;
    enum word_direction dir;
};

struct game_state {
    struct puzzle *puzzle;
    struct solution solution;
    struct cursor cursor;
    struct unpacked_puzzle unpacked;
};

bool state_new(struct game_state *state, const char *pack_name, uint8_t puz_num);
void state_free(struct game_state *state);

#endif //CROSSWORD_STATE_H
