#include "ui.h"

#include <keypadc.h>
#undef NDEBUG
#include <debug.h>

#include "state.h"
#include "keypad.h"

struct word_ref get_cursor_word_ref(const struct puzzle *puzzle, const struct cursor *cursor) {
    const struct cell *cell = &PUZ_CURSOR_CELL(puzzle, cursor);
    uint8_t num = cursor->dir == ACROSS ? cell->clue_a : cell->clue_d;
    struct word_ref result = { .dir = cursor->dir, .num = num };
    return result;
}

void cursor_to_word(struct cursor *cursor, const struct solution *sol, const struct word_ref *word_ref) {
    const struct clue *clue = get_clue(sol->puzzle, word_ref);
    if (!clue) return;

    cursor->dir = word_ref->dir;
    cursor->row = clue->start_row;
    cursor->col = clue->start_col;

    if (sol->filled_words[word_ref->num - 1][word_ref->dir]) {
        // no empty square in this word - go to start
        return;
    }

    uint8_t dx = word_ref->dir == ACROSS;
    uint8_t dy = word_ref->dir == DOWN;

    while (SOL_CURSOR_CELL(sol, cursor) != EMPTY_CELL) {
        cursor->row += dy;
        cursor->col += dx;
    }
}

static void cursor_next_word(struct cursor *cursor, const struct solution *sol) {
    struct word_ref current = get_cursor_word_ref(sol->puzzle, cursor);
    struct word_ref new = next_unfilled(sol, &current);
    if (new.num == 0) {
        new = next_word(sol->puzzle, &current);
    }
    cursor_to_word(cursor, sol, &new);
}

static void cursor_prev_word(struct cursor *cursor, const struct solution *sol) {
    struct word_ref current = get_cursor_word_ref(sol->puzzle, cursor);
    struct word_ref new = prev_unfilled(sol, &current);
    if (new.num == 0) {
        new = prev_word(sol->puzzle, &current);
    }
    cursor_to_word(cursor, sol, &new);
}

// move cursor to the previous non-black square
static void cursor_prev_square(struct cursor *cursor, const struct solution *sol) {
    do {
        uint8_t dx = cursor->dir == ACROSS;
        uint8_t dy = cursor->dir == DOWN;
        if (cursor->row == 0 && cursor->col == 0) {
            cursor->row = sol->puzzle->height - 1;
            cursor->col = sol->puzzle->width - 1;
            cursor->dir = !cursor->dir;
        } else {
            if (dy && cursor->row == 0) {
                cursor->row = sol->puzzle->height - 1;
                cursor->col -= 1;
            } else {
                cursor->row -= dy;
            }
            if (dx && cursor->col == 0) {
                cursor->col = sol->puzzle->width - 1;
                cursor->row -= 1;
            } else {
                cursor->col -= dx;
            }
        }
    } while (SOL_CURSOR_CELL(sol, cursor) == BLACK_CELL);
}

// moves cursor forward by one space
// if it reaches the end of the word, moves to the first open character
// in the first word not strictly before the current word
static void cursor_advance(struct cursor *cursor, const struct solution *sol) {
    struct cursor orig_cursor = *cursor;
    uint8_t dx = cursor->dir == ACROSS;
    uint8_t dy = cursor->dir == DOWN;
    while (SOL_CURSOR_CELL(sol, cursor) != EMPTY_CELL) {
        cursor->row += dy;
        cursor->col += dx;
        if (cursor->row >= sol->puzzle->height ||
            cursor->col >= sol->puzzle->width ||
            SOL_CURSOR_CELL(sol, cursor) == BLACK_CELL) {
            struct word_ref current = get_cursor_word_ref(sol->puzzle, &orig_cursor);
            struct word_ref new = next_unfilled_inclusive(sol, &current);
            if (new.num == 0) {
                new = next_word(sol->puzzle, &current);
            }
            cursor_to_word(cursor, sol, &new);
            break;
        }
    }
}

bool handle_input(struct game_state *state) {
    struct cursor *cursor = &state->cursor;
    struct puzzle *puzzle = state->puzzle;
    struct solution *sol = &state->solution;

    keypad_scan();

    if (keypad_has_edge(kb_KeyClear)) {
        return false;
    }

    if (keypad_has_edge(kb_Key2nd)) {
        cursor->dir = !cursor->dir;
    }

    if (keypad_has_edge(kb_KeyUp)) {
        if (cursor->dir == DOWN) {
            do {
                if (cursor->row == 0) {
                    cursor->row = puzzle->height;
                }
                cursor->row--;
            } while (SOL_CURSOR_CELL(sol, cursor) == BLACK_CELL);
        } else {
            cursor->dir = DOWN;
        }
    }

    if (keypad_has_edge(kb_KeyDown)) {
        if (cursor->dir == DOWN) {
            do {
                cursor->row++;
                if (cursor->row == puzzle->height) {
                    cursor->row = 0;
                }
            } while (SOL_CURSOR_CELL(sol, cursor) == BLACK_CELL);
        } else {
            cursor->dir = DOWN;
        }
    }

    if (keypad_has_edge(kb_KeyLeft)) {
        if (cursor->dir == ACROSS) {
            do {
                if (cursor->col == 0) {
                    cursor->col = puzzle->width;
                }
                cursor->col--;
            } while (SOL_CURSOR_CELL(sol, cursor) == BLACK_CELL);
        } else {
            cursor->dir = ACROSS;
        }
    }

    if (keypad_has_edge(kb_KeyRight)) {
        if (cursor->dir == ACROSS) {
            do {
                cursor->col++;
                if (cursor->col == puzzle->width) {
                    cursor->col = 0;
                }
            } while (SOL_CURSOR_CELL(sol, cursor) == BLACK_CELL);
        } else {
            cursor->dir = ACROSS;
        }
    }

    if (keypad_has_edge(kb_KeyYequ)) {
        cursor_prev_word(cursor, sol);
    }

    if (keypad_has_edge(kb_KeyGraph)) {
        cursor_next_word(cursor, sol);
    }

    if (state->solution.status != CORRECT) {
        char c = keypad_get_letter();
        if (c) {
            set_cell(&state->solution, cursor->row, cursor->col, c);
            cursor_advance(cursor, &state->solution);
        }

        if (keypad_has_edge(kb_KeyDel)) {
            set_cell(&state->solution, cursor->row, cursor->col, EMPTY_CELL);
            cursor_prev_square(cursor, &state->solution);
        }
    }

    return true;
}