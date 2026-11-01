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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "session_handler.h"
#include "log.h"

static void test_is_executable_available() {
    /* /bin/sh or sh should always be available */
    assert(is_executable_available("sh") == 1);
    assert(is_executable_available("/bin/sh") == 1);
    /* non-existent binary */
    assert(is_executable_available("non_existent_binary_xyz_1234") == 0);
    assert(is_executable_available("") == 0);
    assert(is_executable_available(NULL) == 0);
    write_log(LOG_OK, "test_is_executable_available passed.");
}

static void test_detect_available_sessions() {
    wayland_session_entry_t sessions[MAX_SESSIONS];
    memset(sessions, 0, sizeof(sessions));

    int count = detect_available_sessions(sessions, MAX_SESSIONS);
    assert(count >= 0 && count <= MAX_SESSIONS);

    /* Verify each detected session has non-empty name and program */
    for (int i = 0; i < count; i++) {
        assert(strlen(sessions[i].name) > 0);
        assert(strlen(sessions[i].program) > 0);
        assert(sessions[i].argv[0] != NULL);
    }
    write_log(LOG_OK, "test_detect_available_sessions passed.");
}

static void test_prepare_isolated_launch_args() {
    wayland_session_entry_t session;
    memset(&session, 0, sizeof(session));
    strncpy(session.program, "dummy_prog", sizeof(session.program) - 1);
    session.argv[0] = session.program;
    session.argv[1] = (char*)"-a";
    session.argv[2] = (char*)"arg1";
    session.argv[3] = NULL;

    const char* program = NULL;
    char* argv[MAX_SESSION_ARGV];
    int res = prepare_isolated_launch_args(&session, &program, argv);
    assert(res == 0);
    assert(program != NULL);
    assert(argv[0] != NULL);

    /* NULL checks */
    assert(prepare_isolated_launch_args(NULL, &program, argv) == -1);
    assert(prepare_isolated_launch_args(&session, NULL, argv) == -1);
    assert(prepare_isolated_launch_args(&session, &program, NULL) == -1);

    write_log(LOG_OK, "test_prepare_isolated_launch_args passed.");
}

int main(void) {
    write_log(LOG_INFO, "Running session_handler_test...");
    test_is_executable_available();
    test_detect_available_sessions();
    test_prepare_isolated_launch_args();
    write_log(LOG_OK, "All session_handler_test cases passed!");
    return 0;
}

