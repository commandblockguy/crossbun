#ifndef CROSSWORD_GRAPHICS_H
#define CROSSWORD_GRAPHICS_H

#include "state.h"

enum colors : uint8_t {
    BACKGROUND_COLOR,
    BORDER_COLOR,
    TEXT_COLOR,
    SOLVED_CLUE_TEXT_COLOR,
    SELECTED_CLUE_BACKGROUND_COLOR,
    CURSOR_BACKGROUND_COLOR,
    REF_CLUE_BACKGROUND_COLOR,
    CLUE_PANE_BACKGROUND_COLOR,
    CLUE_PANE_LABEL_COLOR,
};

void init_graphics(bool dark_mode);

void draw_game(const struct game_state *state);

#endif //CROSSWORD_GRAPHICS_H
