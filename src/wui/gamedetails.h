/*
 * Copyright (C) 2016 by the Widelands Development Team
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

#ifndef WL_WUI_GAMEDETAILS_H
#define WL_WUI_GAMEDETAILS_H

#include <memory>

#include "graphic/image.h"
#include "logic/game_controller.h"
#include "ui_basic/box.h"
#include "ui_basic/icon.h"
#include "ui_basic/multilinetextarea.h"

/**
 * Data about a savegame/replay that we're interested in.
 */
struct SavegameData {
	std::string filename;
	std::string mapname;
	std::string wincondition;
	std::string minimap_path;
	std::string savedatestring;
	std::string errormessage;

	uint32_t gametime;
	uint32_t nrplayers;
	std::string version;
	time_t savetimestamp;
	GameController::GameType gametype;

	SavegameData()
	   : gametime(0),
	     nrplayers(0),
	     savetimestamp(0),
	     gametype(GameController::GameType::SINGLEPLAYER) {
	}
};

/**
 * Show a Panel with information about a map.
 */
class GameDetails : public UI::Panel {
public:
	enum class Style { kFsMenu, kWui };

	GameDetails(Panel* parent, int32_t x, int32_t y, int32_t max_w, int32_t max_h, Style style);

	void clear();
	void update(const SavegameData& gamedata);

private:
	const Style style_;
	const int padding_;

	UI::Box main_box_;
	UI::MultilineTextarea name_label_;
	UI::MultilineTextarea descr_;
	UI::Icon minimap_icon_;
	std::unique_ptr<const Image> minimap_image_;
};

#endif  // end of include guard: WL_WUI_GAMEDETAILS_H