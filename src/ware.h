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

#ifndef __WARE_DESCR_H
#define __WARE_DESCR_H

class Tribe_Descr;
class Worker_Descr;

/*
Wares
-----

Wares can be stored in warehouses. They can be transferred across an Economy.
They can be traded.
Both items (lumber, stone, ...) and workers are considered wares.
Every ware has a unique name. Note that an item must not have the same name as
a worker.

Item wares are defined on a per-world basis, workers are defined on a per-tribe
basis.
Since the life-times of world and tribe descriptions are a bit dodgy, the 
master list of wares is kept by the Game class. The list is created just before
the game starts.

Note that multiple tribes can define a worker with the same name. The different
"version" of a worker must perform the same job, but they can look different.
*/
class Ware_Descr {
public:
	Ware_Descr(const char *name);
	virtual ~Ware_Descr();
	
	virtual bool is_worker() = 0;
	
	inline const char *get_name() const { return m_name; }
	
private:
	char		m_name[30];
};

class Item_Ware_Descr : public Ware_Descr {
public:
	Item_Ware_Descr(const char *name);
	virtual ~Item_Ware_Descr();
	
	virtual bool is_worker();

	// TODO: actually implement this (parsing from config etc...)
private:
	//Pic*		m_menu_pic;
};

class Worker_Ware_Descr : public Ware_Descr {
public:
	Worker_Ware_Descr(const char *name);
	virtual ~Worker_Ware_Descr();
	
	virtual bool is_worker();
	
	Worker_Descr *get_worker(Tribe_Descr *tribe);
	void add_worker(Tribe_Descr *tribe, Worker_Descr *worker);

private:
	typedef std::map<Tribe_Descr*,Worker_Descr*> Worker_map;

	Worker_map	m_workers;
};

#endif
