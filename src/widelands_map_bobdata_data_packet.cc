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

#include "bob.h"
#include "critter_bob.h"
#include "critter_bob_program.h"
#include "filesystem.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "soldier.h"
#include "transport.h"
#include "tribe.h"
#include "widelands_map_data_packet_ids.h"
#include "widelands_map_bobdata_data_packet.h"
#include "worker.h"
#include "worker_program.h"
#include "error.h"

#define CURRENT_PACKET_VERSION 1

// Bob subtype versions
#define CRITTER_BOB_PACKET_VERSION 1
#define WORKER_BOB_PACKET_VERSION 1

// Worker subtype versions
#define SOLDIER_WORKER_BOB_PACKET_VERSION 1
#define CARRIER_WORKER_BOB_PACKET_VERSION 1

/*
 * Destructor
 */
Widelands_Map_Bobdata_Data_Packet::~Widelands_Map_Bobdata_Data_Packet(void) {
}

/*
 * Read Function
 */
void Widelands_Map_Bobdata_Data_Packet::Read(FileRead* fr, Editor_Game_Base* egbase, bool skip, Widelands_Map_Map_Object_Loader* ol) throw(wexception) {

   // First packet version
   int packet_version=fr->Unsigned16();

   if(packet_version==CURRENT_PACKET_VERSION) {
      // Now the rest data len
      uint len = fr->Unsigned32();
      if(skip) {
         // Skip the rest, flags are not our problem here
         fr->Data(len);
         return;
      }


      while(1) {
         uint reg=fr->Unsigned32();
         if(reg==0xffffffff) break; // No more bobs

         assert(ol->is_object_known(reg));
         Bob* bob=static_cast<Bob*>(ol->get_object_by_file_index(reg));

         uchar read_owner=fr->Unsigned8();
         Player* plr_owner=0;
         if(read_owner) { 
            plr_owner=egbase->get_safe_player(read_owner);
            assert(plr_owner); // He must be there
         }
         
         Coords pos;
         pos.x=fr->Unsigned16();
         pos.y=fr->Unsigned16();

         // Basic initialisation
         bob->set_owner(plr_owner);
         bob->set_position(egbase, pos);

         // Look if we had an transfer
         bool have_transfer=fr->Unsigned8();

         Transfer* trans=0;
         if(have_transfer) {
            trans=bob->m_stack[0].transfer;
            assert(trans);
         }

         /*         if(!have_transfer)
                    bob->reset_tasks(static_cast<Game*>(egbase));
                    */
         bob->m_actid=fr->Unsigned32();

         // Animation
         if(fr->Unsigned8()) { 
            bob->m_anim=bob->get_descr()->get_animation(fr->CString());
         } else 
            bob->m_anim=0;
         bob->m_animstart=fr->Signed32();

         // walking 
         bob->m_walking=(Map_Object::WalkingDir)fr->Signed32();
         bob->m_walkstart=fr->Signed32();
         bob->m_walkend=fr->Signed32();

         uint oldstacksize=bob->m_stack.size();
         bob->m_stack.resize(fr->Unsigned16());
         for(uint i=0; i<bob->m_stack.size(); i++) {
            Bob::State* s=&bob->m_stack[i];
            Bob::Task* task;

            // Task
            std::string taskname=fr->CString();
            if(taskname=="idle") task=&Bob::taskIdle; 
            else if(taskname=="movepath") task=&Bob::taskMovepath;  
            else if(taskname=="forcemove") task=&Bob::taskForcemove; 
            else if(taskname=="roam") task=&Critter_Bob::taskRoam; 
            else if(taskname=="program") {
               if(bob->get_bob_type()==Bob::WORKER) 
                  task=&Worker::taskProgram; 
               else
                  task=&Critter_Bob::taskProgram; 
            } else if(taskname=="transfer") task=&Worker::taskTransfer; 
            else if(taskname=="buildingwork") task=&Worker::taskBuildingwork; 
            else if(taskname=="return") task=&Worker::taskReturn; 
            else if(taskname=="gowarehouse") task=&Worker::taskGowarehouse; 
            else if(taskname=="dropoff") task=&Worker::taskDropoff; 
            else if(taskname=="fetchfromflag") task=&Worker::taskFetchfromflag; 
            else if(taskname=="waitforcapacity") task=&Worker::taskWaitforcapacity; 
            else if(taskname=="leavebuilding") task=&Worker::taskLeavebuilding; 
            else if(taskname=="fugitive") task=&Worker::taskFugitive; 
            else if(taskname=="geologist") task=&Worker::taskGeologist; 
            else if(taskname=="road") task=&Carrier::taskRoad; 
            else if(taskname=="transport") task=&Carrier::taskTransport; 
            else if(taskname=="") continue; // Skip task
            else 
               throw wexception("Unknown task %s in file!\n", taskname.c_str());

            s->task=task;

            s->ivar1=fr->Signed32();
            s->ivar2=fr->Signed32();
            s->ivar3=fr->Signed32();

            s->transfer=0;

            int obj=fr->Unsigned32();
            if(obj) {
               assert(ol->is_object_known(obj));
               s->objvar1=ol->get_object_by_file_index(obj);
            } else
               s->objvar1=0;
            s->svar1=fr->CString();
            s->coords.x=fr->Signed32();
            s->coords.y=fr->Signed32();

            bool diranims=fr->Unsigned8();
            if(diranims) {
               s->diranims=new DirAnimations();
               s->diranims->overwrite_animation(1, bob->get_descr()->get_animation(fr->CString()));
               s->diranims->overwrite_animation(2, bob->get_descr()->get_animation(fr->CString()));
               s->diranims->overwrite_animation(3, bob->get_descr()->get_animation(fr->CString()));
               s->diranims->overwrite_animation(4, bob->get_descr()->get_animation(fr->CString()));
               s->diranims->overwrite_animation(5, bob->get_descr()->get_animation(fr->CString()));
               s->diranims->overwrite_animation(6, bob->get_descr()->get_animation(fr->CString()));
            } else
               s->diranims=0;

            uint pathsteps=fr->Unsigned16();
            if(s->path && i<oldstacksize)
               delete s->path;
            if(pathsteps) {
               Coords start;
               start.x=fr->Unsigned16();
               start.y=fr->Unsigned16();
               Path* path=new Path(egbase->get_map(), start);
               for(uint i=0; i<pathsteps; i++)
                  path->append(fr->Unsigned8());
               s->path=path;
            } else
               s->path=0;

            if(s->transfer && i<oldstacksize && !trans)
               delete s->transfer;

            if(s->task==&Worker::taskGowarehouse || s->task==&Worker::taskTransfer) 
               s->transfer=trans;
            else
               s->transfer=0;

            bool route=fr->Unsigned8();
            if(s->route && i<oldstacksize)
               if(!route) 
                  delete s->route;
               else
                  s->route->clear();

            if(route) {
               Route* route;
               if(!s->route)
                  route=new Route();
               else 
                  route=s->route;
               route->m_totalcost=fr->Signed32();
               int nsteps=fr->Unsigned16();
               for(int i=0; i<nsteps; i++) {
                  int reg=fr->Unsigned32();
                  assert(ol->is_object_known(reg));
                  Flag* flag=static_cast<Flag*>(ol->get_object_by_file_index(reg));
                  route->m_route.push_back(flag);
               }
               s->route=route;
               route->verify(static_cast<Game*>(egbase));
            } else 
               s->route=0;

            // Now programm
            bool program=fr->Unsigned8();
            if(program) {
               std::string progname=fr->CString();
               if(bob->get_bob_type()==Bob::WORKER) 
                  s->program=static_cast<Worker*>(bob)->get_descr()->get_program(progname);
               else
                  s->program=static_cast<Critter_Bob*>(bob)->get_descr()->get_program(progname);
            } else 
               s->program=0;
         }

         // Rest of bob stuff
         bob->m_stack_dirty=fr->Unsigned8();
         bob->m_sched_init_task=fr->Unsigned8();
         bob->m_signal=fr->CString();

         switch(bob->get_bob_type()) {
            case Bob::CRITTER: read_critter_bob(fr,egbase,ol,static_cast<Critter_Bob*>(bob)); break;
            case Bob::WORKER: read_worker_bob(fr,egbase,ol,static_cast<Worker*>(bob)); break;
            default: throw wexception("Unknown sub bob type %i in Widelands_Map_Bobdata_Data_Packet::Read\n", bob->get_bob_type());
         }

         ol->mark_object_as_loaded(bob);
      }
      // DONE
      return;
   }
   throw wexception("Unknown version %i in Widelands_Map_Bobdata_Data_Packet!\n", packet_version);
}

