/*
 *--------------------------------------
 * Program Name:
 * Author:
 * License:
 * Description:
 *--------------------------------------
*/

#include <graphx.h>
#include <ti/getcsc.h>
#undef NDEBUG
#include <debug.h>

#include "graphics.h"
#include "ui.h"
#include "save.h"
#include "menu.h"
#include "puzzle_select.h"

int main(void)
{
    init_graphics(true);

    struct game_state state = {};

    if (!load_save(&state)) {
        if (!select_puzzle(&state)) goto error;
    }

    enum input_command cmd;

    while ((cmd = handle_input(&state)) == NONE) {
        draw_game(&state);
    }

    if (cmd != EXIT_NOSAVE && state.solution.status != CORRECT) {
        save_game(&state);
    }

error:
    state_free(&state);

    gfx_End();
    return 0;
}
