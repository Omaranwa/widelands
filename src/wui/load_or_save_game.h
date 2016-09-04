/*
 * Copyright (C) 2002-2016 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef WL_WUI_LOAD_OR_SAVE_GAME_H
#define WL_WUI_LOAD_OR_SAVE_GAME_H

#include <memory>

#include "graphic/image.h"
#include "logic/game_settings.h"
#include "ui_basic/table.h"
#include "wui/gamedetails.h"

/// Common functions for loading or saving a game or replay.
class LoadOrSaveGame {
public:
	LoadOrSaveGame(UI::Panel* parent, Widelands::Game& g,
						GameSettingsProvider* gsp,
						int tablex, int tabley, int tablew, int tableh,
						bool is_replay = false);

	const SavegameData* selected_entry();
	bool has_selection();
	void fill_table();
	UI::Table<uintptr_t const>& table() {return table_;}

private:
	bool compare_date_descending(uint32_t, uint32_t);

	UI::Table<uintptr_t const> table_;
	bool is_replay_;
	std::vector<SavegameData> games_data_;

	Widelands::Game& game_;
	GameSettingsProvider* settings_;
};

#endif  // end of include guard: WL_WUI_LOAD_OR_SAVE_GAME_H