dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "palm_date_desert_mature",
   descname = _ "Date Palm (Mature)",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 20000",
         "remove=30",
         "seed=palm_date_desert_sapling",
         "animate=idle 20000",
         "remove=20",
         "grow=palm_date_desert_old",
      },
   },
   animations = {
      idle = {
         pictures = path.glob(dirname, "idle_\\d+.png"),
         player_color_masks = {},
         hotspot = { 18, 48 },
         fps = 8,
         sfx = {},
      },
   },
}
--mountain4=            2
