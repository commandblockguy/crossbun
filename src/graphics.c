#include "graphics.h"

#include <graphx.h>
#undef NDEBUG
#include <debug.h>

#include "state.h"

#define MAX_GRID_HEIGHT 225
#define MAX_GRID_WIDTH 225

#define CLUE_MARGIN 2
#define TEXT_HEIGHT 8
#define LINE_SPACING 11

static uint16_t palette_dark[] = {
        [BACKGROUND_COLOR] = gfx_RGBTo1555(53, 53, 53),
        [HIGHLIGHTED_BACKGROUND_COLOR] = gfx_RGBTo1555(93, 93, 93),
        [SELECTED_CLUE_BACKGROUND_COLOR] = gfx_RGBTo1555(79, 62, 47),
        [HIGHLIGHTED_SELECTED_CLUE_BACKGROUND_COLOR] = gfx_RGBTo1555(114, 101, 89),
        [CURSOR_BACKGROUND_COLOR] = gfx_RGBTo1555(118, 76, 39),
        [HIGHLIGHTED_CURSOR_BACKGROUND_COLOR] = gfx_RGBTo1555(145, 112, 82),
        [BORDER_COLOR] = gfx_RGBTo1555(0, 0, 0),
        [TEXT_COLOR] = gfx_RGBTo1555(208, 208, 208),
        [SOLVED_CLUE_TEXT_COLOR] = gfx_RGBTo1555(137, 137, 137),
        [NUMBER_TEXT_COLOR] = gfx_RGBTo1555(137, 137, 137),
        [CLUE_PANE_BACKGROUND_COLOR] = gfx_RGBTo1555(34, 34, 34),
        [CLUE_PANE_LABEL_COLOR] = gfx_RGBTo1555(156, 100, 49),
};

void init_graphics(bool dark_mode) {
    gfx_Begin();
    gfx_SetDrawBuffer();

    // todo: light theme
    gfx_SetPalette(palette_dark, sizeof palette_dark, 0);
    gfx_FillScreen(BACKGROUND_COLOR);
}

// returns the y position of the pixel immediately below the bg rectangle
static int word_wrap_box(const char *text, int base_x, int base_y, int width, int indent_x) {
    int y = base_y;
    const char *cur_word = text;
    int x = indent_x;
    int word_width = 0;
    int hyphen_length = gfx_GetCharWidth('-');
    if (y + LINE_SPACING > GFX_LCD_HEIGHT) {
        goto done;
    }
    gfx_FillRectangle(base_x, y, width, LINE_SPACING); // todo: optimize this
    // todo: work out newlines
    // todo: clean up control flow
    for (const char *c = cur_word; ; c++) {
        if (*c == ' ' || *c == '\n' || !*c) {
            if (word_width > width - 2 * CLUE_MARGIN) {
                // word is longer than one line - hyphenate
                word_width = 0;
                gfx_SetTextXY(x, y + 2);
                for (c = cur_word; x + word_width + hyphen_length <= base_x + width - 2 * CLUE_MARGIN; c++) {
                    word_width += gfx_GetCharWidth(*c);
                    gfx_PrintChar(*c);
                }
                if (cur_word != c) {
                    gfx_PrintChar('-');
                }
                cur_word = c;
                x = base_x + CLUE_MARGIN;
                y += LINE_SPACING;
                if (y + LINE_SPACING > GFX_LCD_HEIGHT) {
                    goto done;
                }
                gfx_FillRectangle(base_x, y, width, LINE_SPACING); // todo: optimize this
                word_width = 0;
                c = cur_word - 1;
                continue;
            } else if (x + word_width <= base_x + width - CLUE_MARGIN) {
                // output word
                gfx_SetTextXY(x, y + 2);
                for (; cur_word < c; cur_word++) {
                    gfx_PrintChar(*cur_word);
                }
                cur_word++; // skip space
                x += word_width + gfx_GetCharWidth(' ');
                word_width = 0;
            } else {
                // move to next line
                x = base_x + CLUE_MARGIN;
                y += LINE_SPACING;
                if (y + LINE_SPACING > GFX_LCD_HEIGHT) {
                    goto done;
                }
                gfx_FillRectangle(base_x, y, width, LINE_SPACING); // todo: optimize this
                word_width = 0;
                c = cur_word - 1;
                continue;
            }
        } else {
            word_width += gfx_GetCharWidth(*c);
        }
        if (!*c || *c == '\n') {
            break;
        }
    }
    y += LINE_SPACING;
done:
    gfx_FillRectangle(base_x, y, width, 2);
    y += 2;
    return y;
}

