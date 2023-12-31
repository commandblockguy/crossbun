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

void test_hover(void *handle) {
    gfx_PrintStringXY(handle, MENU_DIVIDER_X + 25, 100);
}

int test_select(void *handle) {
    char *c = handle;
    return *c == 't';
}

struct menu_entry test_entries[] = {
        {"asdf", "option 1"},
        {"hjkl", "option 2"},
        {"tomato", "tomato"},
};

int main(void)
{
    init_graphics(true);

    menu(3, test_entries, "No puzzle packs found!", test_hover, test_select);

    struct game_state state = {};

    if (!load_save(&state)) {
        if (!state_new(&state, "TEST", 0)) goto error;
    }

    while (handle_input(&state)) {
        draw_game(&state);
    }

    if (state.solution.status != CORRECT) {
        save_game(&state);
    }

error:
    state_free(&state);

    gfx_End();
    return 0;
}
