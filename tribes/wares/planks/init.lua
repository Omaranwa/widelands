dirname = path.dirname(__file__)

tribes:new_ware_type {
   msgctxt = "ware",
   name = "planks",
   -- TRANSLATORS: This is a ware name used in lists of wares
   descname = pgettext("ware", "Planks"),
   -- TRANSLATORS: mass description, e.g. 'The economy needs ...'
   genericname = pgettext("ware", "planks"),
   directory = dirname,
   icon = dirname .. "menu.png",
   default_target_quantity = {
		atlanteans = 40,
		empire = 40
	},
   preciousness = {
		atlanteans = 7,
		empire = 7
	},

   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 12, 17 },
      },
   }
}
