CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall -Wextra

DESTDIR ?= /usr/local

BINS=ff-overlay
all: $(BINS)

ff-overlay: ff-overlay.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ff-overlay $^

install: $(BINS)
	install $(BINS) $(DESTDIR)/bin

clean:
	rm -f $(BINS)
