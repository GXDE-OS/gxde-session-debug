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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "path_handler.h"
#include "../log/log.h"
#include "../global_vars/global_vars.h"

/**
 * @brief Duplicate a string with memory allocation.
 * 
 * @param src  (const char*) The source string to duplicate.
 * @return (char*) A pointer to duplicated string on success.
 */
static char* duplicate_string(const char* src) {
    size_t len;
    char* dst;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src) + 1;
    if (len > MAX_PATH_LENGTH) {
        return NULL;
    }

    dst = malloc(len);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    return dst;
}


/**
 * @brief Get the system data directory path.
 * 
 * @details The function first gets @c XDG_DATA_HOME variable, and returns if
 *          it is set. If not, it will try to get @c HOME variable and return
 *          the path as @c HOME/.local/share if avaliable. If failed, then it
 *          will return @c NULL in the path part.
 * @note The function is actually returning a struct, not just a string.
 * @see The struct @ref path_query_response_t defined in @ref path_handler.h
 * @return (path_query_response_t) A struct containing status code and system
 *                                 data directory path (if found).
 */

path_query_response_t get_system_data_dir() {
    path_query_response_t response;
    const char* xdg_data = getenv("XDG_DATA_HOME");
    if (xdg_data != NULL) {
        response.path = duplicate_string(xdg_data);
        response.status_code = (response.path != NULL) ? 0 : -2;
        return response;
    } else {
        const char* home_dir = getenv("HOME");
        if (home_dir != NULL) {
            char temp_path[MAX_PATH_LENGTH];
            int written = snprintf(temp_path, sizeof(temp_path),
                "%s/.local/share", home_dir);

            if (written < 0 || written >= (int) sizeof(temp_path)) {
                // While exceeding the max buffer size
                write_log_with_tag(LOG_ERROR, "Path Handler",
                    "Error getting path",
                    "The generated system data directory path exceeds the "
                    "MAX_PATH_LEN");
                response.status_code = -2;
                response.path = NULL;
                return response;
            }

            response.path = duplicate_string(temp_path);
            if (response.path == NULL) {
                response.status_code = -2;
                return response;
            }

            response.status_code = -1;
            return response;
        } else {
            response.status_code = -2;
            response.path = NULL;
            return response;
        }
    }
}


/**
 * @brief Get the application data dir object
 * 
 * @details The function first gets the system data directory path, then
 *          appends the application name (to lower) to it and then checks the
 *          existence of the path. If the path does not exist, it will try to
 *          create it.
 * @note If the path does NOT exist, but the system data directory is,
 *       avaliable, then the function will try to create it.
 * @return (path_query_response_t) A struct containing status code and 
 *                                 application data directory path (if found).
 */

path_query_response_t get_application_data_dir() {
    path_query_response_t response;

    path_query_response_t system_data_dir = get_system_data_dir();
    if (system_data_dir.status_code == -2) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The path you requested depends on the system data directory, "
            "but we failed to get that path.");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    struct stat data_dir_st;
    if (!(stat(system_data_dir.path, &data_dir_st) == 0 &&
            S_ISDIR(data_dir_st.st_mode))) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The path you requested depends on the system data directory, "
            "but seems that it does NOT exist on your system.");
        free(system_data_dir.path);
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    char temp[MAX_PATH_LENGTH];
    int written = snprintf(temp, sizeof(temp), "%s/%s",
        system_data_dir.path, APPLICATION_NAME_LOWER);
    free(system_data_dir.path);

    if (written < 0 || written >= (int) sizeof(temp)) {
        // While exceeding the max buffer size
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The generated application data directory path exceeds the "
            "MAX_PATH_LEN");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    struct stat app_data_dir_st;
    if (!(stat(temp, &app_data_dir_st) == 0 &&
            S_ISDIR(app_data_dir_st.st_mode))) {
        if (mkdir(temp, 0755) != 0) {
            write_log_with_tag(LOG_ERROR, "Path Handler",
                "Error getting path",
                "Failed to create the application data path.");
            response.status_code = -2;
            response.path = NULL;
            return response;
        }
    }

    response.path = duplicate_string(temp);
    if (response.path == NULL) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
                "Error getting path",
                "Failed to copy the application data path to response.");
        response.status_code = -2;
        return response;
    }

    response.status_code = 0;
    return response;
}


