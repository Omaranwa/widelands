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

#include "wui/trainingsitewindow.h"

#include "graphic/graphic.h"
#include "ui_basic/tabpanel.h"
#include "wui/soldiercapacitycontrol.h"
#include "wui/soldierlist.h"

static char const* pic_tab_military = "images/wui/buildings/menu_tab_military.png";

using namespace Widelands;


/**
 * Create the \ref TrainingSite specific soldier list tab.
 */
TrainingSiteWindow::TrainingSiteWindow(InteractiveGameBase& parent, TrainingSite& ts)
   : ProductionSiteWindow(parent, ts) {
	init();
}

void TrainingSiteWindow::init() {
	ProductionSiteWindow::init();
	get_tabs()->add("soldiers", g_gr->images().get(pic_tab_military),
						 create_soldier_list(*get_tabs(), igbase(), trainingsite()),
						 _("Soldiers in training"));
}

void TrainingSiteWindow::create_capsbuttons(UI::Box* buttons) {
	ProductionSiteWindow::create_capsbuttons(buttons);
}

/*
===============
Create the training site information window.
===============
*/
BuildingWindow* TrainingSite::create_options_window(InteractiveGameBase& plr) {
	return new TrainingSiteWindow(plr, *this);
}
