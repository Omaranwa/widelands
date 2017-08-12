-- RST
-- init.lua
-- -----------------
--
-- Tribes initialization
--
-- First the units are loaded, then the tribe descriptions.
--
-- All tribes also have some basic information for the load screens
-- and the editor in ``preload.lua``.
--
-- Basic load order (first wares, then immovables etc.) is important,
-- because checks will be made in C++.
-- Also, enhanced/upgraded units need to come before their basic units.
--

tribes = wl.Tribes()
include "scripting/mapobjects.lua"

-- Helper function to check for file name endings
function string.ends(haystack, needle)
   return needle == '' or string.sub(haystack, -string.len(needle)) == needle
end

-- Load all init.lua files in the given table of directory names
function load_directories(directories)
   -- NOCOM remove print output when everything works
   print("┃    LOADING: " .. directories[1])
   while #directories > 0 do
      local filepath = directories[1]
      table.remove(directories, 1)
      if path.is_directory(filepath) then
         local temp_dirs = path.list_directory(filepath)
         for idx, temp_path in ipairs(temp_dirs) do
            if path.is_directory(temp_path) then
               print("┃      Directory: " .. temp_path)
               table.insert(directories, temp_path)
            elseif string.ends(temp_path , "init.lua") then
               print("┃      File: " .. temp_path)
               include(temp_path)
            end
         end
      end
   end
end

print("┏━ Running Lua for tribes:")

