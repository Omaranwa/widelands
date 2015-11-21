dirname = path.dirname(__file__)

animations = {
   idle = {
      template = "badger_idle_??",
      directory = dirname,
      hotspot = { 9, 12 },
      fps = 20,
   },
}
add_walking_animations(animations, dirname, "badger_walk", {13, 15}, 20)

world:new_critter_type{
   name = "badger",
   descname = _ "Badger",
   attributes = { "eatable" },
   programs = {
      remove = { "remove" },
   },
   animations = animations,
}
