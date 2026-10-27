/*
 * Copyright (C) CharOfString <root@charofstring.cc>
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

#ifndef PATH_HANDLER_H
#define PATH_HANDLER_H

#include <string.h>

/**
 * @brief Struct to hold the response of path query functions.
 * @details The struct contains a status code to indicate success or failure,
 *          with a @c status_code of @c 0 indicating succeed and @c -1
 *          indicating fallback/downgraded performance, and @c -2 indicating
 *          failure. The @c path member holds the resulting path if found.
 * 
 */

typedef struct {
    int status_code;
    char* path;
} path_query_response_t;

/* ---------- Public getters ---------- */
path_query_response_t get_system_data_dir();
path_query_response_t get_system_config_dir();
path_query_response_t get_application_data_dir();
path_query_response_t get_application_config_dir();
path_query_response_t get_package_install_dir();
path_query_response_t get_package_info_dir();

#endif  /* PATH_HANDLER_H */
