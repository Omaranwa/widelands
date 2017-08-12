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
--

tribes = wl.Tribes()
include "scripting/mapobjects.lua"

-- Load all init.lua files in the given table of directory names
function load_directories(directories)
   -- Helper function to check for file name endings
   function string.ends(haystack, needle)
      return needle == '' or string.sub(haystack, -string.len(needle)) == needle
   end

   while #directories > 0 do
      local filepath = directories[1]
      table.remove(directories, 1)
      if path.is_directory(filepath) then
         for idx, listed_path in ipairs(path.list_directory(filepath)) do
            if path.is_directory(listed_path) then
               table.insert(directories, listed_path)
            elseif string.ends(listed_path , "init.lua") then
               include(listed_path)
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

   print_loading_message("┃    Ships", function()
      load_directories({"tribes/ships"})
   end)

   -- ===================================
   --    Wares
   -- ===================================

   print_loading_message("┃    Wares", function()
      load_directories({"tribes/wares"})
   end)

   -- ===================================
   --    Immovables
   -- ===================================

   print_loading_message("┃    Immovables", function()
      load_directories({"tribes/immovables"})
   end)

   -- ===================================
   --    Workers, Carriers & Soldiers
   -- ===================================

   print_loading_message("┃    Workers", function()
      load_directories({"tribes/workers"})
   end)

   -- ===================================
   --    Warehouses
   -- ===================================

   print_loading_message("┃    Warehouses", function()
      load_directories({"tribes/buildings/warehouses"})
   end)

   -- ===================================
   --    Productionsites
   -- ===================================

   print_loading_message("┃    Productionsites", function()
      load_directories({"tribes/buildings/productionsites"})
   end)

   -- ===================================
   --    Trainingsites
   -- ===================================

   print_loading_message("┃    Trainingsites", function()
      load_directories({"tribes/buildings/trainingsites"})
   end)

   -- ===================================
   --    Militarysites
   -- ===================================

   print_loading_message("┃    Militarysites", function()
      load_directories({"tribes/buildings/militarysites"})
   end)

   -- ===================================
   --    Partially Finished Buildings
   -- ===================================

   print_loading_message("┃    Partially finished buildings", function()
      load_directories({"tribes/buildings/partially_finished"})
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
