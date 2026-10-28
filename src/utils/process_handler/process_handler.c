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

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process_handler.h"
#include "../log/log.h"

/**
 * @brief Program & argument config.
 */
typedef struct {
    char* program;
    char** argv;
} thread_fork_args_t;

/**
 * @brief Thread worker that calls @c fork_process.
 *
 * @see fork_process.
 * @param arg A @c thread_fork_args_t point that defines program and arguments.
 * @return void* NULL.
 */
static void* thread_fork_worker(void* arg) {
    thread_fork_args_t* task = (thread_fork_args_t*)arg;
    if (task == NULL) {
        return NULL;
    }

    fork_process(task->program, task->argv);

    if (task->argv != NULL) {
        for (size_t i = 0; task->argv[i] != NULL; ++i) {
            free(task->argv[i]);
        }
        free(task->argv);
    }

    if (task->program != NULL) {
        free(task->program);
    }

    free(task);

    return NULL;
}

/**
 * @brief Fork a process & run a program specified in the child process.
 * 
 * @param program (const char*) Path of the desired program, or the program
 *                              name if the desired program exists in PATH.
 * @param argv (char* []) A vector for program argument. It needs to be
 *                        NULL-ended. If you do not wish to have any
 *                        arguments then passing NULL is fine here.
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
        if (argv == NULL) {
            write_log_with_tag(LOG_ERROR, "ProcessHandler",
                               "Failed to allocate memory",
                               "%s", strerror(errno));
            return -1;
        }
        argv[0] = (char*)program;
        argv[1] = NULL;
    }

    pid_t pid = fork();
    if (pid < 0) {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to fork process",
                           "%s", strerror(errno));
        if (!flag_arg_arr_exist && argv != NULL) {
            free(argv);
        }
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
        if (child_pid < 0) {
            write_log_with_tag(LOG_ERROR, "ProcessHandler",
                               "Child process monitor",
                               "wait() failed: %s", strerror(errno));
        } else if (WIFEXITED(status)) {
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

/**
 * @brief Spawn a dedicated background thread to fork and monitor a program.
 *
 * @param program (const char*) Path of the desired program, or the program
 *                              name if the desired program exists in PATH.
 * @param argv (char* [])  A vector for program argument. It needs to be
 *                         NULL-ended. Passing NULL here defaults to
 *                         executing the program with argv[0] = program.
 * @return pthread_t The created thread identifier, or @c 0 on failure.
 */
pthread_t fork_process_in_thread(const char* program, char* argv[]) {
    if (program == NULL || *program == '\0') {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to start thread",
                           "The program name is invalid.");
        return 0;
    }

    thread_fork_args_t* task = malloc(sizeof(thread_fork_args_t));
    if (task == NULL) {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to allocate memory",
                           "%s", strerror(errno));
        return 0;
    }

    size_t prog_len = strlen(program);
    task->program = malloc(prog_len + 1);
    if (task->program == NULL) {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to allocate memory",
                           "%s", strerror(errno));
        free(task);
        return 0;
    }
    memcpy(task->program, program, prog_len + 1);

    if (argv != NULL) {
        size_t argc = 0;
        while (argv[argc] != NULL) {
            argc++;
        }

        task->argv = malloc((argc + 1) * sizeof(char*));
        if (task->argv == NULL) {
            write_log_with_tag(LOG_ERROR, "ProcessHandler",
                               "Failed to allocate memory",
                               "%s", strerror(errno));
            free(task->program);
            free(task);
            return 0;
        }

        for (size_t i = 0; i < argc; ++i) {
            size_t arg_len = strlen(argv[i]);
            task->argv[i] = malloc(arg_len + 1);
            if (task->argv[i] == NULL) {
                write_log_with_tag(LOG_ERROR, "ProcessHandler",
                                   "Failed to allocate memory",
                                   "%s", strerror(errno));
                for (size_t j = 0; j < i; ++j) {
                    free(task->argv[j]);
                }
                free(task->argv);
                free(task->program);
                free(task);
                return 0;
            }
            memcpy(task->argv[i], argv[i], arg_len + 1);
        }
        task->argv[argc] = NULL;
    } else {
        task->argv = NULL;
    }

    pthread_t thread_id;
    int err = pthread_create(&thread_id, NULL, thread_fork_worker, task);
    if (err != 0) {
        write_log_with_tag(LOG_ERROR, "ProcessHandler",
                           "Failed to create thread",
                           "%s", strerror(err));
        if (task->argv != NULL) {
            for (size_t i = 0; task->argv[i] != NULL; ++i) {
                free(task->argv[i]);
            }
            free(task->argv);
        }
        free(task->program);
        free(task);
        return 0;
    }

    return thread_id;
}

