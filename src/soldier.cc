/*
 * Copyright (C) 2002-4 by the Widelands Development Team
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

#include "editor_game_base.h"
#include "error.h"
#include "game.h"
#include "graphic.h"
#include "player.h"
#include "profile.h"
#include "rendertarget.h"
#include "rgbcolor.h"
#include "soldier.h"
#include "transport.h"
#include "tribe.h"
#include "util.h"
#include "wexception.h"


/*
==============================================================================

IdleSoldierSupply IMPLEMENTATION

==============================================================================
*/

class IdleSoldierSupply : public Supply {
	public:
		IdleSoldierSupply(Soldier* w);
		~IdleSoldierSupply();

		void set_economy(Economy* e);

	public:
		virtual PlayerImmovable* get_position(Game* g);
		virtual int get_amount(Game* g, int ware);
		virtual bool is_active(Game* g);

		virtual WareInstance* launch_item(Game* g, int ware);
		virtual Worker* launch_worker(Game* g, int ware);

		virtual Soldier* launch_soldier(Game* g, int ware, Requeriments* req);
		virtual int get_passing_requeriments(Game* g, int ware, Requeriments* r);
		virtual void mark_as_used (Game* g, int ware, Requeriments* r);
	private:
		Soldier*		m_soldier;
		Economy*		m_economy;
};


/*
===============
IdleSoldierSupply::IdleSoldierSupply

Automatically register with the soldier's economy.
===============
*/
IdleSoldierSupply::IdleSoldierSupply(Soldier* s)
{
	m_soldier = s;
	m_economy = 0;

	set_economy(s->get_economy());
}


/*
===============
IdleSoldierSupply::~IdleSoldierSupply

Automatically unregister from economy.
===============
*/
IdleSoldierSupply::~IdleSoldierSupply()
{
	set_economy(0);
}


/*
===============
IdleSoldierSupply::set_economy

Add/remove this supply from the Economy as appropriate.
===============
*/
void IdleSoldierSupply::set_economy(Economy* e)
{
	if (m_economy == e)
		return;

	if (m_economy)
		m_economy->remove_soldier_supply(m_soldier->get_owner()->get_tribe()->get_worker_index(m_soldier->get_name().c_str()), this);

	m_economy = e;

	if (m_economy)
		m_economy->add_soldier_supply(m_soldier->get_owner()->get_tribe()->get_worker_index(m_soldier->get_name().c_str()), this);
}


/*
===============
IdleSoldierSupply::get_position

Return the soldier's position.
===============
*/
PlayerImmovable* IdleSoldierSupply::get_position(Game* g)
{
	return m_soldier->get_location(g);
}


/*
===============
IdleSoldierSupply::get_amount

It's just the one soldier.
===============
*/
int IdleSoldierSupply::get_amount(Game* g, int ware)
{
	if (ware == m_soldier->get_owner()->get_tribe()->get_worker_index(m_soldier->get_name().c_str()))
		return 1;

	return 0;
}


/*
===============
IdleSoldierSupply::is_active

Idle soldiers are always active supplies, because they need to get into a
Warehouse ASAP.
===============
*/
bool IdleSoldierSupply::is_active(Game* g)
{
	return true;
}


/*
===============
IdleSoldierSupply::launch_item
===============
*/
WareInstance* IdleSoldierSupply::launch_item(Game* g, int ware)
{
	throw wexception("IdleSoldierSupply::launch_item() makes no sense.");
}


/*
===============
IdleSodlierSupply::launch_worker
===============
*/
Worker* IdleSoldierSupply::launch_worker(Game* g, int ware)
{
	log ("IdleSoldierSupply::launch_worker() Warning something can go wrong around here.\n");

	return launch_soldier(g, ware, NULL);
}


/*
===============
IdleSodlierSupply::launch_soldier

No need to explicitly launch the soldier.
===============
*/
Soldier* IdleSoldierSupply::launch_soldier(Game* g, int ware, Requeriments* req)
{
	assert(ware == m_soldier->get_owner()->get_tribe()->get_worker_index(m_soldier->get_name().c_str()));

	if (req)
	{
		if (req->check (
				  m_soldier->get_level(atrHP),
		m_soldier->get_level(atrAttack),
		m_soldier->get_level(atrDefense),
		m_soldier->get_level(atrEvade)
							))
		{
			// Ensures that this soldier is used now
			m_soldier->mark (false);
			return m_soldier;
		}
		else
			throw wexception ("IdleSoldierSupply::launch_soldier Fails. Requeriments aren't accomplished.");
	}
	else
		return m_soldier;
}

