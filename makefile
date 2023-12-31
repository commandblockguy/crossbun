# ----------------------------
# Makefile Options
# ----------------------------

NAME = CROSSBUN
ICON = icon.png
DESCRIPTION = "Crossword Game"
COMPRESSED = YES
ARCHIVED = NO
LTO = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
