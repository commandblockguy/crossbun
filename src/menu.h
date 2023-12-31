#ifndef CROSSWORD_MENU_H
#define CROSSWORD_MENU_H

#include <stdint.h>
#include <graphx.h>

#define MENU_DIVIDER_X (GFX_LCD_WIDTH / 2)

struct menu_entry {
    const char *text;
    void *handle;
};

typedef void (*menu_hover_callback_t)(void *handle);
typedef int (*menu_select_callback_t)(void *handle);

int menu(uint24_t num_entries, const struct menu_entry *entries, const char *empty_msg, menu_hover_callback_t hover_callback, menu_select_callback_t select_callback);

#endif //CROSSWORD_MENU_H
