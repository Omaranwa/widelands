dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "bush5",
   descname = _ "Bush",
   editor_category = "plants",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         template = "idle",
         directory = dirname,
         hotspot = { 14, 13 },
      },
   }
}