/*
===============
IdleSodlierSupply::mark_as_used
===============
*/
void IdleSoldierSupply::mark_as_used(Game* g, int ware, Requeriments* req)
{
	assert(ware == m_soldier->get_owner()->get_tribe()->get_worker_index(m_soldier->get_name().c_str()));

	if (req)
	{
		if (req->check (
				  m_soldier->get_level(atrHP),
		m_soldier->get_level(atrAttack),
		m_soldier->get_level(atrDefense),
		m_soldier->get_level(atrEvade)
							))
		{
			// Ensures that this soldier has a request now
			m_soldier->mark (true);
		}
		else
			throw wexception ("IdleSoldierSupply::launch_soldier Fails. Requeriments aren't accomplished.");
	}
	else
		m_soldier->mark (true);
}


/*
===============
IdleSodlierSupply::get_passing_requeriments

No need to explicitly launch the soldier.
===============
*/
int IdleSoldierSupply::get_passing_requeriments(Game* g, int ware, Requeriments* req)
{
	assert(ware == m_soldier->get_owner()->get_tribe()->get_worker_index(m_soldier->get_name().c_str()));

	// Oops we find a marked soldied (in use)
	if (m_soldier->is_marked())
		return 0;

	if (!req)
		return 1;

	if (req->check (
			m_soldier->get_level(atrHP),
	m_soldier->get_level(atrAttack),
	m_soldier->get_level(atrDefense),
	m_soldier->get_level(atrEvade)
						))
		return 1;
	else
		return 0;
}




/*
==============================================================

SOLDIER DESCRIPTION IMPLEMENTATION

==============================================================
*/

/*
===============
Soldier_Descr::Soldier_Descr
Soldier_Descr::~Soldier_Descr
===============
*/
Soldier_Descr::Soldier_Descr(Tribe_Descr *tribe, const char *name)
	: Worker_Descr(tribe, name)
{
	add_attribute(Map_Object::SOLDIER);
}

Soldier_Descr::~Soldier_Descr(void)
{
   m_hp_pics_fn.resize(0);
   m_attack_pics_fn.resize(0);
   m_defense_pics_fn.resize(0);
   m_evade_pics_fn.resize(0);
}

/*
===============
Soldier_Descr::parse

Parse carrier-specific configuration data
===============
*/
void Soldier_Descr::parse(const char *directory, Profile *prof, const EncodeData *encdata)
{
	Worker_Descr::parse(directory, prof, encdata);
   Section* sglobal=prof->get_section("global");
  
   // Parse hitpoints
   std::string hp=sglobal->get_safe_string("hp");
   std::vector<std::string> list;
   split_string(hp, &list, "-");
   if(list.size()!=2) 
      throw wexception("Parse error in hp string: \"%s\" (must be \"min-max\")", hp.c_str());
   uint i=0;
   for(i=0; i<list.size(); i++)
      remove_spaces(&list[i]);
   char* endp;
   m_min_hp= strtol(list[0].c_str(),&endp, 0);
   if(endp && *endp)
      throw wexception("Parse error in hp string: %s is a bad value", list[0].c_str());
   m_max_hp = strtol(list[1].c_str(),&endp, 0);
   if(endp && *endp)
      throw wexception("Parse error in hp string: %s is a bad value", list[1].c_str());

   // Parse attack
   std::string attack=sglobal->get_safe_string("attack");
   list.resize(0);
   split_string(attack, &list, "-");
   if(list.size()!=2) 
      throw wexception("Parse error in attack string: \"%s\" (must be \"min-max\")", attack.c_str());
   for(i=0; i<list.size(); i++)
      remove_spaces(&list[i]);
   m_min_attack= strtol(list[0].c_str(),&endp, 0);
   if(endp && *endp)
      throw wexception("Parse error in attack string: %s is a bad value", list[0].c_str());
   m_max_attack = strtol(list[1].c_str(),&endp, 0);
   if(endp && *endp)
      throw wexception("Parse error in attack string: %s is a bad value", list[1].c_str());

   // Parse defend
   m_defense=sglobal->get_safe_int("defense");
   
   // Parse evade
   m_evade=sglobal->get_safe_int("evade");

   // Parse increases per level
   m_hp_incr=sglobal->get_safe_int("hp_incr_per_level");
   m_attack_incr=sglobal->get_safe_int("attack_incr_per_level");
   m_defense_incr=sglobal->get_safe_int("defense_incr_per_level");
   m_evade_incr=sglobal->get_safe_int("evade_incr_per_level");

   // Parse max levels
   m_max_hp_level=sglobal->get_safe_int("max_hp_level");
   m_max_attack_level=sglobal->get_safe_int("max_attack_level");
   m_max_defense_level=sglobal->get_safe_int("max_defense_level");
   m_max_evade_level=sglobal->get_safe_int("max_evade_level");
 
   // Load the filenames
   m_hp_pics_fn.resize(m_max_hp_level+1);
   m_attack_pics_fn.resize(m_max_attack_level+1);
   m_defense_pics_fn.resize(m_max_defense_level+1);
   m_evade_pics_fn.resize(m_max_evade_level+1);
   char buffer[256];
   std::string dir=directory;
   dir+="/";
   for(i=0; i<=m_max_hp_level; i++) {
      sprintf(buffer, "hp_level_%i_pic", i);
      m_hp_pics_fn[i]=dir;
      m_hp_pics_fn[i]+=sglobal->get_safe_string(buffer);
   }
   for(i=0; i<=m_max_attack_level; i++) {
      sprintf(buffer, "attack_level_%i_pic", i);
      m_attack_pics_fn[i]=dir;
      m_attack_pics_fn[i]+=sglobal->get_safe_string(buffer);
   }
   for(i=0; i<=m_max_defense_level; i++) {
      sprintf(buffer, "defense_level_%i_pic", i);
      m_defense_pics_fn[i]=dir;
      m_defense_pics_fn[i]+=sglobal->get_safe_string(buffer);
   }
   for(i=0; i<=m_max_evade_level; i++) {
      sprintf(buffer, "evade_level_%i_pic", i);
      m_evade_pics_fn[i]=dir;
      m_evade_pics_fn[i]+=sglobal->get_safe_string(buffer);
   }
}

