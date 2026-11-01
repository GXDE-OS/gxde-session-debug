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

#ifndef SESSION_HANDLER_H
#define SESSION_HANDLER_H

#define MAX_SESSIONS 8
#define MAX_SESSION_ARGV 16

/**
 * @brief Representation of an available Wayland nested session option.
 */
typedef struct {
    char name[64];
    char description[128];
    char program[64];
    char* argv[MAX_SESSION_ARGV];
} wayland_session_entry_t;

/**
 * @brief Checks if a binary or script exists and is executable in the system PATH.
 *
 * @param executable (const char*) Binary name or absolute path.
 * @return int 1 if executable is found and executable, 0 otherwise.
 */
int is_executable_available(const char* executable);

/**
 * @brief Dynamically detects installed and supported Wayland sessions.
 *
 * @param sessions (wayland_session_entry_t[]) Array to populate detected sessions.
 * @param max_count (int) Capacity of the sessions array.
 * @return int Number of detected sessions.
 */
int detect_available_sessions(wayland_session_entry_t sessions[], int max_count);

/**
 * @brief Prepares command arguments with D-Bus session bus isolation wrapper.
 *
 * Wraps the given session entry with `dbus-run-session --` if available,
 * to prevent conflicts with the currently running host desktop session bus.
 *
 * @param session (const wayland_session_entry_t*) The session to launch.
 * @param out_program (const char**) Output pointer to program to execute.
 * @param out_argv (char*[]) Output argument vector buffer of at least MAX_SESSION_ARGV elements.
 * @return int 0 on success, -1 on invalid arguments.
 */
int prepare_isolated_launch_args(const wayland_session_entry_t* session,
                                 const char** out_program,
                                 char* out_argv[]);

#endif /* SESSION_HANDLER_H */

