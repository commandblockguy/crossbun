#ifndef CROSSWORD_KEYPAD_H
#define CROSSWORD_KEYPAD_H

#include <keypadc.h>

void keypad_scan(void);
bool keypad_has_edge(kb_lkey_t key);
char keypad_get_letter(void);

#endif //CROSSWORD_KEYPAD_H
