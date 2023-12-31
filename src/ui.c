#include "ui.h"

#include <keypadc.h>
#undef NDEBUG
#include <debug.h>

#include "state.h"

static uint8_t last_keypad_state[8];
static uint8_t rising_edges[8];

static void scan_keypad(void) {
    kb_Scan();
    for (int i = 1; i < 8; i++) {
        uint8_t current_state = kb_Data[i];
        rising_edges[i] = current_state & ~last_keypad_state[i];
        last_keypad_state[i] = current_state;
    }
}

static bool has_edge(kb_lkey_t key) {
    return (rising_edges[key >> 8] & key) != 0;
}

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

kb_lkey_t letter_map[] = {
        kb_KeyMath,
        kb_KeyApps,
        kb_KeyPrgm,
        kb_KeyRecip,
        kb_KeySin,
        kb_KeyCos,
        kb_KeyTan,
        kb_KeyPower,
        kb_KeySquare,
        kb_KeyComma,
        kb_KeyLParen,
        kb_KeyRParen,
        kb_KeyDiv,
        kb_KeyLog,
        kb_Key7,
        kb_Key8,
        kb_Key9,
        kb_KeyMul,
        kb_KeyLn,
        kb_Key4,
        kb_Key5,
        kb_Key6,
        kb_KeySub,
        kb_KeySto,
        kb_Key1,
        kb_Key2,
};

bool handle_input(struct game_state *state) {
    struct cursor *cursor = &state->cursor;
    struct puzzle *puzzle = state->puzzle;
    struct solution *sol = &state->solution;

    scan_keypad();

    if (has_edge(kb_KeyClear)) {
        return false;
    }

    if (has_edge(kb_Key2nd)) {
        cursor->dir = !cursor->dir;
    }

    if (has_edge(kb_KeyUp)) {
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

    if (has_edge(kb_KeyDown)) {
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

    if (has_edge(kb_KeyLeft)) {
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

    if (has_edge(kb_KeyRight)) {
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

    if (has_edge(kb_KeyYequ)) {
        cursor_prev_word(cursor, sol);
    }

    if (has_edge(kb_KeyGraph)) {
        cursor_next_word(cursor, sol);
    }

    if (state->solution.status != CORRECT) {
        for (uint8_t i = 0; i < sizeof letter_map / sizeof letter_map[0]; i++) {
            if (has_edge(letter_map[i])) {
                set_cell(&state->solution, cursor->row, cursor->col, i + 'A');
                cursor_advance(cursor, &state->solution);
            }
        }

        if (has_edge(kb_KeyDel)) {
            set_cell(&state->solution, cursor->row, cursor->col, EMPTY_CELL);
            cursor_prev_square(cursor, &state->solution);
        }
    }

    return true;
}