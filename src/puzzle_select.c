#include "puzzle_select.h"

#include <fileioc.h>
#include <string.h>

#include "menu.h"
#include "graphics.h"

struct puzzle_user_data {
    const struct puzzle_pack *pack;
    const char *pack_name;
    struct game_state *state;
};

static void puzzle_hover_callback(const struct menu_entry *entry, void *data) {
    struct puzzle_user_data *puzzle_entry_data = data;
    const struct puzzle_pack *pack = puzzle_entry_data->pack;
    const struct puzzle_pack_entry *pack_item = entry->data;
    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_PrintStringXY("Width: ", MENU_DIVIDER_X + 2, 3);
    gfx_PrintUInt(pack_item->width, 0);
    gfx_PrintString(" Height: ");
    gfx_PrintUInt(pack_item->height, 0);

    gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
    int y = word_wrap_box(PACK_OFFSET(pack, pack_item->author_offset), MENU_DIVIDER_X, 13, GFX_LCD_WIDTH - MENU_DIVIDER_X, MENU_DIVIDER_X + 22);
    gfx_PrintStringXY("By ", MENU_DIVIDER_X + 2, 15);
    y = word_wrap_box(PACK_OFFSET(pack, pack_item->description_offset), MENU_DIVIDER_X, y + 20, GFX_LCD_WIDTH - MENU_DIVIDER_X, MENU_DIVIDER_X + 2);
    word_wrap_box(PACK_OFFSET(pack, pack_item->copyright_offset), MENU_DIVIDER_X, y + 20, GFX_LCD_WIDTH - MENU_DIVIDER_X, MENU_DIVIDER_X + 2);
}

static int puzzle_select_callback(const struct menu_entry *entry, void *data) {
    struct puzzle_user_data *ped = data;
    const struct puzzle_pack_entry *pack_item = entry->data;

    if (!state_new(ped->state, ped->pack_name, pack_item - ped->pack->puzzles)) {
        return -1;
    }

    return 1;
}

struct pack_entry_data {
    char name[9];
    const struct puzzle_pack *pack;
};

static uint24_t get_pack_count(void) {
    uint24_t i = 0;
    char *var_name;
    void *vat_ptr = NULL;

    while ((var_name = ti_Detect(&vat_ptr, PACK_MAGIC))) {
        if (open_pack(var_name)) {
            i++;
        }
    }

    return i;
}

static bool get_pack_entries(struct menu_entry *entries) {
    uint24_t i = 0;
    char *var_name;
    void *vat_ptr = NULL;

    while ((var_name = ti_Detect(&vat_ptr, PACK_MAGIC))) {
        const struct puzzle_pack *pack = open_pack(var_name);
        if (pack) {
            struct pack_entry_data *entry_data = calloc(1, sizeof *entry_data);
            if (entry_data == NULL) {
                return false;
            }
            strcpy(entry_data->name, var_name);
            entry_data->pack = pack;
            entries[i].data = entry_data;
            entries[i].text = PACK_OFFSET(pack, pack->title_offset);
            i++;
        }
    }

    return true;
}

static void pack_hover_callback(const struct menu_entry *entry, __attribute((unused)) void *data) {
    const struct pack_entry_data *pack_entry = entry->data;
    const struct puzzle_pack *pack = pack_entry->pack;
    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_PrintStringXY("Appvar: ", MENU_DIVIDER_X + 2, 3);
    gfx_PrintString(pack_entry->name);
    gfx_PrintStringXY("Num puzzles: ", MENU_DIVIDER_X + 2, 15);
    gfx_PrintUInt(pack->num_puzzles, 0);
    gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
    int y = word_wrap_box(PACK_OFFSET(pack, pack->title_offset), MENU_DIVIDER_X, 30, GFX_LCD_WIDTH - MENU_DIVIDER_X, MENU_DIVIDER_X + 2);
    word_wrap_box(PACK_OFFSET(pack, pack->description_offset), MENU_DIVIDER_X, y + 20, GFX_LCD_WIDTH - MENU_DIVIDER_X, MENU_DIVIDER_X + 2);
}

static int pack_select_callback(const struct menu_entry *entry, void *data) {
    const struct pack_entry_data *pack_entry = entry->data;
    const struct puzzle_pack *pack = pack_entry->pack;
    struct menu_entry *entries = calloc(pack->num_puzzles, sizeof *entries);
    if (entries == NULL) return -1;

    for (uint24_t i = 0; i < pack->num_puzzles; i++) {
        const struct puzzle_pack_entry *pack_item = &pack->puzzles[i];
        entries[i].text = PACK_OFFSET(pack, pack_item->name_offset);
        entries[i].data = (void*)pack_item;
    }

    struct puzzle_user_data pud = {
            .pack = pack,
            .state = data,
            .pack_name = pack_entry->name,
    };

    int result = menu(pack->num_puzzles, entries, "No puzzles in this pack???", puzzle_hover_callback, puzzle_select_callback, &pud);

    free(entries);

    return result;
}

bool select_puzzle(struct game_state *state) {
    uint24_t pack_count = get_pack_count();
    struct menu_entry *entries = calloc(pack_count, sizeof *entries);
    if (!entries) return false;

    int result = 0;
    if (get_pack_entries(entries)) {
        result = menu(pack_count, entries, "No puzzle packs found!", pack_hover_callback, pack_select_callback, state);
    }

    for (uint24_t i = 0; i < pack_count; i++) {
        free(entries[i].data);
    }

    free(entries);

    return result > 0;
}
