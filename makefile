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

DEPS = $(SRCDIR)/font.inc

# ----------------------------

include $(shell cedev-config --makefile)

$(SRCDIR)/font.inc: $(SRCDIR)/font.txt
	convfont -o c -t $(SRCDIR)/font.txt $(SRCDIR)/font.inc
