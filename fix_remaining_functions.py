#!/usr/bin/env python3
import re

FILE_PATH = '/Users/hedge/workspace/comic2/unpacked.exe.test.c'

# Mapping of FUN_1000_XXXX to descriptive names
HEX_TO_NAME = {
    '0185': 'init_ega_graphics',
    '0202': 'error_handler',
    '1cfe': 'check_tile_collision',
    '1dc0': 'calc_sprite_offset',
    '1f72': 'update_anim_frame',
    '1f89': 'update_frame_alt',
    '1fa4': 'render_sprite',
    '2049': 'update_render',
    '2072': 'update_palette',
    '2bdc': 'graphics_handler',
    '2c39': 'anim_handler',
    '2d06': 'collision_check_2d06',
    '2d31': 'collision_check_2d31',
    '2fee': 'collision_helper_2fee',
    '3019': 'collision_check_3019',
    '3057': 'move_left_cleanup',
    '309d': 'collision_2d_check',
    '30c6': 'sprite_physics',
    '3181': 'entity_collision',
    '31eb': 'sprite_handler_31eb',
    '327d': 'sprite_handler_327d',
    '3468': 'state_transition',
    '35de': 'main_game_loop',
    '3610': 'game_loop_chunk_3610',
    '362a': 'game_loop_chunk_362a',
    '36c0': 'game_loop_chunk_36c0',
    '378e': 'game_loop_chunk_378e',
    '3a17': 'animation_data_handler',
    '3a5f': 'movement_handler_3a5f',
    '3a92': 'action_handler_3a92',
    '3aa0': 'state_setup_3aa0',
    '3ace': 'state_setup_3ace',
    '3b12': 'sprite_handler_3b12',
    '3b51': 'render_handler_3b51',
    '3db6': 'draw_line_db6',
    '3dfb': 'draw_callback',
    '4073': 'graphics_mode_4073',
    '4206': 'render_tiles',
    '437b': 'sprite_render_437b',
    '451c': 'render_level',
    '4579': 'render_chunk_4579',
    '46e6': 'render_chunk_46e6',
    '489e': 'set_palette',
    '48aa': 'set_palette_48aa',
    '48d4': 'mode_setup_48d4',
    '49ad': 'display_handler_49ad',
    '4a70': 'display_handler_4a70',
    '4b46': 'entity_update_4b46',
    '4c06': 'entity_update_4c06',
    '5ceb': 'load_resource_5ceb',
    '5d5f': 'load_resource_5d5f',
    '61c8': 'load_resource_61c8',
    '62a4': 'load_resource_62a4',
    '631a': 'load_resource_631a',
    '633c': 'load_resource_633c',
    '6545': 'load_resource_6545',
    '65ef': 'title_screen',
    '660b': 'menu_loop_660b',
    '677a': 'render_loop_677a',
    '6806': 'render_loop_6806',
    '6da4': 'logic_handler_6da4',
    '6f47': 'logic_handler_6f47',
    '7057': 'logic_handler_7057',
    '71fe': 'ega_graphics_71fe',
    '721b': 'ega_graphics_721b',
    '7238': 'ega_graphics_7238',
    '7255': 'ega_graphics_7255',
    '7261': 'ega_graphics_7261',
    '7278': 'ega_graphics_7278',
    '74e1': 'ega_graphics_74e1',
    '752a': 'ega_graphics_752a',
    '7553': 'ega_graphics_7553',
    '75ad': 'ega_graphics_75ad',
    '7622': 'ega_graphics_7622',
    '7765': 'ega_palette_init',
    '77a3': 'ega_palette_77a3',
    '77f2': 'ega_palette_77f2',
    '7833': 'ega_palette_7833',
    '7849': 'ega_palette_7849',
    '78c6': 'ega_palette_78c6',
    '78e1': 'ega_palette_78e1',
    '790c': 'ega_palette_790c',
    '7927': 'ega_palette_7927',
    '7962': 'ega_palette_7962',
    '7990': 'ega_palette_7990',
    '7999': 'ega_palette_setup',
    '79a2': 'ega_palette_79a2',
    '79ed': 'ega_palette_79ed',
    '7a13': 'ega_blit_plane',
    '7a39': 'ega_blit_7a39',
    '7a89': 'ega_blit_7a89',
    '7aab': 'ega_blit_7aab',
    '7ac8': 'ega_blit_7ac8',
    '7b03': 'ega_blit_7b03',
    '7b31': 'ega_blit_7b31',
    '7b8b': 'ega_blit_7b8b',
    '7bd9': 'ega_blit_7bd9',
    '7bef': 'ega_blit_7bef',
    '7c79': 'ega_blit_7c79',
    '7ccd': 'ega_blit_7ccd',
    '7d1b': 'ega_blit_7d1b',
    '7d31': 'ega_blit_7d31',
}

# Read file
with open(FILE_PATH, 'r') as f:
    content = f.read()

# Find all unique FUN_1000_XXXX references
found_funcs = set(re.findall(r'FUN_1000_[0-9a-f]+', content))
print(f"Found {len(found_funcs)} unique functions to replace")

# Replace each function
replaced_count = 0
for func_name in found_funcs:
    hex_part = func_name.replace('FUN_1000_', '')
    
    if hex_part in HEX_TO_NAME:
        new_name = HEX_TO_NAME[hex_part]
        # Count occurrences before replacing
        count_before = len(re.findall(re.escape(func_name), content))
        content = content.replace(func_name, new_name)
        replaced_count += count_before
        print(f"  Replaced {func_name} -> {new_name} ({count_before} times)")
    else:
        # Generic fallback name
        new_name = f'func_{hex_part}'
        count_before = len(re.findall(re.escape(func_name), content))
        content = content.replace(func_name, new_name)
        replaced_count += count_before
        print(f"  Replaced {func_name} -> {new_name} ({count_before} times) [generic]")

# Write file
with open(FILE_PATH, 'w') as f:
    f.write(content)

print(f"\nTotal replacements: {replaced_count}")
print("Done!")
