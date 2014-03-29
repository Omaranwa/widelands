dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = path.glob(dirname, "badger_idle_e_\\d+.png"),
      player_color_masks = {},
      hotspot = { 9, 12 },
      fps = 20,
   },
}
add_walking_animations(animations, dirname, "badger_walk", {13, 15}, 20)

world:new_critter_type{
   name = "badger",
   descname = _ "Badger",
   swimming = false,
   attributes = { "eatable" },
   programs = {
      remove = { "remove" },
   },
   animations = animations,
}
