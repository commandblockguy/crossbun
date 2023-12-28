#include "solution.h"

#include <stdlib.h>
#undef NDEBUG
#include <debug.h>

bool solution_new(struct solution *solution, const struct puzzle *puzzle) {
    solution->puzzle = puzzle;
    for (uint8_t row = 0; row < puzzle->height; row++) {
        for (uint8_t col = 0; col < puzzle->width; col++) {
            if (puzzle->cells[row][col].content == BLACK_CELL) {
                solution->cells[row][col] = BLACK_CELL;
            } else {
                solution->cells[row][col] = EMPTY_CELL;
            }
        }
    }
    solution->filled_words = calloc(puzzle->num_clue_pairs, sizeof *solution->filled_words);
    return solution->filled_words != NULL;
}

void solution_free(struct solution *solution) {
    free(solution->filled_words);
}

static bool is_word_filled(struct solution *solution, const struct word_ref *word) {
    struct clue *clue = get_clue(solution->puzzle, word);
    if (!clue) return false;

    uint8_t dx = word->dir == ACROSS;
    uint8_t dy = word->dir == DOWN;
    for (
            uint8_t row = clue->start_row, col = clue->start_col;
            row < solution->puzzle->height && col < solution->puzzle->width &&
            solution->cells[row][col] != BLACK_CELL;
            row += dy, col += dx) {
        if (solution->cells[row][col] == EMPTY_CELL) {
            return false;
        }
    }
    return true;
}

void set_cell(struct solution *solution, uint8_t row, uint8_t col, char c) {
    solution->cells[row][col] = c;
    uint8_t clue_a = solution->puzzle->cells[row][col].clue_a;
    struct word_ref ref_a = {ACROSS, clue_a};
    solution->filled_words[clue_a - 1][ACROSS] = is_word_filled(solution, &ref_a);
    uint8_t clue_d = solution->puzzle->cells[row][col].clue_d;
    struct word_ref ref_d = {DOWN, clue_d};
    solution->filled_words[clue_d - 1][DOWN] = is_word_filled(solution, &ref_d);
}

// first unfilled word not before ref
struct word_ref next_unfilled_inclusive(const struct solution *sol, const struct word_ref *ref) {
    struct word_ref result = *ref;
    do {
        if (!sol->filled_words[result.num - 1][result.dir]) {
            return result;
        }
        result = next_word(sol->puzzle, &result);
    } while (result.dir != ref->dir || result.num != ref->num);
    result.num = 0;
    return result;
}

// first unfilled word not before ref
struct word_ref next_unfilled(const struct solution *sol, const struct word_ref *ref) {
    const struct word_ref temp = next_word(sol->puzzle, ref);
    return next_unfilled_inclusive(sol, &temp);
}

// first unfilled word not before ref
struct word_ref prev_unfilled(const struct solution *sol, const struct word_ref *ref) {
    struct word_ref result = *ref;
    do {
        result = prev_word(sol->puzzle, &result);
        if (!sol->filled_words[result.num - 1][result.dir]) {
            return result;
        }
    } while (result.dir != ref->dir || result.num != ref->num);
    result.num = 0;
    return result;
}
