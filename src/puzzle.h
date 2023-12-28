#ifndef CROSSWORD_PUZZLE_H
#define CROSSWORD_PUZZLE_H

#include <stdint.h>
#include <stdbool.h>

#include "util.h"

#define MAX_PUZZLE_SIZE 15

#define EMPTY_CELL ' '
#define BLACK_CELL '.'

struct cell {
    char content;
    uint8_t clue_a; // Across clue number
    uint8_t clue_d; // Down clue number
};

enum word_direction : uint8_t {
    ACROSS,
    DOWN,
};

struct word_ref {
    enum word_direction dir;
    uint8_t num;
};

struct clue {
    const char *text;
    uint8_t start_row;
    uint8_t start_col;
    uint8_t num_refs;
    struct word_ref references[];
};

struct clue_pair {
    struct clue *across;
    struct clue *down;
};

struct puzzle {
    uint8_t width;
    uint8_t height;
    struct cell cells[MAX_PUZZLE_SIZE][MAX_PUZZLE_SIZE];
    uint8_t num_clue_pairs;
    struct clue_pair *clues;
};

struct puzzle *puzzle_new(uint8_t width, uint8_t height, const char *grid, const char **clues);

void puzzle_free(struct puzzle *puzzle);

struct clue *get_clue(const struct puzzle *puzzle, const struct word_ref *ref);

struct word_ref next_word(const struct puzzle *puzzle, const struct word_ref *ref);

#define PUZ_CURSOR_CELL(puz, cursor) ((puz)->cells[(cursor)->row][(cursor)->col])

#endif //CROSSWORD_PUZZLE_H
