/*
 * Copyright (C) 2002, 2006-2008 by the Widelands Development Team
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

#include "ui_button.h"

#include "ui_constants.h"

#include "font_handler.h"
#include "graphic.h"
#include "rendertarget.h"
#include "constants.h"
#include "wlapplication.h"

namespace UI {

/**
 * Initialize a Basic_Button
*/
Basic_Button::Basic_Button
	(Panel * const parent,
	 const int32_t x, const int32_t y, const uint32_t w, const uint32_t h,
	 const bool _enabled, const bool flat,
	 const uint32_t background_picture_id,
	 const uint32_t foreground_picture_id,
	 const std::string & title_text,
	 const std::string & tooltip_text)
	:
	Panel           (parent, x, y, w, h, tooltip_text),
	m_highlighted   (false),
	m_pressed       (false),
	m_enabled       (_enabled),
	m_repeating     (false),
	m_flat          (flat),
	m_title         (title_text),
	m_pic_background(background_picture_id),
	m_pic_custom    (foreground_picture_id),
	m_pic_custom_disabled(g_gr->create_grayed_out_pic(foreground_picture_id)),
	m_clr_down      (229, 161, 2),
	m_draw_caret    (false)
{
	set_think(false);

	switch (background_picture_id) {
	default:
	case 0:
		m_pic_background = g_gr->get_picture(PicMod_UI, "pics/but0.png"); break;
	case 1:
		m_pic_background = g_gr->get_picture(PicMod_UI, "pics/but1.png"); break;
	case 2:
		m_pic_background = g_gr->get_picture(PicMod_UI, "pics/but2.png"); break;
	case 3:
		m_pic_background = g_gr->get_picture(PicMod_UI, "pics/but3.png"); break;
	case 4:
		m_pic_background = g_gr->get_picture(PicMod_UI, "pics/but4.png"); break;
	}
}


Basic_Button::~Basic_Button() {
	if (m_pic_custom_disabled)
		g_gr->free_surface(m_pic_custom_disabled);
}


/**
 * Sets a new picture for the Basic_Button.
*/
void Basic_Button::set_pic(uint32_t picid)
{
	m_title.clear();

	m_pic_custom = picid;
	if (m_pic_custom_disabled)
		g_gr->free_surface(m_pic_custom_disabled);
	m_pic_custom_disabled = g_gr->create_grayed_out_pic(picid);

	update(0, 0, get_w(), get_h());
}


/**
 * Set a text title for the Basic_Button
*/
void Basic_Button::set_title(const std::string & title) {
	m_pic_custom = 0;
	m_title      = title;


	update(0, 0, get_w(), get_h());
}


/**
 * Enable/Disable the button (disabled buttons can't be clicked).
 * Buttons are enabled by default
*/
void Basic_Button::set_enabled(bool on)
{
	// disabled buttons should look different...
	if (on)
		m_enabled = true;
	else {
		if (m_pressed) {
			m_pressed = false;
			set_think(false);
			grab_mouse(false);
		}
		m_enabled = false;
		m_highlighted = false;
	}
	update(0, 0, get_w(), get_h());
}


