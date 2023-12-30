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

#include "puzzle.h"
#include "graphics.h"
#include "ui.h"
#include "pack.h"


int main(void)
{
    init_graphics(true);

    struct game_state state = {
            .cursor = {
                    .row = 0,
                    .col = 0,
                    .dir = ACROSS,
            },
    };

    struct puzzle_pack *pack = open_pack("TEST");
    if (!pack) goto error;

    struct unpacked_puzzle unpacked = load_puzzle(pack, 0);
    if (!unpacked.puzzle) goto error;

    state.puzzle = unpacked.puzzle;

    if (!solution_new(&state.solution, unpacked.puzzle)) {
        goto error;
    }

    while (handle_input(&state)) {
        draw_game(&state);
    }

error:
    solution_free(&state.solution);
    unload_puzzle(&unpacked);

    gfx_End();
    return 0;
}
