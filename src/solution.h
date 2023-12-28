#ifndef CROSSWORD_SOLUTION_H
#define CROSSWORD_SOLUTION_H

#include "puzzle.h"

struct solution {
    const struct puzzle *puzzle;
    char cells[MAX_PUZZLE_SIZE][MAX_PUZZLE_SIZE];
    bool (*filled_words)[2];
};

bool solution_new(struct solution *solution, const struct puzzle *puzzle);

void solution_free(struct solution *solution);

void set_cell(struct solution *solution, uint8_t row, uint8_t col, char c);

struct word_ref next_unfilled(const struct solution *sol, const struct word_ref *ref);

#define SOL_CURSOR_CELL(sol, cursor) ((sol)->cells[(cursor)->row][(cursor)->col])

#endif //CROSSWORD_SOLUTION_H
