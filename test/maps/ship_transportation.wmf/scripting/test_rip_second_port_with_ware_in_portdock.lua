run(function()
   sleep(100)
   game.desired_speed = 10 * 1000

   create_first_port()
   create_second_port()

   start_building_farm()
   port1():set_wares{
      blackwood = 1,
   }
   sleep(12000)

   -- The ship should not yet have picked up the worker from the
   -- portdock.
   assert_equal(p1:get_wares("blackwood"), 1)
   assert_equal(port1():get_wares("blackwood"), 0)

   port2():remove()
   sleep(100)

   stable_save("ware_in_portdock")

   sleep(8000)

   -- Worker should be back in port.
   assert_equal(p1:get_wares("blackwood"), 1)
   assert_equal(port1():get_wares("blackwood"), 1)

   -- Create port again.
   create_second_port()

   while ship:get_wares() == 0 do
      sleep(50)
   end

   print("# All Tests passed.")
   wl.ui.MapView():close()
end)
