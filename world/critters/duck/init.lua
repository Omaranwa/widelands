dirname = path.dirname(__file__)

animations = {
   idle = {
      template = "duck_??",
      directory = dirname,
      sound_effect = {
         directory = dirname,
         name = "duck",
      },
      hotspot = { 5, 7 },
      fps = 4,
   },
}
add_walking_animations(animations, dirname, "duck_walk", {5, 10})

world:new_critter_type{
   name = "duck",
   descname = _ "Duck",
   attributes = { "swimming" },
   programs = {
      remove = { "remove" },
   },
   animations = animations,
}