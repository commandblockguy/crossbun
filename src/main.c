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

    const char *clues[] = {
            "Whispery abcdefghijklmnopqrstuvwyxz YouTube genre",
            "NATO letter",
            "Round Table titles",
            "Juilliard degs.",
            "Cut the grass again",
            "Trunk, in Tottenham",
            "One who might say \"Your money's no good here\"?",
            "\"... good witch ___ bad witch?\"",
            "In base eight",
            "Grp. of shooting stars?",
            "\"If you're watching this, I'm already dead...\" might be said in a video one",
            "Fan's sound",
            "Not just via DMs, say ",
            "Texter's tee-hee",
            "Many ozs.",
            "\"He likes it! Hey Mikey!\"",
            "St. Louis landmark",
            "Pulsate",
            "He went to the woods because he \"wished to live deliberately\"",
            "24 images per second, for most films",
            "Chinese instrument",
            "Small mountains",
            "Categorize",
            "Choice words",
            "What the fish said when it swam into a wall, in a dad joke",
            "They carry loads",
            "Ultimatum ender",
            "Not live",
            "\"You are waaay off, friendo!\"",
            "Seasonal mall hirees",
            "Trig calc.",
            "Computer memories",
            "Beneficial",
            "Jet ski that's not a Jet Ski",
            "WWW address",
            "Soul, to Sartre",
            "Strangely, the ACLU is representing them in a 1st amendment case ",
            "Small quantity",
            "Barrister's topper",
            "Subj. for immigrants",
            "Hallucinogenic initials",
            "\"Flow gently, sweet _____, among thy green braes\" in a Burns poem",
            "Srta: Spain :: ___ : Germany",
            "Red Lobster or Robin",
            "Sea nymphs (that sound like a meteor shower)",
            "Personal quirks",
            "Bygone mail unit that's somewhere between a letter and a postcard",
            "Row of privets, maybe",
            "Fizzle out",
            "Links org.",
            "\"Xanadu\" grp.",
            "Totally tubular",
            "Org. for Federer",
            "\"___ Howser, M.D.\"",
            "___-Magnon man",
            "Delighted squeal",
            "Goddess of the dawn",
            "Jinxed it",
            "Krispy ___",
            "For days ___ (nonstop)",
            "Bikini experiment, for short",
            "Really tease",
            "Prankster's cry of triumph",
            "Muslim civil rights org.",
            "TV cable letters",
            "PDQ",
            "Oscar and Emmy winner Riz",
            "Police dept. alert",
            "Yes, in Yokohama",
            "\"Letter From Birmingham Jail\" author, initially",
            "Technology enthusiasts",
            "Obamacare initials",
            "Where to get dates",
            "Machu Picchu builder",
            "Finnish honey drink with such a low alcohol percentage, kids drink it",
            "With 72A, how you might cross the country (or a clue to the starts of 16, 22, 39, 53, and 65-Across)",
            "Dutch cheese",
            "See 70A",

    };
    struct puzzle *puzzle = puzzle_new(15, 15, "ASMR.BOOT..WILLLIFE.ARCH.THROBFRAMERATE.HILLSASSORT.ANDOR......WHEELBARROWSCOS.URL.AME.NRAAFEW.ESL..AFTONCHAINRESTAURANTHEDGE..DIE.LPGAELO.RAD.CRO.EOSSPOKETOOSOON......RIPON.GOTCHAAHMED.GEARHEADSPALMS.INCA.SIMABIKE..EDAM.TRIP", clues);

//    const char *clues[] = {"A", "B", "C", "D"};
//    struct puzzle *puzzle = puzzle_new(2, 2, "ABCD", clues);

    if (!puzzle) goto error;

    state.puzzle = puzzle;

    if (!solution_new(&state.solution, puzzle)) {
        goto error;
    }

    while (handle_input(&state)) {
        draw_game(&state);
    }

error:
    puzzle_free(puzzle);
    solution_free(&state.solution);

    gfx_End();
    return 0;
}
