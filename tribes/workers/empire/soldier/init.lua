dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = { dirname .. "idle_\\d+.png" },
      hotspot = { 10, 36 },
      fps = 5
   },
   atk_ok_e = {
      pictures = { dirname .. "atk_ok_e_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 10
   },
   atk_fail_e = {
      pictures = { dirname .. "atk_fail_e_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 10
   },
   eva_ok_e = {
      pictures = { dirname .. "eva_ok_e_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 20
   },
   eva_fail_e = {
      pictures = { dirname .. "eva_fail_e_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 10
   },
   atk_ok_w = {
      pictures = { dirname .. "atk_ok_w_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 10
   },
   atk_fail_w = {
      pictures = { dirname .. "atk_fail_w_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 10
   },
   eva_ok_w = {
      pictures = { dirname .. "eva_ok_w_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 20
   },
   eva_fail_w = {
      pictures = { dirname .. "eva_fail_w_\\d+.png" },
      hotspot = { 30, 36 },
      fps = 10
   },
   die_w = {
      pictures = { dirname .. "die_\\d+.png" },
      hotspot = { 10, 36 },
      fps = 10
   },
   die_e = {
      pictures = { dirname .. "die_\\d+.png" },
      hotspot = { 10, 36 },
      fps = 10
   }
}
add_worker_animations(animations, "walk", dirname, "walk", {10, 36}, 10)


tribes:new_soldier_type {
   name = "empire_soldier",
   -- TRANSLATORS: This is a worker name used in lists of workers
   descname = _"Soldier",
   icon = dirname .. "menu.png",
   vision_range = 2,

   buildcost = {
		empire_carrier = 1,
		armor_helmet = 1,
		spear_wooden = 1
	},

	-- TRANSLATORS: Helptext for a worker: Soldier
   helptext = _"Defend and Conquer!",
   animations = animations,

	max_hp_level = 4,
	max_attack_level = 4,
	max_defense_level = 0,
	max_evade_level = 2,

	-- initial values and per level increasements
	hp = 13000,
	hp_incr_per_level = 2100,
	attack = {
		minimum = 1300,
		maximum = 1500
	},
	attack_incr_per_level = 800,
	defense = 5,
	defense_incr_per_level = 5,
	evade = 30,
	evade_incr_per_level = 16,

	hp_level_0_pic = dirname .. "hp_level0.png",
	hp_level_1_pic = dirname .. "hp_level1.png",
	hp_level_2_pic = dirname .. "hp_level2.png",
	hp_level_3_pic = dirname .. "hp_level3.png",
	hp_level_4_pic = dirname .. "hp_level4.png",
	evade_level_0_pic = dirname .. "evade_level0.png",
	evade_level_1_pic = dirname .. "evade_level1.png",
	evade_level_2_pic = dirname .. "evade_level2.png",
	attack_level_0_pic = dirname .. "attack_level0.png",
	attack_level_1_pic = dirname .. "attack_level1.png",
	attack_level_2_pic = dirname .. "attack_level2.png",
	attack_level_3_pic = dirname .. "attack_level3.png",
	attack_level_4_pic = dirname .. "attack_level4.png",
	defense_level_0_pic = dirname .. "defense_level0.png",

	-- Random animations for battle
	attack_success_w = {
		"atk_ok_w",
	},
	attack_success_e = {
		"atk_ok_e",
	},
	attack_failure_w = {
		"atk_fail_w",
	},
	attack_failure_e = {
		"atk_fail_e",
	},
	evade_success_w = {
		"eva_ok_w",
	},
	evade_success_e = {
		"eva_ok_e",
	}
	evade_failure_w = {
		"eva_fail_w",
	}
	evade_failure_e = {
		"eva_fail_e",
	}
	die_w = {
		"die_w",
	}
	die_e = {
		"die_e",
	}
}
