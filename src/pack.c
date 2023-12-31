#include "pack.h"

#include <string.h>
#include <compression.h>
#include <ti/vars.h>
#include <stdlib.h>

#undef NDEBUG
#include <debug.h>

#include "puzzle.h"

struct puzzle_pack *open_pack(const char *filename) {
    var_t *var = os_GetAppVarData(filename, NULL);

    if (var == NULL) {
        return NULL;
    }

    if (var->size < sizeof (struct puzzle_pack)) {
        return NULL;
    }

    struct puzzle_pack *pack = (struct puzzle_pack *)var->data;
    if (memcmp(pack, PACK_MAGIC, strlen(PACK_MAGIC)) != 0) {
        return NULL;
    }
    if (pack->version != CURRENT_VERSION) {
        return NULL;
    }

    return pack;
}

#define PTR(offset) (((void*)(pack)) + offset)

bool load_puzzle(struct unpacked_puzzle *result, const struct puzzle_pack *pack, const char *pack_name, uint8_t puzzle_num) {
    strncpy(result->pack_name, pack_name, 9);
    result->puzzle = NULL;
    result->clues = NULL;
    char *grid = NULL;
    uint8_t *markup = NULL;

    if (pack == NULL || puzzle_num >= pack->num_puzzles) {
        goto error;
    }

    const struct puzzle_pack_entry *entry = &pack->puzzles[puzzle_num];

    grid = malloc(entry->width * entry->height);
    if (!grid) goto error;
    zx0_Decompress(grid, PTR(entry->solution_offset));

    result->clues = malloc(entry->total_clue_length);
    if (!result->clues) goto error;
    zx0_Decompress(result->clues, PTR(entry->clues_offset));

    if (entry->markup_offset) {
        markup = malloc(CEIL_DIV(entry->width * entry->height, 8));
        if (!markup) goto error;
        zx0_Decompress(markup, PTR(entry->markup_offset));
    }

    result->puzzle = puzzle_new(entry->width, entry->height, grid, result->clues, markup);
    if (!result->puzzle) goto error;

    free(markup);
    free(grid);

    return true;

error:
    puzzle_free(result->puzzle);
    free(result->clues);
    free(grid);
    free(markup);

    result->puzzle = NULL;
    result->clues = NULL;
    return false;
}

void unload_puzzle(struct unpacked_puzzle *unpacked) {
    puzzle_free(unpacked->puzzle);
    free(unpacked->clues);
}
