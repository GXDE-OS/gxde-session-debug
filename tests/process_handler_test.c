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

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "process_handler.h"
#include "log.h"

/**
 * @brief Check that @c fork_process() and @c fork_process_in_thread() work properly.
 *
 * 1. Checks that @c fork_process() runs a program with the given arguments.
 * 2. Checks that @c fork_process_in_thread() spawns a thread and monitors child.
 *
 * @return int @c 0 on success, @c 1 on failure.
 */
int main(void) {
    char* argv[] = {"sh", "-c", "exit 7", NULL};

    pid_t pid = fork_process("/bin/sh", argv);
    if (pid < 0) {
        write_log(LOG_ERROR, "fork_process() failed to fork.");
        return 1;
    }
    write_log(LOG_OK, "fork_process() executed successfully.");

    pthread_t tid = fork_process_in_thread("/bin/sh", argv);
    if (tid == 0) {
        write_log(LOG_ERROR, "fork_process_in_thread() failed to create thread.");
        return 1;
    }

    if (pthread_join(tid, NULL) != 0) {
        write_log(LOG_ERROR, "pthread_join() failed.");
        return 1;
    }

    write_log(LOG_OK, "fork_process_in_thread() ran and reaped in background thread.");
    return 0;
}

