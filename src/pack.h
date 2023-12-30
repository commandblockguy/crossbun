#ifndef CROSSWORD_PACK_H
#define CROSSWORD_PACK_H

#include <stdint.h>

#define PACK_MAGIC "CROSS"
#define CURRENT_VERSION 0

struct puzzle_pack_entry {
    uint16_t name_offset;
    uint16_t description_offset;
    uint16_t author_offset;
    uint16_t copyright_offset;
    uint8_t width;
    uint8_t height;
    uint16_t solution_offset;
    uint8_t num_words;
    uint16_t total_clue_length;
    uint16_t clues_offset;
    uint16_t markup_offset;
};

struct puzzle_pack {
    char magic[5];
    uint8_t version;
    uint16_t title_offset;
    uint16_t description_offset;
    uint8_t num_puzzles;
    struct puzzle_pack_entry puzzles[];
};

struct unpacked_puzzle {
    struct puzzle *puzzle;
    char *clues;
};

struct puzzle_pack *open_pack(const char *filename);

// loads a puzzle - must later be unloaded with unload_puzzle
struct unpacked_puzzle load_puzzle(const struct puzzle_pack *pack, uint8_t puzzle_num);

// unloads a puzzle previously loaded with load_puzzle
void unload_puzzle(struct unpacked_puzzle *unpacked);

#endif