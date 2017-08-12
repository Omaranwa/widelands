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
