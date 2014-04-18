dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "beech_summer_sapling",
   descname = _ "Beech (Sapling)",
   -- category = "trees_deciduous",
   size = "small",
   attributes = { "seed" },
   programs = {
      program = {
         "animate=idle 55000",
         "remove=35",
         "grow=beech_summer_pole",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 5, 12 },
         fps = 8,
      },
   },
}
