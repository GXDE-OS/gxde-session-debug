# GXDE Session Debug — build rules (strictly POSIX make)
#
# Sources are expected under src/. Until src/main.c exists, `make` stops
# with "no rule to make target" — that file is implementation step 1.

.POSIX:

NAME     = gxde-session-debug
CC       = cc
CFLAGS   = -O2 -std=c11 -pedantic -Wall -Wextra -pthread
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -Isrc/utils/log \
           -Isrc/utils/process_handler
LDFLAGS  = -pthread
LDLIBS   =
PREFIX   = /usr/local
DESTDIR  =

SRCS = src/main.c \
       src/utils/log/log.c \
       src/utils/process_handler/process_handler.c
OBJS = $(SRCS:.c=.o)
BIN  = $(NAME)

TEST_BIN  = tests/process_handler_test
TEST_SRCS = tests/process_handler_test.c
TEST_OBJS = $(TEST_SRCS:.c=.o) \
            src/utils/log/log.o \
            src/utils/process_handler/process_handler.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(TEST_BIN): $(TEST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(TEST_OBJS) $(LDLIBS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# POSIX make has no automatic header dependencies; list them by hand.
src/utils/log/log.o: src/utils/log/log.h src/utils/log/ansi_escape_code_def.h
src/utils/process_handler/process_handler.o: \
	src/utils/process_handler/process_handler.h src/utils/log/log.h

check: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(BIN) $(TEST_BIN)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(BIN) $(DESTDIR)$(PREFIX)/bin/$(NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)