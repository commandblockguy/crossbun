#include "keypad.h"

#include <keypadc.h>

static uint8_t last_keypad_state[8];
static uint8_t rising_edges[8];

void keypad_scan(void) {
    kb_Scan();
    for (int i = 1; i < 8; i++) {
        uint8_t current_state = kb_Data[i];
        rising_edges[i] = current_state & ~last_keypad_state[i];
        last_keypad_state[i] = current_state;
    }
}

bool keypad_has_edge(kb_lkey_t key) {
    return (rising_edges[key >> 8] & key) != 0;
}

static kb_lkey_t letter_map[] = {
        kb_KeyMath,
        kb_KeyApps,
        kb_KeyPrgm,
        kb_KeyRecip,
        kb_KeySin,
        kb_KeyCos,
        kb_KeyTan,
        kb_KeyPower,
        kb_KeySquare,
        kb_KeyComma,
        kb_KeyLParen,
        kb_KeyRParen,
        kb_KeyDiv,
        kb_KeyLog,
        kb_Key7,
        kb_Key8,
        kb_Key9,
        kb_KeyMul,
        kb_KeyLn,
        kb_Key4,
        kb_Key5,
        kb_Key6,
        kb_KeySub,
        kb_KeySto,
        kb_Key1,
        kb_Key2,
};

char keypad_get_letter(void) {
    for (uint8_t i = 0; i < sizeof letter_map / sizeof letter_map[0]; i++) {
        if (keypad_has_edge(letter_map[i])) {
            return i + 'A';
        }
    }

    return 0;
}
