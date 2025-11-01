# ----------------------------
# Makefile Options
# ----------------------------

NAME = CROSSBUN
ICON = icon.png
DESCRIPTION = "Crossword Game"
COMPRESSED = YES
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

font: $(SRCDIR)/font.txt
	convfont -o c -t $(SRCDIR)/font.txt $(SRCDIR)/font.inc
