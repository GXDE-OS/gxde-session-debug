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

#ifndef PROCESS_HANDLER_H
#define PROCESS_HANDLER_H

#include <sys/types.h>

/* ---------- Public functions ---------- */

/**
 * @brief Fork a process & run a program specified in the child process.
 * 
 * @param program (const char*) Path of the desired program, or the program
 *                              name if the desired program exists in PATH.
 * @param argv (char* [])  A vector for program argument. It needs to be
 *                              NULL-ended. If you do not wish to have any
 *                              arguments then passing NULL is fine here.
 * @note This function will block the thread while waiting for child process,
 *       be aware that it may block your main process is you put it in the
 *       main loop.
 * @return pid_t The child pid on success, or @c -1 on failure.
 */
pid_t fork_process(const char* program, char* argv[]);

#endif /* PROCESS_HANDLER_H */