/*
 * Load the graphics
 */
void Soldier_Descr::load_graphics(void) {
   m_hp_pics.resize(m_max_hp_level+1);
   m_attack_pics.resize(m_max_attack_level+1);
   m_defense_pics.resize(m_max_defense_level+1);
   m_evade_pics.resize(m_max_evade_level+1);
   uint i;
   for(i=0; i<=m_max_hp_level; i++) {
      m_hp_pics[i]=g_gr->get_picture(PicMod_Game, m_hp_pics_fn[i].c_str(), true);
   }
   for(i=0; i<=m_max_attack_level; i++) {
      m_attack_pics[i]=g_gr->get_picture(PicMod_Game, m_attack_pics_fn[i].c_str(), true);
   }
   for(i=0; i<=m_max_defense_level; i++) {
      m_defense_pics[i]=g_gr->get_picture(PicMod_Game, m_defense_pics_fn[i].c_str(), true);
   }
   for(i=0; i<=m_max_evade_level; i++) {
      m_evade_pics[i]=g_gr->get_picture(PicMod_Game, m_evade_pics_fn[i].c_str(), true);
   }
   Worker_Descr::load_graphics();
}

/*
 * Create a new soldier
 */
Bob* Soldier_Descr::create_object() {
   return new Soldier(this);
}

/*
==============================

IMPLEMENTATION

==============================
*/

/*
===============
Soldier::Soldier
Soldier::~Soldier
===============
*/
Soldier::Soldier(Soldier_Descr *descr)
	: Worker(descr)
{
   // all done through init
}

Soldier::~Soldier()
{
}

/*
 * Initialize this soldier
 */
void Soldier::init(Editor_Game_Base* gg) {
	m_hp_level=0;
	m_attack_level=0;
	m_defense_level=0;
	m_evade_level=0;

	m_hp_max=0;
	m_min_attack=get_descr()->get_min_attack();
	m_max_attack=get_descr()->get_max_attack();
	m_defense=get_descr()->get_defense();
	m_evade=get_descr()->get_evade();
	if(gg->is_game()) {
		Game* g= static_cast<Game*>(g);
		int range=get_descr()->get_max_hp()-get_descr()->get_min_hp();
		int value=g->logic_rand() % range;
		m_hp_max=value;
	}
	m_hp_current=m_hp_max;

	m_marked = false;

	Worker::init(gg);
}

/*
 * Set this soldiers level. Automatically sets the new values
 */
