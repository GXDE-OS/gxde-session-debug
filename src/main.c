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
#include <pthread.h>

#include "log.h"
#include "session_handler.h"
#include "menu.h"
#include "process_handler.h"

/**
 * @brief Main entry of the program.
 *
 * @param argc (int) Argument count.
 * @param argv (char*[]) Argument vector.
 * @return int Exit code.
 */
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    write_log_with_tag(LOG_INFO, "Main", "Init", "Program initializing...");

    wayland_session_entry_t sessions[MAX_SESSIONS];
    memset(sessions, 0, sizeof(sessions));

    int session_count = detect_available_sessions(sessions, MAX_SESSIONS);
    write_log_with_tag(LOG_INFO, "Main", "SessionDetector",
                       "Detected %d available Wayland session(s).", session_count);

    int selected = prompt_user_session_choice(sessions, session_count);
    if (selected < 0) {
        write_log_with_tag(LOG_INFO, "Main", "Exit", "Session selection aborted by user.");
        return EXIT_SUCCESS;
    }

    write_log_with_tag(LOG_INFO, "Main", "Launcher", "Launching nested session: %s...",
                       sessions[selected].name);

    const char* launch_program = NULL;
    char* launch_argv[MAX_SESSION_ARGV];
    if (prepare_isolated_launch_args(&sessions[selected], &launch_program, launch_argv) != 0) {
        write_log_with_tag(LOG_ERROR, "Main", "Launcher",
                           "Failed to prepare isolated launch arguments.");
        return EXIT_FAILURE;
    }

    pthread_t thread = fork_process_in_thread(launch_program, launch_argv);
    if (thread == 0) {
        write_log_with_tag(LOG_ERROR, "Main", "Launcher",
                           "Failed to launch session worker thread.");
        return EXIT_FAILURE;
    }

    write_log_with_tag(LOG_OK, "Main", "Launcher",
                       "Session launched in background thread (tid=%lu). Waiting for session to terminate...",
                       (unsigned long)thread);

    /* Wait for child process execution thread to finish */
    if (pthread_join(thread, NULL) != 0) {
        write_log_with_tag(LOG_ERROR, "Main", "Wait",
                           "Failed to wait for session thread.");
        return EXIT_FAILURE;
    }

    write_log_with_tag(LOG_OK, "Main", "Exit", "Nested session terminated. Goodbye!");
    return EXIT_SUCCESS;
}

