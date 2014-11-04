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

#ifndef WL_GRAPHIC_SDL_GAME_RENDERER_H
#define WL_GRAPHIC_SDL_GAME_RENDERER_H

#include "graphic/game_renderer.h"

struct Texture;
struct Vertex;

/**
 * Software-rendering implementation of @ref GameRenderer.
 */
class SdlGameRenderer : public GameRenderer {
protected:
	void draw() override;

private:
	void draw_terrain();

	/**
	 * Helper function to draw two terrain triangles. This is called from the
	 * rendermap() functions.
	 */
	void draw_field
		(RenderTarget & dst,
		 const Vertex  &  f_vert,
		 const Vertex  &  r_vert,
		 const Vertex  & bl_vert,
		 const Vertex  & br_vert,
		 uint8_t         roads,
		 const Texture & tr_d_texture,
		 const Texture &  l_r_texture,
		 const Texture &  f_d_texture,
		 const Texture &  f_r_texture);
};

#endif  // end of include guard: WL_GRAPHIC_SDL_GAME_RENDERER_H