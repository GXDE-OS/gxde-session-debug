# GXDE Session Debug — build rules (strictly POSIX make)

.POSIX:

NAME     = gxde-session-debug
CC       = cc
CFLAGS   = -O2
CPPFLAGS =
LDFLAGS  =
LDLIBS   =

PREFIX   = /usr/local
DESTDIR  =

# Flags the project cannot build without. They are kept apart from the
# user-overridable CFLAGS/CPPFLAGS/LDFLAGS above, so packagers (e.g. the
# Debian build, which passes dpkg-buildflags on the command line) can
# replace those without dropping the language standard, POSIX feature
# macro, include paths, warnings, or pthread support.
ALL_CFLAGS   = -std=c11 -pedantic -Wall -Wextra -pthread $(CFLAGS)
ALL_CPPFLAGS = -D_POSIX_C_SOURCE=200809L \
               -Isrc/utils/log \
               -Isrc/utils/process_handler \
               -Isrc/session_handler \
               -Isrc/menu \
               $(CPPFLAGS)
ALL_LDFLAGS  = -pthread $(LDFLAGS)

SRCS = src/main.c \
       src/menu/menu.c \
       src/session_handler/session_handler.c \
       src/utils/log/log.c \
       src/utils/process_handler/process_handler.c
OBJS = $(SRCS:.c=.o)
BIN  = $(NAME)

TEST_PROCESS_BIN  = tests/process_handler_test
TEST_PROCESS_SRCS = tests/process_handler_test.c
TEST_PROCESS_OBJS = $(TEST_PROCESS_SRCS:.c=.o) \
                    src/utils/log/log.o \
                    src/utils/process_handler/process_handler.o

TEST_SESSION_BIN  = tests/session_handler_test
TEST_SESSION_SRCS = tests/session_handler_test.c
TEST_SESSION_OBJS = $(TEST_SESSION_SRCS:.c=.o) \
                    src/session_handler/session_handler.o \
                    src/utils/log/log.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(ALL_LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(TEST_PROCESS_BIN): $(TEST_PROCESS_OBJS)
	$(CC) $(ALL_LDFLAGS) -o $@ $(TEST_PROCESS_OBJS) $(LDLIBS)

$(TEST_SESSION_BIN): $(TEST_SESSION_OBJS)
	$(CC) $(ALL_LDFLAGS) -o $@ $(TEST_SESSION_OBJS) $(LDLIBS)

.c.o:
	$(CC) $(ALL_CPPFLAGS) $(ALL_CFLAGS) -c -o $@ $<

# POSIX make has no automatic header dependencies; list them by hand.
src/main.o: src/main.c src/session_handler/session_handler.h src/menu/menu.h \
            src/utils/log/log.h src/utils/process_handler/process_handler.h
src/menu/menu.o: src/menu/menu.c src/menu/menu.h \
                 src/session_handler/session_handler.h src/utils/log/log.h
src/session_handler/session_handler.o: src/session_handler/session_handler.c \
                                       src/session_handler/session_handler.h \
                                       src/utils/log/log.h
src/utils/log/log.o: src/utils/log/log.h src/utils/log/ansi_escape_code_def.h
src/utils/process_handler/process_handler.o: \
	src/utils/process_handler/process_handler.h src/utils/log/log.h
tests/process_handler_test.o: tests/process_handler_test.c \
                              src/utils/process_handler/process_handler.h \
                              src/utils/log/log.h
tests/session_handler_test.o: tests/session_handler_test.c \
                              src/session_handler/session_handler.h \
                              src/utils/log/log.h

check: $(TEST_PROCESS_BIN) $(TEST_SESSION_BIN)
	./$(TEST_PROCESS_BIN)
	./$(TEST_SESSION_BIN)

clean:
	rm -f $(OBJS) $(TEST_PROCESS_OBJS) $(TEST_SESSION_OBJS) $(BIN) $(TEST_PROCESS_BIN) $(TEST_SESSION_BIN)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(BIN) $(DESTDIR)$(PREFIX)/bin/$(NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
