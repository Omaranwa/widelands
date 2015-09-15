dirname = path.dirname(__file__)

tribes:new_productionsite_type {
   msgctxt = "barbarians_building",
   name = "barbarians_granitemine",
   -- TRANSLATORS: This is a building name used in lists of buildings
   descname = pgettext("barbarians_building", "Granite Mine"),
   directory = dirname,
   icon = dirname .. "menu.png",
   size = "mine",

   buildcost = {
		log = 4,
		granite = 2
	},
	return_on_dismantle = {
		log = 2,
		granite = 1
	},

   animations = {
		idle = {
			pictures = path.list_directory(dirname, "idle_\\d+.png"),
			hotspot = { 42, 35 },
		},
		build = {
			pictures = path.list_directory(dirname, "build_\\d+.png"),
			hotspot = { 42, 35 },
		},
		working = {
			pictures = path.list_directory(dirname, "working_\\d+.png"),
			hotspot = { 42, 35 },
		},
		empty = {
			pictures = path.list_directory(dirname, "empty_\\d+.png"),
			hotspot = { 42, 35 },
		},
	},

   aihints = {
		mines = "stones",
		prohibited_till = 900
   },

	working_positions = {
		barbarians_miner = 1
	},

   inputs = {
		ration = 8
	},
   outputs = {
		"granite"
   },

	programs = {
		work = {
			-- TRANSLATORS: Completed/Skipped/Did not start mining granite because ...
			descname = _"mining granite",
			actions = {
				"sleep=20000",
				"return=skipped unless economy needs granite",
				"consume=ration",
				"animate=working 20000",
				"mine=stones 2 100 5 17",
				"produce=granite:2"
			}
		},
	},
	out_of_resource_notification = {
		title = _"Main Granite Vein Exhausted",
		message =
			_"This granite mine’s main vein is exhausted. Expect strongly diminished returns on investment." .. " " ..
			-- TRANSLATORS: "it" is a mine.
			_" This mine can’t be enhanced any further, so you should consider dismantling or destroying it.",
	},
}