void Widelands_Map_Bobdata_Data_Packet::read_critter_bob(FileRead* fr, Editor_Game_Base*, Widelands_Map_Map_Object_Loader*, Critter_Bob*) {
   int version=fr->Unsigned16();

   if(version==CRITTER_BOB_PACKET_VERSION) {
      // No data for critter bob currently
   } else 
      throw wexception("Unknown version %i in Critter Bob Subpacket!\n", version); 
}

void Widelands_Map_Bobdata_Data_Packet::read_worker_bob(FileRead* fr, Editor_Game_Base* egbase, Widelands_Map_Map_Object_Loader* ol, Worker* worker) {
   int version=fr->Unsigned16();

   if(version==WORKER_BOB_PACKET_VERSION) {
      switch(worker->get_worker_type()) {
         case Worker_Descr::NORMAL: break;
         case Worker_Descr::SOLDIER: 
         {
            int soldierversion=fr->Unsigned16();
            if(soldierversion==SOLDIER_WORKER_BOB_PACKET_VERSION) {
               Soldier* soldier=static_cast<Soldier*>(worker);

               soldier->m_hp_current=fr->Unsigned32();
               soldier->m_hp_max=fr->Unsigned32();
               soldier->m_min_attack=fr->Unsigned32();
               soldier->m_max_attack=fr->Unsigned32();
               soldier->m_defense=fr->Unsigned32();
               soldier->m_evade=fr->Unsigned32();
               soldier->m_hp_level=fr->Unsigned32();
               soldier->m_attack_level=fr->Unsigned32();
               soldier->m_defense_level=fr->Unsigned32();
               soldier->m_evade_level=fr->Unsigned32();
            } else
               throw wexception("Unknown version %i in Soldier Bob (SubSubPacket)!", soldierversion);
         }
         break;

         case Worker_Descr::CARRIER:
         {
            int carrierversion=fr->Unsigned16();
            if(carrierversion==CARRIER_WORKER_BOB_PACKET_VERSION) {
               Carrier* c=static_cast<Carrier*>(worker);
               c->m_acked_ware=fr->Signed32();         
            }
         }
         break;
         default: throw wexception("Unknown Worker %i in Widelands_Map_Bobdata_Data_Packet::read_worker_bob()\n", worker->get_worker_type());
      } 

      // location
      uint reg=fr->Unsigned32();
      if(reg) {
         assert(ol->is_object_known(reg));
         worker->set_location(static_cast<PlayerImmovable*>(ol->get_object_by_file_index(reg)));
         assert(worker->m_location.get(egbase));
      } else
         worker->m_location=0;

      
      // Carried item
      reg=fr->Unsigned32();
      if(reg) {
         assert(ol->is_object_known(reg));
         worker->m_carried_item=ol->get_object_by_file_index(reg);
      } else
         worker->m_carried_item=0;

      // Skip supply

      worker->m_needed_exp=fr->Signed32();
      worker->m_current_exp=fr->Signed32();

      Economy* eco=0;
      if(worker->m_location.get(egbase)) 
         eco=static_cast<PlayerImmovable*>(worker->m_location.get(egbase))->get_economy();
      
      worker->set_economy(eco);
      if(worker->m_carried_item.get(egbase))
         static_cast<WareInstance*>(worker->m_carried_item.get(egbase))->set_economy(eco);


   } else 
      throw wexception("Unknown version %i in Worker Bob Subpacket!\n", version); 
 
}

