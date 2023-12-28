#include "puzzle.h"

#include <string.h>
#include <stdlib.h>

struct puzzle *puzzle_new(uint8_t width, uint8_t height, const char *grid, const char **clues) {
    if (width > MAX_PUZZLE_SIZE || height > MAX_PUZZLE_SIZE) {
        return NULL;
    }
    struct puzzle *result = calloc(sizeof *result, 1);
    if (result == NULL) goto error;
    result->width = width;
    result->height = height;
    result->num_clue_pairs = 0;
    const char **current_clue = clues;
    for (uint8_t row = 0; row < height; row++) {
        for (uint8_t col = 0; col < width; col++) {
            struct cell *cell = &result->cells[row][col];
            cell->content = grid[width * row + col];
            cell->clue_a = col ? result->cells[row][col - 1].clue_a : 0;
            cell->clue_d = row ? result->cells[row - 1][col].clue_d : 0;
            bool is_black = grid[width * row + col] == BLACK_CELL;
            bool is_left_black   = col == 0          || grid[width * row + (col - 1)] == BLACK_CELL;
            bool is_right_black  = col == width - 1  || grid[width * row + (col + 1)] == BLACK_CELL;
            bool is_top_black    = row == 0          || grid[width * (row - 1) + col] == BLACK_CELL;
            bool is_bottom_black = row == height - 1 || grid[width * (row + 1) + col] == BLACK_CELL;
            bool has_across_clue = !is_black && is_left_black && !is_right_black;
            bool has_down_clue   = !is_black && is_top_black  && !is_bottom_black;
            bool has_clue = has_across_clue || has_down_clue;
            if (has_clue) {
                struct clue_pair *realloced = realloc(result->clues, (sizeof *result->clues) * (result->num_clue_pairs + 1));
                if (realloced == NULL) goto error;
                result->num_clue_pairs++;
                result->clues = realloced;
                struct clue_pair *current_pair = &result->clues[result->num_clue_pairs - 1];
                current_pair->across = NULL;
                current_pair->down = NULL;
                if (has_across_clue) {
                    struct clue *clue = malloc(sizeof *clue);
                    if (clue == NULL) goto error;
                    current_pair->across = clue;
                    clue->text = *current_clue++;
                    clue->start_col = col;
                    clue->start_row = row;
                    // TODO: parse references
                    clue->num_refs = 0;
                    cell->clue_a = result->num_clue_pairs;
                }
                if (has_down_clue) {
                    struct clue *clue = malloc(sizeof *clue);
                    if (clue == NULL) goto error;
                    current_pair->down = clue;
                    clue->text = *current_clue++;
                    clue->start_col = col;
                    clue->start_row = row;
                    // TODO: parse references
                    clue->num_refs = 0;
                    cell->clue_d = result->num_clue_pairs;
                }
            }
            if (cell->content == BLACK_CELL) {
                cell->clue_a = 0;
                cell->clue_d = 0;
            }
        }
    }
    return result;
error:
    puzzle_free(result);
    return NULL;
}

void puzzle_free(struct puzzle *puzzle) {
    if (puzzle == NULL) return;

    for (int i = 0; i < puzzle->num_clue_pairs; i++) {
        free(puzzle->clues[i].across);
        free(puzzle->clues[i].down);
    }

    free(puzzle->clues);

    free(puzzle);
}

struct clue *get_clue(const struct puzzle *puzzle, const struct word_ref *ref) {
    if (ref->num == 0) return NULL;
    const struct clue_pair *pair = &puzzle->clues[ref->num - 1];
    return ref->dir == ACROSS ? pair->across : pair->down;
}

struct word_ref next_word(const struct puzzle *puzzle, const struct word_ref *ref) {
    struct word_ref result = *ref;
    do {
        result.num++;
        if (result.num > puzzle->num_clue_pairs) {
            result.dir = !result.dir;
            result.num = 0;
        }
    } while(!get_clue(puzzle, &result));
    return result;
}
