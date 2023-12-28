#ifndef CROSSWORD_STATE_H
#define CROSSWORD_STATE_H

#include "puzzle.h"
#include "solution.h"

struct cursor {
    uint8_t row;
    uint8_t col;
    enum word_direction dir;
};

struct game_state {
    struct puzzle *puzzle;
    struct solution solution;
    struct cursor cursor;
};

#endif //CROSSWORD_STATE_H