/*
 * Write Function
 */
void Widelands_Map_Bobdata_Data_Packet::Write(FileWrite* fw, Editor_Game_Base* egbase, Widelands_Map_Map_Object_Saver* os) throw(wexception) {
   // first of all the magic bytes
   fw->Unsigned16(PACKET_BOBDATA);

   // now packet version
   fw->Unsigned16(CURRENT_PACKET_VERSION);

   // Here we will insert skip data (packet lenght) 
   // later, write a dummy for know
   int filepos = fw->GetFilePos();
   fw->Unsigned32(0x00000000);
   fw->ResetByteCounter();
  
   Map* map=egbase->get_map();
   for(ushort y=0; y<map->get_height(); y++) {
      for(ushort x=0; x<map->get_width(); x++) {

         std::vector<Bob*> bobarr;

         egbase->get_map()->find_bobs(Coords(x,y), 0, &bobarr);

         for(uint i=0; i<bobarr.size(); i++) {
            Bob* ibob=bobarr[i];
            for(uint j=i; j<bobarr.size(); j++) {
               Bob* jbob=bobarr[j];
               if(ibob->get_file_serial() < jbob->get_file_serial()) {
                  bobarr[i] = jbob;
                  bobarr[j] = ibob;
                  ibob=jbob;
               } 
            }
         }

         for(uint i=0; i<bobarr.size(); i++) {
            Bob* bob=bobarr[i];
            assert(os->is_object_known(bob));
            uint reg=os->get_object_file_index(bob);

            fw->Unsigned32(reg);
            // BOB STUFF

            if(bob->m_owner) 
               fw->Unsigned8(bob->m_owner->get_player_number());
            else 
               fw->Unsigned8(0);

            // m_position
            fw->Unsigned16(bob->m_position.x);
            fw->Unsigned16(bob->m_position.y);
            // FIELD can't be saved

            // m_linknext, linkpprev are handled automatically

            // Are we currently transfering?
            if(bob->m_stack.size() && bob->m_stack[0].transfer) 
               fw->Unsigned8(1);
            else
               fw->Unsigned8(0);

            fw->Unsigned32(bob->m_actid);

            // Animation
            if(bob->m_anim) {
               fw->Unsigned8(1);
               fw->CString(bob->get_descr()->get_animation_name(bob->m_anim).c_str());
            } else 
               fw->Unsigned8(0);
            fw->Signed32(bob->m_animstart);

            // WalkingDirection
            fw->Signed32(bob->m_walking);

            // WalkingStart
            fw->Signed32(bob->m_walkstart);

            // WalkEnd
            fw->Signed32(bob->m_walkend);

            // Nr of States
            fw->Unsigned16(bob->m_stack.size());
            for(uint i=0; i<bob->m_stack.size(); i++) {
               Bob::State* s=&bob->m_stack[i];

               // Write name, enough to reconstruct the
               // Task structure
               fw->CString(s->task->name);

               fw->Signed32(s->ivar1);
               fw->Signed32(s->ivar2);
               fw->Signed32(s->ivar3);

               Map_Object* obj=s->objvar1.get(egbase);
               if(obj) {
                  assert(os->is_object_known(obj));
                  fw->Unsigned32(os->get_object_file_index(obj));
               } else 
                  fw->Unsigned32(0);

               fw->CString(s->svar1.c_str());

               // Coords
               fw->Signed32(s->coords.x);
               fw->Signed32(s->coords.y);

               if(s->diranims) {
                  fw->Unsigned8(1);
                  fw->CString(bob->get_descr()->get_animation_name(s->diranims->get_animation(1)).c_str());
                  fw->CString(bob->get_descr()->get_animation_name(s->diranims->get_animation(2)).c_str());
                  fw->CString(bob->get_descr()->get_animation_name(s->diranims->get_animation(3)).c_str());
                  fw->CString(bob->get_descr()->get_animation_name(s->diranims->get_animation(4)).c_str());
                  fw->CString(bob->get_descr()->get_animation_name(s->diranims->get_animation(5)).c_str());
                  fw->CString(bob->get_descr()->get_animation_name(s->diranims->get_animation(6)).c_str());
               } else 
                  fw->Unsigned8(0);

               // Path
               if(s->path) {
                  fw->Unsigned16(s->path->get_nsteps());
                  Coords pstart=s->path->get_start();
                  fw->Unsigned16(pstart.x);
                  fw->Unsigned16(pstart.y);
                  for(int i=0; i<s->path->get_nsteps(); i++) 
                     fw->Unsigned8(s->path->get_step(i));
               } else
                  fw->Unsigned16(0);

               // Route
               if(s->route) {
                  fw->Unsigned8(1);
                  fw->Signed32(s->route->get_totalcost());
                  fw->Unsigned16(s->route->get_nrsteps());
                  for(int i=0; i<s->route->get_nrsteps(); i++) {
                     Flag* f=s->route->get_flag(egbase, i);
                     assert(os->is_object_known(f));
                     fw->Unsigned32(os->get_object_file_index(f));
                  }
               } else 
                  fw->Unsigned8(0);

               // Programm
               if(s->program) {
                  fw->Unsigned8(1);
                  fw->CString(s->program->get_name().c_str());
               } else 
                  fw->Unsigned8(0);


            }

            fw->Unsigned8(bob->m_stack_dirty);
            fw->Unsigned8(bob->m_sched_init_task);
            fw->CString(bob->m_signal.c_str());

            switch(bob->get_bob_type()) {
               case Bob::CRITTER: write_critter_bob(fw,egbase,os,static_cast<Critter_Bob*>(bob)); break;
               case Bob::WORKER: write_worker_bob(fw,egbase,os,static_cast<Worker*>(bob)); break;
               default: throw wexception("Unknown sub bob type %i in Widelands_Map_Bobdata_Data_Packet::Write\n", bob->get_bob_type());

            }

            os->mark_object_as_saved(bob);
         }

      }
   }
   fw->Unsigned32(0xffffffff); // No more bobs
 
   // Now, write the packet length
   fw->Unsigned32(fw->GetByteCounter(), filepos);
   
   // DONE
}

