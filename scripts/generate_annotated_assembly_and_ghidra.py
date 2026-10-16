import os
import re
import sys

# Complete Dictionary of all 182 functions
ANNOTATIONS = {
    # Startup / Core / System
    "1000:0064": ("entry", "System / Startup", "Main game executable entry point. Initializes CPU speed calibration, checks for EGA with 256K, initializes video mode 0Dh, sets up interrupt handlers, and transfers execution to main dispatcher."),
    "1000:015d": ("vid_set_ega_palette", "Video / Palette", "Waits for EGA vertical retrace and writes 16 color values to the EGA Attribute Controller palette registers (Ports 0x3da / 0x3c0)."),
    "1000:0185": ("vid_fade_screen", "Video / Palette", "Cycles/fades the EGA palette out to black over a sequence of frame delays for smooth screen transitions."),
    "1000:0202": ("sys_fatal_error", "System / Error", "Restores text video mode, prints the fatal error string at DS:0x0046 using DOS INT 21h AH=09h, and exits to DOS."),
    "1000:0215": ("sys_exit_to_dos", "System / Lifecycle", "Restores original interrupt vectors, cleans up video mode, and terminates the program via DOS INT 21h AH=4Ch."),
    "1000:022b": ("vid_set_active_page_0", "Video / Paging", "Sets active EGA display page to page 0 (offset 0x0000 in VRAM A000:0000)."),
    "1000:0238": ("vid_set_active_page_1", "Video / Paging", "Sets active EGA display page to page 1 (offset 0x8000 in VRAM A000:0000) for double buffering."),
    "1000:027a": ("timer_delay_ticks", "System / Timing", "Synchronous frame delay routine. Decrements AX against the hardware timer tick flag updated by INT 08h ISR."),
    "1000:02b4": ("vid_init_screen_pages", "Video / Paging", "Initializes both EGA video pages (0 and 1), clears screen memory, and sets up active palette."),
    "1000:0599": ("sys_restore_interrupts", "System / Interrupts", "Restores original BIOS and DOS interrupt vectors (INT 08h, 09h, 15h, 23h, 03h) before exit."),
    "1000:05c2": ("isr_keyboard_int09", "Interrupts / Input", "Custom hardware INT 09h Keyboard ISR. Reads raw scancodes from port 0x60, manages key press/release state matrix, and signals EOI to PIC."),
    "1000:067c": ("isr_ctrl_break_int23", "Interrupts / Lifecycle", "Custom INT 23h DOS Ctrl-C / Ctrl-Break handler. Prevents abnormal exit and jumps to clean exit routine."),
    "1000:0683": ("isr_timer_sound_int08", "Interrupts / Audio", "Custom hardware INT 08h Timer ISR. Drives the PC speaker sound synthesis engine, decrements note durations, updates tone frequencies, and ticks frame counters."),
    "1000:08c7": ("snd_api_int03", "Audio / API", "Sound Engine API dispatcher hooked to INT 03h. Accepts commands: AL=0 (stop sound), AL=1 (play SFX at BX with priority CL), AL=2 (query/stop music), AL=3 (toggle sound)."),
    "1000:093a": ("isr_joystick_int15", "Interrupts / Input", "Custom BIOS INT 15h AH=84h Joystick handler. Directly polls game port 0x201 for button states and axis potentiometer discharge timing."),
    "1000:09c9": ("sys_install_interrupts", "System / Interrupts", "Installs custom interrupt handlers by hooking interrupt vectors for INT 08h, INT 09h, INT 15h, INT 23h, and INT 03h."),

    # Sprite Rendering & Room Tilemap Setup
    "1000:18e0": ("spr_render_player", "Sprites / Player", "Renders Captain Comic sprite at coordinates (X, Y) using planar EGA masking, selected animation frame, and directional facing."),
    "1000:198e": ("map_init_tileset", "Tilemap / Level", "Initializes room tile attributes, copies tileset graphics into offscreen EGA memory, and configures tile properties."),
    "1000:1a63": ("map_spawn_room_entities", "Entities / Spawning", "Parses room entity definitions from file buffer and initializes active monster, item, and hazard slots."),
    "1000:1b8d": ("map_load_room", "Tilemap / Streaming", "Loads room data package for specified room index: streams layout (.0), collision masks (.1), tileset (.2), and entities (.3)."),
    "1000:1bfb": ("util_calc_stride", "Math / Geometry", "Calculates byte offset within display memory from tile grid coordinates."),
    "1000:1c0b": ("map_parse_room_header", "Tilemap / Streaming", "Validates room data file format, parses room dimensions, and sets up boundary coordinates."),
    "1000:1cfe": ("map_get_tile_at", "Tilemap / Collision", "Looks up the tile index and collision flags at specified tile coordinate (X, Y)."),
    "1000:1d2c": ("map_draw_background", "Tilemap / Rendering", "Draws complete room background tilemap to offscreen EGA video buffer."),
    "1000:1dc0": ("map_draw_tile_column", "Tilemap / Rendering", "Draws a single vertical column of 16x16 tiles during horizontal scrolling."),
    "1000:1e09": ("map_draw_tile_row", "Tilemap / Rendering", "Draws a single horizontal row of 16x16 tiles during vertical scrolling."),
    "1000:1e86": ("map_render_full_screen", "Tilemap / Rendering", "Renders full visible screen grid of tiles to offscreen video page."),
    "1000:1f72": ("map_set_tile_attr", "Tilemap / Flags", "Sets tile attribute and collision flags in the active room collision buffer."),
    "1000:1f89": ("map_clear_tile_attr", "Tilemap / Flags", "Clears tile attribute and collision flags in the active room collision buffer."),

    # File I/O & Decompression
    "1000:1fa4": ("file_load_pack_rle", "File I/O / Compression", "Opens file via DOS INT 21h AH=3Dh and decompresses Byte-Run / PackBits RLE compressed data into destination buffer."),
    "1000:1ffe": ("file_load_raw_mask", "File I/O / Assets", "Reads raw collision mask table (.1) from disk into room collision buffer."),
    "1000:2022": ("file_load_tileset", "File I/O / Assets", "Reads raw tile graphic bitmap data (.2) from disk into tileset buffer."),
    "1000:2049": ("file_load_entity_data", "File I/O / Assets", "Reads room item/entity spawn parameters (.3) from disk."),
    "1000:2072": ("ent_init_monsters", "Entities / Management", "Initializes monster state machines, animation timers, and patrol bounds for the loaded room."),

    # Game Lifecycle & Transitions
    "1000:20fe": ("game_start_room", "Game / Lifecycle", "Initializes Comic at room spawn coordinates, loads room assets, and enters main game loop."),
    "1000:21b8": ("game_respawn_player", "Game / Lifecycle", "Handles player death, decrements lives counter, checks for Game Over, or restarts player at room spawn point."),
    "1000:21f2": ("game_victory_sequence", "Game / Sequence", "Handles the end-game victory sequence, awards bonus points, and displays the ending cutscene."),
    "1000:2205": ("game_over_sequence", "Game / Sequence", "Handles Game Over sequence, plays game over jingle, and transitions to high score entry."),
    "1000:2253": ("game_switch_room", "Game / Streaming", "Processes room transition doorways, saves room state, loads destination room, and spawns player."),
    "1000:227b": ("spr_animate_player", "Sprites / Player", "Updates Comic's walking, jumping, and shooting animation frame counters based on velocity."),
    "1000:2323": ("spr_set_player_frame", "Sprites / Player", "Sets specific sprite frame index for Comic based on current stance and facing direction."),

    # Main Game Loop & Player Physics / Collision
    "1000:2341": ("game_main_loop", "Game / Core", "Master gameplay loop. Polls user input, updates physics, checks collisions, updates monsters/projectiles, blits sprites, flips video pages, and synchronizes frames."),
    "1000:2a10": ("player_update_physics", "Player / Physics", "Processes player horizontal velocity, acceleration, jump mechanics, gravity, ladder climbing, and swimming."),
    "1000:2b8e": ("player_check_ground_collision", "Player / Collision", "Checks for solid ground tiles below player feet to handle landing and ground movement."),
    "1000:2bdc": ("player_check_ceiling_collision", "Player / Collision", "Detects collisions with ceiling tiles above Comic's head during jumps."),
    "1000:2bf7": ("player_check_wall_collision_left", "Player / Collision", "Detects collisions with solid walls to Comic's left side."),
    "1000:2c32": ("player_stop_x", "Player / Physics", "Halts horizontal velocity when walking into an obstruction."),
    "1000:2c39": ("player_check_wall_collision_right", "Player / Collision", "Detects collisions with solid walls to Comic's right side."),
    "1000:2c54": ("player_check_climbing", "Player / Physics", "Checks if Comic is on a ladder tile and processes vertical climbing input."),
    "1000:2c9d": ("player_check_swimming", "Player / Physics", "Checks for water tiles; applies fluid buoyancy, swimming controls, and checks for underwater boots."),
    "1000:2d06": ("player_move_left", "Player / Movement", "Applies leftward acceleration to player and sets facing direction to left."),
    "1000:2d31": ("player_move_right", "Player / Movement", "Applies rightward acceleration to player and sets facing direction to right."),
    "1000:2d61": ("player_do_jump", "Player / Physics", "Initiates jump state, applies vertical impulse (enhanced if jump boots are held), and plays jump sound."),
    "1000:2dc6": ("player_stop_jump", "Player / Physics", "Trims jump velocity when jump key is released early for variable jump height."),
    "1000:2edc": ("player_process_controls", "Player / Input", "Translates raw keyboard/joystick input vectors into player movement, jumping, and shooting actions."),
    "1000:2fee": ("player_take_damage", "Player / Health", "Deducts shield points, sets invulnerability timer, plays damage sound effect, and triggers death if shields reach zero."),
    "1000:3019": ("player_add_health", "Player / Health", "Increases player shield points up to maximum capacity and plays recharge sound."),
    "1000:3026": ("player_check_hazard_collision", "Player / Collision", "Checks player bounding box against environmental hazards (spikes, lava, acid)."),
    "1000:3057": ("player_zero_score_delta", "Player / Score", "Resets per-frame score delta accumulator."),
    "1000:305e": ("player_check_crush", "Player / Collision", "Checks if Comic is crushed between moving platforms or solid blocks."),
    "1000:309d": ("player_fall_death", "Player / Lifecycle", "Handles player falling into bottomless pit / lethal hazard."),

    # Weapons & Projectiles
    "1000:30c6": ("player_fire_projectile", "Weapons / Fire", "Spawns player blast fireball, checks weapon power level (normal, corkscrew, fireball), deducts ammo/power, and plays sound."),
    "1000:3107": ("proj_init_trajectory", "Weapons / Projectiles", "Calculates initial velocity vector, sprite angle, and lifetime for spawned projectile."),
    "1000:3181": ("proj_spawn_burst", "Weapons / Projectiles", "Spawns multiple projectile burst when upgraded weapons (corkscrew / spread blast) are active."),
    "1000:31eb": ("proj_update_all", "Weapons / Projectiles", "Updates all active player projectiles: advances position, checks tile collision, and triggers explosions."),
    "1000:327d": ("proj_check_monster_hit", "Weapons / Collision", "Tests intersection between player projectiles and monster bounding boxes; damages monsters."),

    # Monsters & Enemy AI
    "1000:330f": ("ent_update_all", "Entities / AI", "Iterates through all active monsters: updates movement AI, animation frames, and tests contact with Comic."),
    "1000:3468": ("ent_spawn_enemy", "Entities / Spawning", "Spawns a new monster into an available entity slot from room spawn tables."),
    "1000:34a9": ("ent_update_ai", "Entities / AI", "Processes AI behavioral patterns: patrollers, jumpers, fliers, projectiles, and boss attacks."),
    "1000:35de": ("ent_check_player_contact", "Entities / Collision", "Tests collision between player bounding box and enemy body; deals damage on contact."),
    "1000:3610": ("ent_destroy", "Entities / Lifecycle", "Destroys enemy, spawns explosion effect, plays destruction sound, and adds points to score."),
    "1000:362a": ("ent_check_tile_collision", "Entities / Collision", "Checks monster collision against solid walls and platforms in the room."),
    "1000:3657": ("ent_turn_around", "Entities / AI", "Reverses monster movement direction upon reaching platform edges or walls."),

    # Items & Inventory
    "1000:36c0": ("item_collect_pickup", "Items / Pickups", "Handles item collection: gems, door keys, teleport wand, boots, lantern, potion, and treasures."),
    "1000:378e": ("hud_render_all", "HUD / Display", "Draws complete on-screen HUD: score digits, shield bar, extra lives, active weapon, keys, and inventory items."),
    "1000:385f": ("hud_update_score", "HUD / Score", "Adds points to total score, handles high score check, and triggers score display redraw."),
    "1000:39ad": ("hud_draw_lives", "HUD / Display", "Renders Comic head icons representing remaining lives."),
    "1000:3a17": ("hud_draw_item_icon", "HUD / Display", "Draws specific item graphic icon into inventory box."),
    "1000:3a5f": ("hud_draw_shield_bar", "HUD / Display", "Draws health/shield energy meter using color-coded bar segments."),
    "1000:3a92": ("hud_draw_keys", "HUD / Display", "Draws collected key icons in HUD inventory bar."),
    "1000:3aa0": ("hud_draw_digits", "HUD / Display", "Renders decimal digit string on screen using custom EGA numeric font."),
    "1000:3ace": ("hud_draw_number_word", "HUD / Display", "Converts 16-bit word value into decimal representation and draws it."),
    "1000:3b12": ("hud_draw_char", "HUD / Display", "Draws single 8x8 font glyph to HUD."),
    "1000:3b51": ("hud_flash_item", "HUD / Animation", "Animates flashing highlight around newly acquired inventory item."),

    # Sound FX & Music
    "1000:3bc6": ("snd_play_pickup", "Audio / SFX", "Plays item pickup sound effect via PC speaker."),
    "1000:3c13": ("snd_play_blast", "Audio / SFX", "Plays laser/fireball blasting sound effect."),
    "1000:3cb1": ("snd_play_hit", "Audio / SFX", "Plays enemy hit / player damage sound effect."),
    "1000:3db6": ("snd_play_jump", "Audio / SFX", "Plays jump sound effect."),
    "1000:3dfb": ("spr_render_explosion", "Sprites / Animation", "Animates explosion particle burst when enemies or hazards are destroyed."),
    "1000:3e1f": ("snd_play_door_open", "Audio / SFX", "Plays door opening / key unlock sound effect."),
    "1000:3e98": ("snd_play_teleport", "Audio / SFX", "Plays teleportation portal sound effect."),
    "1000:3f0a": ("snd_play_level_music", "Audio / Music", "Initializes background theme tune for current game zone."),
    "1000:401b": ("math_random_byte", "Math / PRNG", "Generates pseudo-random byte using linear congruential algorithm."),
    "1000:4073": ("math_calc_distance", "Math / Geometry", "Calculates Manhattan/Euclidean distance approximation between two entity positions."),
    "1000:409c": ("boss_update_state", "Boss / AI", "Updates boss state machine, attack phases, invulnerability periods, and health."),

    # UI Windows & Text Rendering
    "1000:4206": ("ui_draw_window_box", "UI / Windows", "Draws bordered UI dialog window with drop shadow and title header."),
    "1000:42d4": ("ui_clear_window", "UI / Windows", "Clears interior area of dialog box with background color."),
    "1000:42e8": ("ui_draw_window_border", "UI / Windows", "Blits decorative border tiles and corner pieces around dialog box."),
    "1000:4339": ("vid_set_write_mode_0", "Video / Registers", "Configures EGA Graphics Controller to Write Mode 0 for standard memory writes."),
    "1000:437b": ("ui_draw_text_string", "UI / Text", "Blits text string to EGA display buffer at pixel coordinate (X, Y) with color attribute."),
    "1000:451c": ("ui_draw_dialog_message", "UI / Windows", "Displays centered dialog box with text prompt and waits for user keypress."),
    "1000:4579": ("ui_erase_dialog", "UI / Windows", "Restores original background graphics obscured by a temporary dialog box."),
    "1000:45a6": ("snd_play_dialog_beep", "Audio / SFX", "Plays UI cursor navigation beep sound."),
    "1000:46e6": ("game_pause_handler", "Game / State", "Handles Game Pause state: displays 'GAME PAUSED' banner, pauses timers, and polls Q key to quit."),

    # Specialized Entity & Boss Logic
    "1000:4870": ("snd_play_chest_open", "Audio / SFX", "Plays treasure chest unlock sound effect."),
    "1000:489e": ("item_check_has_item", "Items / Inventory", "Checks if specified item flag is set in player's inventory bitmask."),
    "1000:48aa": ("item_add_to_inventory", "Items / Inventory", "Sets item flag in player inventory bitmask and triggers item pickup notification."),
    "1000:48d4": ("item_remove_from_inventory", "Items / Inventory", "Clears item flag from player inventory bitmask upon item consumption."),
    "1000:49ad": ("spr_render_monster", "Sprites / Monsters", "Renders monster sprite with planar EGA color plane writes and transparency masking."),
    "1000:4a70": ("spr_get_monster_frame", "Sprites / Animation", "Looks up graphic offset for monster animation frame from sprite catalog."),
    "1000:4b25": ("vid_flip_buffers", "Video / Double Buffering", "Toggles active and visible video page offsets for flicker-free double buffering."),
    "1000:4b46": ("spr_blit_masked", "Video / Sprites", "Low-level masked EGA sprite blitter using latch registers and bitmasking."),
    "1000:4c06": ("math_clamp_coords", "Math / Geometry", "Clamps entity X/Y coordinates to stay within room bounding box."),
    "1000:4c25": ("map_scroll_camera", "Tilemap / Camera", "Calculates camera scroll offsets relative to player position to keep Comic on screen."),
    "1000:4cc1": ("map_check_room_bounds", "Tilemap / Streaming", "Tests if player has crossed room boundaries to trigger screen transition."),
    "1000:4d41": ("boss_render_sprites", "Boss / Rendering", "Renders multi-segment animated boss monster graphics."),
    "1000:4f15": ("ent_render_all", "Entities / Rendering", "Iterates through all active monsters, items, and projectiles, rendering their sprites."),
    "1000:5090": ("spr_render_water_ripple", "Tilemap / Animation", "Renders animated surface ripple effect over water tiles."),
    "1000:5ceb": ("map_update_animated_tiles", "Tilemap / Animation", "Advances animation frame counters for animated room tiles (water, lava, torches)."),
    "1000:5d5f": ("map_redraw_dirty_tiles", "Tilemap / Rendering", "Redraws tiles marked dirty to restore background behind moving sprites."),

    # Game Menus, File Saving, and Protection
    "1000:5e52": ("sys_main_dispatcher", "System / Dispatcher", "Top-level game dispatcher. Initializes PRNG from DOS time, loads configuration, checks high scores, and runs main menu loop."),
    "1000:61c8": ("input_poll_devices", "Input / Polling", "Polls keyboard buffer and joystick states to update the current input vector."),
    "1000:62a4": ("file_load_keys_def", "File I/O / Config", "Loads custom key configuration bindings from 'KEYS.DEF' file."),
    "1000:631a": ("file_save_high_scores", "File I/O / High Scores", "Saves high score table entries to disk."),
    "1000:633c": ("file_load_high_scores", "File I/O / High Scores", "Loads high score table entries from disk."),
    "1000:6545": ("sys_generate_system_hash", "System / Security", "Hashes VGA/System BIOS ROM bytes to generate system signature for copy protection and seed PRNG."),
    "1000:65ef": ("file_check_config_exists", "File I/O / Config", "Checks if configuration file 'FR.CFG' exists on disk."),
    "1000:660b": ("file_save_config", "File I/O / Config", "Writes audio, input, and video configuration settings to 'FR.CFG'."),
    "1000:6668": ("game_save_load_menu", "Game / Saves", "Save/Load game slot interface. Handles saving and loading games to 'GAME.000' through 'GAME.003'."),
    "1000:677a": ("ui_draw_slot_info", "UI / Saves", "Displays metadata for a saved game slot (score, level, items)."),
    "1000:6806": ("game_execute_script", "Game / Scripting", "Interprets cutscene and scripted event commands during story sequences."),
    "1000:6da4": ("ui_prompt_yes_no", "UI / Dialogs", "Displays Yes/No prompt dialog box and waits for user confirmation."),
    "1000:6de7": ("snd_play_cursor_move", "Audio / SFX", "Plays menu cursor navigation tick sound."),
    "1000:6df4": ("ui_main_menu", "UI / Menu", "Main title menu loop: displays options (Play Game, Instructions, Story, Demo, High Scores, Key Config, Exit)."),
    "1000:6f47": ("ui_high_scores_screen", "UI / High Scores", "Renders the Hall of Fame high scores screen."),
    "1000:7057": ("ui_instructions_screen", "UI / Instructions", "Displays scrolling gameplay instructions, item descriptions, and key controls."),
    "1000:7164": ("ui_story_screen", "UI / Story", "Presents illustrated storyline cutscene sequence explaining Captain Comic's mission on Planet Ogron."),
    "1000:71fe": ("ui_draw_story_page_1", "UI / Story", "Renders page 1 of story prologue artwork and narrative."),
    "1000:721b": ("ui_draw_story_page_2", "UI / Story", "Renders page 2 of story prologue artwork and narrative."),
    "1000:7238": ("ui_draw_story_page_3", "UI / Story", "Renders page 3 of story prologue artwork and narrative."),
    "1000:7255": ("ui_wait_for_keypress", "UI / Input", "Waits for user keypress or timer timeout before advancing text screen."),
    "1000:7261": ("ui_fade_in_story_page", "UI / Palette", "Fades in palette for story illustration."),
    "1000:7278": ("ui_wait_retrace_ticks", "Video / Timing", "Delays execution for specified number of EGA vertical retraces."),
    "1000:7298": ("ui_title_screen", "UI / Title", "Displays Captain Comic II: Fractured Reality title screen logo, artwork, and copyright notices."),
    "1000:74e1": ("ui_draw_title_stars", "UI / Title", "Renders animated twinkling starfield background on title screen."),
    "1000:752a": ("ui_update_title_star", "UI / Title", "Updates brightness and position of an individual star in the starfield."),
    "1000:7553": ("math_lfsr_random", "Math / PRNG", "Generates pseudo-random number using Linear Feedback Shift Register."),
    "1000:75ad": ("ui_draw_title_logo", "UI / Title", "Draws multicolor Captain Comic II title logo onto video buffer."),
    "1000:7622": ("ui_clear_title_logo", "UI / Title", "Erases title logo graphic area."),

    # Low-level EGA Hardware Registers & Planar Blitting
    "1000:7741": ("vid_init_ega_regs", "Video / Hardware", "Initializes EGA hardware registers (Sequencer, Graphics Controller, CRTC) to 320x200 16-color mode."),
    "1000:774e": ("vid_wait_vblank", "Video / Hardware", "Polls Port 0x3da bit 3 until vertical retrace blanking interval begins."),
    "1000:7765": ("vid_wait_not_vblank", "Video / Hardware", "Polls Port 0x3da bit 3 until vertical retrace blanking interval ends."),
    "1000:77a3": ("vid_clear_page", "Video / Hardware", "Clears 32KB EGA video page with zero bytes across all 4 planes."),
    "1000:77f2": ("vid_fill_plane", "Video / Hardware", "Fills specific EGA bitplane with pattern byte."),
    "1000:7833": ("vid_set_read_plane", "Video / Hardware", "Configures EGA Read Map Select register (Port 0x3ce index 4)."),
    "1000:7849": ("vid_copy_page", "Video / Hardware", "Fast VRAM-to-VRAM page copy using EGA internal latch registers."),
    "1000:78c6": ("vid_set_map_mask", "Video / Hardware", "Sets EGA Sequencer Map Mask register (Port 0x3c4 index 2) to select plane write enables."),
    "1000:78e1": ("vid_set_bit_mask", "Video / Hardware", "Sets EGA Graphics Controller Bit Mask register (Port 0x3ce index 8)."),
    "1000:790c": ("vid_set_write_mode_2", "Video / Hardware", "Configures EGA Graphics Controller to Write Mode 2 for color-per-pixel blits."),
    "1000:7927": ("vid_restore_write_mode", "Video / Hardware", "Restores EGA Graphics Controller to Write Mode 0."),
    "1000:7962": ("vid_set_color_compare", "Video / Hardware", "Configures EGA Color Compare register (Port 0x3ce index 2)."),
    "1000:7990": ("vid_load_default_palette", "Video / Palette", "Loads standard outdoor 16-color EGA palette."),
    "1000:7999": ("vid_load_alt_palette", "Video / Palette", "Loads alternate cavern/underground 16-color EGA palette."),
    "1000:79a2": ("vid_set_overscan_color", "Video / Palette", "Sets screen border/overscan color via BIOS INT 10h AH=0Bh."),
    "1000:79c7": ("vid_load_palette_buffer", "Video / Palette", "Loads custom palette array into active palette staging buffer."),
    "1000:79ed": ("vid_cycle_palette_step", "Video / Palette", "Rotates cycling palette colors by one step to animate waterfalls and energy barriers."),
    "1000:7a13": ("vid_apply_palette", "Video / Palette", "Writes palette staging buffer into EGA Attribute Controller palette registers."),
    "1000:7a39": ("vid_dim_palette", "Video / Palette", "Dims palette colors toward black for screen fade-out."),
    "1000:7a68": ("vid_out_dx_ax", "Video / Hardware", "Helper routine to write index and data byte simultaneously to EGA I/O port."),
    "1000:7a89": ("vid_in_status", "Video / Hardware", "Reads EGA Input Status Register 1 from Port 0x3da."),
    "1000:7aab": ("vid_setup_blit_rect", "Video / Blitting", "Calculates destination VRAM address and row strides for rectangular blit operation."),
    "1000:7ac8": ("vid_set_data_rotate", "Video / Hardware", "Configures EGA Data Rotate / Function Select register (Port 0x3ce index 3)."),
    "1000:7b03": ("vid_reset_data_rotate", "Video / Hardware", "Resets EGA Data Rotate register to default direct copy mode."),
    "1000:7b31": ("vid_blit_tile_aligned", "Video / Blitting", "Fast 16x16 tile blitter for byte-aligned horizontal positions."),
    "1000:7b8b": ("vid_blit_tile_shifted", "Video / Blitting", "16x16 tile blitter with bit-shift rotation for smooth pixel-accurate horizontal scrolling."),
    "1000:7bd9": ("vid_blit_glyph_aligned", "Video / Blitting", "8x8 font glyph blitter aligned to byte boundaries."),
    "1000:7bef": ("vid_blit_glyph_shifted", "Video / Blitting", "8x8 font glyph blitter with arbitrary pixel X shift."),
    "1000:7c79": ("vid_blit_sprite_masked_aligned", "Video / Blitting", "Masked 16x16 sprite blitter aligned to byte boundaries."),
    "1000:7ccd": ("vid_blit_sprite_masked_shifted", "Video / Blitting", "Masked 16x16 sprite blitter with arbitrary pixel X shift."),
    "1000:7d1b": ("vid_blit_small_sprite_aligned", "Video / Blitting", "Small 8x8 projectile/bullet sprite blitter aligned to byte boundaries."),
    "1000:7d31": ("vid_blit_small_sprite_shifted", "Video / Blitting", "Small 8x8 projectile/bullet sprite blitter with pixel shift."),
    "1000:7dbb": ("vid_blit_planar_block", "Video / Blitting", "Universal planar block blitter: blits arbitrary width and height rectangles across all 4 EGA planes.")
}

