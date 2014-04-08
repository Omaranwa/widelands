world = wl.World()

world:new_editor_category{
   name = "green",
   descname = _ "Summer",
   picture = "world/pics/editor_category_green.png",
}
world:new_editor_category{
   name = "wasteland",
   descname = _ "Wasteland",
   picture = "world/pics/editor_category_wasteland.png",
}
world:new_editor_category{
   name = "winter",
   descname = _ "Winter",
   picture = "world/pics/editor_category_winter.png",
}
world:new_editor_category{
   name = "desert",
   descname = _ "Desert",
   picture = "world/pics/editor_category_desert.png",
}

include "world/resources/init.lua"
include "world/terrains/init.lua"

include "world/immovables/grass1/init.lua"
include "world/immovables/manmade/snowman/init.lua"
include "world/immovables/skeleton1/init.lua"
include "world/immovables/skeleton2/init.lua"
include "world/immovables/skeleton3/init.lua"
include "world/immovables/skeleton4/init.lua"
include "world/immovables/trees/aspen_summer_old/init.lua"
include "world/immovables/trees/fallentree/init.lua"
include "world/immovables/trees/deadtree1/init.lua"
include "world/immovables/trees/deadtree2/init.lua"
include "world/immovables/trees/deadtree3/init.lua"
include "world/immovables/trees/deadtree4/init.lua"
include "world/immovables/trees/deadtree5/init.lua"
include "world/immovables/trees/deadtree6/init.lua"

-- Adds 6 animations for each walking direction into 'table'. The pictures are
-- searched for in 'dirname'. All files should look like this
-- 'basename'_(e|ne|se|sw|w|nw)_\d+.png. 'hotspot' is the hotspot for blitting.
function add_walking_animations(table, dirname, basename, hotspot, fps)
   for idx, dir in ipairs{ "ne", "e", "se", "sw", "w", "nw" } do
      table["walk_" .. dir] = {
         pictures = path.glob(dirname, basename .. "_" .. dir .. "_\\d+.png"),
         player_color_masks = {},
         hotspot = hotspot,
         fps = fps,
      }
   end
end

include "world/critters/badger/init.lua"
include "world/critters/brownbear/init.lua"
include "world/critters/bunny/init.lua"
include "world/critters/chamois/init.lua"
include "world/critters/deer/init.lua"
include "world/critters/duck/init.lua"
include "world/critters/elk/init.lua"
include "world/critters/fox/init.lua"
include "world/critters/lynx/init.lua"
include "world/critters/marten/init.lua"
include "world/critters/reindeer/init.lua"
include "world/critters/sheep/init.lua"
include "world/critters/stag/init.lua"
include "world/critters/wisent/init.lua"
include "world/critters/wolf/init.lua"
