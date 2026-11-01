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
#include <unistd.h>

#include "session_handler.h"
#include "log.h"

int is_executable_available(const char* executable) {
    if (executable == NULL || *executable == '\0') {
        return 0;
    }

    if (strchr(executable, '/') != NULL) {
        return access(executable, X_OK) == 0;
    }

    const char* path_env = getenv("PATH");
    if (path_env == NULL) {
        return 0;
    }

    char* path_copy = strdup(path_env);
    if (path_copy == NULL) {
        return 0;
    }

    int found = 0;
    char* saveptr = NULL;
    char* dir = strtok_r(path_copy, ":", &saveptr);
    while (dir != NULL) {
        char full_path[512];
        int written = snprintf(full_path, sizeof(full_path), "%s/%s", dir, executable);
        if (written > 0 && (size_t)written < sizeof(full_path)) {
            if (access(full_path, X_OK) == 0) {
                found = 1;
                break;
            }
        }
        dir = strtok_r(NULL, ":", &saveptr);
    }

    free(path_copy);
    return found;
}

int detect_available_sessions(wayland_session_entry_t sessions[], int max_count) {
    int count = 0;

    /* 1. GXDE with flakewm (full DDE desktop inside nested window) */
    if (is_executable_available("flakewm") && is_executable_available("startdde") &&
        count < max_count) {
        strncpy(sessions[count].name, "GXDE (flake-wm full session)",
                sizeof(sessions[count].name) - 1);
        sessions[count].name[sizeof(sessions[count].name) - 1] = '\0';

        strncpy(sessions[count].description,
                "Run full GXDE desktop inside isolated nested flake-wm",
                sizeof(sessions[count].description) - 1);
        sessions[count].description[sizeof(sessions[count].description) - 1] = '\0';

        strncpy(sessions[count].program, "flakewm", sizeof(sessions[count].program) - 1);
        sessions[count].program[sizeof(sessions[count].program) - 1] = '\0';

        sessions[count].argv[0] = sessions[count].program;
        sessions[count].argv[1] = (char*)"-s";
        sessions[count].argv[2] = (char*)"/usr/bin/startdde";
        sessions[count].argv[3] = NULL;
        count++;
    }

    /* 2. flakewm alone (compositor only, lightweight for window manager debugging) */
    if (is_executable_available("flakewm") && count < max_count) {
        strncpy(sessions[count].name, "flake-wm (compositor only)",
                sizeof(sessions[count].name) - 1);
        sessions[count].name[sizeof(sessions[count].name) - 1] = '\0';

        strncpy(sessions[count].description,
                "Run standalone nested flake-wm compositor without DDE",
                sizeof(sessions[count].description) - 1);
        sessions[count].description[sizeof(sessions[count].description) - 1] = '\0';

        strncpy(sessions[count].program, "flakewm", sizeof(sessions[count].program) - 1);
        sessions[count].program[sizeof(sessions[count].program) - 1] = '\0';

        sessions[count].argv[0] = sessions[count].program;
        sessions[count].argv[1] = NULL;
        count++;
    }

    /* 3. GXDE with wlcom (only shown if startgxde_wlcom exists) */
    if (is_executable_available("startgxde_wlcom") && count < max_count) {
        strncpy(sessions[count].name, "GXDE (gxde-wlcom)", sizeof(sessions[count].name) - 1);
        sessions[count].name[sizeof(sessions[count].name) - 1] = '\0';

        strncpy(sessions[count].description, "Run GXDE session with gxde-wlcom compositor",
                sizeof(sessions[count].description) - 1);
        sessions[count].description[sizeof(sessions[count].description) - 1] = '\0';

        strncpy(sessions[count].program, "startgxde_wlcom", sizeof(sessions[count].program) - 1);
        sessions[count].program[sizeof(sessions[count].program) - 1] = '\0';

        sessions[count].argv[0] = sessions[count].program;
        sessions[count].argv[1] = NULL;
        count++;
    }

    return count;
}

int prepare_isolated_launch_args(const wayland_session_entry_t* session,
                                 const char** out_program,
                                 char* out_argv[]) {
    if (session == NULL || out_program == NULL || out_argv == NULL) {
        return -1;
    }

    if (is_executable_available("dbus-run-session")) {
        write_log_with_tag(LOG_INFO, "SessionHandler", "Launcher",
                           "Enabling D-Bus session bus isolation via dbus-run-session.");
        *out_program = "dbus-run-session";
        out_argv[0] = (char*)"dbus-run-session";
        out_argv[1] = (char*)"--";
        out_argv[2] = (char*)session->program;

        int j = 3;
        for (int i = 1; session->argv[i] != NULL && j < (MAX_SESSION_ARGV - 1); i++, j++) {
            out_argv[j] = session->argv[i];
        }
        out_argv[j] = NULL;
    } else {
        write_log_with_tag(LOG_WARNING, "SessionHandler", "Launcher",
                           "dbus-run-session not found; launching without D-Bus isolation.");
        *out_program = session->program;
        int i = 0;
        for (; session->argv[i] != NULL && i < (MAX_SESSION_ARGV - 1); i++) {
            out_argv[i] = session->argv[i];
        }
        out_argv[i] = NULL;
    }

    return 0;
}

