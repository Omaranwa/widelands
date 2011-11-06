/*
 * Copyright (C) 2011 by the Widelands Development Team
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

#include "mapastar.h"

#include "path.h"

namespace Widelands {

/**
 * Recover a shortest path from one of the sources introduced by @ref MapAStar::push
 * to the given destination @p dest, which must have been discovered
 * in the A-star process already.
 */
void MapAStarBase::pathto(Coords dest, Path & path) const
{
	path.m_end = dest;
	path.m_path.clear();

	Coords cur = dest;
	for (;;) {
		const Pathfield & pf(pathfield(cur));

		assert(pf.cycle == pathfields->cycle);

		if (pf.backlink == IDLE)
			break;

		path.m_path.push_back(pf.backlink);

		map.get_neighbour(cur, get_reverse_dir(pf.backlink), &cur);
	}

	path.m_start = cur;
}

} // namespace Widelands
