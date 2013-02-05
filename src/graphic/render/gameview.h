/*
 * Copyright (C) 2010-2013 by the Widelands Development Team
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

#ifndef WIDELANDS_GAMEVIEW_H
#define WIDELANDS_GAMEVIEW_H

#include "graphic/rendertarget.h"

namespace Widelands {
	struct Player;
	struct Editor_Game_Base;
};

/**
 * This class renders the minimap.
 *
 * @todo Rename this class to reflect its new purpose;
 * check for software vs. OpenGL rendering related refactorings
 */
class GameView : public RenderTarget
{
public:
	GameView(RenderTarget & rt) :
		RenderTarget(rt) {}
	virtual ~GameView() {}

	/**
	 * Render the minimap. If player is not 0, it renders from that player's
	 * point of view.
	 */
	void renderminimap
		(Widelands::Editor_Game_Base const & egbase,
		 Widelands::Player           const * player,
		 Point                               viewpoint,
		 uint32_t                            flags);

protected:
	/// A helper function to draw the minimap. This is called from
	/// renderminimap().
	void draw_minimap
		(Widelands::Editor_Game_Base const &,
		 Widelands::Player           const *,
		 Rect                                rc,
		 Point                               viewpt,
		 Point                               framept,
		 uint32_t                            flags);
};

#endif //WIDELANDS_GAMEVIEW_H
