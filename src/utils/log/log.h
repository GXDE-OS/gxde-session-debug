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

#ifndef LOG_H
#define LOG_H

/* ---------- Public enums ---------- */

/**
 * @brief Log level used to control output style and destination stream.
 */
typedef enum {
    LOG_INFO,
    LOG_OK,
    LOG_WARNING,
    LOG_ERROR
} log_level_t;

/* ---------- Public functions ---------- */

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

void write_log(log_level_t level, const char* content, ...);

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
                          const char* content, ...);

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
                        const char* content, ...);

#endif /* LOG_H */
