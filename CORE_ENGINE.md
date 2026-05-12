# Captain Comic 2: Core Engine Analysis (sub_35DE)

The main game loop logic is contained in `sub_35DE` and its many chunks. It follows a priority-based state machine.

## Main Dispatcher (loc_2341)

Address: `loc_2341` (Line 3394 in `comic2.asm`)

### Logic Sequence:
1.  **Level Transition Check**
    *   Variables: `ds:8ECh`, `ds:921h`
    *   Target: `loc_275C` (`handle_level_transition`)
2.  **Special Logic 1**
    *   Variables: `ds:924h` > 1
    *   Target: `loc_3657`
3.  **Special Logic 2**
    *   Variables: `ds:92Ch` > 1
    *   Target: `loc_386F`
4.  **Airborne Physics Check**
    *   Variables: `ds:89Dh` != 0 (`comic_is_airborne`)
    *   Target: `loc_2EDC` (`handle_airborne_movement`)
5.  **Sub-state 3610 Check**
    *   Variables: `ds:896h` == 2, and others
    *   Calls: `sub_3610`
6.  **Grounded Physics Check**
    *   Variables: `ds:897h` != 0 (`comic_is_physics_active`)
    *   Target: `loc_2A10` (`update_player_physics`)
7.  **Animation Check**
    *   Variables: `ds:8A3h` != 0 (`comic_is_animation_active`)
    *   Target: `loc_3107` (`handle_player_animation`)
8.  **Attack Check**
    *   Variables: `ds:8A7h` != 0 (`comic_is_attack_active`)
    *   Target: `loc_34A9` (`handle_attack_animation`)
9.  **Distance-based Interaction Check**
    *   Variables: `ds:92Ch` == 1 and `|ds:6C6h - ds:930h|` < 18h
    *   Target: `loc_385F`
10. **Tile Hazard Check**
    *   Calls: `sub_1CFE` (`get_tile_at_pixels`)
    *   If tile in [ds:294h, ds:296h] -> `loc_2DC6` (`handle_player_death/hazard`)
11. **Special State 8A6 Check**
    *   Variables: `ds:8A6h` != 0
    *   Target: `loc_330F` (`check_player_state`)
12. **Input Handling (If No Active State)**
    *   **Jump Logic (`loc_243F`)**:
        *   If `key_state_jump` == 1 and `comic_jump_counter` > 0:
            *   Set `comic_is_physics_active` = 1
            *   Initiate jump or downward jump based on `key_state_down`.
    *   **Movement Logic (`loc_2548`)**:
        *   If `key_state_left` == 1: Call `sub_2BDC` (`move_player_left`)
        *   If `key_state_right` == 1: Call `sub_2C39` (`move_player_right`)
    *   **Floor Check**:
        *   Call `sub_2C9D` (`check_floor_collision`)
        *   If not on floor: Set `comic_is_physics_active` = 1, `comic_y_vel` = 5 (fall start).

## Key Subroutines
*   `sub_1CFE`: `get_tile_at_pixels(ax=x, bx=y)` - Returns tile type in AX.
*   `sub_2BDC`: `move_player_left` - Updates X and checks left collision.
*   `sub_2C39`: `move_player_right` - Updates X and checks right collision.
*   `sub_2C9D`: `check_floor_collision` - Checks tiles below player.
*   `sub_2A10`: `update_player_physics` - Main gravity and grounded movement routine.
*   `sub_2EDC`: `handle_airborne_movement` - Airborne physics (jump/fall arcs).
*   `sub_5D5F`: `update_entities` - Master loop for enemy AI and object updates.
*   `sub_7DBB`: `draw_sprite` - Renders a sprite to the back buffer.
*   `sub_18E0`: `setup_render_viewport` - Recalculates screen window relative to world (X, Y).
*   `sub_1B8D`: `load_resource` - Loads sprite or map data into memory.

## State Variables (DS Base: 24FE0h)
*   `ds:6C6h`: `comic_x` (word) - Player X world coordinate.
*   `ds:6C8h`: `comic_y` (word) - Player Y world coordinate.
*   `ds:892h`: `comic_x_vel` (word) - Horizontal velocity.
*   `ds:894h`: `comic_y_vel` (word) - Vertical velocity.
*   `ds:897h`: `comic_is_physics_active` (byte) - 1 when jumping/falling/sliding.
*   `ds:898h`: `comic_jump_counter` (byte) - Remaining ticks for jump boost.
*   `ds:8A3h`: `comic_is_animation_active` (byte) - 1 during scripted animations.
*   `ds:8A7h`: `comic_is_attack_active` (byte) - 1 when firing fireball.
*   `ds:8ADh`: `comic_facing` (byte) - 1:Right, 2:Left.
*   `ds:8AEh`: `comic_state` (byte) - 0:Idle, 1:Move, 2:Jump, 3:Die, 4:Climb, 6:Attack.
*   `ds:8EDh`: `comic_hp` (byte) - Player health (max 12).
*   `ds:222h`: `current_level` (word) - Level index.
*   `ds:6FAh`: `current_map` (word) - Map index.
