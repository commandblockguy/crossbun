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


int main(void)
{
    init_graphics(true);

    struct game_state state = {};

    if (!state_new(&state, "TEST", 0)) goto error;

    while (handle_input(&state)) {
        draw_game(&state);
    }

error:
    state_free(&state);

    gfx_End();
    return 0;
}
