dirname = path.dirname(__file__)

tribes:new_immovable_type{
   name = "ashes",
   -- TRANSLATORS: This is an immovable name used in lists of immovables
   descname = _"Ashes",
   programs = {
		program = {
			"animate=idle 45000",
			"remove=",
      }
   },
   helptext = {
		default = ""
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 40, 39 },
      },
   }
}
