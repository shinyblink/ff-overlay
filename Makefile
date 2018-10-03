CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall -Wextra

BINS=ff-overlay

all: $(BINS)

ff-overlay: ff-overlay.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ff-overlay $^

clean:
	rm -f $(BINS)