print_loading_message("┗━ took", function()
   -- ===================================
   --    Ships
   -- ===================================

   print_loading_message("┃    Ships", function() load_directories({"tribes/ships"}) end)

   -- ===================================
   --    Wares
   -- ===================================

   print_loading_message("┃    Wares", function() load_directories({"tribes/wares"}) end)

   -- ===================================
   --    Immovables
   -- ===================================

   print_loading_message("┃    Immovables", function() load_directories({"tribes/immovables"}) end)

   -- ===================================
   --    Workers, Carriers & Soldiers
   -- ===================================

   print_loading_message("┃    Workers", function() load_directories({"tribes/workers"}) end)

   -- ===================================
   --    Warehouses
   -- ===================================

   print_loading_message("┃    Warehouses", function()
      include "tribes/buildings/warehouses/atlanteans/headquarters/init.lua"
      include "tribes/buildings/warehouses/atlanteans/port/init.lua"
      include "tribes/buildings/warehouses/atlanteans/warehouse/init.lua"
      include "tribes/buildings/warehouses/barbarians/headquarters/init.lua"
      include "tribes/buildings/warehouses/barbarians/headquarters_interim/init.lua"
      include "tribes/buildings/warehouses/barbarians/port/init.lua"
      include "tribes/buildings/warehouses/barbarians/warehouse/init.lua"
      include "tribes/buildings/warehouses/empire/headquarters/init.lua"
      include "tribes/buildings/warehouses/empire/headquarters_shipwreck/init.lua"
      include "tribes/buildings/warehouses/empire/port/init.lua"
      include "tribes/buildings/warehouses/empire/warehouse/init.lua"
   end)

   -- ===================================
   --    Productionsites
   -- ===================================

   print_loading_message("┃    Productionsites", function()
      -- Atlanteans small
      include "tribes/buildings/productionsites/atlanteans/quarry/init.lua"
      include "tribes/buildings/productionsites/atlanteans/woodcutters_house/init.lua"
      include "tribes/buildings/productionsites/atlanteans/foresters_house/init.lua"
      include "tribes/buildings/productionsites/atlanteans/fishers_house/init.lua"
      include "tribes/buildings/productionsites/atlanteans/fishbreeders_house/init.lua"
      include "tribes/buildings/productionsites/atlanteans/hunters_house/init.lua"
      include "tribes/buildings/productionsites/atlanteans/well/init.lua"
      include "tribes/buildings/productionsites/atlanteans/gold_spinning_mill/init.lua"
      include "tribes/buildings/productionsites/atlanteans/scouts_house/init.lua"
      -- Atlanteans medium
      include "tribes/buildings/productionsites/atlanteans/sawmill/init.lua"
      include "tribes/buildings/productionsites/atlanteans/smokery/init.lua"
      include "tribes/buildings/productionsites/atlanteans/mill/init.lua"
      include "tribes/buildings/productionsites/atlanteans/bakery/init.lua"
      include "tribes/buildings/productionsites/atlanteans/charcoal_kiln/init.lua"
      include "tribes/buildings/productionsites/atlanteans/smelting_works/init.lua"
      include "tribes/buildings/productionsites/atlanteans/shipyard/init.lua"
      include "tribes/buildings/productionsites/atlanteans/toolsmithy/init.lua"
      include "tribes/buildings/productionsites/atlanteans/weaponsmithy/init.lua"
      include "tribes/buildings/productionsites/atlanteans/armorsmithy/init.lua"
      include "tribes/buildings/productionsites/atlanteans/barracks/init.lua"

      -- Atlanteans big
      include "tribes/buildings/productionsites/atlanteans/horsefarm/init.lua"
      include "tribes/buildings/productionsites/atlanteans/farm/init.lua"
      include "tribes/buildings/productionsites/atlanteans/blackroot_farm/init.lua"
      include "tribes/buildings/productionsites/atlanteans/spiderfarm/init.lua"
      include "tribes/buildings/productionsites/atlanteans/weaving_mill/init.lua"

      -- Atlanteans mines
      include "tribes/buildings/productionsites/atlanteans/crystalmine/init.lua"
      include "tribes/buildings/productionsites/atlanteans/coalmine/init.lua"
      include "tribes/buildings/productionsites/atlanteans/ironmine/init.lua"
      include "tribes/buildings/productionsites/atlanteans/goldmine/init.lua"
      -- Barbarians small
      include "tribes/buildings/productionsites/barbarians/quarry/init.lua"
      include "tribes/buildings/productionsites/barbarians/lumberjacks_hut/init.lua"
      include "tribes/buildings/productionsites/barbarians/rangers_hut/init.lua"
      include "tribes/buildings/productionsites/barbarians/fishers_hut/init.lua"
      include "tribes/buildings/productionsites/barbarians/hunters_hut/init.lua"
      include "tribes/buildings/productionsites/barbarians/gamekeepers_hut/init.lua"
      include "tribes/buildings/productionsites/barbarians/well/init.lua"
      include "tribes/buildings/productionsites/barbarians/scouts_hut/init.lua"
      -- Barbarians medium
      include "tribes/buildings/productionsites/barbarians/wood_hardener/init.lua"
      include "tribes/buildings/productionsites/barbarians/lime_kiln/init.lua"
      include "tribes/buildings/productionsites/barbarians/reed_yard/init.lua"
      include "tribes/buildings/productionsites/barbarians/bakery/init.lua"
      include "tribes/buildings/productionsites/barbarians/brewery/init.lua"
      include "tribes/buildings/productionsites/barbarians/micro_brewery/init.lua"
      include "tribes/buildings/productionsites/barbarians/big_inn/init.lua"
      include "tribes/buildings/productionsites/barbarians/inn/init.lua"
      include "tribes/buildings/productionsites/barbarians/tavern/init.lua"
      include "tribes/buildings/productionsites/barbarians/charcoal_kiln/init.lua"
      include "tribes/buildings/productionsites/barbarians/smelting_works/init.lua"
      include "tribes/buildings/productionsites/barbarians/shipyard/init.lua"
      include "tribes/buildings/productionsites/barbarians/warmill/init.lua"
      include "tribes/buildings/productionsites/barbarians/ax_workshop/init.lua"
      include "tribes/buildings/productionsites/barbarians/metal_workshop/init.lua"
      include "tribes/buildings/productionsites/barbarians/barracks/init.lua"

      -- Barbarians big
      include "tribes/buildings/productionsites/barbarians/cattlefarm/init.lua"
      include "tribes/buildings/productionsites/barbarians/farm/init.lua"
      include "tribes/buildings/productionsites/barbarians/weaving_mill/init.lua"
      include "tribes/buildings/productionsites/barbarians/helmsmithy/init.lua"
      -- Barbarians mines
      include "tribes/buildings/productionsites/barbarians/granitemine/init.lua"
      include "tribes/buildings/productionsites/barbarians/coalmine_deeper/init.lua"
      include "tribes/buildings/productionsites/barbarians/coalmine_deep/init.lua"
      include "tribes/buildings/productionsites/barbarians/coalmine/init.lua"
      include "tribes/buildings/productionsites/barbarians/ironmine_deeper/init.lua"
      include "tribes/buildings/productionsites/barbarians/ironmine_deep/init.lua"
      include "tribes/buildings/productionsites/barbarians/ironmine/init.lua"
      include "tribes/buildings/productionsites/barbarians/goldmine_deeper/init.lua"
      include "tribes/buildings/productionsites/barbarians/goldmine_deep/init.lua"
      include "tribes/buildings/productionsites/barbarians/goldmine/init.lua"
      -- Empire small
      include "tribes/buildings/productionsites/empire/quarry/init.lua"
      include "tribes/buildings/productionsites/empire/lumberjacks_house/init.lua"
      include "tribes/buildings/productionsites/empire/foresters_house/init.lua"
      include "tribes/buildings/productionsites/empire/fishers_house/init.lua"
      include "tribes/buildings/productionsites/empire/hunters_house/init.lua"
      include "tribes/buildings/productionsites/empire/well/init.lua"
      include "tribes/buildings/productionsites/empire/scouts_house/init.lua"
      -- Empire medium
      include "tribes/buildings/productionsites/empire/stonemasons_house/init.lua"
      include "tribes/buildings/productionsites/empire/sawmill/init.lua"
      include "tribes/buildings/productionsites/empire/mill/init.lua"
      include "tribes/buildings/productionsites/empire/bakery/init.lua"
      include "tribes/buildings/productionsites/empire/brewery/init.lua"
      include "tribes/buildings/productionsites/empire/vineyard/init.lua"
      include "tribes/buildings/productionsites/empire/winery/init.lua"
      include "tribes/buildings/productionsites/empire/inn/init.lua"
      include "tribes/buildings/productionsites/empire/tavern/init.lua"
      include "tribes/buildings/productionsites/empire/charcoal_kiln/init.lua"
      include "tribes/buildings/productionsites/empire/smelting_works/init.lua"
      include "tribes/buildings/productionsites/empire/shipyard/init.lua"
      include "tribes/buildings/productionsites/empire/toolsmithy/init.lua"
      include "tribes/buildings/productionsites/empire/armorsmithy/init.lua"
      -- Empire big
      include "tribes/buildings/productionsites/empire/donkeyfarm/init.lua"
      include "tribes/buildings/productionsites/empire/farm/init.lua"
      include "tribes/buildings/productionsites/empire/piggery/init.lua"
      include "tribes/buildings/productionsites/empire/sheepfarm/init.lua"
      include "tribes/buildings/productionsites/empire/weaving_mill/init.lua"
      include "tribes/buildings/productionsites/empire/weaponsmithy/init.lua"
      include "tribes/buildings/productionsites/empire/barracks/init.lua"

      -- Empire mines
      include "tribes/buildings/productionsites/empire/coalmine_deep/init.lua"
      include "tribes/buildings/productionsites/empire/coalmine/init.lua"
      include "tribes/buildings/productionsites/empire/ironmine_deep/init.lua"
      include "tribes/buildings/productionsites/empire/ironmine/init.lua"
      include "tribes/buildings/productionsites/empire/marblemine_deep/init.lua"
      include "tribes/buildings/productionsites/empire/marblemine/init.lua"
      include "tribes/buildings/productionsites/empire/goldmine_deep/init.lua"
      include "tribes/buildings/productionsites/empire/goldmine/init.lua"
   end)

   -- ===================================
   --    Trainingsites
   -- ===================================

   print_loading_message("┃    Trainingsites", function()
      include "tribes/buildings/trainingsites/atlanteans/dungeon/init.lua"
      include "tribes/buildings/trainingsites/atlanteans/labyrinth/init.lua"
      include "tribes/buildings/trainingsites/barbarians/battlearena/init.lua"
      include "tribes/buildings/trainingsites/barbarians/trainingcamp/init.lua"
      include "tribes/buildings/trainingsites/empire/colosseum/init.lua"
      include "tribes/buildings/trainingsites/empire/arena/init.lua"
      include "tribes/buildings/trainingsites/empire/trainingcamp/init.lua"
   end)

   -- ===================================
   --    Militarysites
   -- ===================================

   print_loading_message("┃    Militarysites", function()
      include "tribes/buildings/militarysites/atlanteans/guardhouse/init.lua"
      include "tribes/buildings/militarysites/atlanteans/guardhall/init.lua"
      include "tribes/buildings/militarysites/atlanteans/tower_small/init.lua"
      include "tribes/buildings/militarysites/atlanteans/tower_high/init.lua"
      include "tribes/buildings/militarysites/atlanteans/tower/init.lua"
      include "tribes/buildings/militarysites/atlanteans/castle/init.lua"

      include "tribes/buildings/militarysites/barbarians/sentry/init.lua"
      include "tribes/buildings/militarysites/barbarians/barrier/init.lua"
      include "tribes/buildings/militarysites/barbarians/tower/init.lua"
      include "tribes/buildings/militarysites/barbarians/citadel/init.lua"
      include "tribes/buildings/militarysites/barbarians/fortress/init.lua"

      include "tribes/buildings/militarysites/empire/sentry/init.lua"
      include "tribes/buildings/militarysites/empire/blockhouse/init.lua"
      include "tribes/buildings/militarysites/empire/barrier/init.lua"
      include "tribes/buildings/militarysites/empire/outpost/init.lua"
      include "tribes/buildings/militarysites/empire/tower/init.lua"
      include "tribes/buildings/militarysites/empire/castle/init.lua"
      include "tribes/buildings/militarysites/empire/fortress/init.lua"
   end)

   -- ===================================
   --    Partially Finished Buildings
   -- ===================================

   print_loading_message("┃    Partially finished buildings", function()
      include "tribes/buildings/partially_finished/constructionsite/init.lua"
      include "tribes/buildings/partially_finished/dismantlesite/init.lua"
   end)

   -- ===================================
   --    Tribes
   -- ===================================
   print_loading_message("┃    Tribe infos", function()
      include "tribes/atlanteans.lua"
      include "tribes/barbarians.lua"
      include "tribes/empire.lua"
   end)
end)
