wl_library(map_io_map_loader
  SRCS
    map_loader.h
    widelands_map_loader.cc
    widelands_map_loader.h
    s2map.cc
    s2map.h
  DEPENDS
    base_exceptions
    base_i18n
    base_log
    base_macros
    base_scoped_timer
    io_fileread
    io_filesystem
    logic
    map_io
    scripting_lua_interface
)

# TODO(sirver): separate map_object_loader/saver into
# own library.
wl_library(map_io
  SRCS
    coords_profile.cc
    coords_profile.h
    tribes_legacy_lookup_table.cc
    tribes_legacy_lookup_table.h
    world_legacy_lookup_table.cc
    world_legacy_lookup_table.h
    map_allowed_building_types_packet.cc
    map_allowed_building_types_packet.h
    map_allowed_worker_types_packet.cc
    map_allowed_worker_types_packet.h
    map_bob_packet.cc
    map_bob_packet.h
    map_building_packet.cc
    map_building_packet.h
    map_buildingdata_packet.cc
    map_buildingdata_packet.h
    map_data_packet.h
    map_elemental_packet.cc
    map_elemental_packet.h
    map_exploration_packet.cc
    map_exploration_packet.h
    map_flag_packet.cc
    map_flag_packet.h
    map_flagdata_packet.cc
    map_flagdata_packet.h
    map_heights_packet.cc
    map_heights_packet.h
    map_images.cc
    map_images.h
    map_message_saver.h
    map_node_ownership_packet.cc
    map_node_ownership_packet.h
    map_object_loader.cc
    map_object_loader.h
    map_object_packet.cc
    map_object_packet.h
    map_object_saver.cc
    map_object_saver.h
    map_objective_packet.cc
    map_objective_packet.h
    map_player_names_and_tribes_packet.cc
    map_player_names_and_tribes_packet.h
    map_player_position_packet.cc
    map_player_position_packet.h
    map_players_messages_packet.cc
    map_players_messages_packet.h
    map_players_view_packet.cc
    map_players_view_packet.h
    map_port_spaces_packet.cc
    map_port_spaces_packet.h
    map_resources_packet.cc
    map_resources_packet.h
    map_road_packet.cc
    map_road_packet.h
    map_roaddata_packet.cc
    map_roaddata_packet.h
    map_saver.cc
    map_saver.h
    map_scripting_packet.cc
    map_scripting_packet.h
    map_terrain_packet.cc
    map_terrain_packet.h
    map_version_packet.cc
    map_version_packet.h
  DEPENDS
    base_exceptions
    base_log
    base_macros
    base_scoped_timer
    build_info
    economy
    graphic
    graphic_image_io
    graphic_minimap_renderer
    graphic_playercolor
    graphic_surface
    helper
    io_fileread
    io_filesystem
    logic
    logic_constants
    logic_widelands_geometry
    profile
    scripting_logic
)
