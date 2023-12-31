#ifndef CROSSWORD_GRAPHICS_H
#define CROSSWORD_GRAPHICS_H

#include "state.h"

enum colors : uint8_t {
    BACKGROUND_COLOR,
    HIGHLIGHTED_BACKGROUND_COLOR,
    SELECTED_CLUE_BACKGROUND_COLOR,
    HIGHLIGHTED_SELECTED_CLUE_BACKGROUND_COLOR,
    CURSOR_BACKGROUND_COLOR,
    HIGHLIGHTED_CURSOR_BACKGROUND_COLOR,
    BORDER_COLOR,
    TEXT_COLOR,
    SOLVED_CLUE_TEXT_COLOR,
    NUMBER_TEXT_COLOR,
    CLUE_PANE_BACKGROUND_COLOR,
    CLUE_PANE_LABEL_COLOR,
    INCORRECT_TEXT_COLOR,
    COMPLETE_TEXT_COLOR,
};

void init_graphics(bool dark_mode);

void draw_game(const struct game_state *state);

int word_wrap_box(const char *text, int base_x, int base_y, int width, int indent_x);

#endif //CROSSWORD_GRAPHICS_H
