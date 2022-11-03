CC ?= cc
CFLAGS ?= -Os
CPPLAGS += -pedantic -Wall -Wextra

PREFIX ?= /usr/local
DESTDIR ?= /

BINS=ff-overlay
all: $(BINS)

ff-overlay: ff-overlay.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ff-overlay $^ -lm

install: $(BINS)
	install -d $(DESTDIR)/$(PREFIX)/bin
	install $(BINS) $(DESTDIR)/$(PREFIX)/bin

clean:
	rm -f $(BINS)
