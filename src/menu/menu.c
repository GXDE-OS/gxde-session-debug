/*
 * Copyright (C) 2026 CharOfString <root@charofstring.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "log.h"

#define INPUT_BUFFER_SIZE 64

int prompt_user_session_choice(const wayland_session_entry_t sessions[], int count) {
    if (count <= 0) {
        write_log_with_tag(LOG_WARNING, "Menu", "SessionDetector",
                           "No supported Wayland sessions found on the system.");
        return -1;
    }

    printf("\nAvailable Wayland sessions:\n");
    for (int i = 0; i < count; i++) {
        printf("  [%d] %-30s - %s\n", i + 1, sessions[i].name, sessions[i].description);
    }
    printf("  [q] Quit\n\n");

    char buf[INPUT_BUFFER_SIZE];
    while (1) {
        printf("Select a session to launch nested [1-%d, q]: ", count);
        fflush(stdout);

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("\n");
            return -1;
        }

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0) {
            return -1;
        }

        char* endptr = NULL;
        long choice = strtol(buf, &endptr, 10);
        if (endptr != buf && *endptr == '\0' && choice >= 1 && choice <= count) {
            return (int)(choice - 1);
        }

        printf("Invalid input \"%s\". Please enter a number between 1 and %d or 'q'.\n",
               buf, count);
    }
}

