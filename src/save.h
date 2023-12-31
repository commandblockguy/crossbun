#ifndef CROSSWORD_SAVE_H
#define CROSSWORD_SAVE_H

#include "state.h"

void save_game(const struct game_state *state);
bool load_save(struct game_state *state);

#endif //CROSSWORD_SAVE_H