/**
 * @brief Get the system config directory path.
 * 
 * @details The function first gets @c XDG_CONFIG_HOME variable, and returns if
 *          it is set. If not, it will try to get @c HOME variable and return
 *          the path as @c HOME/.config if avaliable. If failed, then it
 *          will return @c NULL in the path part.
 * @note The function is actually returning a struct, not just a string.
 * @see The struct @ref path_query_response_t defined in @ref path_handler.h
 * @return (path_query_response_t) A struct containing status code and system
 *                                 config directory path (if found).
 */

path_query_response_t get_system_config_dir() {
    path_query_response_t response;
    const char* xdg_config = getenv("XDG_CONFIG_HOME");
    if (xdg_config != NULL) {
        response.path = duplicate_string(xdg_config);
        response.status_code = (response.path != NULL) ? 0 : -2;
        return response;
    } else {
        const char* home_dir = getenv("HOME");
        if (home_dir != NULL) {
            char temp_path[MAX_PATH_LENGTH];
            int written = snprintf(temp_path, sizeof(temp_path),
                "%s/.config", home_dir);

            if (written < 0 || written >= (int) sizeof(temp_path)) {
                // While exceeding the max buffer size
                write_log_with_tag(LOG_ERROR, "Path Handler",
                    "Error getting path",
                    "The generated system config directory path exceeds the "
                    "MAX_PATH_LEN");
                response.status_code = -2;
                response.path = NULL;
                return response;
            }

            response.path = duplicate_string(temp_path);
            if (response.path == NULL) {
                response.status_code = -2;
                return response;
            }

            response.status_code = -1;
            return response;
        } else {
            response.status_code = -2;
            response.path = NULL;
            return response;
        }
    }
}


/**
 * @brief Get the application config directory path.
 * 
 * @details The function first gets the system config directory path, then
 *          appends the application name (to lower) to it and then checks the
 *          existence of the path. If the path does not exist, it will try to
 *          create it.
 * @note If the path does NOT exist, but the system config directory is,
 *       avaliable, then the function will try to create it.
 * @return (path_query_response_t) A struct containing status code and 
 *                                 application config directory path (if found)
 */

path_query_response_t get_application_config_dir() {
    path_query_response_t response;

    path_query_response_t system_config_dir = get_system_config_dir();
    if (system_config_dir.status_code == -2) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The path you requested depends on the system config directory, "
            "but we failed to get that path.");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    struct stat config_dir_st;
    if (!(stat(system_config_dir.path, &config_dir_st) == 0 &&
            S_ISDIR(config_dir_st.st_mode))) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The path you requested depends on the system config directory, "
            "but seems that it does NOT exist on your system.");
        free(system_config_dir.path);
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    char temp[MAX_PATH_LENGTH];
    int written = snprintf(temp, sizeof(temp), "%s/%s",
        system_config_dir.path, APPLICATION_NAME_LOWER);
    free(system_config_dir.path);

    if (written < 0 || written >= (int) sizeof(temp)) {
        // While exceeding the max buffer size
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The generated application config directory path exceeds the "
            "MAX_PATH_LEN");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    struct stat app_config_dir_st;
    if (!(stat(temp, &app_config_dir_st) == 0 &&
            S_ISDIR(app_config_dir_st.st_mode))) {
        if (mkdir(temp, 0755) != 0) {
            write_log_with_tag(LOG_ERROR, "Path Handler",
                "Error getting path",
                "Failed to create the application config path.");
            response.status_code = -2;
            response.path = NULL;
            return response;
        }
    }

    response.path = duplicate_string(temp);
    if (response.path == NULL) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
                "Error getting path",
                "Failed to copy the application config path to response.");
        response.status_code = -2;
        return response;
    }

    response.status_code = 0;
    return response;
}