/**
 * Redraw the button
*/
void Basic_Button::draw(RenderTarget* dst)
{
	// Draw the background
	if (not m_flat)
		dst->tile
			(Rect(Point(0, 0), get_w(), get_h()),
			 m_pic_background,
			 Point(get_x(), get_y()));

	if (m_enabled and m_highlighted)
		dst->brighten_rect
			(Rect(Point(0, 0), get_w(), get_h()), MOUSE_OVER_BRIGHT_FACTOR);

	if (m_pic_custom) {// if we got a picture, draw it centered
		uint32_t cpw, cph;
		g_gr->get_picture_size(m_pic_custom, cpw, cph);

		//  ">> 1" is almost like "/ 2", but simpler for signed types (difference
		//  is that -1 >> 1 is -1 but -1 / 2 is 0).
		dst->blit
			(Point
			 	((get_w() - static_cast<int32_t>(cpw)) >> 1,
			 	 (get_h() - static_cast<int32_t>(cph)) >> 1),
			 m_enabled ? m_pic_custom : m_pic_custom_disabled);
	} else if (m_title.length()) { //  otherwise draw title string centered
		g_fh->draw_string
			(*dst,
			 UI_FONT_SMALL,
			 m_enabled ? UI_FONT_CLR_FG : UI_FONT_CLR_DISABLED, UI_FONT_CLR_BG,
			 Point(get_w() >> 1, get_h() >> 1),
			 m_title.c_str(),
			 Align_Center,
			 -1,
			 Widget_Cache_None,
			 0,
			 (m_draw_caret ?
			  m_title.length() : std::numeric_limits<uint32_t>::max()));
	}

	//  draw border
	//  a pressed but not highlighted button occurs when the user has pressed
	//  the left mouse button and then left the area of the button or the button
	//  stays pressed when it is pressed once
	RGBColor black(0, 0, 0);

	if (not m_flat) {
		assert(2 <= get_w());
		assert(2 <= get_h());
		//  button is a normal one, not flat
		if (not m_pressed or not m_highlighted) {
			//  top edge
			dst->brighten_rect
				(Rect(Point(0, 0), get_w(), 2), BUTTON_EDGE_BRIGHT_FACTOR);
			//  left edge
			dst->brighten_rect
				(Rect(Point(0, 2), 2, get_h() - 2), BUTTON_EDGE_BRIGHT_FACTOR);
			//  bottom edge
			dst->fill_rect(Rect(Point(2, get_h() - 2), get_w() - 2, 1), black);
			dst->fill_rect(Rect(Point(1, get_h() - 1), get_w() - 1, 1), black);
			//  right edge
			dst->fill_rect(Rect(Point(get_w() - 2, 2), 1, get_h() - 2), black);
			dst->fill_rect(Rect(Point(get_w() - 1, 1), 1, get_h() - 1), black);
		} else {
			//  bottom edge
			dst->brighten_rect
				(Rect(Point(0, get_h() - 2), get_w(), 2),
				 BUTTON_EDGE_BRIGHT_FACTOR);
			//  right edge
			dst->brighten_rect
				(Rect(Point(get_w() - 2, 0), 2, get_h() - 2),
				 BUTTON_EDGE_BRIGHT_FACTOR);
			//  top edge
			dst->fill_rect(Rect(Point(0, 0), get_w() - 1, 1), black);
			dst->fill_rect(Rect(Point(0, 1), get_w() - 2, 1), black);
			//  left edge
			dst->fill_rect(Rect(Point(0, 0), 1, get_h() - 1), black);
			dst->fill_rect(Rect(Point(1, 0), 1, get_h() - 2), black);
		}
	} else {
		//  Button is flat, do not draw borders, instead, if it is pressed, draw
		//  a box around it.
		if (m_pressed && m_highlighted)
			dst->draw_rect(Rect(Point(0, 0), get_w(), get_h()), m_clr_down);
	}
}

void Basic_Button::think()
{
	assert(m_repeating);
	assert(m_pressed);
	Panel::think();


	if (m_highlighted) {
		int32_t const time = WLApplication::get()->get_time();
		if (m_time_nextact <= time) {
			m_time_nextact += MOUSE_BUTTON_AUTOREPEAT_TICK; //  schedule next tick
			if (m_time_nextact < time)
				m_time_nextact = time;
			play_click();
			send_signal_clicked();
			//  The button may not exist at this point (for example if the button
			//  closed the dialog that it is part of). So member variables may no
			//  longer be accessed.
		}
	}
}

/**
 * Update highlighted status
*/
void Basic_Button::handle_mousein(bool const inside)
{
	m_highlighted = inside && m_enabled;
	update(0, 0, get_w(), get_h());
}


/**
 * Update the pressed status of the button
*/
bool Basic_Button::handle_mousepress(const Uint8 btn, int32_t, int32_t) {
	if (btn != SDL_BUTTON_LEFT)
		return false;

	if (m_enabled) {
		assert(m_highlighted);
		grab_mouse(true);
		m_pressed = true;
		if (m_repeating) {
			m_time_nextact =
				WLApplication::get()->get_time() + MOUSE_BUTTON_AUTOREPEAT_DELAY;
			set_think(true);
		}
	}
	update(0, 0, get_w(), get_h());

	return true;
}
bool Basic_Button::handle_mouserelease(const Uint8 btn, int32_t, int32_t) {
	if (btn != SDL_BUTTON_LEFT)
		return false;

	set_think(false);
	if (m_pressed) {
		m_pressed = false;
		grab_mouse(false);
		update(0, 0, get_w(), get_h());
		if (m_highlighted && m_enabled) {
			play_click();
			send_signal_clicked();
			//  The button may not exist at this point (for example if the button
			//  closed the dialog that it is part of). So member variables may no
			//  longer be accessed.
		}
	}
	return true;
}
};
