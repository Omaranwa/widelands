/*
 * Copyright (C) 2006-2012 by the Widelands Development Team
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <SDL_ttf.h>
#include <boost/format.hpp>

#include "graphic/image_cache.h"
#include "md5.h"
#include "rt_errors.h"
#include "sdl_helper.h"

#include "sdl_ttf_font_impl.h"

using namespace std;
using namespace boost;

static const int SHADOW_OFFSET = 1;
static const SDL_Color SHADOW_CLR = {0, 0, 0, SDL_ALPHA_OPAQUE};

namespace RT {

SDLTTF_Font::SDLTTF_Font(TTF_Font * font, const string& face, int ptsize) :
	font_(font), style_(TTF_STYLE_NORMAL), font_name_(face), ptsize_(ptsize) {
}

SDLTTF_Font::~SDLTTF_Font() {
	TTF_CloseFont(font_);
	font_ = 0;
}

void SDLTTF_Font::dimensions(const string& txt, int style, uint32_t * gw, uint32_t * gh) {
	m_set_style(style);

	int w, h;
	TTF_SizeUTF8(font_, txt.c_str(), &w, &h);

	if (style & SHADOW) {
		w += SHADOW_OFFSET; h += SHADOW_OFFSET;
	}
	*gw = w; *gh = h;
}

const IPicture& SDLTTF_Font::render(IGraphic & gr, const string& txt, const RGBColor& clr, int style) {
	SimpleMD5Checksum checksum;
	checksum.Data(font_name_.c_str(), font_name_.size());
	checksum.Data(&ptsize_, sizeof(ptsize_));
	checksum.Data(txt.c_str(), txt.size());
	checksum.Data(&clr.r, 1);
	checksum.Data(&clr.g, 1);
	checksum.Data(&clr.b, 1);
	checksum.Data(&style, sizeof(style));
	checksum.FinishChecksum();
	const string cs = checksum.GetChecksum().str();

	const IPicture* rv = gr.imgcache().get(PicMod_Text, cs);
	if (rv) return *rv;

	m_set_style(style);

	SDL_Surface * text_surface = 0;

	SDL_Color sdlclr = {clr.r, clr.g, clr.b, 0};
	if (style & SHADOW) {
		SDL_Surface * tsurf = TTF_RenderUTF8_Blended(font_, txt.c_str(), sdlclr);
		SDL_Surface * shadow = TTF_RenderUTF8_Blended(font_, txt.c_str(), SHADOW_CLR);
		text_surface = empty_sdl_surface(shadow->w + SHADOW_OFFSET, shadow->h + SHADOW_OFFSET, true);

		if (text_surface->format->BitsPerPixel != 32)
			throw RenderError("SDL_TTF did not return a 32 bit surface for shadow text. Giving up!");

		SDL_Rect dstrct1 = {0, 0, 0, 0};
		SDL_SetAlpha(shadow, 0, SDL_ALPHA_OPAQUE);
		SDL_BlitSurface(shadow, 0, text_surface, &dstrct1);

		Uint32* spix = static_cast<Uint32*>(tsurf->pixels);
		Uint32* dpix = static_cast<Uint32*>(text_surface->pixels);

		// Alpha Blend the Text onto the Shadow. This is really slow, but it is
		// the only compatible way to do it using SDL 1.2. SDL 2.0 offers more
		// functionality but is not yet released.
		Uint8 sr, sg, sb, sa, dr, dg, db, da, outa, outr = 0, outg = 0, outb = 0;
		for (uint32_t y = 0; y < tsurf->h; ++y) {
			for (uint32_t x = 0; x < tsurf->w; ++x) {
				size_t sidx = (y * tsurf->pitch + 4 * x) / 4;
				size_t didx = ((y + SHADOW_OFFSET) * text_surface->pitch + (x + SHADOW_OFFSET) * 4) / 4;

				SDL_GetRGBA(spix[sidx], tsurf->format, &sr, &sg, &sb, &sa);
				SDL_GetRGBA(dpix[didx], text_surface->format, &dr, &dg, &db, &da);

				outa = (255 * sa + da * (255 - sa)) / 255;
				if (outa) {
					outr = (255 * sa * sr + da * dr * (255 - sa)) / outa / 255;
					outg = (255 * sa * sg + da * dg * (255 - sa)) / outa / 255;
					outb = (255 * sa * sb + da * db * (255 - sa)) / outa / 255;
				}
				dpix[didx] = SDL_MapRGBA(text_surface->format, outr, outg, outb, outa);
			}
		}
		SDL_FreeSurface(tsurf);
		SDL_FreeSurface(shadow);
	} else
		text_surface = TTF_RenderUTF8_Blended(font_, txt.c_str(), sdlclr);

	if (not text_surface)
		throw RenderError((format("Rendering '%s' gave the error: %s") % txt % TTF_GetError()).str());

	return
		*gr.imgcache().insert(PicMod_Text, cs, gr.convert_sdl_surface_to_picture(text_surface, true));
}

uint32_t SDLTTF_Font::ascent(int style) const {
	uint32_t rv = TTF_FontAscent(font_);
	if (style & SHADOW)
		rv += SHADOW_OFFSET;
	return rv;
}

void SDLTTF_Font::m_set_style(int style) {
	int sdl_style = TTF_STYLE_NORMAL;
	if (style & BOLD) sdl_style |= TTF_STYLE_BOLD;
	if (style & ITALIC) sdl_style |= TTF_STYLE_ITALIC;
	if (style & UNDERLINE) sdl_style |= TTF_STYLE_UNDERLINE;

	// Remember the last style. This should avoid that SDL_TTF flushes its
	// glyphcache all too often
	if (sdl_style == style_)
		return;
	style_ = sdl_style;
	TTF_SetFontStyle(font_, sdl_style);
}

}  // namespace RT