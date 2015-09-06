dirname = path.dirname(__file__)

tribes:new_ware_type {
   msgctxt = "ware",
   name = "bread_paddle",
   -- TRANSLATORS: This is a ware name used in lists of wares
   descname = pgettext("ware", "Bread Paddle"),
   -- TRANSLATORS: mass description, e.g. 'The economy needs ...'
   genericname = pgettext("ware", "bread paddles"),
   directory = dirname,
   icon = dirname .. "menu.png",
   default_target_quantity = {
		atlanteans = 1,
		barbarians = 1,
		empire = 1
	},
   preciousness = {
		atlanteans = 0,
		barbarians = 0,
		empire = 0
	},

   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 6, 6 },
      },
   }
}