static void draw_clues(const struct game_state *state, int base_x) {
    const struct puzzle *puzzle = state->puzzle;
    const struct cursor *cursor = &state->cursor;
    const struct solution *solution = &state->solution;

    int y = LINE_SPACING + 3;

    gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
    gfx_FillRectangle_NoClip(base_x, 0, GFX_LCD_WIDTH - base_x, y);

    gfx_SetTextFGColor(CLUE_PANE_LABEL_COLOR);
    gfx_PrintStringXY(cursor->dir == ACROSS ? "ACROSS" : "DOWN", base_x + CLUE_MARGIN, 2);

    const struct cell *selected_cell = &puzzle->cells[cursor->row][cursor->col];
    uint8_t selected_clue_num = cursor->dir == ACROSS ? selected_cell->clue_a : selected_cell->clue_d;
    uint8_t clue_num = selected_clue_num ? selected_clue_num : 1;
    enum word_direction dir = cursor->dir;
    for (; y + LINE_SPACING < GFX_LCD_HEIGHT; clue_num++) {
        if (clue_num > puzzle->num_clue_pairs) {
            clue_num = 1;
            dir = !dir;

            gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
            int new_y = y + LINE_SPACING + 3;
            gfx_FillRectangle_NoClip(base_x, y, GFX_LCD_WIDTH - base_x, new_y);

            gfx_SetTextFGColor(CLUE_PANE_LABEL_COLOR);
            gfx_PrintStringXY(dir == ACROSS ? "ACROSS" : "DOWN", base_x + CLUE_MARGIN, y + 2);

            y = new_y;

            if (clue_num > puzzle->num_clue_pairs) continue;
        }
        const struct clue_pair *pair = &puzzle->clues[clue_num - 1];
        const struct clue *clue = dir == ACROSS ? pair->across : pair->down;
        if (!clue)  continue;

        int indent_x = base_x + 24;
        if (solution->filled_words[clue_num - 1][dir]) {
            gfx_SetTextFGColor(SOLVED_CLUE_TEXT_COLOR);
        } else {
            gfx_SetTextFGColor(TEXT_COLOR);
        }
        if (dir == cursor->dir && clue_num == selected_clue_num) {
            gfx_SetColor(SELECTED_CLUE_BACKGROUND_COLOR);
        } else {
            gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
        }
        int new_y = word_wrap_box(clue->text, base_x, y, GFX_LCD_WIDTH - base_x, indent_x);
        gfx_SetTextXY(base_x + CLUE_MARGIN, y + CLUE_MARGIN);
        gfx_PrintUInt(clue_num, 0);
        gfx_PrintChar('.');
        y = new_y;
    }

    gfx_SetColor(CLUE_PANE_BACKGROUND_COLOR);
    gfx_FillRectangle_NoClip(base_x, y, GFX_LCD_WIDTH - base_x, GFX_LCD_HEIGHT - y);
}

void draw_game(const struct game_state *state) {
    const struct puzzle *puzzle = state->puzzle;
    const struct cursor *cursor = &state->cursor;
    const struct solution *solution = &state->solution;

    // todo: remove
    gfx_FillScreen(BACKGROUND_COLOR);

    int base_x = 0;
    int base_y = 0;
    uint8_t cell_size = min(MAX_GRID_HEIGHT / puzzle->height, MAX_GRID_WIDTH / puzzle->width);
    uint8_t text_scale = cell_size * 2 / 3 / TEXT_HEIGHT;

    int hint_box_start = cell_size * puzzle->width + 1;
    draw_clues(state, hint_box_start);

    const struct cell *selected_cell = &puzzle->cells[cursor->row][cursor->col];
    uint8_t selected_clue_num = cursor->dir == ACROSS ? selected_cell->clue_a : selected_cell->clue_d;

    for (uint8_t row = 0; row < puzzle->height; row++) {
        for (uint8_t col = 0; col < puzzle->width; col++) {
            const struct cell *current_cell = &puzzle->cells[row][col];
            char c = solution->cells[row][col];
            gfx_SetColor(BORDER_COLOR);
            if (c == BLACK_CELL) {
                gfx_FillRectangle(base_x + cell_size * col, base_y + cell_size * row, cell_size + 1, cell_size + 1);
            } else {
                gfx_Rectangle(base_x + cell_size * col, base_y + cell_size * row, cell_size + 1, cell_size + 1);
                uint8_t current_clue_num = cursor->dir == ACROSS ? current_cell->clue_a : current_cell->clue_d;
                bool highlighted = puzzle->cells[row][col].highlighted;
                if (cursor->row == row && cursor->col == col) {
                    gfx_SetColor(CURSOR_BACKGROUND_COLOR + highlighted);
                } else if (current_clue_num && current_clue_num == selected_clue_num) {
                    gfx_SetColor(SELECTED_CLUE_BACKGROUND_COLOR + highlighted);
                } else {
                    gfx_SetColor(BACKGROUND_COLOR + highlighted);
                }
                gfx_FillRectangle_NoClip(base_x + cell_size * col + 1, base_y + cell_size * row + 1, cell_size - 1, cell_size - 1);

                if (c == EMPTY_CELL && current_cell->word_num) {
                    gfx_SetTextFGColor(NUMBER_TEXT_COLOR);
                    gfx_SetTextScale(1, 1);
                    gfx_SetTextXY(base_x + col * cell_size + 1, base_y + row * cell_size + 1);
                    gfx_PrintUInt(current_cell->word_num, 0);
                }

                gfx_SetTextFGColor(TEXT_COLOR);
                gfx_SetTextScale(text_scale, text_scale);

                uint8_t letter_offset_x = (cell_size + 2 - (gfx_GetCharWidth(c) - text_scale)) / 2;
                uint8_t letter_offset_y = (cell_size + 2 - text_scale * (TEXT_HEIGHT - 1)) / 2;

                gfx_SetTextXY(base_x + col * cell_size + letter_offset_x, base_y + row * cell_size + letter_offset_y);
                gfx_PrintChar(c);
            }
        }
    }
    gfx_SetTextScale(1, 1);

    gfx_BlitBuffer();
}