print(f"Total handcrafted annotations: {len(ANNOTATIONS)}")

# Write Ghidra Script to apply all annotations
ghidra_script_path = "/Users/hedge/workspace/comic2-rea/scripts/ApplyAllAnnotations.java"
with open(ghidra_script_path, "w") as f:
    f.write('''import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.SourceType;

import java.util.HashMap;
import java.util.Map;

public class ApplyAllAnnotations extends GhidraScript {
    @Override
    public void run() throws Exception {
        FunctionManager fm = currentProgram.getFunctionManager();
        int renamedCount = 0;
        int commentedCount = 0;

''')
    for addr, (name, category, desc) in ANNOTATIONS.items():
        clean_desc = desc.replace('"', '\\"')
        f.write(f'        applyFunc(fm, "{addr}", "{name}", "{category}", "{clean_desc}");\n')
        f.write(f'        renamedCount++; commentedCount++;\n')

    f.write('''
        println("Successfully applied annotations to " + renamedCount + " functions in Ghidra database.");
    }

    private void applyFunc(FunctionManager fm, String addrStr, String name, String category, String desc) {
        Address addr = toAddr(addrStr);
        Function func = fm.getFunctionAt(addr);
        if (func == null) {
            try {
                func = createFunction(addr, name);
            } catch (Exception e) {
                // ignore
            }
        }
        if (func != null) {
            try {
                func.setName(name, SourceType.USER_DEFINED);
                String fullComment = "[" + category + "]\\n" + desc;
                func.setComment(fullComment);
            } catch (Exception e) {
                println("Could not rename/comment function at " + addrStr + ": " + e.getMessage());
            }
        }
    }
}
''')

print("Generated Ghidra script ApplyAllAnnotations.java")
