/*
 * Copyright (C) 2002 by the Widelands Development Team
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

#ifndef __S__SINGLEPMENUE_H
#define __S__SINGLEPMENUE_H

#include "fullscreen_menu_base.h"

/**
 * Fullscreen Menu for SinglePlayer.
 * Here you select what game you want to play.
 */
class Fullscreen_Menu_SinglePlayer : public Fullscreen_Menu_Base {
   public:
      Fullscreen_Menu_SinglePlayer();

      enum {
         sp_skirmish,
         sp_tutorial,
         sp_loadgame,
         sp_back
      };

};

#endif // __S__SINGLEPMENUE_H
