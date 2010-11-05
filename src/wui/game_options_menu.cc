/*
 * Copyright (C) 2002-2004, 2006, 2008-2009 by the Widelands Development Team
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

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

#include "game_options_menu.h"

#include "ui_fsmenu/fileview.h"
#include "game_options_sound_menu.h"
#include "game_main_menu_save_game.h"
#include "graphic/graphic.h"
#include "i18n.h"
#include "sound/sound_handler.h"


GameOptionsMenu::GameOptionsMenu
	(Interactive_GameBase                         & gb,
	 UI::UniqueWindow::Registry                   & registry,
	 Interactive_GameBase::Game_Main_Menu_Windows & windows)
:
	UI::UniqueWindow
		(&gb, "options", &registry,
		 102,
		 vmargin()
		 + 4 * (20 + vspacing()) + 2 * vgap() +
		 35 + vspacing() + 35 +
		 vmargin(),
		 _("Options")),
	m_gb(gb),
	m_windows(windows),
	readme
		(this, "readme",
		 posx(0, 1),
		 vmargin() + 0 * (20 + vspacing()) + 0 * vgap(),
		 buttonw(1), 20,
		 g_gr->get_picture(PicMod_UI, "pics/but4.png"),
		 boost::bind(&UI::UniqueWindow::Registry::toggle, boost::ref(m_windows.readme)),
		 _("README")),
	license
		(this, "license",
		 posx(0, 1),
		 vmargin() + 1 * (20 + vspacing()) + 0 * vgap(),
		 buttonw(1), 20,
		 g_gr->get_picture(PicMod_UI, "pics/but4.png"),
		 boost::bind(&UI::UniqueWindow::Registry::toggle, boost::ref(m_windows.license)),
		 _("License")),
	authors
		(this, "authors",
		 posx(0, 1),
		 vmargin() + 2 * (20 + vspacing()) + 0 * vgap(),
		 buttonw(1), 20,
		 g_gr->get_picture(PicMod_UI, "pics/but4.png"),
		 boost::bind(&UI::UniqueWindow::Registry::toggle, boost::ref(m_windows.authors)),
		 _("Authors")),
	sound
		(this, "sound_options",
		 posx(0, 1),
		 vmargin() + 3 * (20 + vspacing()) + 1 * vgap(),
		 buttonw(1), 20,
		 g_gr->get_picture(PicMod_UI, "pics/but4.png"),
		 boost::bind(&GameOptionsMenu::clicked_sound, boost::ref(*this)),
		 _("Sound Options")),
	save_game
		(this, "save_game",
		 posx(0, 1),
		 vmargin() + 4 * (20 + vspacing()) + 2 * vgap(),
		 buttonw(1), 35,
		 g_gr->get_picture(PicMod_UI, "pics/but4.png"),
		 g_gr->get_picture(PicMod_Game, "pics/menu_save_game.png"),
		 boost::bind(&GameOptionsMenu::clicked_save_game, boost::ref(*this)),
		 _("Save Game")),
	exit_game
		(this, "exit_game",
		 posx(0, 1),
		 vmargin() + 4 * (20 + vspacing()) + 2 * vgap() +
		 35 + vspacing(),
		 buttonw(1), 35,
		 g_gr->get_picture(PicMod_UI, "pics/but4.png"),
		 g_gr->get_picture(PicMod_Game, "pics/menu_exit_game.png"),
		 boost::bind(&GameOptionsMenu::clicked_exit_game, boost::ref(*this)),
		 _("Exit Game"))
{

	m_windows.readme.constr = boost::bind(&fileview_window, boost::ref(m_gb), _1, "txts/README");
	m_windows.license.constr = boost::bind(&fileview_window, boost::ref(m_gb), _1, "txts/COPYING");
	m_windows.authors.constr = boost::bind(&fileview_window, boost::ref(m_gb), _1, "txts/developers");
	// For some reason the latter does not work. It seemms that
	// boost::lambda::bind is less powerful than boost::bind, but the
	// latter does not work with boost::lamda::new_ptr
	// m_windows.sound_options.constr = boost::lambda::bind(boost::lambda::new_ptr<GameOptionsSoundMenu>(), boost::ref(m_gb),_1);

#define INIT_BTN_HOOKS(registry, btn)                                        \
 registry.onCreate = boost::bind(&UI::Button::set_perm_pressed, &btn, true);  \
 registry.onDelete = boost::bind(&UI::Button::set_perm_pressed, &btn, false); \
 if (registry.window) btn.set_perm_pressed(true);                            \

	INIT_BTN_HOOKS(m_windows.readme, readme)
	INIT_BTN_HOOKS(m_windows.license, license)
	INIT_BTN_HOOKS(m_windows.authors, authors)
	INIT_BTN_HOOKS(m_windows.sound_options, sound)

	set_inner_size
		(hmargin() + hmargin() +
		 std::max(static_cast<int32_t>(get_inner_w()), readme.get_w()),
		 get_inner_h());
	if (get_usedefaultpos())
		center_to_parent();
}

void GameOptionsMenu::clicked_sound() {
	if (m_windows.sound_options.window)
		delete m_windows.sound_options.window;
	else
		new GameOptionsSoundMenu(m_gb, m_windows.sound_options);
}

void GameOptionsMenu::clicked_save_game() {
	new Game_Main_Menu_Save_Game(m_gb, m_windows.savegame);
	die();
}

void GameOptionsMenu::clicked_exit_game() {m_gb.end_modal(0);}
