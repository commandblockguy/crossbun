#include "save.h"

#include <ti/vars.h>
#include <stdio.h>
#include <string.h>

#define SAVE_FILENAME "XBunSave"

struct save {
    uint8_t version;
    char pack[9];
    uint8_t puzzle_num;
    struct cursor cursor;
    char cells[];
};

void save_game(const struct game_state *state) {
    struct save save = {
            .version = CURRENT_VERSION,
            .puzzle_num = state->unpacked.puzzle_num,
            .cursor = state->cursor,
    };
    strncpy(save.pack, state->unpacked.pack_name, 9);
    FILE *f = fopen(SAVE_FILENAME, "w");
    if (!f) return;
    fwrite(&save, sizeof save, 1, f);
    for (uint8_t row = 0; row < state->puzzle->height; row++) {
        fwrite(state->solution.cells[row], 1, state->puzzle->width, f);
    }
    fclose(f);
}

bool load_save(struct game_state *state) {
    var_t *var = os_GetAppVarData(SAVE_FILENAME, NULL);
    if (!var || var->size < sizeof (struct save)) {
        return false;
    }

    struct save *save = (struct save*)var->data;
    if (save->version != CURRENT_VERSION) return false;

    if (!state_new(state, save->pack, save->puzzle_num)) {
        return false;
    }

    uint8_t height = state->puzzle->height;
    uint8_t width = state->puzzle->width;
    for (uint8_t row = 0; row < height; row++) {
        for (uint8_t col = 0; col < width; col++) {
            set_cell(&state->solution, row, col, save->cells[width * row + col]);
        }
    }

    state->cursor = save->cursor;

    return true;
}