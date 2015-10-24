-- This config file sets the order of the starting conditions
dirname = path.dirname(__file__)

return {
	dirname .. "collectors.lua",
	dirname .. "wood_gnome.lua",
	dirname .. "territorial_time.lua",
	dirname .. "territorial_lord.lua",
	dirname .. "defeat_all.lua",
	dirname .. "endless_game.lua",
	dirname .. "endless_game_fogless.lua",
        dirname .. "artifacts.lua",
}