void Widelands_Map_Bobdata_Data_Packet::write_critter_bob(FileWrite* fw, Editor_Game_Base* egbase, Widelands_Map_Map_Object_Saver* os, Critter_Bob* critter) {
   fw->Unsigned16(CRITTER_BOB_PACKET_VERSION);
}

void Widelands_Map_Bobdata_Data_Packet::write_worker_bob(FileWrite* fw, Editor_Game_Base* egbase, Widelands_Map_Map_Object_Saver* os, Worker* worker) {
   fw->Unsigned16(WORKER_BOB_PACKET_VERSION);

   switch(worker->get_worker_type()) {
      case Worker_Descr::NORMAL: break; 
      case Worker_Descr::SOLDIER:
      {
         fw->Unsigned16(SOLDIER_WORKER_BOB_PACKET_VERSION);
         Soldier* soldier=static_cast<Soldier*>(worker);

         fw->Unsigned32(soldier->m_hp_current);
         fw->Unsigned32(soldier->m_hp_max);
         fw->Unsigned32(soldier->m_min_attack);
         fw->Unsigned32(soldier->m_max_attack);
         fw->Unsigned32(soldier->m_defense);
         fw->Unsigned32(soldier->m_evade);
         fw->Unsigned32(soldier->m_hp_level);
         fw->Unsigned32(soldier->m_attack_level);
         fw->Unsigned32(soldier->m_defense_level);
         fw->Unsigned32(soldier->m_evade_level);
      }
      break;

      case Worker_Descr::CARRIER:
      {
         fw->Unsigned16(CARRIER_WORKER_BOB_PACKET_VERSION);
         Carrier* c=static_cast<Carrier*>(worker);

         fw->Signed32(c->m_acked_ware);
      }
      break;
      default: throw wexception("Unknown Worker %i in Widelands_Map_Bobdata_Data_Packet::write_worker_bob()\n", worker->get_worker_type());
   }                     

   // location
   Map_Object* loca=worker->m_location.get(egbase);
   if(loca) {
      assert(os->is_object_known(loca));
      fw->Unsigned32(os->get_object_file_index(loca));
   } else 
      fw->Unsigned32(0);

   // Economy is not our beer

   // Carried item
   Map_Object* carried_item=worker->m_carried_item.get(egbase);
   if(carried_item) {
      assert(os->is_object_known(carried_item));
      fw->Unsigned32(os->get_object_file_index(carried_item));
   } else
      fw->Unsigned32(0);

   /*This is not needed
    * Write if a idle supply is to be created
   if(worker->m_supply) 
      fw->Unsigned8(1);
   else
      fw->Unsigned8(0);
      */

   fw->Signed32(worker->m_needed_exp);
   fw->Signed32(worker->m_current_exp);
}
