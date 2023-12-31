#include "state.h"
#include "pack.h"
#include "ui.h"

bool state_new(struct game_state *state, const char *pack_name, uint8_t puz_num) {
    struct puzzle_pack *pack = open_pack(pack_name);
    if (pack == NULL) {
        return false;
    }

    if (!load_puzzle(&state->unpacked, pack, pack_name, puz_num)) {
        return false;
    }

    state->puzzle = state->unpacked.puzzle;

    if (!solution_new(&state->solution, state->unpacked.puzzle)) {
        return false;
    }

    struct word_ref first = first_word(state->solution.puzzle);
    cursor_to_word(&state->cursor, &state->solution, &first);

    return true;
}

void state_free(struct game_state *state) {
    solution_free(&state->solution);
    unload_puzzle(&state->unpacked);
}