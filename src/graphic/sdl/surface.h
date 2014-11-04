/*
 * Copyright (C) 2010-2011 by the Widelands Development Team
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

#ifndef WL_GRAPHIC_SDL_SURFACE_H
#define WL_GRAPHIC_SDL_SURFACE_H

#include <SDL_render.h>

#include "base/rect.h"
#include "graphic/color.h"
#include "graphic/surface.h"

/**
* This implements SDL rendering. Do not use this class directly. The right
* way is to use the base struct Surface wherever possible. Everything which
* needs to know about the underlying renderer should go to the graphics
* subdirectory.
*/
class SDLSurface : public Surface {
public:
	// The surface set by SetVideoMode must not be freed according to the SDL
	// docs, so we need 'free_surface_on_delete'.
	SDLSurface(SDL_Surface* surface, bool free_surface_on_delete = true);
	virtual ~SDLSurface();

	// Implements Image
	uint16_t width() const override {return m_w;}
	uint16_t height() const override {return m_h;}

	// Implements Surface
	void blit(const Point&, const Surface*, const Rect& srcrc, Composite cm) override;
	void fill_rect(const Rect&, RGBAColor) override;

	// Implements Surface
	void draw_rect(const Rect&, RGBColor) override;
	virtual void draw_line
		(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const RGBColor&, uint8_t width) override;
	void brighten_rect(const Rect&, int32_t factor) override;

	const SDL_PixelFormat & format() const override;
	void lock(LockMode) override;
	void unlock(UnlockMode) override;
	uint32_t get_pixel(uint16_t x, uint16_t y) override;
	void set_pixel(uint16_t x, uint16_t y, uint32_t clr) override;
	uint16_t get_pitch() const override {return m_surface->pitch;}
	uint8_t * get_pixels() const override;

	SDL_Surface * get_sdl_surface() const {return m_surface;}

	void set_subwin(const Rect& r);
	void unset_subwin();

protected:
	SDL_Surface * m_surface;
	int32_t m_offsx;
	int32_t m_offsy;
	uint16_t m_w, m_h;
	bool m_free_surface_on_delete;
};

#endif  // end of include guard: WL_GRAPHIC_SDL_SURFACE_H