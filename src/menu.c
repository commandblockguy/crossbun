#include "menu.h"

#include <string.h>

#include "keypad.h"
#include "graphics.h"

#define MENU_BASE_X 40
#define TITLE "CROSSBUN"

int menu(uint24_t num_entries, const struct menu_entry *entries, const char *empty_msg, menu_hover_callback_t hover_callback, menu_select_callback_t select_callback) {
    uint24_t selection = 0;
    gfx_FillScreen(CLUE_PANE_BACKGROUND_COLOR);

    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_SetTextScale(3, 3);
    for (uint8_t i = 0; i < strlen(TITLE); i++) {
        uint8_t y = (i + 1) * (GFX_LCD_HEIGHT / (1 + strlen(TITLE))) - 12;
        gfx_SetTextXY(10, y);
        gfx_PrintChar(TITLE[i]);
    }
    gfx_SetTextScale(1, 1);

    if (num_entries == 0) {
        gfx_PrintStringXY(empty_msg, (GFX_LCD_WIDTH - gfx_GetStringWidth(empty_msg)) / 2, GFX_LCD_HEIGHT / 2);
        gfx_BlitBuffer();
        do {
            keypad_scan();
        } while (!keypad_has_edge(kb_KeyClear));
        return 0;
    }

    while (true) {
        keypad_scan();

        if (keypad_has_edge(kb_KeyClear)) {
            return 0;
        }

        if (keypad_has_edge(kb_KeyUp)) {
            if (selection == 0) selection = num_entries;
            selection--;
        }

        if (keypad_has_edge(kb_KeyDown)) {
            selection++;
            if (selection == num_entries) selection = 0;
        }

        if (keypad_has_edge(kb_Key2nd) || keypad_has_edge(kb_KeyEnter)) {
            int result = select_callback(entries[selection].handle);
            if (result) {
                return result;
            }
        }

        uint8_t y = 0;
        for (uint24_t i = 0; i < num_entries; i++) {
            if (i == selection) {
                gfx_SetColor(HIGHLIGHTED_BACKGROUND_COLOR);
            } else {
                gfx_SetColor(BACKGROUND_COLOR);
            }
            y = word_wrap_box(entries[i].text, MENU_BASE_X, y, MENU_DIVIDER_X - MENU_BASE_X, MENU_BASE_X + 6);
        }
        gfx_SetColor(BACKGROUND_COLOR);
        gfx_FillRectangle(MENU_BASE_X, y, MENU_DIVIDER_X - MENU_BASE_X, GFX_LCD_HEIGHT - y);

        gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
        gfx_FillRectangle_NoClip(MENU_DIVIDER_X, 0, GFX_LCD_WIDTH - MENU_DIVIDER_X, GFX_LCD_HEIGHT);
        hover_callback(entries[selection].handle);

        gfx_BlitBuffer();
    }
}