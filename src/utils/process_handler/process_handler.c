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

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "process_handler.h"
#include "../log/log.h"

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
pid_t fork_process(const char* program, char* argv[]) {
    int flag_arg_arr_exist = 1;
    if (program == NULL) {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to fork process",
                           "The program cannot be NULL.");
        return -1;
    }

    if (*program == '\0') {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to fork process",
                           "The program cannot be empty.");
        return -1;
    }

    if (argv == NULL) {
        write_log_with_tag(LOG_INFO, "ProcessHandler",
                           "Argument handler",
                           "Empty argument array detected.");
        flag_arg_arr_exist = 0;
        argv = malloc(2 * sizeof(char*));
        argv[0] = program;
        argv[1] = NULL;
    }

    pid_t pid = fork();
    if (pid < 0) {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to fork process",
                           "%s", stderr(errno));
        return pid;
    } else if (pid == 0) {
        write_log_with_tag(LOG_INFO, "ProcessHandler",
                           "Starting child process",
                           "Attempting to start %s...", program);
        execvp(program, argv);

        /* As always, shouldn't get to this line if start failure. */
        _exit(127);
    } else {
        int status;
        pid_t child_pid = wait(&status);
        if (WIFEXITED(status)) {
            write_log_with_tag(LOG_INFO, "ProcessHandler",
                           "Child process monitor",
                           "Program %s ends normally.", program);
        } else if (WIFSIGNALED(status)) {
            write_log_with_tag(LOG_WARNING, "ProcessHandler",
                           "Child process monitor",
                           "Program %s exits abnormally.", program);
        }
    }

    if (!flag_arg_arr_exist && argv != NULL) {
        free(argv);
    }
    return pid;
}
