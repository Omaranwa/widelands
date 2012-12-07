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

#ifndef SDL_SURFACE_TEXTURE_H
#define SDL_SURFACE_TEXTURE_H

#include "sdl_surface.h"

// TODO(sirver): maybe too much virtual inheritance around here
// TODO(sirver): eventuell kann screen von texture ableiten und nur funktionen überschreiben.
class SDLSurfaceTexture : public virtual SDLSurface {
public:
	SDLSurfaceTexture(SDL_Surface & surface) :
		SDLSurface(surface)
	{}
	virtual ~SDLSurfaceTexture();
};
#endif /* end of include guard: SDL_SURFACE_TEXTURE_H */

