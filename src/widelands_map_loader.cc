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
#include "filesystem.h"
#include "map.h"
#include "player.h"
#include "tribe.h"
#include "widelands_map_allowed_buildings_data_packet.h"
#include "widelands_map_battle_data_packet.h"
#include "widelands_map_bob_data_packet.h"
#include "widelands_map_bobdata_data_packet.h"
#include "widelands_map_building_data_packet.h"
#include "widelands_map_buildingdata_data_packet.h"
#include "widelands_map_elemental_data_packet.h"
#include "widelands_map_event_data_packet.h"
#include "widelands_map_event_chain_data_packet.h"
#include "widelands_map_extradata_data_packet.h"
#include "widelands_map_flag_data_packet.h"
#include "widelands_map_flagdata_data_packet.h"
#include "widelands_map_data_packet_ids.h"
#include "widelands_map_heights_data_packet.h"
#include "widelands_map_immovable_data_packet.h"
#include "widelands_map_immovabledata_data_packet.h"
#include "widelands_map_loader.h"
#include "widelands_map_map_object_loader.h"
#include "widelands_map_objective_data_packet.h"
#include "widelands_map_owned_fields_data_packet.h"
#include "widelands_map_player_names_and_tribes_data_packet.h"
#include "widelands_map_player_position_data_packet.h"
#include "widelands_map_resources_data_packet.h"
#include "widelands_map_road_data_packet.h"
#include "widelands_map_roaddata_data_packet.h"
#include "widelands_map_seen_fields_data_packet.h"
#include "widelands_map_terrain_data_packet.h"
#include "widelands_map_trigger_data_packet.h"
#include "widelands_map_variable_data_packet.h"
#include "widelands_map_ware_data_packet.h"
#include "widelands_map_waredata_data_packet.h"
#include "world.h"


/*
 * Constructor
 */
Widelands_Map_Loader::Widelands_Map_Loader(FileSystem* fs, Map* map) :
   Map_Loader("", map) {
   m_fs=fs;
   m_map=map;
   m_mol=0;
}

/*
 * Destructor
 */
Widelands_Map_Loader::~Widelands_Map_Loader(void) {
   if(m_mol)
      delete m_mol;
   delete m_fs;
}

/*
 * This function preloads map so that
 * the map class returns valid data for all
 * the get_info() functions (_width, _nrplayers..)
 */
int Widelands_Map_Loader::preload_map(bool scenario) {
   assert(get_state()!=STATE_LOADED);

   m_map->cleanup();

   // Load elemental data block
   Widelands_Map_Elemental_Data_Packet mp;

   mp.Pre_Read(m_fs, m_map);

   if(!World::exists_world(m_map->get_world_name())) {
      throw wexception("%s: %s", m_map->get_world_name(), "World doesn't exist!");
   }

   Widelands_Map_Player_Names_And_Tribes_Data_Packet* dp=new Widelands_Map_Player_Names_And_Tribes_Data_Packet();
   dp->Pre_Read(m_fs, m_map, !scenario);
   delete dp;

   set_state(STATE_PRELOADED);

   return 0;
}

/*
 * Load the complete map and make sure that it runs without problems
 */
int Widelands_Map_Loader::load_map_complete(Editor_Game_Base* egbase, bool scenario) {

   // Load elemental data block (again)
   Widelands_Map_Elemental_Data_Packet mp;
   mp.Pre_Read(m_fs, m_map);

   // now, load the world, load the rest infos from the map
   m_map->load_world();
   // Postload the world which provides all the immovables found on a map
   m_map->get_world()->postload(egbase);
   m_map->set_size(m_map->m_width, m_map->m_height);

   if(m_mol)
      delete m_mol;
   m_mol=new Widelands_Map_Map_Object_Loader;

   Widelands_Map_Data_Packet* dp;

   // MANDATORY PACKETS
   // Start with writing the map out, first Elemental data
   // PRELOAD DATA BEGIN
   log("Reading Elemental Data ... ");
   dp= new Widelands_Map_Elemental_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // now player names and tribes
   log("Reading Player Names And Tribe Data ... ");
   dp=new Widelands_Map_Player_Names_And_Tribes_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");
   // PRELOAD DATA END

   // now heights
   log("Reading Heights Data ... ");
   dp=new Widelands_Map_Heights_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // and terrains
   log("Reading Terrain Data ... ");
   dp=new Widelands_Map_Terrain_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // now immovables
   log("Reading Immovable Data ... ");
   dp=new Widelands_Map_Immovable_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // now player pos
   log("Reading Player Start Position Data ... ");
   dp=new Widelands_Map_Player_Position_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // now bobs
   log("Reading Bob Data ... ");
   dp=new Widelands_Map_Bob_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // now resources
   log("Reading Resources Data ... ");
   dp=new Widelands_Map_Resources_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // NON MANDATORY PACKETS BELOW THIS POINT
   // Map Extra Data
   log("Reading Map Extra Data ... ");
   dp=new Widelands_Map_Extradata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // Triggers
   log("Reading Trigger Data ... ");
   dp=new Widelands_Map_Trigger_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // Objectives: Depend on triggers
   log("Reading Objective Data ... ");
   dp=new Widelands_Map_Objective_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // Events, depend on trigger, objectives
   log("Reading Event Data ... ");
   dp=new Widelands_Map_Event_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // Event Chains
   log("Reading Event Chain Data ... ");
   dp=new Widelands_Map_EventChain_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Allowed Buildings Data ... ");
   dp=new Widelands_Map_Allowed_Buildings_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // We always write the next few packets since it
   // takes too much time looking if it really is needed
   // !!!!!!!!!! NOTE
   // This packet must be before any building or road packet. So do not
   // change this order without knowing what you do
   // EXISTENT PACKETS
   log("Reading Flag Data ... ");
   dp=new Widelands_Map_Flag_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Road Data ... ");
   dp=new Widelands_Map_Road_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Building Data ... ");
   dp=new Widelands_Map_Building_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");


   log("Reading Map Ware Data ... ");
   dp=new Widelands_Map_Ware_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   // DATA PACKETS
   log("Reading Flagdata Data ... ");
   dp=new Widelands_Map_Flagdata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Roaddata Data ... ");
   dp=new Widelands_Map_Roaddata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");


   log("Reading Buildingdata Data ... ");
   dp=new Widelands_Map_Buildingdata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");


   log("Reading Waredata Data ... ");
   dp=new Widelands_Map_Waredata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Bobdata Data ... ");
   dp=new Widelands_Map_Bobdata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Immovabledata Data ... ");
   dp=new Widelands_Map_Immovabledata_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Owned-Fields Data ... ");
   dp=new Widelands_Map_Owned_Fields_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Seen-Fields Data ... ");
   dp=new Widelands_Map_Seen_Fields_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");


   // This should be at least after loading Soldiers (Bobs)
   // NOTE DO NOT CHANGE THE PLACE UNLESS YOU KNOW WHAT ARE YOU DOING
   log("Reading Battle Data ... ");
   dp=new Widelands_Map_Battle_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   log("Reading Variable Data ... ");
   dp=new Widelands_Map_Variable_Data_Packet();
   dp->Read(m_fs, egbase, !scenario, m_mol);
   delete dp;
   log("done!\n ");

   if(m_mol->get_nr_unloaded_objects())
      log("WARNING: There are %i unloaded objects. This is a bug, please consider committing!\n", m_mol->get_nr_unloaded_objects());

   m_map->recalc_whole_map();

   set_state(STATE_LOADED);

   return 0;
}
