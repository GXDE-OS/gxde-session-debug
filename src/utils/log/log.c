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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "log.h"
#include "ansi_escape_code_def.h"

/**
 * @brief Print the colored log level to stdout/stderr.
 * 
 * This is an internal function. It prints the log header (e.g. @c [INFO]) to
 * @c stdout (or @c stderr, if the log level is @c LOG_ERROR.) These headers
 * are colored using ANSI escape codes
 * 
 * @see src/utils/log/ansi_escape_code_def.h for the color definitions.
 *
 * @param level (log_level_t) The log level.
 * @return void
 */

void print_level_prefix(log_level_t level) {
    switch (level) {
        case LOG_INFO:
            fprintf(stdout, "%s", BOLD_BLUE_FORMAT);
            fprintf(stdout, "[INFO]");
            fprintf(stdout, "%s ", RESET_FORMAT);
            break;
        case LOG_OK:
            fprintf(stdout, "%s", BOLD_GREEN_FORMAT);
            fprintf(stdout, "[ OK ]");
            fprintf(stdout, "%s ", RESET_FORMAT);
            break;
        case LOG_WARNING:
            fprintf(stdout, "%s", BOLD_YELLOW_FORMAT);
            fprintf(stdout, "[WARNING]");
            fprintf(stdout, "%s ", RESET_FORMAT);
            break;
        case LOG_ERROR:
            fprintf(stderr, "%s", BOLD_RED_FORMAT);
            fprintf(stderr, "[ERROR]");
            fprintf(stderr, "%s ", RESET_FORMAT);
            break;
        default:
            fprintf(stdout, "%s", BOLD_CYAN_FORMAT);
            fprintf(stdout, "[DEBUG] ");
            fprintf(stdout, "%s ", RESET_FORMAT);
            break;
    }
}

/**
 * @brief Print the log content to stdout/stderr.
 * 
 * This function outputs the log message with the approate header. e.g.
 * [INFO] Hello.
 * 
 * If the log level is @c LOG_ERROR, the message will be printed to @c stderr.
 * Otherwise the log is printed through @c stdout.
 * 
 * @param level (log_level_t) The log level.
 * @param content (const char*) The log message.
 * @param ... (variadic) Optional format arguments.
 * @return void
 */

void write_log(log_level_t level, const char* content, ...) {
    print_level_prefix(level);
    va_list args;
    va_start(args, content);
    vfprintf(level == LOG_ERROR ? stderr : stdout, content, args);
    fprintf(level == LOG_ERROR ? stderr : stdout, "\n");
    va_end(args);
}

/**
 * @brief Print the log title and content to stdout/stderr.
 * 
 * This function outputs the log title and message with the approate header.
 * e.g. [INFO] Greetings: Hello.
 * 
 * In the example above, "Greetings" is the title and "Hello." is the message
 * body.
 * 
 * If the log level is @c LOG_ERROR, the message will be printed to @c stderr.
 * Otherwise the log is printed through @c stdout.
 * 
 * @param level (log_level_t) The log level.
 * @param title (const char*) The log title.
 * @param content (const char*) The log message.
 * @param ... (variadic) Optional format arguments.
 * @return void
 */

void write_log_with_title(log_level_t level, const char* title,
                          const char* content, ...) {
    print_level_prefix(level);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", BOLD_FORMAT);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", title);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", RESET_FORMAT);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", ": ");

    va_list args;
    va_start(args, content);
    vfprintf(level == LOG_ERROR ? stderr : stdout, content, args);
    fprintf(level == LOG_ERROR ? stderr : stdout, "\n");
    va_end(args);
}

/**
 * @brief Print the log tag, title and content to stdout/stderr.
 * 
 * This function outputs the log tag, title and message with the approate
 * header. e.g. [INFO] Main - Greetings: Hello.
 * 
 * In the example above, "Main" is the tag, "Greetings" is the title and
 * "Hello." is the message body.
 * 
 * If the log level is @c LOG_ERROR, the message will be printed to @c stderr.
 * Otherwise the log is printed through @c stdout.
 * 
 * @param level (log_level_t) The log level.
 * @param tag (const char*) The log tag.
 * @param title (const char*) The log title.
 * @param content (const char*) The log message.
 * @param ... (variadic) Optional format arguments.
 * @return void
 */

void write_log_with_tag(log_level_t level, const char* tag, const char* title,
                        const char* content, ...) {
    print_level_prefix(level);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", BOLD_UNDERLINE_FORMAT);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", tag);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", " - ");
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", title);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", RESET_FORMAT);
    fprintf(level == LOG_ERROR ? stderr : stdout, "%s", ": ");

    va_list args;
    va_start(args, content);
    vfprintf(level == LOG_ERROR ? stderr : stdout, content, args);
    fprintf(level == LOG_ERROR ? stderr : stdout, "\n");
    va_end(args);
}
