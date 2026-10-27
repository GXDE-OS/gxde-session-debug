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

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process_handler.h"
#include "log.h"

/**
 * @brief Check that @c fork_process() runs a program with the given arguments.
 *
 * The child runs @c /bin/sh with the arguments @c -c "exit 7". The parent waits
 * for it and verifies that the PID returned by @c fork_process() is exactly the
 * PID that was reaped, and that the exit status is @c 7, which proves that the
 * argument vector reached the new program.
 *
 * @return int @c 0 on success, @c 1 on failure.
 */

int main(void) {
    char* argv[] = {"sh", "-c", "exit 7", NULL};

    pid_t pid = fork_process("/bin/sh", argv);
    if (pid < 0) {
        return 1;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) != pid) {
        write_log(LOG_ERROR, "waitpid() did not reap the forked child.");
        return 1;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 7) {
        write_log(LOG_ERROR, "Child did not exit normally with status 7.");
        return 1;
    }

    write_log(LOG_OK, "fork_process() ran /bin/sh with the given arguments.");
    return 0;
}