/**
 * @brief Get the package installation dir object
 * 
 * @details The function first gets the data directory path, if succeed,
 *          it appends @c /packages to it and then checks the existance
 *          of the path generated. If the path does not exist, it will try
 *          to create it.
 * @note If app data directory does NOT exist, then the function will return
 *       failure.
 * @return (path_query_response_t) A struct containing status code and 
 *                                 package installation path (if found).
 */

path_query_response_t get_package_install_dir() {
    path_query_response_t response;

    path_query_response_t app_data_dir = get_application_data_dir();
    if (app_data_dir.status_code == -2) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The path you requested depends on the app data directory, "
            "but we failed to get that path.");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    char package_install_dir[MAX_PATH_LENGTH];
    int string_cat_result = snprintf(package_install_dir,
        sizeof(package_install_dir), "%s/packages", app_data_dir.path);

    if (string_cat_result < 0 || (size_t)string_cat_result >=
            sizeof(package_install_dir)) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error concatenating string",
            "The new package installation path exceeds MAX_PATH_LENGTH.");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    struct stat package_installation_dir_st;
    if (!(stat(package_install_dir, &package_installation_dir_st) == 0 &&
            S_ISDIR(package_installation_dir_st.st_mode))) {
        if (mkdir(package_install_dir, 0755) != 0) {
            write_log_with_tag(LOG_ERROR, "Path Handler",
                "Error getting path",
                "Failed to create the package installation path.");
            response.status_code = -2;
            response.path = NULL;
            return response;
        }
    }

    response.path = duplicate_string(package_install_dir);
    if (response.path == NULL) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "Failed to copy the package installation path to response.");
        response.status_code = -2;
        return response;
    }

    response.status_code = 0;
    return response;
}

/**
 * @brief Get the package info dir object
 * 
 * @details The function first gets the data directory path, if succeed,
 *          it appends @c /repo to it and then checks the existance
 *          of the path generated. If the path does not exist, it will try
 *          to create it.
 * @note If app data directory does NOT exist, then the function will return
 *       failure.
 * @return (path_query_response_t) A struct containing status code and 
 *                                 package info path (if found).
 */

path_query_response_t get_package_info_dir() {
    path_query_response_t response;

    path_query_response_t app_data_dir = get_application_data_dir();
    if (app_data_dir.status_code == -2) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "The path you requested depends on the app data directory, "
            "but we failed to get that path.");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    char package_info_dir[MAX_PATH_LENGTH];
    int string_cat_result = snprintf(package_info_dir,
        sizeof(package_info_dir), "%s/repo", app_data_dir.path);

    if (string_cat_result < 0 || (size_t)string_cat_result >=
            sizeof(package_info_dir)) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error concatenating string",
            "The new package info path exceeds MAX_PATH_LENGTH.");
        response.status_code = -2;
        response.path = NULL;
        return response;
    }

    struct stat package_info_dir_st;
    if (!(stat(package_info_dir, &package_info_dir_st) == 0 &&
            S_ISDIR(package_info_dir_st.st_mode))) {
        if (mkdir(package_info_dir, 0755) != 0) {
            write_log_with_tag(LOG_ERROR, "Path Handler",
                "Error getting path",
                "Failed to create the package info path.");
            response.status_code = -2;
            response.path = NULL;
            return response;
        }
    }

    response.path = duplicate_string(package_info_dir);
    if (response.path == NULL) {
        write_log_with_tag(LOG_ERROR, "Path Handler",
            "Error getting path",
            "Failed to copy the package info path to response.");
        response.status_code = -2;
        return response;
    }

    response.status_code = 0;
    return response;
}
