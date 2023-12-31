#ifndef CROSSWORD_UI_H
#define CROSSWORD_UI_H

#include <stdint.h>

#include "state.h"

enum input_command : uint8_t {
    NONE,
    EXIT,
    EXIT_NOSAVE,
};

enum input_command handle_input(struct game_state *state);

void cursor_to_word(struct cursor *cursor, const struct solution *sol, const struct word_ref *word_ref);

#endif //CROSSWORD_UI_H