void Soldier::set_level(uint hp, uint attack, uint defense, uint evade) {
   set_hp_level(hp);
   set_attack_level(attack);
   set_defense_level(defense);
   set_evade_level(evade);
}
void Soldier::set_hp_level(uint hp) {
   assert(hp>=m_hp_level && hp<=get_descr()->get_max_hp_level());
   
   while(m_hp_level<hp) {
      ++m_hp_level;
      m_hp_max+=get_descr()->get_hp_incr_per_level();
      m_hp_current+=get_descr()->get_hp_incr_per_level();
   }
}
void Soldier::set_attack_level(uint attack) {
   assert(attack>=m_attack_level && attack<=get_descr()->get_max_attack_level());
   
   while(m_attack_level<attack) {
      ++m_attack_level;
      m_min_attack+=get_descr()->get_attack_incr_per_level();
      m_max_attack+=get_descr()->get_attack_incr_per_level();
   }
}
void Soldier::set_defense_level(uint defense) {
   assert(defense>=m_defense_level && defense<=get_descr()->get_max_defense_level());
   
   while(m_defense_level<defense) {
      ++m_defense_level;
      m_defense+=get_descr()->get_defense_incr_per_level();
   }
}
void Soldier::set_evade_level(uint evade) {
   assert(evade>=m_evade_level && evade<=get_descr()->get_max_evade_level());
   
   while(m_evade_level<evade) {
      ++m_evade_level;
      m_evade+=get_descr()->get_evade_incr_per_level();
   }
}

uint Soldier::get_level(tAttribute at)
{
	switch (at)
	{
		case atrHP:		return m_hp_level;
		case atrAttack:	return m_attack_level;
		case atrDefense:return m_defense_level;
		case atrEvade:	return m_evade_level;
	}
	throw wexception ("Soldier::get_level attribute not identified.)");
}

void Soldier::heal (uint hp)
{
	// Ensures that we can only heal, don't hurt throught this method
	assert (hp >= 0);

	m_hp_current += hp;

	if (m_hp_current > m_hp_max)
		m_hp_current = m_hp_max;
}

/*
 * Draw this soldier. This basically draws him as a worker, but add hitpoints
 */
void Soldier::draw(Editor_Game_Base* g, RenderTarget* dst, Point pos) {
   uint anim = get_current_anim();

	if (!anim)
		return;

	Point drawpos;
	calc_drawpos(g, pos, &drawpos);

   int w, h;
   g_gr->get_animation_size(anim, g->get_gametime() - get_animstart(), &w, &h);
   
   // Draw energy bar
   // first: draw white sourrounding
   const int frame_width=w<<1;  // width * 2
   const int frame_height=5;
   const int frame_beginning_x=drawpos.x-(frame_width>>1);  // TODO: these should be calculated from the hot spot, not assumed
   const int frame_beginning_y=drawpos.y-h-7;     
   dst->draw_rect(frame_beginning_x, frame_beginning_y, frame_width, frame_height, HP_FRAMECOLOR);
   // Draw energybar
   float percent = (float)m_hp_current/m_hp_max;
   int energy_width=static_cast<int>(percent * (frame_width-2));
   RGBColor color;
   if (percent <= 0.15)
		color = RGBColor(255, 0, 0);
	else if (percent <= 0.5)
		color = RGBColor(255, 255, 0);
	else
		color = RGBColor(17,192,17);
   dst->fill_rect(frame_beginning_x+1, frame_beginning_y+1, energy_width, frame_height-2, color); 

   // Draw information fields about levels
   // first, gather informations
   uint hppic=get_hp_level_pic();
   uint attackpic=get_attack_level_pic();
   uint defensepic=get_defense_level_pic();
   uint evadepic=get_evade_level_pic();
   int hpw,hph,atw,ath,dew,deh,evw,evh; 
   g_gr->get_picture_size(hppic, &hpw, &hph);
   g_gr->get_picture_size(attackpic, &atw, &ath);
   g_gr->get_picture_size(defensepic, &dew, &deh);
   g_gr->get_picture_size(evadepic, &evw, &evh);

   dst->blit(frame_beginning_x, frame_beginning_y-hph-ath, attackpic);
   dst->blit(frame_beginning_x+(frame_width>>1), frame_beginning_y-evh-deh, defensepic);
   dst->blit(frame_beginning_x, frame_beginning_y-hph, hppic);
   dst->blit(frame_beginning_x+(frame_width>>1), frame_beginning_y-evh, evadepic);
   
	Worker::draw(g,dst,pos);
}


/*
===============
Soldier::start_task_gowarehouse

Get the soldier to move to the nearest warehouse.
The soldier is added to the list of usable wares, so he may be reassigned to
a new task immediately.
===============
*/
void Soldier::start_task_gowarehouse(Game* g)
{
	assert(!m_supply);

	push_task(g, &taskGowarehouse);

	m_supply = (IdleWorkerSupply*) new IdleSoldierSupply(this);
}

