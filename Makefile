# Copyright (C) CharOfString <root@charofstring.cc>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

.POSIX:

NAME     = gxde-session-debug
CC       = cc
CFLAGS   = -O2 -std=c11 -pedantic -Wall -Wextra
CPPFLAGS = -Isrc/utils/log
LDFLAGS  =
LDLIBS   =
PREFIX   = /usr/local
DESTDIR  =

SRCS = src/main.c src/utils/log/log.c src/utils/path_handler/path_handler.c
OBJS = $(SRCS:.c=.o)
BIN  = $(NAME)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

# POSIX make has no automatic header dependencies; list them by hand.
src/utils/log/log.o: src/utils/log/log.h src/utils/log/ansi_escape_code_def.h

clean:
	rm -f $(OBJS) $(BIN)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(BIN) $(DESTDIR)$(PREFIX)/bin/$(NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
