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

#ifndef MENU_H
#define MENU_H

#include "session_handler.h"

/**
 * @brief A menu providing session selector.
 *
 * @param sessions (const wayland_session_entry_t[]) Array of sessions.
 * @param count    (int)                             Total count of sessions.
 * @return int Selected index, or -1 if invalid response.
 */
int prompt_user_session_choice(const wayland_session_entry_t sessions[], int count);

#endif /* MENU_H */
