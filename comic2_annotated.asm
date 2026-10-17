; ==============================================================================
; CAPTAIN COMIC II: FRACTURED REALITY (1990 by Michael A. Denio)
; Comprehensive Annotated DOS 16-bit 8086 Assembly Disassembly
; Reverse Engineered and Annotated via REA & Ghidra Analysis
; Target: unpacked.exe (MS-DOS Executable, 16-bit Real Mode, EGA 320x200 16-color)
; ==============================================================================

; ARCHITECTURAL OVERVIEW:
; - CPU Target: Intel 8086 / 8088 / 80286+ Real Mode (16-bit)
; - Graphics: IBM Enhanced Graphics Adapter (EGA) Mode 0Dh (320x200 16-color planar)
; - Video RAM: 0xA000:0000 with 64KB planar double-buffering (Page 0 @ 0x0000, Page 1 @ 0x8000)
; - Sound: IBM PC Speaker 8253 PIT Timer / Port 0x61 bit-banging driven by INT 08h ISR
; - Audio API: Custom Software Interrupt 03h (AL=cmd, BX=sfx_struct, CL=priority)
; - Input: Custom hardware INT 09h Keyboard ISR + Game Port 0x201 / BIOS INT 15h Joystick
; - Compression: Byte-Run / PackBits RLE decompression engine for level and tile packs
; - Segments:
;     CS = 0x1000  (All game engine executable code: 0x0064 - 0x7E46, data tables 0x7E4A - 0xD5CF)
;     DS = 0x34FE  (Main game variables, strings, state flags, save slot paths)
;     ES = 0xA000  (EGA video display buffer / active page)
;     Aux Segment = 0x1D5D (Decompression scratchpad & collision mask staging)
;     Palette Segment = 0x3206 (EGA color palette & DAC tables)
; ==============================================================================

; ------------------------------------------------------------------------------
; FUNCTION: entry  [System / Startup]
; ADDRESS:  1000:0064  (Length: 0xf9 bytes)
; PURPOSE:  Main game executable entry point. Initializes CPU speed calibration, checks for EGA with 256K, initializes video mode 0Dh, sets up interrupt handlers, and transfers execution to main dispatcher.
; ------------------------------------------------------------------------------
entry:
    CLD                                  ; 1000:0064
    MOV AX,0x34fe                        ; 1000:0065
    MOV DS,AX                            ; 1000:0068
    IN AL,0x61                           ; 1000:006a
    AND AL,0xfc                          ; 1000:006c
    OUT 0x61,AL                          ; 1000:006e
    CALL 0x1000:09c9                     ; 1000:0070 ; -> 1000:09c9
    MOV AX,0x1                           ; 1000:0073
    CALL 0x1000:027a                     ; 1000:0076 ; -> 1000:027a
    MOV CX,0xffff                        ; 1000:0079
    IN AL,DX                             ; 1000:007c
    CMP byte ptr CS:[0x245],0x1          ; 1000:007d ; -> 1000:0245
    JZ 0x1000:0090                       ; 1000:0083 ; -> 1000:0090
    LOOP 0x1000:007c                     ; 1000:0085 ; -> 1000:007c
    MOV word ptr CS:[0x62],0x500         ; 1000:0087 ; -> 1000:0062
    JMP 0x1000:00af                      ; 1000:008e ; -> 1000:00af
    NEG CX                               ; 1000:0090
    MOV AX,CX                            ; 1000:0092
    XOR DX,DX                            ; 1000:0094
    MOV CX,0x1c                          ; 1000:0096
    DIV CX                               ; 1000:0099
    MOV CS:[0x62],AX                     ; 1000:009b ; -> 1000:0062
    MOV CX,0x500                         ; 1000:009f
    SUB CX,AX                            ; 1000:00a2
    JC 0x1000:00af                       ; 1000:00a4 ; -> 1000:00af
    SHR CX,0x1                           ; 1000:00a6
    SHR CX,0x1                           ; 1000:00a8
    ADD word ptr CS:[0x62],CX            ; 1000:00aa ; -> 1000:0062
    MOV AX,0xf00                         ; 1000:00af
    INT 0x10                             ; 1000:00b2
    CMP AL,0xd                           ; 1000:00b4
    XOR AH,AH                            ; 1000:00b6
    MOV [0x0],AX                         ; 1000:00b8 ; -> 34fe:0000
    MOV AX,0xd                           ; 1000:00bb
    INT 0x10                             ; 1000:00be
    MOV AX,0xf00                         ; 1000:00c0
    INT 0x10                             ; 1000:00c3
    CMP AL,0xd                           ; 1000:00c5
    JNZ 0x1000:00d6                      ; 1000:00c7 ; -> 1000:00d6
    MOV AX,0x1200                        ; 1000:00c9
    MOV BX,0x10                          ; 1000:00cc
    INT 0x10                             ; 1000:00cf
    CMP BL,0x3                           ; 1000:00d1
    JGE 0x1000:00e6                      ; 1000:00d4 ; -> 1000:00e6
    MOV AX,0x2                           ; 1000:00d6
    INT 0x10                             ; 1000:00d9
    MOV AH,0x9                           ; 1000:00db
    LEA DX,[0xa0]                        ; 1000:00dd
    INT 0x21                             ; 1000:00e1
    JMP 0x1000:0215                      ; 1000:00e3 ; -> 1000:0215
    MOV AX,0xa000                        ; 1000:00e6
    MOV ES,AX                            ; 1000:00e9
    CALL 0x1000:7741                     ; 1000:00eb ; -> 1000:7741
    PUSH DS                              ; 1000:00ee
    MOV AX,0x3206                        ; 1000:00ef
    MOV DS,AX                            ; 1000:00f2
    MOV SI,0x1cd0                        ; 1000:00f4
    MOV DI,word ptr CS:[0x773f]          ; 1000:00f7 ; -> 1000:773f
    CALL 0x1000:79c7                     ; 1000:00fc ; -> 1000:79c7
    MOV SI,0x0                           ; 1000:00ff
    MOV DI,word ptr CS:[0x773f]          ; 1000:0102 ; -> 1000:773f
    CALL 0x1000:7a13                     ; 1000:0107 ; -> 1000:7a13
    POP DS                               ; 1000:010a
    MOV DX,0x2                           ; 1000:010b
    CALL 0x1000:015d                     ; 1000:010e ; -> 1000:015d
    CALL 0x1000:774e                     ; 1000:0111 ; -> 1000:774e
    MOV DX,0x13                          ; 1000:0114
    CALL 0x1000:015d                     ; 1000:0117 ; -> 1000:015d
    MOV byte ptr CS:[0x61],0x0           ; 1000:011a ; -> 1000:0061
    MOV CX,0x19                          ; 1000:0120
    MOV AX,0x1                           ; 1000:0123
    CALL 0x1000:027a                     ; 1000:0126 ; -> 1000:027a
    CMP byte ptr CS:[0x61],0x0           ; 1000:0129 ; -> 1000:0061
    JNZ 0x1000:0136                      ; 1000:012f ; -> 1000:0136
    LOOP 0x1000:0123                     ; 1000:0131 ; -> 1000:0123
    CALL 0x1000:7164                     ; 1000:0133 ; -> 1000:7164
    PUSH DS                              ; 1000:0136
    MOV AX,0x3206                        ; 1000:0137
    MOV DS,AX                            ; 1000:013a
    MOV SI,0x1cd0                        ; 1000:013c
    MOV DI,word ptr CS:[0x773f]          ; 1000:013f ; -> 1000:773f
    CALL 0x1000:79c7                     ; 1000:0144 ; -> 1000:79c7
    POP DS                               ; 1000:0147
    MOV DX,0x2                           ; 1000:0148
    CALL 0x1000:015d                     ; 1000:014b ; -> 1000:015d
    CALL 0x1000:774e                     ; 1000:014e ; -> 1000:774e
    MOV AX,0x3                           ; 1000:0151
    CALL 0x1000:027a                     ; 1000:0154 ; -> 1000:027a
    CALL 0x1000:02b4                     ; 1000:0157 ; -> 1000:02b4
    JMP 0x1000:5e52                      ; 1000:015a ; -> 1000:5e52

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_ega_palette  [Video / Palette]
; ADDRESS:  1000:015d  (Length: 0x28 bytes)
; CALLERS:  FUN_1000_02b4, FUN_1000_7298, FUN_1000_7164, FUN_1000_0185, entry
; PURPOSE:  Waits for EGA vertical retrace and writes 16 color values to the EGA Attribute Controller palette registers (Ports 0x3da / 0x3c0).
; ------------------------------------------------------------------------------
vid_set_ega_palette:
    PUSH SI                              ; 1000:015d
    MOV SI,DX                            ; 1000:015e
    MOV DX,0x3da                         ; 1000:0160
    IN AL,DX                             ; 1000:0163
    TEST AL,0x8                          ; 1000:0164
    JZ 0x1000:0163                       ; 1000:0166 ; -> 1000:0163
    CLI                                  ; 1000:0168
    MOV DX,0x3c0                         ; 1000:0169
    MOV CX,0x10                          ; 1000:016c
    XOR BX,BX                            ; 1000:016f
    PUSH CX                              ; 1000:0171
    MOV CL,byte ptr [SI]                 ; 1000:0172
    INC SI                               ; 1000:0174
    MOV AX,BX                            ; 1000:0175
    INC BX                               ; 1000:0177
    OUT DX,AL                            ; 1000:0178
    MOV AL,CL                            ; 1000:0179
    OUT DX,AL                            ; 1000:017b
    POP CX                               ; 1000:017c
    LOOP 0x1000:0171                     ; 1000:017d ; -> 1000:0171
    MOV AL,0x20                          ; 1000:017f
    OUT DX,AL                            ; 1000:0181
    POP SI                               ; 1000:0182
    STI                                  ; 1000:0183
    RET                                  ; 1000:0184

; ------------------------------------------------------------------------------
; FUNCTION: vid_fade_screen  [Video / Palette]
; ADDRESS:  1000:0185  (Length: 0x3b bytes)
; CALLERS:  FUN_1000_21f2
; PURPOSE:  Cycles/fades the EGA palette out to black over a sequence of frame delays for smooth screen transitions.
; ------------------------------------------------------------------------------
vid_fade_screen:
    CALL 0x1000:022b                     ; 1000:0185 ; -> 1000:022b
    MOV AX,0xd                           ; 1000:0188
    INT 0x10                             ; 1000:018b
    CALL 0x1000:7741                     ; 1000:018d ; -> 1000:7741
    PUSH DS                              ; 1000:0190
    MOV AX,0x3206                        ; 1000:0191
    MOV DS,AX                            ; 1000:0194
    MOV SI,0x1cd0                        ; 1000:0196
    MOV DI,word ptr CS:[0x773f]          ; 1000:0199 ; -> 1000:773f
    CALL 0x1000:79c7                     ; 1000:019e ; -> 1000:79c7
    POP DS                               ; 1000:01a1
    MOV DX,0x1eb                         ; 1000:01a2
    MOV DI,word ptr CS:[0x773f]          ; 1000:01a5 ; -> 1000:773f
    CALL 0x1000:7999                     ; 1000:01aa ; -> 1000:7999
    MOV DX,0x24                          ; 1000:01ad
    CALL 0x1000:015d                     ; 1000:01b0 ; -> 1000:015d
    CALL 0x1000:774e                     ; 1000:01b3 ; -> 1000:774e
    MOV AX,0x1                           ; 1000:01b6
    MOV BX,0xdb                          ; 1000:01b9
    MOV CX,0x1                           ; 1000:01bc
    INT3                                 ; 1000:01bf

; ------------------------------------------------------------------------------
; FUNCTION: ui_intro_cutscene_setup  [UI / Cutscene]
; ADDRESS:  1000:01c0  (Length: 0x42 bytes)
; PURPOSE:  Fades screen, loads title palette, plays intro melody, and jumps to story screen.
; ------------------------------------------------------------------------------
ui_intro_cutscene_setup:
    CALL sys_flush_keyboard_buffer       ; 1000:01c0
    PUSH DS                              ; 1000:01c3
    MOV AX,0x3206                        ; 1000:01c4 (relocated SEG_PALETTE)
    MOV DS,AX                            ; 1000:01c7
    MOV SI,0x1cd0                        ; 1000:01c9
    MOV DI,word ptr CS:[0x773f]          ; 1000:01cc
    CALL 0x1000:79c7                     ; 1000:01d1 ; -> vid_load_palette_buffer
    POP DS                               ; 1000:01d4
    MOV DX,0x1f5                         ; 1000:01d5
    MOV DI,word ptr CS:[0x773f]          ; 1000:01d8
    CALL 0x1000:7a13                     ; 1000:01dd ; -> vid_apply_palette
    MOV DX,0x24                          ; 1000:01e0
    CALL 0x1000:015d                     ; 1000:01e3 ; -> vid_set_ega_palette
    CALL 0x1000:774e                     ; 1000:01e6 ; -> vid_wait_vblank
    MOV AX,0x1                           ; 1000:01e9
    MOV BX,0x96b6                        ; 1000:01ec
    MOV CX,0x1                           ; 1000:01ef
    INT3                                 ; 1000:01f2
    CALL sys_flush_keyboard_buffer       ; 1000:01f3
    MOV AX,0xd                           ; 1000:01f6
    INT 0x10                             ; 1000:01f9
    CALL 0x1000:7741                     ; 1000:01fb ; -> vid_init_ega_regs
    JMP 0x1000:63ac                      ; 1000:01fe ; -> ui_story_screen
    RET                                  ; 1000:0201

; ------------------------------------------------------------------------------
; FUNCTION: sys_fatal_error  [System / Error]
; ADDRESS:  1000:0202  (Length: 0x12 bytes)
; CALLERS:  FUN_1000_633c, FUN_1000_2022, FUN_1000_2049, FUN_1000_631a, FUN_1000_6668, FUN_1000_79a2, FUN_1000_1ffe, FUN_1000_1fa4, FUN_1000_2072
; PURPOSE:  Restores text video mode, prints the fatal error string at DS:0x0046 using DOS INT 21h AH=09h, and exits to DOS.
; ------------------------------------------------------------------------------
sys_fatal_error:
    MOV AX,[0x0]                         ; 1000:0202
    INT 0x10                             ; 1000:0205
    MOV AH,0x9                           ; 1000:0207
    LEA DX,[0x46]                        ; 1000:0209
    INT 0x21                             ; 1000:020d
    CALL 0x1000:0599                     ; 1000:020f ; -> 1000:0599
    JMP 0x1000:0215                      ; 1000:0212 ; -> 1000:0215

; ------------------------------------------------------------------------------
; FUNCTION: sys_exit_to_dos  [System / Lifecycle]
; ADDRESS:  1000:0215  (Length: 0x4 bytes)
; CALLERS:  FUN_1000_660b, FUN_1000_7057, entry, FUN_1000_0202
; PURPOSE:  Restores original interrupt vectors, cleans up video mode, and terminates the program via DOS INT 21h AH=4Ch.
; ------------------------------------------------------------------------------
sys_exit_to_dos:
    MOV AX,0x2                           ; 1000:0215
    INT3                                 ; 1000:0218
    CALL sys_restore_all_interrupts      ; 1000:0219
    MOV AX,[0x0]                         ; 1000:021c
    INT 0x10                             ; 1000:021f
    LEA BX,[0x35]                        ; 1000:0221
    MOV AH,0x4c                          ; 1000:0225
    INT 0x21                             ; 1000:0227
    DW 0x0000                            ; 1000:0229

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_active_page_0  [Video / Paging]
; ADDRESS:  1000:022b  (Length: 0xd bytes)
; CALLERS:  FUN_1000_1b8d, FUN_1000_0185, FUN_1000_7278
; PURPOSE:  Sets active EGA display page to page 0 (offset 0x0000 in VRAM A000:0000).
; ------------------------------------------------------------------------------
vid_set_active_page_0:
    MOV byte ptr CS:[0x22a],0x0          ; 1000:022b ; -> 1000:022a
    MOV byte ptr CS:[0x229],0x0          ; 1000:0231 ; -> 1000:0229
    RET                                  ; 1000:0237

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_active_page_1  [Video / Paging]
; ADDRESS:  1000:0238  (Length: 0xd bytes)
; CALLERS:  FUN_1000_1b8d, FUN_1000_7298
; PURPOSE:  Sets active EGA display page to page 1 (offset 0x8000 in VRAM A000:0000) for double buffering.
; ------------------------------------------------------------------------------
vid_set_active_page_1:
    MOV byte ptr CS:[0x22a],0x0          ; 1000:0238 ; -> 1000:022a
    MOV byte ptr CS:[0x229],0x1          ; 1000:023e ; -> 1000:0229
    RET                                  ; 1000:0244

; ------------------------------------------------------------------------------
; IN-CODE HARDWARE & INPUT STATE TABLE (CS:0245 - CS:0279)  (Length: 53 bytes)
; ------------------------------------------------------------------------------
; 1000:0245  game_tick_flag          db 0x00
; 1000:0246  irq0_parity             db 0x00
; 1000:0247  sound_is_playing        db 0x00
; 1000:0248  sound_is_enabled        db 0x01
; 1000:0249  sound_priority          db 0x00
; 1000:024a  sound_data_offset       dw 0x0000
; 1000:024c  sound_note_counter      dw 0x0000
; 1000:024e  sound_data_segment      dw 0x0000
; 1000:0250  scancode_esc            db 0x01
; 1000:0251  scancode_f1             db 0x3b (Unmute)
; 1000:0252  scancode_f2             db 0x3c (Mute)
; 1000:0253  scancode_f3             db 0x3d
; 1000:0254  scancode_f4             db 0x3e
; 1000:0255  scancode_pgup           db 0x49
; 1000:0256  scancode_pgdn           db 0x51
; 1000:0257  scancode_home           db 0x47
; 1000:0258  key_state_left          db 0x00
; 1000:0259  key_state_right         db 0x00
; 1000:025a  key_state_up            db 0x00
; 1000:025b  key_state_down          db 0x00
; 1000:025c  key_state_jump          db 0x00
; 1000:025d  key_state_fire          db 0x00
; 1000:025e  map_scancode_left       db 0x4b (Left Arrow)
; 1000:025f  map_scancode_right      db 0x4d (Right Arrow)
; 1000:0260  map_scancode_up         db 0x48 (Up Arrow)
; 1000:0261  map_scancode_down       db 0x50 (Down Arrow)
; 1000:0262  map_scancode_jump       db 0x39 (Spacebar)
; 1000:0263  map_scancode_fire       db 0x52 (Insert)
; 1000:0264  key_state_jump_tmp      db 0x00
; 1000:0265  key_state_fire_tmp      db 0x00
; 1000:0266  key_state_right_tmp     db 0x00
; 1000:0267  key_state_left_tmp      db 0x00
; 1000:0268  key_state_up_tmp        db 0x00
; 1000:0269  key_state_down_tmp      db 0x00
; 1000:026a  joystick_temp_x         dw 0x0000
; 1000:026c  joystick_temp_y         dw 0x0000
; 1000:026e  joystick_x_low          dw 0x0000
; 1000:0270  joystick_x_high         dw 0x0000
; 1000:0272  joystick_y_low          dw 0x0000
; 1000:0274  joystick_y_high         dw 0x0000
; 1000:0276  joystick_is_calibrated  dw 0x0000
; 1000:0278  current_melody_ptr      dw 0x0000

; ------------------------------------------------------------------------------
; FUNCTION: timer_delay_ticks  [System / Timing]
; ADDRESS:  1000:027a  (Length: 0x12 bytes)
; CALLERS:  FUN_1000_3c13, FUN_1000_21f2, FUN_1000_20fe, FUN_1000_3db6, FUN_1000_2341, FUN_1000_46e6, entry, FUN_1000_7278
; PURPOSE:  Synchronous frame delay routine. Decrements AX against the hardware timer tick flag updated by INT 08h ISR.
; ------------------------------------------------------------------------------
timer_delay_ticks:
    CMP byte ptr CS:[0x245],0x1          ; 1000:027a ; -> 1000:0245
    JNZ 0x1000:027a                      ; 1000:0280 ; -> 1000:027a
    MOV byte ptr CS:[0x245],0x0          ; 1000:0282 ; -> 1000:0245
    DEC AX                               ; 1000:0288
    JNZ 0x1000:027a                      ; 1000:0289 ; -> 1000:027a
    RET                                  ; 1000:028b

; ------------------------------------------------------------------------------
; FUNCTION: sys_print_string  [System / Text]
; ADDRESS:  1000:028c  (Length: 0x13 bytes)
; PURPOSE:  Prints null-terminated string at DS:BX using BIOS teletype INT 10h AH=0Eh.
; ------------------------------------------------------------------------------
sys_print_string:
    MOV AL,[BX]                          ; 1000:028c
    INC BX                               ; 1000:028e
    OR AL,AL                             ; 1000:028f
    JZ .done                             ; 1000:0291
    PUSH BX                              ; 1000:0293
    MOV BX,0x7                           ; 1000:0294
    MOV AH,0xe                           ; 1000:0297
    INT 0x10                             ; 1000:0299
    POP BX                               ; 1000:029b
    JMP sys_print_string                 ; 1000:029c
.done:
    RET                                  ; 1000:029e

; ------------------------------------------------------------------------------
; FUNCTION: sys_print_xor_string  [System / Text]
; ADDRESS:  1000:029f  (Length: 0x15 bytes)
; PURPOSE:  Decodes and prints XOR 0x25 obfuscated string at DS:BX via BIOS teletype INT 10h AH=0Eh.
; ------------------------------------------------------------------------------
sys_print_xor_string:
    MOV AL,[BX]                          ; 1000:029f
    INC BX                               ; 1000:02a1
    XOR AL,0x25                          ; 1000:02a2
    OR AL,AL                             ; 1000:02a4
    JZ .done                             ; 1000:02a6
    PUSH BX                              ; 1000:02a8
    MOV BX,0x7                           ; 1000:02a9
    MOV AH,0xe                           ; 1000:02ac
    INT 0x10                             ; 1000:02ae
    POP BX                               ; 1000:02b0
    JMP sys_print_xor_string             ; 1000:02b1
.done:
    RET                                  ; 1000:02b3

; ------------------------------------------------------------------------------
; FUNCTION: vid_init_screen_pages  [Video / Paging]
; ADDRESS:  1000:02b4  (Length: 0x36 bytes)
; CALLERS:  entry
; PURPOSE:  Initializes both EGA video pages (0 and 1), clears screen memory, and sets up active palette.
; ------------------------------------------------------------------------------
vid_init_screen_pages:
    PUSH DS                              ; 1000:02b4
    MOV AX,0x3206                        ; 1000:02b5
    MOV DS,AX                            ; 1000:02b8
    MOV SI,0x1cd0                        ; 1000:02ba
    MOV DI,word ptr CS:[0x773f]          ; 1000:02bd ; -> 1000:773f
    CALL 0x1000:79c7                     ; 1000:02c2 ; -> 1000:79c7
    MOV SI,0xcdc                         ; 1000:02c5
    MOV DI,word ptr CS:[0x773f]          ; 1000:02c8 ; -> 1000:773f
    CALL 0x1000:7a13                     ; 1000:02cd ; -> 1000:7a13
    POP DS                               ; 1000:02d0
    MOV DX,0x2                           ; 1000:02d1
    CALL 0x1000:015d                     ; 1000:02d4 ; -> 1000:015d
    CALL 0x1000:774e                     ; 1000:02d7 ; -> 1000:774e
    MOV DX,0x24                          ; 1000:02da
    CALL 0x1000:015d                     ; 1000:02dd ; -> 1000:015d
    MOV AX,0x1                           ; 1000:02e0
    MOV BX,0xdb                          ; 1000:02e3
    MOV CX,0x1                           ; 1000:02e6
    INT3                                 ; 1000:02e9

; ------------------------------------------------------------------------------
; FUNCTION: file_load_keys_def_and_prompt  [Input / Config]
; ADDRESS:  1000:02ea  (Length: 0x52 bytes)
; PURPOSE:  Loads KEYBOARD.DEF; prompts user or configures keys/joystick.
; ------------------------------------------------------------------------------
file_load_keys_def_and_prompt:
    LEA DX,[0x1ff]                       ; 1000:02ea
    MOV AX,0x3d00                        ; 1000:02ee
    INT 0x21                             ; 1000:02f1
    JC .prompt_user                      ; 1000:02f3
    MOV BX,AX                            ; 1000:02f5
    PUSH DS                              ; 1000:02f7
    MOV AX,CS                            ; 1000:02f8
    MOV DS,AX                            ; 1000:02fa
    MOV DX,0x25e                         ; 1000:02fc (map_scancode_left)
    MOV CX,0x6                           ; 1000:02ff
    MOV AX,0x3f00                        ; 1000:0302
    INT 0x21                             ; 1000:0305
    MOV AX,0x3e00                        ; 1000:0307
    INT 0x21                             ; 1000:030a
    POP DS                               ; 1000:030c
.prompt_user:
    CALL sys_flush_keyboard_buffer       ; 1000:030d
    CMP byte ptr CS:[0x61],0x25          ; 1000:0310 ('K' - Redefine keys)
    JNZ .check_exit                      ; 1000:0316
    JMP input_setup_keyboard             ; 1000:0318
.check_exit:
    CMP byte ptr CS:[0x61],0x1           ; 1000:031b (Esc - Exit)
    JNZ .check_joystick                  ; 1000:0321
    JMP sys_exit_to_dos                  ; 1000:0323
.check_joystick:
    CMP byte ptr CS:[0x61],0x24          ; 1000:0326 ('J' - Calibrate joystick)
    JZ input_calibrate_joystick          ; 1000:032c
vid_set_mode_and_init:
    MOV AX,0xd                           ; 1000:032e
    INT 0x10                             ; 1000:0331
    MOV AX,0xa000                        ; 1000:0333
    MOV ES,AX                            ; 1000:0336
    CALL 0x1000:7741                     ; 1000:0338 ; -> vid_init_ega_regs
    RET                                  ; 1000:033b

; ------------------------------------------------------------------------------
; FUNCTION: input_calibrate_joystick  [Input / Joystick]
; ADDRESS:  1000:033c  (Length: 0xe8 bytes)
; PURPOSE:  Calibrates joystick center, upper-left, and lower-right thresholds.
; ------------------------------------------------------------------------------
input_calibrate_joystick:
    MOV AX,0x2                           ; 1000:033c
    INT 0x10                             ; 1000:033f
    PUSH DS                              ; 1000:0341
    MOV AX,0x1d5d                        ; 1000:0342 (relocated SEG_SCRATCH)
    MOV DS,AX                            ; 1000:0345
    MOV byte ptr CS:[0x61],0x0           ; 1000:0347
    LEA DX,[0x0]                         ; 1000:034d
    MOV AH,0x9                           ; 1000:0351
    INT 0x21                             ; 1000:0353
    CALL input_wait_joystick_or_key      ; 1000:0355
    OR AX,AX                             ; 1000:0358
    JNZ .read_center                     ; 1000:035a
    JMP .abort                           ; 1000:035c
.read_center:
    MOV AH,0x84                          ; 1000:035f
    MOV DX,0x1                           ; 1000:0361
    INT 0x15                             ; 1000:0364
    MOV word ptr CS:[joystick_temp_x],AX ; 1000:0366
    MOV word ptr CS:[joystick_temp_y],BX ; 1000:036a
    LEA DX,[0xa8]                        ; 1000:036f
    MOV AH,0x9                           ; 1000:0373
    INT 0x21                             ; 1000:0375
    CALL input_wait_joystick_or_key      ; 1000:0377
    OR AX,AX                             ; 1000:037a
    JNZ .read_upper_left                 ; 1000:037c
    JMP .abort                           ; 1000:037e
.read_upper_left:
    MOV AH,0x84                          ; 1000:0381
    MOV DX,0x1                           ; 1000:0383
    INT 0x15                             ; 1000:0386
    MOV BX,word ptr CS:[joystick_temp_x] ; 1000:0388
    SUB BX,AX                            ; 1000:038d
    SHR BX,1                             ; 1000:038f
    MOV word ptr CS:[joystick_x_low],BX  ; 1000:0391
    LEA DX,[0xe6]                        ; 1000:0396
    MOV AH,0x9                           ; 1000:039a
    INT 0x21                             ; 1000:039c
    CALL input_wait_joystick_or_key      ; 1000:039e
    OR AX,AX                             ; 1000:03a1
    JNZ .read_lower_right                ; 1000:03a3
    JMP .abort                           ; 1000:03a5
.read_lower_right:
    MOV AH,0x84                          ; 1000:03a8
    MOV DX,0x1                           ; 1000:03aa
    INT 0x15                             ; 1000:03ad
    MOV BX,word ptr CS:[joystick_temp_x] ; 1000:03af
    SUB AX,BX                            ; 1000:03b4
    SHR AX,1                             ; 1000:03b6
    ADD BX,AX                            ; 1000:03b8
    MOV word ptr CS:[joystick_x_high],BX ; 1000:03ba
    LEA DX,[0x125]                       ; 1000:03bf
    MOV AH,0x9                           ; 1000:03c3
    INT 0x21                             ; 1000:03c5
    CALL input_wait_joystick_or_key      ; 1000:03c7
    OR AX,AX                             ; 1000:03ca
    JNZ .read_y_low                      ; 1000:03cc
    JMP .abort                           ; 1000:03ce
.read_y_low:
    MOV AH,0x84                          ; 1000:03d1
    MOV DX,0x1                           ; 1000:03d3
    INT 0x15                             ; 1000:03d6
    MOV AX,word ptr CS:[joystick_temp_y] ; 1000:03d8
    SUB AX,BX                            ; 1000:03dc
    SHR AX,1                             ; 1000:03de
    MOV word ptr CS:[joystick_y_low],AX  ; 1000:03e0
    LEA DX,[0x162]                       ; 1000:03e4
    MOV AH,0x9                           ; 1000:03e8
    INT 0x21                             ; 1000:03ea
    CALL input_wait_joystick_or_key      ; 1000:03ec
    OR AX,AX                             ; 1000:03ef
    JNZ .read_y_high                     ; 1000:03f1
    JMP .abort                           ; 1000:03f3
.read_y_high:
    MOV AH,0x84                          ; 1000:03f6
    MOV DX,0x1                           ; 1000:03f8
    INT 0x15                             ; 1000:03fb
    MOV AX,word ptr CS:[joystick_temp_y] ; 1000:03fd
    SUB BX,AX                            ; 1000:0401
    SHR BX,1                             ; 1000:0403
    ADD AX,BX                            ; 1000:0405
    MOV word ptr CS:[joystick_y_high],AX ; 1000:0407
    POP DS                               ; 1000:040b
    MOV word ptr CS:[joystick_is_calibrated],0x1 ; 1000:040c
    JMP vid_set_mode_and_init            ; 1000:0413
.abort:
    POP DS                               ; 1000:0416
    MOV AX,0xd                           ; 1000:0417
    INT 0x10                             ; 1000:041a
    MOV AX,0xa000                        ; 1000:041c
    MOV ES,AX                            ; 1000:041f
    JMP vid_init_screen_pages            ; 1000:0421

; ------------------------------------------------------------------------------
; FUNCTION: input_wait_joystick_or_key  [Input / Poll]
; ADDRESS:  1000:0424  (Length: 0x38 bytes)
; PURPOSE:  Waits for joystick button press or keypress, returning status in AX.
; ------------------------------------------------------------------------------
input_wait_joystick_or_key:
    MOV AX,0x2                           ; 1000:0424
    CALL timer_delay_ticks               ; 1000:0427
    CMP byte ptr CS:[0x61],0x0           ; 1000:042a
    JNZ .key_pressed                     ; 1000:0430
    MOV AH,0x84                          ; 1000:0432
    XOR DX,DX                            ; 1000:0434
    INT 0x15                             ; 1000:0436
    TEST AL,0x10                         ; 1000:0438
    JZ input_wait_joystick_or_key        ; 1000:043a
    TEST AL,0x20                         ; 1000:043c
    JZ input_wait_joystick_or_key        ; 1000:043e
.wait_release:
    XOR AX,AX                            ; 1000:0440
    CMP byte ptr CS:[0x61],0x0           ; 1000:0442
    JNZ .key_pressed                     ; 1000:0448
    MOV AH,0x84                          ; 1000:044a
    XOR DX,DX                            ; 1000:044c
    INT 0x15                             ; 1000:044e
    TEST AL,0x10                         ; 1000:0450
    JZ .button_pressed                   ; 1000:0452
    TEST AL,0x20                         ; 1000:0454
    JNZ .wait_release                    ; 1000:0456
.button_pressed:
    MOV AX,0x1                           ; 1000:0458
.key_pressed:
    RET                                  ; 1000:045b

; ------------------------------------------------------------------------------
; FUNCTION: input_setup_keyboard  [Input / Config]
; ADDRESS:  1000:045c  (Length: 0xef bytes)
; PURPOSE:  Interactive wizard for mapping game keys; saves to KEYBOARD.DEF.
; ------------------------------------------------------------------------------
input_setup_keyboard:
    MOV AX,0x2                           ; 1000:045c
    INT 0x10                             ; 1000:045f
    PUSH DS                              ; 1000:0461
    MOV AX,0x1d5d                        ; 1000:0462 (relocated SEG_SCRATCH)
    MOV DS,AX                            ; 1000:0465
    LEA DX,[0x1a0]                       ; 1000:0467
    MOV AH,0x9                           ; 1000:046b
    INT 0x21                             ; 1000:046d
    MOV byte ptr CS:[map_scancode_up],0x0    ; 1000:046f
    MOV byte ptr CS:[map_scancode_down],0x0  ; 1000:0475
    MOV byte ptr CS:[map_scancode_left],0x0  ; 1000:047b
    MOV byte ptr CS:[map_scancode_right],0x0 ; 1000:0481
    MOV byte ptr CS:[map_scancode_fire],0x0  ; 1000:0487
    MOV byte ptr CS:[map_scancode_jump],0x0  ; 1000:048d
    LEA DX,[0x1d6]                       ; 1000:0493
    MOV AH,0x9                           ; 1000:0497
    INT 0x21                             ; 1000:0499
    CALL input_read_mapped_key           ; 1000:049b
    MOV byte ptr CS:[map_scancode_left],AL ; 1000:049e
    LEA DX,[0x205]                       ; 1000:04a2
    MOV AH,0x9                           ; 1000:04a6
    INT 0x21                             ; 1000:04a8
    CALL input_read_mapped_key           ; 1000:04aa
    MOV byte ptr CS:[map_scancode_right],AL ; 1000:04ad
    LEA DX,[0x234]                       ; 1000:04b1
    MOV AH,0x9                           ; 1000:04b5
    INT 0x21                             ; 1000:04b7
    CALL input_read_mapped_key           ; 1000:04b9
    MOV byte ptr CS:[map_scancode_up],AL ; 1000:04bc
    LEA DX,[0x263]                       ; 1000:04c0
    MOV AH,0x9                           ; 1000:04c4
    INT 0x21                             ; 1000:04c6
    CALL input_read_mapped_key           ; 1000:04c8
    MOV byte ptr CS:[map_scancode_down],AL ; 1000:04cb
    LEA DX,[0x292]                       ; 1000:04cf
    MOV AH,0x9                           ; 1000:04d3
    INT 0x21                             ; 1000:04d5
    CALL input_read_mapped_key           ; 1000:04d7
    MOV byte ptr CS:[map_scancode_jump],AL ; 1000:04da
    LEA DX,[0x2c1]                       ; 1000:04de
    MOV AH,0x9                           ; 1000:04e2
    INT 0x21                             ; 1000:04e4
    CALL input_read_mapped_key           ; 1000:04e6
    MOV byte ptr CS:[map_scancode_fire],AL ; 1000:04e9
    LEA DX,[0x2f0]                       ; 1000:04ed
    MOV AH,0x9                           ; 1000:04f1
    INT 0x21                             ; 1000:04f3
    XOR AX,AX                            ; 1000:04f5
    CALL sys_flush_keyboard_buffer       ; 1000:04f7
    CMP byte ptr CS:[0x61],0x31          ; 1000:04fa ('N' - Redo)
    JZ .redo                             ; 1000:0500
    LEA DX,[0x327]                       ; 1000:0502
    MOV AH,0x9                           ; 1000:0506
    INT 0x21                             ; 1000:0508
    CALL sys_flush_keyboard_buffer       ; 1000:050a
    CMP byte ptr CS:[0x61],0x15          ; 1000:050d ('Y' - Save)
    JZ .save_keys                        ; 1000:0513
    POP DS                               ; 1000:0515
    JMP vid_set_mode_and_init            ; 1000:0516
.redo:
    POP DS                               ; 1000:0519
    JMP input_setup_keyboard             ; 1000:051a
.save_keys:
    POP DS                               ; 1000:051d
    LEA DX,[0x1ff]                       ; 1000:051e
    MOV AX,0x3c00                        ; 1000:0522
    XOR CX,CX                            ; 1000:0525
    INT 0x21                             ; 1000:0527
    JC .error_exit                       ; 1000:0529
    MOV BX,AX                            ; 1000:052b
    MOV SI,DS                            ; 1000:052d
    MOV AX,CS                            ; 1000:052f
    MOV DS,AX                            ; 1000:0531
    MOV AX,0x4000                        ; 1000:0533
    MOV CX,0x6                           ; 1000:0536
    MOV DX,0x25e                         ; 1000:0539 (map_scancode_left)
    INT 0x21                             ; 1000:053c
    MOV DS,SI                            ; 1000:053e
    MOV AX,0x3e00                        ; 1000:0540
    INT 0x21                             ; 1000:0543
    JMP vid_set_mode_and_init            ; 1000:0545
.error_exit:
    JMP sys_exit_to_dos                  ; 1000:0548

; ------------------------------------------------------------------------------
; FUNCTION: input_read_mapped_key  [Input / Config]
; ADDRESS:  1000:054b  (Length: 0x4e bytes)
; PURPOSE:  Reads user key, validates against duplicates, displays key name.
; ------------------------------------------------------------------------------
input_read_mapped_key:
    CALL sys_flush_keyboard_buffer       ; 1000:054b
    MOV BL,AL                            ; 1000:054e
    XOR BH,BH                            ; 1000:0550
    MOV SI,BX                            ; 1000:0552
    CMP BL,byte ptr CS:[map_scancode_jump] ; 1000:0554
    JZ input_read_mapped_key             ; 1000:0559
    CMP BL,byte ptr CS:[map_scancode_left] ; 1000:055b
    JZ input_read_mapped_key             ; 1000:0560
    CMP BL,byte ptr CS:[map_scancode_right]; 1000:0562
    JZ input_read_mapped_key             ; 1000:0567
    CMP BL,byte ptr CS:[map_scancode_fire] ; 1000:0569
    JZ input_read_mapped_key             ; 1000:056e
    CMP BL,byte ptr CS:[map_scancode_up]   ; 1000:0570
    JZ input_read_mapped_key             ; 1000:0575
    CMP BL,byte ptr CS:[map_scancode_down] ; 1000:0577
    JZ input_read_mapped_key             ; 1000:057c
    DEC BX                               ; 1000:057e
    JZ input_read_mapped_key             ; 1000:057f
    CMP BX,0x52                          ; 1000:0581
    JG input_read_mapped_key             ; 1000:0584
    SHL BX,1                             ; 1000:0586
    SHL BX,1                             ; 1000:0588
    SHL BX,1                             ; 1000:058a
    LEA DX,[0x35f]                       ; 1000:058c
    ADD DX,BX                            ; 1000:0590
    MOV AH,0x9                           ; 1000:0592
    INT 0x21                             ; 1000:0594
    MOV AX,SI                            ; 1000:0596
    RET                                  ; 1000:0598

; ------------------------------------------------------------------------------
; FUNCTION: sys_flush_keyboard_buffer  [System / Input]
; ADDRESS:  1000:0599  (Length: 0x25 bytes)
; CALLERS:  ui_intro_cutscene_setup, file_load_keys_def_and_prompt, input_setup_keyboard, input_read_mapped_key
; PURPOSE:  Flushes BIOS keyboard buffer and waits for scancode.
; ------------------------------------------------------------------------------
sys_flush_keyboard_buffer:
sys_restore_interrupts:
    MOV byte ptr CS:[0x61],0x0           ; 1000:0599 ; -> 1000:0061
    CMP byte ptr CS:[0x61],0x0           ; 1000:059f ; -> 1000:0061
    JZ 0x1000:059f                       ; 1000:05a5 ; -> 1000:059f
    PUSH ES                              ; 1000:05a7
    XOR AX,AX                            ; 1000:05a8
    MOV ES,AX                            ; 1000:05aa
    MOV AL,CS:[0x61]                     ; 1000:05ac ; -> 1000:0061
    CLI                                  ; 1000:05b0
    MOV CL,byte ptr ES:[0x41a]           ; 1000:05b1
    MOV byte ptr ES:[0x41c],CL           ; 1000:05b6
    STI                                  ; 1000:05bb
    POP ES                               ; 1000:05bc
    RET                                  ; 1000:05bd

; ------------------------------------------------------------------------------
; FUNCTION: isr_keyboard_int09  [Interrupts / Input]
; ADDRESS:  1000:05c2  (Length: 0xb6 bytes)
; PURPOSE:  Custom hardware INT 09h Keyboard ISR. Reads raw scancodes from port 0x60, manages key press/release state matrix, and signals EOI to PIC.
; ------------------------------------------------------------------------------
isr_keyboard_int09:
    PUSH AX                              ; 1000:05c2
    PUSH BX                              ; 1000:05c3
    PUSH CX                              ; 1000:05c4
    PUSH DX                              ; 1000:05c5
    IN AL,0x60                           ; 1000:05c6
    PUSH AX                              ; 1000:05c8
    CMP byte ptr CS:[0x60],0x1           ; 1000:05c9
    JNZ 0x1000:05d9                      ; 1000:05cf ; -> 1000:05d9
    PUSHF                                ; 1000:05d1
    CALLF [0x5be]                        ; 1000:05d2
    JMP 0x1000:05f3                      ; 1000:05d7 ; -> 1000:05f3
    CLI                                  ; 1000:05d9
    IN AL,0x61                           ; 1000:05da
    JMP 0x1000:05de                      ; 1000:05dc ; -> 1000:05de
    OR AL,0x80                           ; 1000:05de
    OUT 0x61,AL                          ; 1000:05e0
    JMP 0x1000:05e4                      ; 1000:05e2 ; -> 1000:05e4
    IN AL,0x61                           ; 1000:05e4
    JMP 0x1000:05e8                      ; 1000:05e6 ; -> 1000:05e8
    AND AL,0x7f                          ; 1000:05e8
    OUT 0x61,AL                          ; 1000:05ea
    JMP 0x1000:05ee                      ; 1000:05ec ; -> 1000:05ee
    MOV AL,0x20                          ; 1000:05ee
    OUT 0x20,AL                          ; 1000:05f0
    STI                                  ; 1000:05f2
    POP AX                               ; 1000:05f3
    MOV DX,0x1                           ; 1000:05f4
    TEST AL,0x80                         ; 1000:05f7
    JZ 0x1000:0601                       ; 1000:05f9 ; -> 1000:0601
    AND AL,0x7f                          ; 1000:05fb
    XOR DX,DX                            ; 1000:05fd
    JMP 0x1000:0605                      ; 1000:05ff ; -> 1000:0605
    MOV CS:[0x61],AL                     ; 1000:0601
    CMP AL,byte ptr CS:[0x262]           ; 1000:0605
    JZ 0x1000:0639                       ; 1000:060a ; -> 1000:0639
    CMP AL,byte ptr CS:[0x263]           ; 1000:060c
    JZ 0x1000:0640                       ; 1000:0611 ; -> 1000:0640
    CMP AL,byte ptr CS:[0x25e]           ; 1000:0613
    JZ 0x1000:0647                       ; 1000:0618 ; -> 1000:0647
    CMP AL,byte ptr CS:[0x25f]           ; 1000:061a
    JZ 0x1000:064e                       ; 1000:061f ; -> 1000:064e
    CMP AL,byte ptr CS:[0x260]           ; 1000:0621
    JZ 0x1000:0655                       ; 1000:0626 ; -> 1000:0655
    CMP AL,byte ptr CS:[0x261]           ; 1000:0628
    JZ 0x1000:065c                       ; 1000:062d ; -> 1000:065c
    CMP AL,0x3c                          ; 1000:062f
    JZ 0x1000:0663                       ; 1000:0631 ; -> 1000:0663
    CMP AL,0x3b                          ; 1000:0633
    JZ 0x1000:066b                       ; 1000:0635 ; -> 1000:066b
    JMP 0x1000:0673                      ; 1000:0637 ; -> 1000:0673
    MOV byte ptr CS:[0x25c],DL           ; 1000:0639
    JMP 0x1000:0673                      ; 1000:063e ; -> 1000:0673
    MOV byte ptr CS:[0x25d],DL           ; 1000:0640
    JMP 0x1000:0673                      ; 1000:0645 ; -> 1000:0673
    MOV byte ptr CS:[0x258],DL           ; 1000:0647
    JMP 0x1000:0673                      ; 1000:064c ; -> 1000:0673
    MOV byte ptr CS:[0x259],DL           ; 1000:064e
    JMP 0x1000:0673                      ; 1000:0653 ; -> 1000:0673
    MOV byte ptr CS:[0x25a],DL           ; 1000:0655
    JMP 0x1000:0673                      ; 1000:065a ; -> 1000:0673
    MOV byte ptr CS:[0x25b],DL           ; 1000:065c
    JMP 0x1000:0673                      ; 1000:0661 ; -> 1000:0673
    MOV byte ptr CS:[0x248],0x0          ; 1000:0663
    JMP 0x1000:0673                      ; 1000:0669 ; -> 1000:0673
    MOV byte ptr CS:[0x248],0x1          ; 1000:066b
    JMP 0x1000:0673                      ; 1000:0671 ; -> 1000:0673
    POP DX                               ; 1000:0673
    POP CX                               ; 1000:0674
    POP BX                               ; 1000:0675
    POP AX                               ; 1000:0676
    IRET                                 ; 1000:0677

; ------------------------------------------------------------------------------
; FUNCTION: isr_ctrl_break_int23  [Interrupts / Lifecycle]
; ADDRESS:  1000:067c  (Length: 0x3 bytes)
; PURPOSE:  Custom INT 23h DOS Ctrl-C / Ctrl-Break handler. Prevents abnormal exit and jumps to clean exit routine.
; ------------------------------------------------------------------------------
isr_ctrl_break_int23:
    JMP 0x1000:0215                      ; 1000:067c ; -> 1000:0215

; ------------------------------------------------------------------------------
; FUNCTION: isr_timer_sound_int08  [Interrupts / Audio]
; ADDRESS:  1000:0683  (Length: 0x240 bytes)
; PURPOSE:  Custom hardware INT 08h Timer ISR. Drives the PC speaker sound synthesis engine, decrements note durations, updates tone frequencies, and ticks frame counters.
; ------------------------------------------------------------------------------
isr_timer_sound_int08:
    PUSH AX                              ; 1000:0683
    PUSH BX                              ; 1000:0684
    PUSH CX                              ; 1000:0685
    PUSH DX                              ; 1000:0686
    PUSH ES                              ; 1000:0687
    MOV AL,CS:[0x246]                    ; 1000:0688
    INC AL                               ; 1000:068c
    CMP AL,0x2                           ; 1000:068e
    JGE 0x1000:0695                      ; 1000:0690 ; -> 1000:0695
    JMP 0x1000:07d6                      ; 1000:0692 ; -> 1000:07d6
    MOV byte ptr CS:[0x245],0x1          ; 1000:0695
    CMP word ptr CS:[0x276],0x1          ; 1000:069b
    JZ 0x1000:06a6                       ; 1000:06a1 ; -> 1000:06a6
    JMP 0x1000:07ae                      ; 1000:06a3 ; -> 1000:07ae
    MOV AL,CS:[0x25c]                    ; 1000:06a6
    MOV CS:[0x264],AL                    ; 1000:06aa
    MOV byte ptr CS:[0x25c],0x0          ; 1000:06ae
    MOV AL,CS:[0x25d]                    ; 1000:06b4
    MOV CS:[0x265],AL                    ; 1000:06b8
    MOV byte ptr CS:[0x25d],0x0          ; 1000:06bc
    MOV AL,CS:[0x259]                    ; 1000:06c2
    MOV CS:[0x266],AL                    ; 1000:06c6
    MOV byte ptr CS:[0x259],0x0          ; 1000:06ca
    MOV AL,CS:[0x258]                    ; 1000:06d0
    MOV CS:[0x267],AL                    ; 1000:06d4
    MOV byte ptr CS:[0x258],0x0          ; 1000:06d8
    MOV AL,CS:[0x25a]                    ; 1000:06de
    MOV CS:[0x268],AL                    ; 1000:06e2
    MOV byte ptr CS:[0x25a],0x0          ; 1000:06e6
    MOV AL,CS:[0x25b]                    ; 1000:06ec
    MOV CS:[0x269],AL                    ; 1000:06f0
    MOV byte ptr CS:[0x25b],0x0          ; 1000:06f4
    MOV AH,0x84                          ; 1000:06fa
    XOR DX,DX                            ; 1000:06fc
    INT 0x15                             ; 1000:06fe
    TEST AL,0x20                         ; 1000:0700
    JNZ 0x1000:070a                      ; 1000:0702 ; -> 1000:070a
    MOV byte ptr CS:[0x25c],0x1          ; 1000:0704
    TEST AL,0x10                         ; 1000:070a
    JNZ 0x1000:0714                      ; 1000:070c ; -> 1000:0714
    MOV byte ptr CS:[0x25d],0x1          ; 1000:070e
    MOV AH,0x84                          ; 1000:0714
    MOV DX,0x1                           ; 1000:0716
    INT 0x15                             ; 1000:0719
    CMP AX,word ptr CS:[0x26e]           ; 1000:071b
    JG 0x1000:072a                       ; 1000:0720 ; -> 1000:072a
    MOV byte ptr CS:[0x258],0x1          ; 1000:0722
    JMP 0x1000:0737                      ; 1000:0728 ; -> 1000:0737
    CMP AX,word ptr CS:[0x270]           ; 1000:072a
    JL 0x1000:0737                       ; 1000:072f ; -> 1000:0737
    MOV byte ptr CS:[0x259],0x1          ; 1000:0731
    CMP BX,word ptr CS:[0x272]           ; 1000:0737
    JG 0x1000:0746                       ; 1000:073c ; -> 1000:0746
    MOV byte ptr CS:[0x25a],0x1          ; 1000:073e
    JMP 0x1000:0753                      ; 1000:0744 ; -> 1000:0753
    CMP BX,word ptr CS:[0x274]           ; 1000:0746
    JL 0x1000:0753                       ; 1000:074b ; -> 1000:0753
    MOV byte ptr CS:[0x25b],0x1          ; 1000:074d
    MOV AL,CS:[0x25c]                    ; 1000:0753
    XOR byte ptr CS:[0x264],AL           ; 1000:0757
    MOV AL,CS:[0x25d]                    ; 1000:075c
    XOR byte ptr CS:[0x265],AL           ; 1000:0760
    MOV AL,CS:[0x259]                    ; 1000:0765
    XOR byte ptr CS:[0x266],AL           ; 1000:0769
    MOV AL,CS:[0x258]                    ; 1000:076e
    XOR byte ptr CS:[0x267],AL           ; 1000:0772
    MOV AL,CS:[0x25a]                    ; 1000:0777
    XOR byte ptr CS:[0x268],AL           ; 1000:077b
    MOV AL,CS:[0x25b]                    ; 1000:0780
    XOR byte ptr CS:[0x269],AL           ; 1000:0784
    MOV AL,CS:[0x264]                    ; 1000:0789
    OR AL,byte ptr CS:[0x265]            ; 1000:078d
    OR AL,byte ptr CS:[0x266]            ; 1000:0792
    OR AL,byte ptr CS:[0x267]            ; 1000:0797
    OR AL,byte ptr CS:[0x268]            ; 1000:079c
    OR AL,byte ptr CS:[0x269]            ; 1000:07a1
    JZ 0x1000:07ae                       ; 1000:07a6 ; -> 1000:07ae
    MOV byte ptr CS:[0x61],0xff          ; 1000:07a8
    MOV AL,CS:[0x61]                     ; 1000:07ae
    CMP byte ptr CS:[0x251],AL           ; 1000:07b2
    JNZ 0x1000:07c3                      ; 1000:07b7 ; -> 1000:07c3
    MOV byte ptr CS:[0x61],0x0           ; 1000:07b9
    MOV AX,0x0                           ; 1000:07bf
    INT3                                 ; 1000:07c2
    CMP byte ptr CS:[0x252],AL           ; 1000:07c3
    JNZ 0x1000:07d4                      ; 1000:07c8 ; -> 1000:07d4
    MOV byte ptr CS:[0x61],0x0           ; 1000:07ca
    MOV AX,0x2                           ; 1000:07d0
    INT3                                 ; 1000:07d3
    XOR AL,AL                            ; 1000:07d4
    MOV CS:[0x246],AL                    ; 1000:07d6
    MOV AX,CS:[0x24e]                    ; 1000:07da
    MOV ES,AX                            ; 1000:07de
    CMP byte ptr CS:[0x247],0x1          ; 1000:07e0
    JNZ 0x1000:0843                      ; 1000:07e6 ; -> 1000:0843
    DEC word ptr CS:[0x24c]              ; 1000:07e8
    JG 0x1000:0849                       ; 1000:07ed ; -> 1000:0849
    MOV BX,word ptr CS:[0x24a]           ; 1000:07ef
    ADD word ptr CS:[0x24a],0x2          ; 1000:07f4
    MOV AX,word ptr ES:[BX]              ; 1000:07fa
    OR AX,AX                             ; 1000:07fd
    JZ 0x1000:0837                       ; 1000:07ff ; -> 1000:0837
    MOV BX,AX                            ; 1000:0801
    IN AL,0x61                           ; 1000:0803
    AND AL,0xfc                          ; 1000:0805
    OUT 0x61,AL                          ; 1000:0807
    MOV AL,0xb6                          ; 1000:0809
    OUT 0x43,AL                          ; 1000:080b
    MOV AL,BL                            ; 1000:080d
    OUT 0x42,AL                          ; 1000:080f
    MOV AL,BH                            ; 1000:0811
    OUT 0x42,AL                          ; 1000:0813
    MOV BX,word ptr CS:[0x24a]           ; 1000:0815
    ADD word ptr CS:[0x24a],0x2          ; 1000:081a
    MOV AX,word ptr ES:[BX]              ; 1000:0820
    MOV CS:[0x24c],AX                    ; 1000:0823
    CMP byte ptr CS:[0x248],0x1          ; 1000:0827
    JNZ 0x1000:0849                      ; 1000:082d ; -> 1000:0849
    IN AL,0x61                           ; 1000:082f
    OR AL,0x3                            ; 1000:0831
    OUT 0x61,AL                          ; 1000:0833
    JMP 0x1000:0849                      ; 1000:0835 ; -> 1000:0849
    MOV byte ptr CS:[0x247],0x0          ; 1000:0837
    MOV byte ptr CS:[0x249],0x0          ; 1000:083d
    IN AL,0x61                           ; 1000:0843
    AND AL,0xfc                          ; 1000:0845
    OUT 0x61,AL                          ; 1000:0847
    CMP byte ptr CS:[0x22a],0x0          ; 1000:0849
    JZ 0x1000:08b9                       ; 1000:084f ; -> 1000:08b9
    MOV BX,word ptr CS:[0x278]           ; 1000:0851
    OR BX,BX                             ; 1000:0856
    JZ 0x1000:08b9                       ; 1000:0858 ; -> 1000:08b9
    PUSH DS                              ; 1000:085a
    PUSH SI                              ; 1000:085b
    CLD                                  ; 1000:085c
    MOV SI,BX                            ; 1000:085d
    MOV AX,CS                            ; 1000:085f
    MOV DS,AX                            ; 1000:0861
    MOV DX,0x3da                         ; 1000:0863
    IN AL,DX                             ; 1000:0866
    TEST AL,0x8                          ; 1000:0867
    JZ 0x1000:0866                       ; 1000:0869 ; -> 1000:0866
    MOV DX,0x3c0                         ; 1000:086b
    LODSW SI                             ; 1000:086e
    MOV CX,AX                            ; 1000:086f
    PUSH CX                              ; 1000:0871
    LODSW SI                             ; 1000:0872
    OR AX,AX                             ; 1000:0873
    JZ 0x1000:0880                       ; 1000:0875 ; -> 1000:0880
    DEC AX                               ; 1000:0877
    MOV word ptr [SI + -0x2],AX          ; 1000:0878
    ADD SI,0x6                           ; 1000:087b
    JMP 0x1000:08b1                      ; 1000:087e ; -> 1000:08b1
    LODSW SI                             ; 1000:0880
    MOV BX,word ptr [SI]                 ; 1000:0881
    ADD SI,0x2                           ; 1000:0883
    ADD BX,AX                            ; 1000:0886
    MOV CL,byte ptr [BX]                 ; 1000:0888
    CMP CL,0x18                          ; 1000:088a
    JNZ 0x1000:089e                      ; 1000:088d ; -> 1000:089e
    MOV CL,byte ptr [BX + 0x1]           ; 1000:088f
    SUB BX,AX                            ; 1000:0892
    XOR AX,AX                            ; 1000:0894
    MOV CH,0x0                           ; 1000:0896
    ADD AX,CX                            ; 1000:0898
    ADD BX,CX                            ; 1000:089a
    MOV CL,byte ptr [BX]                 ; 1000:089c
    ADD AX,0x2                           ; 1000:089e
    MOV word ptr [SI + -0x4],AX          ; 1000:08a1
    MOV AL,byte ptr [BX + 0x1]           ; 1000:08a4
    XOR AH,AH                            ; 1000:08a7
    MOV word ptr [SI + -0x6],AX          ; 1000:08a9
    LODSW SI                             ; 1000:08ac
    OUT DX,AL                            ; 1000:08ad
    MOV AL,CL                            ; 1000:08ae
    OUT DX,AL                            ; 1000:08b0
    POP CX                               ; 1000:08b1
    LOOP 0x1000:0871                     ; 1000:08b2 ; -> 1000:0871
    POP SI                               ; 1000:08b4
    POP DS                               ; 1000:08b5
    MOV AL,0x20                          ; 1000:08b6
    OUT DX,AL                            ; 1000:08b8
    POP ES                               ; 1000:08b9
    POP DX                               ; 1000:08ba
    POP CX                               ; 1000:08bb
    POP BX                               ; 1000:08bc
    POP AX                               ; 1000:08bd
    JMPF CS:[0x67f]                      ; 1000:08be

; ------------------------------------------------------------------------------
; FUNCTION: snd_api_int03  [Audio / API]
; ADDRESS:  1000:08c7  (Length: 0x6f bytes)
; PURPOSE:  Sound Engine API dispatcher hooked to INT 03h. Accepts commands: AL=0 (stop sound), AL=1 (play SFX at BX with priority CL), AL=2 (query/stop music), AL=3 (toggle sound).
; ------------------------------------------------------------------------------
snd_api_int03:
    CMP AL,0x0                           ; 1000:08c7
    JZ 0x1000:0907                       ; 1000:08c9 ; -> 1000:0907
    CMP AL,0x1                           ; 1000:08cb
    JZ 0x1000:08df                       ; 1000:08cd ; -> 1000:08df
    CMP AL,0x2                           ; 1000:08cf
    JZ 0x1000:090f                       ; 1000:08d1 ; -> 1000:090f
    CMP AL,0x3                           ; 1000:08d3
    JZ 0x1000:0917                       ; 1000:08d5 ; -> 1000:0917
    CMP AL,0x4                           ; 1000:08d7
    JZ 0x1000:0927                       ; 1000:08d9 ; -> 1000:0927
    XOR AL,0xff                          ; 1000:08db
    JMP 0x1000:0935                      ; 1000:08dd ; -> 1000:0935
    CMP byte ptr CS:[0x249],CL           ; 1000:08df
    JG 0x1000:0935                       ; 1000:08e4 ; -> 1000:0935
    MOV byte ptr CS:[0x249],CL           ; 1000:08e6
    CLI                                  ; 1000:08eb
    MOV byte ptr CS:[0x247],0x1          ; 1000:08ec
    MOV word ptr CS:[0x24a],BX           ; 1000:08f2
    MOV AX,DS                            ; 1000:08f7
    MOV CS:[0x24e],AX                    ; 1000:08f9
    MOV word ptr CS:[0x24c],0x1          ; 1000:08fd
    STI                                  ; 1000:0904
    JMP 0x1000:0935                      ; 1000:0905 ; -> 1000:0935
    MOV byte ptr CS:[0x248],0x1          ; 1000:0907
    JMP 0x1000:092f                      ; 1000:090d ; -> 1000:092f
    MOV byte ptr CS:[0x248],0x0          ; 1000:090f
    JMP 0x1000:092f                      ; 1000:0915 ; -> 1000:092f
    CLI                                  ; 1000:0917
    MOV byte ptr CS:[0x247],0x0          ; 1000:0918
    MOV byte ptr CS:[0x249],0x0          ; 1000:091e
    STI                                  ; 1000:0924
    JMP 0x1000:092f                      ; 1000:0925 ; -> 1000:092f
    MOV AL,CS:[0x247]                    ; 1000:0927
    XOR AH,AH                            ; 1000:092b
    JMP 0x1000:0935                      ; 1000:092d ; -> 1000:0935
    IN AL,0x61                           ; 1000:092f
    AND AL,0xfc                          ; 1000:0931
    OUT 0x61,AL                          ; 1000:0933
    IRET                                 ; 1000:0935

; ------------------------------------------------------------------------------
; FUNCTION: isr_joystick_int15  [Interrupts / Input]
; ADDRESS:  1000:093a  (Length: 0x39 bytes)
; PURPOSE:  Custom BIOS INT 15h AH=84h Joystick handler. Directly polls game port 0x201 for button states and axis potentiometer discharge timing.
; ------------------------------------------------------------------------------
isr_joystick_int15:
    CMP AH,0x84                          ; 1000:093a
    JZ 0x1000:0942                       ; 1000:093d ; -> 1000:0942
    JMP 0x1000:09c4                      ; 1000:093f ; -> 1000:09c4
    CMP DL,0x1                           ; 1000:0942
    JG 0x1000:096e                       ; 1000:0945 ; -> 1000:096e
    MOV DX,0x201                         ; 1000:0947
    JZ 0x1000:0952                       ; 1000:094a ; -> 1000:0952
    IN AL,DX                             ; 1000:094c
    AND AL,0xf0                          ; 1000:094d
    JMP 0x1000:096e                      ; 1000:094f ; -> 1000:096e
    MOV BL,0x1                           ; 1000:0952
    CALL 0x1000:096f                     ; 1000:0954 ; -> 1000:096f
    PUSH CX                              ; 1000:0957
    SHL BL,0x1                           ; 1000:0958
    CALL 0x1000:096f                     ; 1000:095a ; -> 1000:096f
    PUSH CX                              ; 1000:095d
    SHL BL,0x1                           ; 1000:095e
    CALL 0x1000:096f                     ; 1000:0960 ; -> 1000:096f
    PUSH CX                              ; 1000:0963
    SHL BL,0x1                           ; 1000:0964
    CALL 0x1000:096f                     ; 1000:0966 ; -> 1000:096f
    MOV DX,CX                            ; 1000:0969
    POP CX                               ; 1000:096b
    POP BX                               ; 1000:096c
    POP AX                               ; 1000:096d
    IRET                                 ; 1000:096e
    JMPF CS:[0x936]                      ; 1000:09c4

; ------------------------------------------------------------------------------
; FUNCTION: sys_install_interrupts  [System / Interrupts]
; ADDRESS:  1000:09c9  (Length: 0x95 bytes)
; CALLERS:  entry
; PURPOSE:  Installs custom interrupt handlers by hooking interrupt vectors for INT 08h, INT 09h, INT 15h, INT 23h, and INT 03h.
; ------------------------------------------------------------------------------
sys_install_interrupts:
    PUSH DS                              ; 1000:09c9
    XOR AX,AX                            ; 1000:09ca
    MOV DS,AX                            ; 1000:09cc
    CLI                                  ; 1000:09ce
    LEA BX,[0x5be]                       ; 1000:09cf
    MOV AX,[0x26]                        ; 1000:09d3
    MOV word ptr CS:[BX + 0x2],AX        ; 1000:09d6 ; -> 1000:05c0
    MOV AX,[0x24]                        ; 1000:09da
    MOV word ptr CS:[BX],AX              ; 1000:09dd ; -> 1000:05be
    LEA AX,[0x5c2]                       ; 1000:09e0
    MOV [0x24],AX                        ; 1000:09e4
    MOV word ptr [0x26],CS               ; 1000:09e7
    LEA BX,[0x67f]                       ; 1000:09eb
    MOV AX,[0x22]                        ; 1000:09ef
    MOV word ptr CS:[BX + 0x2],AX        ; 1000:09f2 ; -> 1000:0681
    MOV AX,[0x20]                        ; 1000:09f6
    MOV word ptr CS:[BX],AX              ; 1000:09f9 ; -> 1000:067f
    LEA AX,[0x683]                       ; 1000:09fc
    MOV [0x20],AX                        ; 1000:0a00
    MOV word ptr [0x22],CS               ; 1000:0a03
    LEA BX,[0x678]                       ; 1000:0a07
    MOV AX,[0x8e]                        ; 1000:0a0b
    MOV word ptr CS:[BX + 0x2],AX        ; 1000:0a0e ; -> 1000:067a
    MOV AX,[0x8c]                        ; 1000:0a12
    MOV word ptr CS:[BX],AX              ; 1000:0a15 ; -> 1000:0678
    LEA AX,[0x67c]                       ; 1000:0a18
    MOV [0x8c],AX                        ; 1000:0a1c
    MOV word ptr [0x8e],CS               ; 1000:0a1f
    LEA BX,[0x8c3]                       ; 1000:0a23
    MOV AX,[0xe]                         ; 1000:0a27
    MOV word ptr CS:[BX + 0x2],AX        ; 1000:0a2a ; -> 1000:08c5
    MOV AX,[0xc]                         ; 1000:0a2e
    MOV word ptr CS:[BX],AX              ; 1000:0a31 ; -> 1000:08c3
    LEA AX,[0x8c7]                       ; 1000:0a34
    MOV [0xc],AX                         ; 1000:0a38
    MOV word ptr [0xe],CS                ; 1000:0a3b
    LEA BX,[0x936]                       ; 1000:0a3f
    MOV AX,[0x56]                        ; 1000:0a43
    MOV word ptr CS:[BX + 0x2],AX        ; 1000:0a46 ; -> 1000:0938
    MOV AX,[0x54]                        ; 1000:0a4a
    MOV word ptr CS:[BX],AX              ; 1000:0a4d ; -> 1000:0936
    LEA AX,[0x93a]                       ; 1000:0a50
    MOV [0x54],AX                        ; 1000:0a54
    MOV word ptr [0x56],CS               ; 1000:0a57
    STI                                  ; 1000:0a5b
    POP DS                               ; 1000:0a5c
    RET                                  ; 1000:0a5d

; ------------------------------------------------------------------------------
; FUNCTION: sys_restore_all_interrupts  [System / Interrupts]
; ADDRESS:  1000:0a5e  (Length: 0x5e bytes)
; PURPOSE:  Restores original BIOS and DOS interrupt vectors (INT 08h, 09h, 15h, 23h, 03h) from saved CS pointers.
; ------------------------------------------------------------------------------
sys_restore_all_interrupts:
    PUSH DS                              ; 1000:0a5e
    XOR AX,AX                            ; 1000:0a5f
    MOV DS,AX                            ; 1000:0a61
    CLI                                  ; 1000:0a63
    LEA BX,[0x5be]                       ; 1000:0a64
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:0a68
    MOV [0x26],AX                        ; 1000:0a6c
    MOV AX,word ptr CS:[BX]              ; 1000:0a6f
    MOV [0x24],AX                        ; 1000:0a72
    LEA BX,[0x67f]                       ; 1000:0a75
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:0a79
    MOV [0x22],AX                        ; 1000:0a7d
    MOV AX,word ptr CS:[BX]              ; 1000:0a80
    MOV [0x20],AX                        ; 1000:0a83
    LEA BX,[0x678]                       ; 1000:0a86
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:0a8a
    MOV [0x8e],AX                        ; 1000:0a8e
    MOV AX,word ptr CS:[BX]              ; 1000:0a91
    MOV [0x8c],AX                        ; 1000:0a94
    LEA BX,[0x8c3]                       ; 1000:0a97
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:0a9b
    MOV [0xe],AX                         ; 1000:0a9f
    MOV AX,word ptr CS:[BX]              ; 1000:0aa2
    MOV [0xc],AX                         ; 1000:0aa5
    LEA BX,[0x936]                       ; 1000:0aa8
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:0aac
    MOV [0x56],AX                        ; 1000:0ab0
    MOV AX,word ptr CS:[BX]              ; 1000:0ab3
    MOV [0x54],AX                        ; 1000:0ab6
    STI                                  ; 1000:0ab9
    POP DS                               ; 1000:0aba
    RET                                  ; 1000:0abb

; ------------------------------------------------------------------------------
; FUNCTION: sys_detect_cpu  [System / CPU]
; ADDRESS:  1000:0abc  (Length: 0x27 bytes)
; PURPOSE:  Detects CPU architecture (0=8086/8088, 1=80286, 2=80386+) using FLAGS bit 12-15 probing.
; ------------------------------------------------------------------------------
sys_detect_cpu:
    PUSHF                                ; 1000:0abc
    XOR AX,AX                            ; 1000:0abd
    PUSH AX                              ; 1000:0abf
    POPF                                 ; 1000:0ac0
    PUSHF                                ; 1000:0ac1
    POP AX                               ; 1000:0ac2
    SUB AX,0xf000                        ; 1000:0ac3
    JNC .is_8086                         ; 1000:0ac6
    MOV BX,0xf000                        ; 1000:0ac8
    PUSH BX                              ; 1000:0acb
    POPF                                 ; 1000:0acc
    PUSHF                                ; 1000:0acd
    POP BX                               ; 1000:0ace
    TEST BX,0xf000                       ; 1000:0acf
    JZ .is_286                           ; 1000:0ad3
    MOV AX,0x2                           ; 1000:0ad5
    JMP .done                            ; 1000:0ad8
.is_286:
    MOV AX,0x1                           ; 1000:0ada
    JMP .done                            ; 1000:0add
.is_8086:
    XOR AX,AX                            ; 1000:0adf
.done:
    POPF                                 ; 1000:0ae1
    RET                                  ; 1000:0ae2

; ------------------------------------------------------------------------------
; FUNCTION: spr_render_player  [Sprites / Player]
; ADDRESS:  1000:18e0  (Length: 0xae bytes)
; CALLERS:  FUN_1000_21b8, FUN_1000_227b, FUN_1000_20fe
; PURPOSE:  Renders Captain Comic sprite at coordinates (X, Y) using planar EGA masking, selected animation frame, and directional facing.
; ------------------------------------------------------------------------------
spr_render_player:
    MOV AL,[0x8ad]                       ; 1000:18e0
    MOV [0x218],AL                       ; 1000:18e3
    MOV AL,[0x89d]                       ; 1000:18e6
    MOV [0x219],AL                       ; 1000:18e9
    MOV AX,[0x6c6]                       ; 1000:18ec
    MOV [0x210],AX                       ; 1000:18ef
    SUB AX,0x60                          ; 1000:18f2
    JNC 0x1000:18f9                      ; 1000:18f5 ; -> 1000:18f9
    XOR AX,AX                            ; 1000:18f7
    MOV BX,word ptr [0x29c]              ; 1000:18f9
    SUB BX,0xd0                          ; 1000:18fd
    CMP AX,BX                            ; 1000:1901
    JLE 0x1000:1907                      ; 1000:1903 ; -> 1000:1907
    MOV AX,BX                            ; 1000:1905
    MOV [0x6c2],AX                       ; 1000:1907
    MOV AX,[0x6c8]                       ; 1000:190a
    MOV [0x212],AX                       ; 1000:190d
    SUB AX,0x40                          ; 1000:1910
    JNC 0x1000:1917                      ; 1000:1913 ; -> 1000:1917
    XOR AX,AX                            ; 1000:1915
    MOV BX,word ptr [0x29e]              ; 1000:1917
    SUB BX,0xa0                          ; 1000:191b
    CMP AX,BX                            ; 1000:191f
    JLE 0x1000:1925                      ; 1000:1921 ; -> 1000:1925
    MOV AX,BX                            ; 1000:1923
    MOV [0x6c4],AX                       ; 1000:1925
    CMP word ptr [0x6fa],0x2             ; 1000:1928
    JNZ 0x1000:1959                      ; 1000:192d ; -> 1000:1959
    CMP word ptr [0x6fc],0x1             ; 1000:192f
    JNZ 0x1000:1959                      ; 1000:1934 ; -> 1000:1959
    CMP word ptr [0x6c6],0x0             ; 1000:1936
    JNZ 0x1000:1959                      ; 1000:193b ; -> 1000:1959
    MOV byte ptr [0x924],0x2             ; 1000:193d
    MOV word ptr [0x928],0x0             ; 1000:1942
    MOV word ptr [0x92a],0x20            ; 1000:1948
    MOV byte ptr [0x925],0x2             ; 1000:194e
    MOV word ptr [0x926],0x8             ; 1000:1953
    CMP word ptr [0x6fa],0x9             ; 1000:1959
    JNZ 0x1000:198a                      ; 1000:195e ; -> 1000:198a
    CMP word ptr [0x6fc],0x1             ; 1000:1960
    JNZ 0x1000:198a                      ; 1000:1965 ; -> 1000:198a
    CMP word ptr [0x6c6],0x0             ; 1000:1967
    JNZ 0x1000:198a                      ; 1000:196c ; -> 1000:198a
    MOV byte ptr [0x92c],0x2             ; 1000:196e
    MOV word ptr [0x930],0x0             ; 1000:1973
    MOV word ptr [0x932],0xc0            ; 1000:1979
    MOV byte ptr [0x92d],0x0             ; 1000:197f
    MOV word ptr [0x92e],0x0             ; 1000:1984
    CALL 0x1000:49ad                     ; 1000:198a ; -> 1000:49ad
    RET                                  ; 1000:198d

; ------------------------------------------------------------------------------
; FUNCTION: map_init_tileset  [Tilemap / Level]
; ADDRESS:  1000:198e  (Length: 0xd5 bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Initializes room tile attributes, copies tileset graphics into offscreen EGA memory, and configures tile properties.
; ------------------------------------------------------------------------------
map_init_tileset:
    LEA SI,[0x16a8]                      ; 1000:198e
    MOV AX,[0x6fa]                       ; 1000:1992
    SHL AX,0x1                           ; 1000:1995
    ADD SI,AX                            ; 1000:1997
    MOV SI,word ptr CS:[SI]              ; 1000:1999
    MOV BX,word ptr CS:[SI]              ; 1000:199c
    CMP word ptr [0x6fa],0x5             ; 1000:199f
    JNZ 0x1000:19b0                      ; 1000:19a4 ; -> 1000:19b0
    CMP byte ptr [0x220],0x5             ; 1000:19a6
    JL 0x1000:19b0                       ; 1000:19ab ; -> 1000:19b0
    MOV BX,0x16c6                        ; 1000:19ad
    MOV word ptr CS:[0x278],BX           ; 1000:19b0 ; -> 1000:0278
    MOV word ptr CS:[BX + 0x2],0x0       ; 1000:19b5 ; -> 1000:16c8
    MOV word ptr CS:[BX + 0x4],0x0       ; 1000:19bb ; -> 1000:16ca
    MOV word ptr CS:[BX + 0xa],0x0       ; 1000:19c1 ; -> 1000:16d0
    MOV word ptr CS:[BX + 0xc],0x0       ; 1000:19c7 ; -> 1000:16d2
    MOV word ptr CS:[BX + 0x12],0x0      ; 1000:19cd ; -> 1000:16d8
    MOV word ptr CS:[BX + 0x14],0x0      ; 1000:19d3 ; -> 1000:16da
    MOV BX,word ptr CS:[SI + 0x2]        ; 1000:19d9
    MOV AH,byte ptr CS:[BX]              ; 1000:19dd
    MOV byte ptr [0x6ca],AH              ; 1000:19e0
    MOV AL,byte ptr CS:[BX + 0x1]        ; 1000:19e4
    MOV [0x6cb],AL                       ; 1000:19e8
    SUB AL,AH                            ; 1000:19eb
    ADD BX,0x2                           ; 1000:19ed
    LEA DI,[0x6cc]                       ; 1000:19f0
    INC AL                               ; 1000:19f4
    MOV CL,byte ptr CS:[BX]              ; 1000:19f6
    INC BX                               ; 1000:19f9
    MOV byte ptr [DI],CL                 ; 1000:19fa
    INC DI                               ; 1000:19fc
    DEC AL                               ; 1000:19fd
    JNZ 0x1000:19f6                      ; 1000:19ff ; -> 1000:19f6
    MOV BX,word ptr CS:[SI + 0x4]        ; 1000:1a01
    MOV AH,byte ptr CS:[BX]              ; 1000:1a05
    MOV byte ptr [0x6d6],AH              ; 1000:1a08
    MOV AL,byte ptr CS:[BX + 0x1]        ; 1000:1a0c
    MOV [0x6d7],AL                       ; 1000:1a10
    SUB AL,AH                            ; 1000:1a13
    ADD BX,0x2                           ; 1000:1a15
    LEA DI,[0x6d8]                       ; 1000:1a18
    INC AL                               ; 1000:1a1c
    MOV CL,byte ptr CS:[BX]              ; 1000:1a1e
    INC BX                               ; 1000:1a21
    MOV byte ptr [DI],CL                 ; 1000:1a22
    INC DI                               ; 1000:1a24
    DEC AL                               ; 1000:1a25
    JNZ 0x1000:1a1e                      ; 1000:1a27 ; -> 1000:1a1e
    MOV BX,word ptr CS:[SI + 0x6]        ; 1000:1a29
    MOV AH,byte ptr CS:[BX]              ; 1000:1a2d
    MOV byte ptr [0x6e2],AH              ; 1000:1a30
    MOV AL,byte ptr CS:[BX + 0x1]        ; 1000:1a34
    MOV [0x6e3],AL                       ; 1000:1a38
    SUB AL,AH                            ; 1000:1a3b
    ADD BX,0x2                           ; 1000:1a3d
    LEA DI,[0x6e4]                       ; 1000:1a40
    INC AL                               ; 1000:1a44
    MOV CL,byte ptr CS:[BX]              ; 1000:1a46
    INC BX                               ; 1000:1a49
    MOV byte ptr [DI],CL                 ; 1000:1a4a
    INC DI                               ; 1000:1a4c
    DEC AL                               ; 1000:1a4d
    JNZ 0x1000:1a46                      ; 1000:1a4f ; -> 1000:1a46
    MOV byte ptr [0x93a],0x0             ; 1000:1a51
    CMP word ptr [0x6fa],0xc             ; 1000:1a56
    JNZ 0x1000:1a62                      ; 1000:1a5b ; -> 1000:1a62
    MOV byte ptr [0x93a],0x1             ; 1000:1a5d
    RET                                  ; 1000:1a62

; ------------------------------------------------------------------------------
; FUNCTION: map_spawn_room_entities  [Entities / Spawning]
; ADDRESS:  1000:1a63  (Length: 0x12a bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Parses room entity definitions from file buffer and initializes active monster, item, and hazard slots.
; ------------------------------------------------------------------------------
map_spawn_room_entities:
    LEA SI,[0x16a8]                      ; 1000:1a63
    MOV AX,[0x6fa]                       ; 1000:1a67
    SHL AX,0x1                           ; 1000:1a6a
    ADD SI,AX                            ; 1000:1a6c
    MOV SI,word ptr CS:[SI]              ; 1000:1a6e
    MOV BX,word ptr CS:[SI + 0x8]        ; 1000:1a71
    MOV AX,[0x6fc]                       ; 1000:1a75
    SHL AX,0x1                           ; 1000:1a78
    ADD BX,AX                            ; 1000:1a7a
    MOV BX,word ptr CS:[BX]              ; 1000:1a7c
    MOV AX,word ptr CS:[BX]              ; 1000:1a7f
    MOV [0x6ee],AX                       ; 1000:1a82
    ADD BX,0x2                           ; 1000:1a85
    MOV word ptr [0x6f0],BX              ; 1000:1a88
    MOV BX,word ptr CS:[SI + 0xa]        ; 1000:1a8c
    MOV AX,[0x6fc]                       ; 1000:1a90
    SHL AX,0x1                           ; 1000:1a93
    ADD BX,AX                            ; 1000:1a95
    MOV BX,word ptr CS:[BX]              ; 1000:1a97
    MOV AX,word ptr CS:[BX]              ; 1000:1a9a
    MOV [0x6f2],AX                       ; 1000:1a9d
    ADD BX,0x2                           ; 1000:1aa0
    MOV word ptr [0x6f4],BX              ; 1000:1aa3
    MOV BX,word ptr CS:[SI + 0xc]        ; 1000:1aa7
    MOV AX,[0x6fc]                       ; 1000:1aab
    SHL AX,0x1                           ; 1000:1aae
    ADD BX,AX                            ; 1000:1ab0
    MOV BX,word ptr CS:[BX]              ; 1000:1ab2
    MOV AX,word ptr CS:[BX]              ; 1000:1ab5
    MOV [0x6f6],AX                       ; 1000:1ab8
    ADD BX,0x2                           ; 1000:1abb
    MOV word ptr [0x6f8],BX              ; 1000:1abe
    LEA SI,[0xaf0]                       ; 1000:1ac2
    LEA DI,[0x700]                       ; 1000:1ac6
    XOR CX,CX                            ; 1000:1aca
    MOV word ptr [0x6fe],0x0             ; 1000:1acc
    MOV BX,word ptr [0x6fa]              ; 1000:1ad2
    MOV DX,word ptr [0x6fc]              ; 1000:1ad6
    CMP BX,word ptr CS:[SI]              ; 1000:1ada ; -> 1000:0af0
    JNZ 0x1000:1b0d                      ; 1000:1add ; -> 1000:1b0d
    CMP DX,word ptr CS:[SI + 0x2]        ; 1000:1adf ; -> 1000:0af2
    JNZ 0x1000:1b0d                      ; 1000:1ae3 ; -> 1000:1b0d
    MOV AX,word ptr CS:[SI + 0x4]        ; 1000:1ae5 ; -> 1000:0af4
    MOV word ptr [DI],AX                 ; 1000:1ae9
    MOV AX,word ptr CS:[SI + 0x6]        ; 1000:1aeb ; -> 1000:0af6
    MOV word ptr [DI + 0x2],AX           ; 1000:1aef
    MOV AX,word ptr CS:[SI + 0x8]        ; 1000:1af2 ; -> 1000:0af8
    MOV word ptr [DI + 0x6],AX           ; 1000:1af6
    MOV word ptr [DI + 0x4],CX           ; 1000:1af9
    MOV AX,word ptr CS:[SI + 0xa]        ; 1000:1afc ; -> 1000:0afa
    AND AX,0x8000                        ; 1000:1b00
    OR word ptr [DI + 0x4],AX            ; 1000:1b03
    INC word ptr [0x6fe]                 ; 1000:1b06
    ADD DI,0x8                           ; 1000:1b0a
    INC CX                               ; 1000:1b0d
    ADD SI,0xc                           ; 1000:1b0e
    CMP CX,word ptr [0x940]              ; 1000:1b11
    JNZ 0x1000:1ada                      ; 1000:1b15 ; -> 1000:1ada
    MOV SI,0x8ee                         ; 1000:1b17
    MOV CX,0x6                           ; 1000:1b1a
    MOV word ptr [SI],0xffff             ; 1000:1b1d
    ADD SI,0x8                           ; 1000:1b21
    LOOP 0x1000:1b1d                     ; 1000:1b24 ; -> 1000:1b1d
    MOV byte ptr [0x8b1],0x0             ; 1000:1b26
    MOV SI,0x8b2                         ; 1000:1b2b
    MOV CX,0x6                           ; 1000:1b2e
    MOV byte ptr [SI],0x0                ; 1000:1b31
    ADD SI,0x9                           ; 1000:1b34
    LOOP 0x1000:1b31                     ; 1000:1b37 ; -> 1000:1b31
    MOV byte ptr [0x924],0x0             ; 1000:1b39
    CMP word ptr [0x6fa],0x2             ; 1000:1b3e
    JNZ 0x1000:1b63                      ; 1000:1b43 ; -> 1000:1b63
    CMP word ptr [0x6fc],0x0             ; 1000:1b45
    JNZ 0x1000:1b63                      ; 1000:1b4a ; -> 1000:1b63
    MOV byte ptr [0x924],0x1             ; 1000:1b4c
    MOV word ptr [0x928],0xb0            ; 1000:1b51
    MOV word ptr [0x92a],0xe0            ; 1000:1b57
    MOV word ptr [0x926],0x0             ; 1000:1b5d
    MOV byte ptr [0x92c],0x0             ; 1000:1b63
    CMP word ptr [0x6fa],0x9             ; 1000:1b68
    JNZ 0x1000:1b87                      ; 1000:1b6d ; -> 1000:1b87
    CMP word ptr [0x6fc],0x0             ; 1000:1b6f
    JNZ 0x1000:1b87                      ; 1000:1b74 ; -> 1000:1b87
    MOV byte ptr [0x92c],0x1             ; 1000:1b76
    MOV word ptr [0x930],0x558           ; 1000:1b7b
    MOV word ptr [0x932],0x20            ; 1000:1b81
    MOV byte ptr [0x8a8],0x0             ; 1000:1b87
    RET                                  ; 1000:1b8c

; ------------------------------------------------------------------------------
; FUNCTION: map_load_room  [Tilemap / Streaming]
; ADDRESS:  1000:1b8d  (Length: 0x6e bytes)
; CALLERS:  FUN_1000_2253, FUN_1000_21b8, FUN_1000_227b, FUN_1000_2323, FUN_1000_20fe, FUN_1000_409c
; PURPOSE:  Loads room data package for specified room index: streams layout (.0), collision masks (.1), tileset (.2), and entities (.3).
; ------------------------------------------------------------------------------
map_load_room:
    CMP word ptr [0x6fa],AX              ; 1000:1b8d
    JZ 0x1000:1be2                       ; 1000:1b91 ; -> 1000:1be2
    CALL 0x1000:1c0b                     ; 1000:1b93 ; -> 1000:1c0b
    CALL 0x1000:022b                     ; 1000:1b96 ; -> 1000:022b
    CMP BX,-0x1                          ; 1000:1b99
    JZ 0x1000:1ba4                       ; 1000:1b9c ; -> 1000:1ba4
    MOV word ptr [0x6fc],0xffff          ; 1000:1b9e
    MOV [0x6fa],AX                       ; 1000:1ba4
    PUSH AX                              ; 1000:1ba7
    PUSH BX                              ; 1000:1ba8
    LEA BX,[0x93ec]                      ; 1000:1ba9
    SHL AX,0x1                           ; 1000:1bad
    ADD BX,AX                            ; 1000:1baf
    MOV BX,word ptr [BX]                 ; 1000:1bb1
    MOV DX,word ptr [BX]                 ; 1000:1bb3
    ADD BX,0x2                           ; 1000:1bb5
    PUSH BX                              ; 1000:1bb8
    CALL 0x1000:1fa4                     ; 1000:1bb9 ; -> 1000:1fa4
    POP BX                               ; 1000:1bbc
    MOV DX,word ptr [BX]                 ; 1000:1bbd
    ADD BX,0x2                           ; 1000:1bbf
    PUSH BX                              ; 1000:1bc2
    CALL 0x1000:1ffe                     ; 1000:1bc3 ; -> 1000:1ffe
    POP BX                               ; 1000:1bc6
    MOV DX,word ptr [BX]                 ; 1000:1bc7
    ADD BX,0x2                           ; 1000:1bc9
    PUSH BX                              ; 1000:1bcc
    CALL 0x1000:2022                     ; 1000:1bcd ; -> 1000:2022
    POP BX                               ; 1000:1bd0
    MOV DX,word ptr [BX]                 ; 1000:1bd1
    OR DX,DX                             ; 1000:1bd3
    JZ 0x1000:1bda                       ; 1000:1bd5 ; -> 1000:1bda
    CALL 0x1000:2049                     ; 1000:1bd7 ; -> 1000:2049
    CALL 0x1000:198e                     ; 1000:1bda ; -> 1000:198e
    CALL 0x1000:0238                     ; 1000:1bdd ; -> 1000:0238
    POP BX                               ; 1000:1be0
    POP AX                               ; 1000:1be1
    CMP word ptr [0x6fc],BX              ; 1000:1be2
    JZ 0x1000:1bfa                       ; 1000:1be6 ; -> 1000:1bfa
    CMP BX,-0x1                          ; 1000:1be8
    JZ 0x1000:1bfa                       ; 1000:1beb ; -> 1000:1bfa
    MOV word ptr [0x6fc],BX              ; 1000:1bed
    CALL 0x1000:2072                     ; 1000:1bf1 ; -> 1000:2072
    CALL 0x1000:1e86                     ; 1000:1bf4 ; -> 1000:1e86
    CALL 0x1000:1a63                     ; 1000:1bf7 ; -> 1000:1a63
    RET                                  ; 1000:1bfa

; ------------------------------------------------------------------------------
; FUNCTION: util_calc_stride  [Math / Geometry]
; ADDRESS:  1000:1bfb  (Length: 0x10 bytes)
; CALLERS:  FUN_1000_1c0b
; PURPOSE:  Calculates byte offset within display memory from tile grid coordinates.
; ------------------------------------------------------------------------------
util_calc_stride:
    CMP word ptr CS:[BX],-0x1            ; 1000:1bfb
    JZ 0x1000:1c07                       ; 1000:1bff ; -> 1000:1c07
    CMP word ptr CS:[BX],0x5             ; 1000:1c01
    JNZ 0x1000:1c09                      ; 1000:1c05 ; -> 1000:1c09
    CLC                                  ; 1000:1c07
    RET                                  ; 1000:1c08
    STC                                  ; 1000:1c09
    RET                                  ; 1000:1c0a

; ------------------------------------------------------------------------------
; FUNCTION: map_parse_room_header  [Tilemap / Streaming]
; ADDRESS:  1000:1c0b  (Length: 0xf3 bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Validates room data file format, parses room dimensions, and sets up boundary coordinates.
; ------------------------------------------------------------------------------
map_parse_room_header:
    PUSH AX                              ; 1000:1c0b
    PUSH BX                              ; 1000:1c0c
    PUSH BP                              ; 1000:1c0d
    MOV BP,SP                            ; 1000:1c0e
    SUB SP,0x2                           ; 1000:1c10
    CMP AX,0x5                           ; 1000:1c13
    JZ 0x1000:1c1e                       ; 1000:1c16 ; -> 1000:1c1e
    MOV SP,BP                            ; 1000:1c18
    POP BP                               ; 1000:1c1a
    POP BX                               ; 1000:1c1b
    POP AX                               ; 1000:1c1c
    RET                                  ; 1000:1c1d
    MOV BX,0x10b4                        ; 1000:1c1e
    CALL 0x1000:1bfb                     ; 1000:1c21 ; -> 1000:1bfb
    JC 0x1000:1c63                       ; 1000:1c24 ; -> 1000:1c63
    MOV BX,0x11e0                        ; 1000:1c26
    CALL 0x1000:1bfb                     ; 1000:1c29 ; -> 1000:1bfb
    JC 0x1000:1c63                       ; 1000:1c2c ; -> 1000:1c63
    MOV BX,0x12a0                        ; 1000:1c2e
    CALL 0x1000:1bfb                     ; 1000:1c31 ; -> 1000:1bfb
    JC 0x1000:1c63                       ; 1000:1c34 ; -> 1000:1c63
    MOV BX,0x1324                        ; 1000:1c36
    CALL 0x1000:1bfb                     ; 1000:1c39 ; -> 1000:1bfb
    JC 0x1000:1c63                       ; 1000:1c3c ; -> 1000:1c63
    MOV BX,0x14f8                        ; 1000:1c3e
    CALL 0x1000:1bfb                     ; 1000:1c41 ; -> 1000:1bfb
    JC 0x1000:1c63                       ; 1000:1c44 ; -> 1000:1c63
    MOV BX,0x1558                        ; 1000:1c46
    CALL 0x1000:1bfb                     ; 1000:1c49 ; -> 1000:1bfb
    JC 0x1000:1c63                       ; 1000:1c4c ; -> 1000:1c63
    CMP byte ptr [0x220],0x5             ; 1000:1c4e
    JGE 0x1000:1c18                      ; 1000:1c53 ; -> 1000:1c18
    MOV word ptr CS:[0x93dd],0xffff      ; 1000:1c55 ; -> 1000:93dd
    MOV byte ptr [0x220],0x5             ; 1000:1c5c
    JMP 0x1000:1c18                      ; 1000:1c61 ; -> 1000:1c18
    MOV BH,0xc0                          ; 1000:1c63
    XOR BL,BL                            ; 1000:1c65
    PUSH ES                              ; 1000:1c67
    MOV ES,BX                            ; 1000:1c68
    XOR SI,SI                            ; 1000:1c6a
    XOR AX,AX                            ; 1000:1c6c
    MOV CX,0x40                          ; 1000:1c6e
    ADD AX,word ptr ES:[SI]              ; 1000:1c71
    ADD SI,0x2                           ; 1000:1c74
    LOOP 0x1000:1c71                     ; 1000:1c77 ; -> 1000:1c71
    MOV word ptr [BP + -0x2],AX          ; 1000:1c79 ; -> Stack[-0x8]
    MOV AX,ES                            ; 1000:1c7c
    ADD AH,0x30                          ; 1000:1c7e
    MOV ES,AX                            ; 1000:1c81
    MOV SI,0x8000                        ; 1000:1c83
    ADD SI,0x7000                        ; 1000:1c86
    XOR AX,AX                            ; 1000:1c8a
    XOR BX,BX                            ; 1000:1c8c
    MOV CX,0x40                          ; 1000:1c8e
    ADD AX,word ptr ES:[SI]              ; 1000:1c91 ; -> f000:f000
    ADD BX,word ptr ES:[SI + 0xf80]      ; 1000:1c94 ; -> f000:ff80
    ADD SI,0x2                           ; 1000:1c99
    LOOP 0x1000:1c91                     ; 1000:1c9c ; -> 1000:1c91
    ADD word ptr [BP + -0x2],AX          ; 1000:1c9e ; -> Stack[-0x8]
    ADD word ptr [BP + -0x2],BX          ; 1000:1ca1 ; -> Stack[-0x8]
    POP ES                               ; 1000:1ca4
    MOV AX,[0x9ce6]                      ; 1000:1ca5
    ADD AX,word ptr [BP + -0x2]          ; 1000:1ca8 ; -> Stack[-0x8]
    MOV DX,AX                            ; 1000:1cab
    SHR AX,0x1                           ; 1000:1cad
    SHR AX,0x1                           ; 1000:1caf
    SHR AX,0x1                           ; 1000:1cb1
    SHR AX,0x1                           ; 1000:1cb3
    SHR AX,0x1                           ; 1000:1cb5
    ADD DX,AX                            ; 1000:1cb7
    AND DX,0x7ff                         ; 1000:1cb9
    ADD DL,0x34                          ; 1000:1cbd
    ADC DH,0x12                          ; 1000:1cc0
    MOV AX,DX                            ; 1000:1cc3
    MOV BX,DX                            ; 1000:1cc5
    MOV CX,DX                            ; 1000:1cc7
    SHL BX,0x1                           ; 1000:1cc9
    SHL AX,0x1                           ; 1000:1ccb
    SHL BX,0x1                           ; 1000:1ccd
    SHL CX,0x1                           ; 1000:1ccf
    SHL AX,0x1                           ; 1000:1cd1
    SHL BX,0x1                           ; 1000:1cd3
    SHL AX,0x1                           ; 1000:1cd5
    SHL BX,0x1                           ; 1000:1cd7
    ADD DX,AX                            ; 1000:1cd9
    ADD DX,BX                            ; 1000:1cdb
    ADD DX,CX                            ; 1000:1cdd
    TEST DX,0x40                         ; 1000:1cdf
    JZ 0x1000:1ce7                       ; 1000:1ce3 ; -> 1000:1ce7
    NEG DX                               ; 1000:1ce5
    MOV AX,0x990                         ; 1000:1ce7
    MOV BX,0x93aa                        ; 1000:1cea
    MOV CX,BX                            ; 1000:1ced
    XOR DX,word ptr [0x9cf8]             ; 1000:1cef
    JNZ 0x1000:1cf7                      ; 1000:1cf3 ; -> 1000:1cf7
    MOV CX,AX                            ; 1000:1cf5
    MOV word ptr [0x93a8],CX             ; 1000:1cf7
    JMP 0x1000:1c18                      ; 1000:1cfb ; -> 1000:1c18

; ------------------------------------------------------------------------------
; FUNCTION: map_get_tile_at  [Tilemap / Collision]
; ADDRESS:  1000:1cfe  (Length: 0x2e bytes)
; CALLERS:  FUN_1000_2fee, FUN_1000_2c9d, FUN_1000_36c0, FUN_1000_385f, FUN_1000_2edc, FUN_1000_2d61, FUN_1000_2341, FUN_1000_309d, FUN_1000_3107, FUN_1000_3019, FUN_1000_378e, FUN_1000_5d5f
; PURPOSE:  Looks up the tile index and collision flags at specified tile coordinate (X, Y).
; ------------------------------------------------------------------------------
map_get_tile_at:
    SHR AX,0x1                           ; 1000:1cfe
    SHR AX,0x1                           ; 1000:1d00
    SHR AX,0x1                           ; 1000:1d02
    SHR AX,0x1                           ; 1000:1d04
    SHR BX,0x1                           ; 1000:1d06
    SHR BX,0x1                           ; 1000:1d08
    SHR BX,0x1                           ; 1000:1d0a
    SHR BX,0x1                           ; 1000:1d0c
    CMP BX,word ptr [0x29a]              ; 1000:1d0e
    JL 0x1000:1d1a                       ; 1000:1d12 ; -> 1000:1d1a
    XOR BX,BX                            ; 1000:1d14
    MOV AX,0x100                         ; 1000:1d16
    RET                                  ; 1000:1d19
    MOV CX,BX                            ; 1000:1d1a
    SHL CX,0x1                           ; 1000:1d1c
    MOV BX,0x2a0                         ; 1000:1d1e
    ADD BX,CX                            ; 1000:1d21
    MOV BX,word ptr [BX]                 ; 1000:1d23
    ADD BX,AX                            ; 1000:1d25
    MOV AL,byte ptr [BX]                 ; 1000:1d27
    XOR AH,AH                            ; 1000:1d29
    RET                                  ; 1000:1d2b

; ------------------------------------------------------------------------------
; FUNCTION: map_draw_background  [Tilemap / Rendering]
; ADDRESS:  1000:1d2c  (Length: 0x94 bytes)
; CALLERS:  FUN_1000_3c13, FUN_1000_4073, FUN_1000_3e98, FUN_1000_3e1f, FUN_1000_2341, FUN_1000_46e6
; PURPOSE:  Draws complete room background tilemap to offscreen EGA video buffer.
; ------------------------------------------------------------------------------
map_draw_background:
    PUSH BP                              ; 1000:1d2c
    MOV BP,SP                            ; 1000:1d2d
    SUB SP,0x6                           ; 1000:1d2f
    MOV AX,[0x6c4]                       ; 1000:1d32
    AND AX,0xfff0                        ; 1000:1d35
    SHR AX,0x1                           ; 1000:1d38
    SHR AX,0x1                           ; 1000:1d3a
    SHR AX,0x1                           ; 1000:1d3c
    LEA BX,[0x4a0]                       ; 1000:1d3e
    ADD BX,AX                            ; 1000:1d42
    MOV SI,word ptr [BX]                 ; 1000:1d44
    MOV AX,[0x6c2]                       ; 1000:1d46
    SHR AX,0x1                           ; 1000:1d49
    SHR AX,0x1                           ; 1000:1d4b
    SHR AX,0x1                           ; 1000:1d4d
    MOV word ptr [BP + -0x6],AX          ; 1000:1d4f ; -> Stack[-0x8]
    ADD SI,AX                            ; 1000:1d52
    MOV AX,[0x6c4]                       ; 1000:1d54
    AND AX,0xf                           ; 1000:1d57
    SHL AX,0x1                           ; 1000:1d5a
    LEA BX,[0x6a0]                       ; 1000:1d5c
    ADD BX,AX                            ; 1000:1d60
    ADD SI,word ptr [BX]                 ; 1000:1d62
    MOV DI,word ptr CS:[0x773f]          ; 1000:1d64 ; -> 1000:773f
    ADD DI,0x141                         ; 1000:1d69
    MOV word ptr [BP + -0x2],0x0         ; 1000:1d6d ; -> Stack[-0x4]
    MOV CX,word ptr [0x6c0]              ; 1000:1d72
    SHL CX,0x1                           ; 1000:1d76
    SHL CX,0x1                           ; 1000:1d78
    SHL CX,0x1                           ; 1000:1d7a
    SHL CX,0x1                           ; 1000:1d7c
    SUB CX,word ptr [0x6c4]              ; 1000:1d7e
    JLE 0x1000:1d90                      ; 1000:1d82 ; -> 1000:1d90
    CMP CX,0xa0                          ; 1000:1d84
    JLE 0x1000:1d8d                      ; 1000:1d88 ; -> 1000:1d8d
    MOV CX,0xa0                          ; 1000:1d8a
    MOV word ptr [BP + -0x2],CX          ; 1000:1d8d ; -> Stack[-0x4]
    MOV CX,word ptr [BP + -0x2]          ; 1000:1d90 ; -> Stack[-0x4]
    NEG CX                               ; 1000:1d93
    ADD CX,0xa0                          ; 1000:1d95
    MOV word ptr [BP + -0x4],CX          ; 1000:1d99 ; -> Stack[-0x6]
    MOV CL,0x0                           ; 1000:1d9c
    MOV AX,0x47cc                        ; 1000:1d9e
    CALL 0x1000:1dc0                     ; 1000:1da1 ; -> 1000:1dc0
    MOV CL,0x1                           ; 1000:1da4
    MOV AX,0x3fcc                        ; 1000:1da6
    CALL 0x1000:1dc0                     ; 1000:1da9 ; -> 1000:1dc0
    MOV CL,0x2                           ; 1000:1dac
    MOV AX,0x4fcc                        ; 1000:1dae
    CALL 0x1000:1dc0                     ; 1000:1db1 ; -> 1000:1dc0
    MOV CL,0x3                           ; 1000:1db4
    MOV AX,0x57cc                        ; 1000:1db6
    CALL 0x1000:1dc0                     ; 1000:1db9 ; -> 1000:1dc0
    MOV SP,BP                            ; 1000:1dbc
    POP BP                               ; 1000:1dbe
    RET                                  ; 1000:1dbf

; ------------------------------------------------------------------------------
; FUNCTION: map_draw_tile_column  [Tilemap / Rendering]
; ADDRESS:  1000:1dc0  (Length: 0x49 bytes)
; CALLERS:  FUN_1000_1d2c
; PURPOSE:  Draws a single vertical column of 16x16 tiles during horizontal scrolling.
; ------------------------------------------------------------------------------
map_draw_tile_column:
    PUSH SI                              ; 1000:1dc0
    PUSH DI                              ; 1000:1dc1
    PUSH AX                              ; 1000:1dc2
    CALL 0x1000:7a68                     ; 1000:1dc3 ; -> 1000:7a68
    MOV DX,word ptr [0x298]              ; 1000:1dc6
    SHL DX,0x1                           ; 1000:1dca
    SUB DX,0x1a                          ; 1000:1dcc
    MOV BX,word ptr [BP + -0x2]          ; 1000:1dcf
    OR BX,BX                             ; 1000:1dd2
    JZ 0x1000:1ded                       ; 1000:1dd4 ; -> 1000:1ded
    PUSH DS                              ; 1000:1dd6
    MOV AX,0xa000                        ; 1000:1dd7
    MOV DS,AX                            ; 1000:1dda
    MOV CX,0xd                           ; 1000:1ddc
    MOVSW.REP ES:DI,SI                   ; 1000:1ddf
    ADD SI,DX                            ; 1000:1de1
    ADD DI,0xe                           ; 1000:1de3
    DEC BX                               ; 1000:1de6
    JNZ 0x1000:1ddc                      ; 1000:1de7 ; -> 1000:1ddc
    POP DS                               ; 1000:1de9
    MOV SI,word ptr [BP + -0x6]          ; 1000:1dea
    POP AX                               ; 1000:1ded
    MOV BX,word ptr [BP + -0x4]          ; 1000:1dee
    OR BX,BX                             ; 1000:1df1
    JZ 0x1000:1e06                       ; 1000:1df3 ; -> 1000:1e06
    PUSH DS                              ; 1000:1df5
    MOV DS,AX                            ; 1000:1df6
    MOV CX,0xd                           ; 1000:1df8
    MOVSW.REP ES:DI,SI                   ; 1000:1dfb
    ADD SI,DX                            ; 1000:1dfd
    ADD DI,0xe                           ; 1000:1dff
    DEC BX                               ; 1000:1e02
    JNZ 0x1000:1df8                      ; 1000:1e03 ; -> 1000:1df8
    POP DS                               ; 1000:1e05
    POP DI                               ; 1000:1e06
    POP SI                               ; 1000:1e07
    RET                                  ; 1000:1e08

; ------------------------------------------------------------------------------
; FUNCTION: map_draw_tile_row  [Tilemap / Rendering]
; ADDRESS:  1000:1e09  (Length: 0x7d bytes)
; CALLERS:  FUN_1000_3181
; PURPOSE:  Draws a single horizontal row of 16x16 tiles during vertical scrolling.
; ------------------------------------------------------------------------------
map_draw_tile_row:
    SHR BX,0x1                           ; 1000:1e09
    SHR BX,0x1                           ; 1000:1e0b
    SHR BX,0x1                           ; 1000:1e0d
    SHR BX,0x1                           ; 1000:1e0f
    PUSH BX                              ; 1000:1e11
    SHL BX,0x1                           ; 1000:1e12
    LEA DX,[0x4a0]                       ; 1000:1e14
    ADD BX,DX                            ; 1000:1e18
    MOV DI,word ptr [BX]                 ; 1000:1e1a
    SHR AX,0x1                           ; 1000:1e1c
    SHR AX,0x1                           ; 1000:1e1e
    SHR AX,0x1                           ; 1000:1e20
    SHR AX,0x1                           ; 1000:1e22
    SHL AX,0x1                           ; 1000:1e24
    ADD DI,AX                            ; 1000:1e26
    MOV AL,CL                            ; 1000:1e28
    XOR AH,AH                            ; 1000:1e2a
    SHL AX,0x1                           ; 1000:1e2c
    SHL AX,0x1                           ; 1000:1e2e
    SHL AX,0x1                           ; 1000:1e30
    SHL AX,0x1                           ; 1000:1e32
    SHL AX,0x1                           ; 1000:1e34
    SHL AX,0x1                           ; 1000:1e36
    SHL AX,0x1                           ; 1000:1e38
    LEA SI,[0x13a8]                      ; 1000:1e3a
    ADD SI,AX                            ; 1000:1e3e
    POP AX                               ; 1000:1e40
    CMP AX,word ptr [0x6c0]              ; 1000:1e41
    JL 0x1000:1e6d                       ; 1000:1e45 ; -> 1000:1e6d
    MOV CX,0x0                           ; 1000:1e47
    MOV AX,0x47cc                        ; 1000:1e4a
    CALL 0x1000:1f89                     ; 1000:1e4d ; -> 1000:1f89
    MOV CX,0x1                           ; 1000:1e50
    MOV AX,0x3fcc                        ; 1000:1e53
    CALL 0x1000:1f89                     ; 1000:1e56 ; -> 1000:1f89
    MOV CX,0x2                           ; 1000:1e59
    MOV AX,0x4fcc                        ; 1000:1e5c
    CALL 0x1000:1f89                     ; 1000:1e5f ; -> 1000:1f89
    MOV CX,0x3                           ; 1000:1e62
    MOV AX,0x57cc                        ; 1000:1e65
    CALL 0x1000:1f89                     ; 1000:1e68 ; -> 1000:1f89
    JMP 0x1000:1e85                      ; 1000:1e6b ; -> 1000:1e85
    MOV CX,0x0                           ; 1000:1e6d
    CALL 0x1000:1f72                     ; 1000:1e70 ; -> 1000:1f72
    MOV CX,0x1                           ; 1000:1e73
    CALL 0x1000:1f72                     ; 1000:1e76 ; -> 1000:1f72
    MOV CX,0x2                           ; 1000:1e79
    CALL 0x1000:1f72                     ; 1000:1e7c ; -> 1000:1f72
    MOV CX,0x3                           ; 1000:1e7f
    CALL 0x1000:1f72                     ; 1000:1e82 ; -> 1000:1f72
    RET                                  ; 1000:1e85

; ------------------------------------------------------------------------------
; FUNCTION: map_render_full_screen  [Tilemap / Rendering]
; ADDRESS:  1000:1e86  (Length: 0xeb bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Renders full visible screen grid of tiles to offscreen video page.
; ------------------------------------------------------------------------------
map_render_full_screen:
    PUSH BP                              ; 1000:1e86
    MOV BP,SP                            ; 1000:1e87
    SUB SP,0x2                           ; 1000:1e89
    XOR DX,DX                            ; 1000:1e8c
    MOV AX,0x600                         ; 1000:1e8e
    MOV CX,word ptr [0x298]              ; 1000:1e91
    DIV CX                               ; 1000:1e95
    MOV [0x6c0],AX                       ; 1000:1e97
    MOV DX,word ptr [0x298]              ; 1000:1e9a
    SHL DX,0x1                           ; 1000:1e9e
    LEA BX,[0x6a0]                       ; 1000:1ea0
    XOR AX,AX                            ; 1000:1ea4
    MOV CX,0x10                          ; 1000:1ea6
    MOV word ptr [BX],AX                 ; 1000:1ea9
    ADD BX,0x2                           ; 1000:1eab
    ADD AX,DX                            ; 1000:1eae
    LOOP 0x1000:1ea9                     ; 1000:1eb0 ; -> 1000:1ea9
    MOV DX,AX                            ; 1000:1eb2
    LEA BX,[0x4a0]                       ; 1000:1eb4
    MOV AX,0x4000                        ; 1000:1eb8
    XOR CX,CX                            ; 1000:1ebb
    MOV word ptr [BX],AX                 ; 1000:1ebd
    ADD AX,DX                            ; 1000:1ebf
    ADD BX,0x2                           ; 1000:1ec1
    INC CX                               ; 1000:1ec4
    CMP CX,word ptr [0x6c0]              ; 1000:1ec5
    JL 0x1000:1ebd                       ; 1000:1ec9 ; -> 1000:1ebd
    XOR AX,AX                            ; 1000:1ecb
    MOV word ptr [BX],AX                 ; 1000:1ecd
    ADD AX,DX                            ; 1000:1ecf
    ADD BX,0x2                           ; 1000:1ed1
    INC CX                               ; 1000:1ed4
    CMP CX,word ptr [0x29a]              ; 1000:1ed5
    JL 0x1000:1ecd                       ; 1000:1ed9 ; -> 1000:1ecd
    MOV word ptr [BP + -0x2],0x0         ; 1000:1edb ; -> Stack[-0x4]
    LEA SI,[0x9a8]                       ; 1000:1ee0
    MOV CX,word ptr [0x298]              ; 1000:1ee4
    MOV AX,word ptr [BP + -0x2]          ; 1000:1ee8 ; -> Stack[-0x4]
    SHL AX,0x1                           ; 1000:1eeb
    LEA BX,[0x4a0]                       ; 1000:1eed
    ADD BX,AX                            ; 1000:1ef1
    MOV DI,word ptr [BX]                 ; 1000:1ef3
    LODSB SI                             ; 1000:1ef5
    XOR AH,AH                            ; 1000:1ef6
    PUSH SI                              ; 1000:1ef8
    SHL AX,0x1                           ; 1000:1ef9
    SHL AX,0x1                           ; 1000:1efb
    SHL AX,0x1                           ; 1000:1efd
    SHL AX,0x1                           ; 1000:1eff
    SHL AX,0x1                           ; 1000:1f01
    SHL AX,0x1                           ; 1000:1f03
    SHL AX,0x1                           ; 1000:1f05
    LEA SI,[0x13a8]                      ; 1000:1f07
    ADD SI,AX                            ; 1000:1f0b
    MOV AX,word ptr [BP + -0x2]          ; 1000:1f0d ; -> Stack[-0x4]
    CMP AX,word ptr [0x6c0]              ; 1000:1f10
    JL 0x1000:1f3f                       ; 1000:1f14 ; -> 1000:1f3f
    PUSH CX                              ; 1000:1f16
    MOV CX,0x0                           ; 1000:1f17
    MOV AX,0x47cc                        ; 1000:1f1a
    CALL 0x1000:1f89                     ; 1000:1f1d ; -> 1000:1f89
    MOV CX,0x1                           ; 1000:1f20
    MOV AX,0x3fcc                        ; 1000:1f23
    CALL 0x1000:1f89                     ; 1000:1f26 ; -> 1000:1f89
    MOV CX,0x2                           ; 1000:1f29
    MOV AX,0x4fcc                        ; 1000:1f2c
    CALL 0x1000:1f89                     ; 1000:1f2f ; -> 1000:1f89
    MOV CX,0x3                           ; 1000:1f32
    MOV AX,0x57cc                        ; 1000:1f35
    CALL 0x1000:1f89                     ; 1000:1f38 ; -> 1000:1f89
    POP CX                               ; 1000:1f3b
    JMP 0x1000:1f59                      ; 1000:1f3c ; -> 1000:1f59
    PUSH CX                              ; 1000:1f3f
    MOV CX,0x0                           ; 1000:1f40
    CALL 0x1000:1f72                     ; 1000:1f43 ; -> 1000:1f72
    MOV CX,0x1                           ; 1000:1f46
    CALL 0x1000:1f72                     ; 1000:1f49 ; -> 1000:1f72
    MOV CX,0x2                           ; 1000:1f4c
    CALL 0x1000:1f72                     ; 1000:1f4f ; -> 1000:1f72
    MOV CX,0x3                           ; 1000:1f52
    CALL 0x1000:1f72                     ; 1000:1f55 ; -> 1000:1f72
    POP CX                               ; 1000:1f58
    ADD DI,0x2                           ; 1000:1f59
    POP SI                               ; 1000:1f5c
    LOOP 0x1000:1ef5                     ; 1000:1f5d ; -> 1000:1ef5
    INC word ptr [BP + -0x2]             ; 1000:1f5f ; -> Stack[-0x4]
    MOV AX,word ptr [BP + -0x2]          ; 1000:1f62 ; -> Stack[-0x4]
    CMP AX,word ptr [0x29a]              ; 1000:1f65
    JGE 0x1000:1f6e                      ; 1000:1f69 ; -> 1000:1f6e
    JMP 0x1000:1ee4                      ; 1000:1f6b ; -> 1000:1ee4
    MOV SP,BP                            ; 1000:1f6e
    POP BP                               ; 1000:1f70
    RET                                  ; 1000:1f71

; ------------------------------------------------------------------------------
; FUNCTION: map_set_tile_attr  [Tilemap / Flags]
; ADDRESS:  1000:1f72  (Length: 0x17 bytes)
; CALLERS:  FUN_1000_1e09, FUN_1000_1e86
; PURPOSE:  Sets tile attribute and collision flags in the active room collision buffer.
; ------------------------------------------------------------------------------
map_set_tile_attr:
    CALL 0x1000:7a68                     ; 1000:1f72 ; -> 1000:7a68
    MOV CX,0x10                          ; 1000:1f75
    MOV DX,word ptr [0x298]              ; 1000:1f78
    SHL DX,0x1                           ; 1000:1f7c
    SUB DX,0x2                           ; 1000:1f7e
    PUSH DI                              ; 1000:1f81
    MOVSW ES:DI,SI                       ; 1000:1f82
    ADD DI,DX                            ; 1000:1f83
    LOOP 0x1000:1f82                     ; 1000:1f85 ; -> 1000:1f82
    POP DI                               ; 1000:1f87
    RET                                  ; 1000:1f88

; ------------------------------------------------------------------------------
; FUNCTION: map_clear_tile_attr  [Tilemap / Flags]
; ADDRESS:  1000:1f89  (Length: 0x1b bytes)
; CALLERS:  FUN_1000_1e09, FUN_1000_1e86
; PURPOSE:  Clears tile attribute and collision flags in the active room collision buffer.
; ------------------------------------------------------------------------------
map_clear_tile_attr:
    PUSH ES                              ; 1000:1f89
    MOV ES,AX                            ; 1000:1f8a
    CALL 0x1000:7a68                     ; 1000:1f8c ; -> 1000:7a68
    MOV CX,0x10                          ; 1000:1f8f
    MOV DX,word ptr [0x298]              ; 1000:1f92
    SHL DX,0x1                           ; 1000:1f96
    SUB DX,0x2                           ; 1000:1f98
    PUSH DI                              ; 1000:1f9b
    MOVSW ES:DI,SI                       ; 1000:1f9c
    ADD DI,DX                            ; 1000:1f9d
    LOOP 0x1000:1f9c                     ; 1000:1f9f ; -> 1000:1f9c
    POP DI                               ; 1000:1fa1
    POP ES                               ; 1000:1fa2
    RET                                  ; 1000:1fa3

; ------------------------------------------------------------------------------
; FUNCTION: file_load_pack_rle  [File I/O / Compression]
; ADDRESS:  1000:1fa4  (Length: 0x59 bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Opens file via DOS INT 21h AH=3Dh and decompresses Byte-Run / PackBits RLE compressed data into destination buffer.
; ------------------------------------------------------------------------------
file_load_pack_rle:
    MOV AX,0x3d00                        ; 1000:1fa4
    INT 0x21                             ; 1000:1fa7
    JNC 0x1000:1fae                      ; 1000:1fa9 ; -> 1000:1fae
    JMP 0x1000:0202                      ; 1000:1fab ; -> 1000:0202
    MOV BX,AX                            ; 1000:1fae
    LEA DX,[0x292]                       ; 1000:1fb0
    MOV CX,0x6                           ; 1000:1fb4
    MOV AX,0x3f00                        ; 1000:1fb7
    INT 0x21                             ; 1000:1fba
    PUSH DS                              ; 1000:1fbc
    PUSH ES                              ; 1000:1fbd
    MOV CX,DS                            ; 1000:1fbe
    MOV ES,CX                            ; 1000:1fc0
    MOV CX,0x1d5d                        ; 1000:1fc2
    MOV DS,CX                            ; 1000:1fc5
    LEA DX,[0x600]                       ; 1000:1fc7
    MOV CX,0x7000                        ; 1000:1fcb
    MOV AX,0x3f00                        ; 1000:1fce
    INT 0x21                             ; 1000:1fd1
    MOV AX,0x3e00                        ; 1000:1fd3
    INT 0x21                             ; 1000:1fd6
    LEA SI,[0x600]                       ; 1000:1fd8
    LEA DI,[0x13a8]                      ; 1000:1fdc
    XOR CH,CH                            ; 1000:1fe0
    LODSB SI                             ; 1000:1fe2 ; -> 1d5d:0600
    CMP AL,0x0                           ; 1000:1fe3
    JL 0x1000:1fec                       ; 1000:1fe5 ; -> 1000:1fec
    JG 0x1000:1ff5                       ; 1000:1fe7 ; -> 1000:1ff5
    JMP 0x1000:1ffb                      ; 1000:1fe9 ; -> 1000:1ffb
    NEG AL                               ; 1000:1fec
    MOV CL,AL                            ; 1000:1fee
    LODSB SI                             ; 1000:1ff0 ; -> 1d5d:0603
    STOSB.REP ES:DI                      ; 1000:1ff1
    JMP 0x1000:1fe2                      ; 1000:1ff3 ; -> 1000:1fe2
    MOV CL,AL                            ; 1000:1ff5
    MOVSB.REP ES:DI,SI                   ; 1000:1ff7 ; -> 1d5d:0601
    JMP 0x1000:1fe2                      ; 1000:1ff9 ; -> 1000:1fe2
    POP ES                               ; 1000:1ffb
    POP DS                               ; 1000:1ffc
    RET                                  ; 1000:1ffd

; ------------------------------------------------------------------------------
; FUNCTION: file_load_raw_mask  [File I/O / Assets]
; ADDRESS:  1000:1ffe  (Length: 0x24 bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Reads raw collision mask table (.1) from disk into room collision buffer.
; ------------------------------------------------------------------------------
file_load_raw_mask:
    MOV AX,0x3d00                        ; 1000:1ffe
    INT 0x21                             ; 1000:2001
    JNC 0x1000:2008                      ; 1000:2003 ; -> 1000:2008
    JMP 0x1000:0202                      ; 1000:2005 ; -> 1000:0202
    MOV BX,AX                            ; 1000:2008
    PUSH DS                              ; 1000:200a
    MOV AX,0x1d5d                        ; 1000:200b
    MOV DS,AX                            ; 1000:200e
    MOV DX,0x600                         ; 1000:2010
    MOV CX,0xffff                        ; 1000:2013
    MOV AX,0x3f00                        ; 1000:2016
    INT 0x21                             ; 1000:2019
    POP DS                               ; 1000:201b
    MOV AX,0x3e00                        ; 1000:201c
    INT 0x21                             ; 1000:201f
    RET                                  ; 1000:2021

; ------------------------------------------------------------------------------
; FUNCTION: file_load_tileset  [File I/O / Assets]
; ADDRESS:  1000:2022  (Length: 0x27 bytes)
; CALLERS:  FUN_1000_1b8d, FUN_1000_7298, FUN_1000_7164
; PURPOSE:  Reads raw tile graphic bitmap data (.2) from disk into tileset buffer.
; ------------------------------------------------------------------------------
file_load_tileset:
    MOV AX,0x3d00                        ; 1000:2022
    INT 0x21                             ; 1000:2025
    JNC 0x1000:202c                      ; 1000:2027 ; -> 1000:202c
    JMP 0x1000:0202                      ; 1000:2029 ; -> 1000:0202
    MOV BX,AX                            ; 1000:202c
    MOV DS,word ptr CS:[0x5e]            ; 1000:202e ; -> 1000:005e
    MOV DX,0x0                           ; 1000:2033
    MOV CX,0xffff                        ; 1000:2036
    MOV AX,0x3f00                        ; 1000:2039
    INT 0x21                             ; 1000:203c
    MOV DS,word ptr CS:[0x5c]            ; 1000:203e ; -> 1000:005c
    MOV AX,0x3e00                        ; 1000:2043
    INT 0x21                             ; 1000:2046
    RET                                  ; 1000:2048

; ------------------------------------------------------------------------------
; FUNCTION: file_load_entity_data  [File I/O / Assets]
; ADDRESS:  1000:2049  (Length: 0x29 bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Reads room item/entity spawn parameters (.3) from disk.
; ------------------------------------------------------------------------------
file_load_entity_data:
    MOV AX,0x3d00                        ; 1000:2049
    INT 0x21                             ; 1000:204c
    JNC 0x1000:2053                      ; 1000:204e ; -> 1000:2053
    JMP 0x1000:0202                      ; 1000:2050 ; -> 1000:0202
    MOV BX,AX                            ; 1000:2053
    MOV DX,0x9ec0                        ; 1000:2055
    MOV CX,0xc00                         ; 1000:2058
    MOV AX,0x3f00                        ; 1000:205b
    INT 0x21                             ; 1000:205e
    MOV AX,0x3e00                        ; 1000:2060
    INT 0x21                             ; 1000:2063
    MOV CX,0xc00                         ; 1000:2065
    MOV BX,0x9ec0                        ; 1000:2068
    XOR byte ptr [BX],0x25               ; 1000:206b
    INC BX                               ; 1000:206e
    LOOP 0x1000:206b                     ; 1000:206f ; -> 1000:206b
    RET                                  ; 1000:2071

; ------------------------------------------------------------------------------
; FUNCTION: ent_init_monsters  [Entities / Management]
; ADDRESS:  1000:2072  (Length: 0x8b bytes)
; CALLERS:  FUN_1000_1b8d
; PURPOSE:  Initializes monster state machines, animation timers, and patrol bounds for the loaded room.
; ------------------------------------------------------------------------------
ent_init_monsters:
    PUSH DS                              ; 1000:2072
    PUSH ES                              ; 1000:2073
    MOV CX,DS                            ; 1000:2074
    MOV ES,CX                            ; 1000:2076
    MOV CX,0x1d5d                        ; 1000:2078
    MOV DS,CX                            ; 1000:207b
    MOV SI,0x600                         ; 1000:207d
    ADD SI,0x2                           ; 1000:2080
    CMP AX,word ptr [SI]                 ; 1000:2083 ; -> 1d5d:0602
    JZ 0x1000:208c                       ; 1000:2085 ; -> 1000:208c
    POP ES                               ; 1000:2087
    POP DS                               ; 1000:2088
    JMP 0x1000:0202                      ; 1000:2089 ; -> 1000:0202
    ADD SI,0x2                           ; 1000:208c
    MOV CX,BX                            ; 1000:208f
    SHL BX,0x1                           ; 1000:2091
    ADD BX,CX                            ; 1000:2093
    SHL BX,0x1                           ; 1000:2095
    ADD SI,BX                            ; 1000:2097
    LODSW SI                             ; 1000:2099
    MOV ES:[0x298],AX                    ; 1000:209a
    LODSW SI                             ; 1000:209e
    MOV ES:[0x29a],AX                    ; 1000:209f
    LODSW SI                             ; 1000:20a3
    MOV SI,0x600                         ; 1000:20a4
    ADD SI,AX                            ; 1000:20a7
    LEA DI,[0x9a8]                       ; 1000:20a9
    XOR CH,CH                            ; 1000:20ad
    LODSB SI                             ; 1000:20af
    CMP AL,0x0                           ; 1000:20b0
    JL 0x1000:20b9                       ; 1000:20b2 ; -> 1000:20b9
    JG 0x1000:20c2                       ; 1000:20b4 ; -> 1000:20c2
    JMP 0x1000:20c8                      ; 1000:20b6 ; -> 1000:20c8
    NEG AL                               ; 1000:20b9
    MOV CL,AL                            ; 1000:20bb
    LODSB SI                             ; 1000:20bd
    STOSB.REP ES:DI                      ; 1000:20be
    JMP 0x1000:20af                      ; 1000:20c0 ; -> 1000:20af
    MOV CL,AL                            ; 1000:20c2
    MOVSB.REP ES:DI,SI                   ; 1000:20c4
    JMP 0x1000:20af                      ; 1000:20c6 ; -> 1000:20af
    POP ES                               ; 1000:20c8
    POP DS                               ; 1000:20c9
    MOV BX,0x2a0                         ; 1000:20ca
    MOV CX,word ptr [0x29a]              ; 1000:20cd
    MOV AX,0x9a8                         ; 1000:20d1
    MOV DX,word ptr [0x298]              ; 1000:20d4
    MOV word ptr [BX],AX                 ; 1000:20d8
    ADD AX,DX                            ; 1000:20da
    ADD BX,0x2                           ; 1000:20dc
    LOOP 0x1000:20d8                     ; 1000:20df ; -> 1000:20d8
    MOV AX,[0x298]                       ; 1000:20e1
    SHL AX,0x1                           ; 1000:20e4
    SHL AX,0x1                           ; 1000:20e6
    SHL AX,0x1                           ; 1000:20e8
    SHL AX,0x1                           ; 1000:20ea
    MOV [0x29c],AX                       ; 1000:20ec
    MOV AX,[0x29a]                       ; 1000:20ef
    SHL AX,0x1                           ; 1000:20f2
    SHL AX,0x1                           ; 1000:20f4
    SHL AX,0x1                           ; 1000:20f6
    SHL AX,0x1                           ; 1000:20f8
    MOV [0x29e],AX                       ; 1000:20fa
    RET                                  ; 1000:20fd

; ------------------------------------------------------------------------------
; FUNCTION: game_start_room  [Game / Lifecycle]
; ADDRESS:  1000:20fe  (Length: 0xb8 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Initializes Comic at room spawn coordinates, loads room assets, and enters main game loop.
; ------------------------------------------------------------------------------
game_start_room:
    MOV word ptr [0x892],0x0             ; 1000:20fe
    MOV AX,word ptr CS:[SI + 0xa]        ; 1000:2104
    MOV BX,word ptr CS:[SI + 0xc]        ; 1000:2108
    MOV CX,word ptr CS:[SI + 0xe]        ; 1000:210c
    PUSH CX                              ; 1000:2110
    PUSH BX                              ; 1000:2111
    PUSH AX                              ; 1000:2112
    CMP byte ptr CS:[SI + 0x6],0x0       ; 1000:2113
    JZ 0x1000:214c                       ; 1000:2118 ; -> 1000:214c
    CMP byte ptr CS:[SI + 0x6],0xff      ; 1000:211a
    JZ 0x1000:213f                       ; 1000:211f ; -> 1000:213f
    CALL 0x1000:3cb1                     ; 1000:2121 ; -> 1000:3cb1
    CMP SI,0xb8a4                        ; 1000:2124
    JNZ 0x1000:214f                      ; 1000:2128 ; -> 1000:214f
    CMP byte ptr [0x221],0x1             ; 1000:212a
    JNZ 0x1000:213a                      ; 1000:212f ; -> 1000:213a
    MOV AX,CS:[0xb97a]                   ; 1000:2131 ; -> 1000:b97a
    CALL 0x1000:3f0a                     ; 1000:2135 ; -> 1000:3f0a
    JMP 0x1000:214f                      ; 1000:2138 ; -> 1000:214f
    ADD SP,0x6                           ; 1000:213a
    JMP 0x1000:2198                      ; 1000:213d ; -> 1000:2198
    MOV AX,0x1                           ; 1000:213f
    LEA BX,[0x93cc]                      ; 1000:2142
    MOV CX,0x9                           ; 1000:2146
    INT3                                 ; 1000:2149
    CALL 0x1000:3bc6                     ; 1000:214c ; -> 1000:3bc6
    POP AX                               ; 1000:214f
    POP BX                               ; 1000:2150
    POP CX                               ; 1000:2151
    CMP AX,0xffff                        ; 1000:2152
    JZ 0x1000:21c0                       ; 1000:2155 ; -> 1000:21c0
    CMP AX,0xfffe                        ; 1000:2157
    JNZ 0x1000:215f                      ; 1000:215a ; -> 1000:215f
    JMP 0x1000:21f2                      ; 1000:215c ; -> 1000:21f2
    PUSH CX                              ; 1000:215f
    CALL 0x1000:1b8d                     ; 1000:2160 ; -> 1000:1b8d
    POP CX                               ; 1000:2163
    MOV SI,word ptr [0x6f0]              ; 1000:2164
    OR CX,CX                             ; 1000:2168
    JZ 0x1000:2173                       ; 1000:216a ; -> 1000:2173
    ADD SI,0x12                          ; 1000:216c
    DEC CL                               ; 1000:216f
    JNZ 0x1000:216c                      ; 1000:2171 ; -> 1000:216c
    MOV AX,word ptr CS:[SI + 0x4]        ; 1000:2173
    SHR AX,0x1                           ; 1000:2177
    ADD AX,word ptr CS:[SI]              ; 1000:2179
    MOV [0x6c6],AX                       ; 1000:217c
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:217f
    MOV [0x6c8],AX                       ; 1000:2183
    PUSH SI                              ; 1000:2186
    CALL 0x1000:18e0                     ; 1000:2187 ; -> 1000:18e0
    POP SI                               ; 1000:218a
    CMP SI,0xb8a4                        ; 1000:218b
    JNZ 0x1000:2198                      ; 1000:218f ; -> 1000:2198
    MOV AX,CS:[0xb97a]                   ; 1000:2191 ; -> 1000:b97a
    CALL 0x1000:3f0a                     ; 1000:2195 ; -> 1000:3f0a
    CMP byte ptr CS:[SI + 0x6],0x0       ; 1000:2198
    JZ 0x1000:21a9                       ; 1000:219d ; -> 1000:21a9
    CMP byte ptr CS:[SI + 0x6],0xff      ; 1000:219f
    JZ 0x1000:21a9                       ; 1000:21a4 ; -> 1000:21a9
    CALL 0x1000:3c13                     ; 1000:21a6 ; -> 1000:3c13
    MOV byte ptr CS:[0x25a],0x0          ; 1000:21a9 ; -> 1000:025a
    MOV AX,0x1                           ; 1000:21af
    CALL 0x1000:027a                     ; 1000:21b2 ; -> 1000:027a
    JMP 0x1000:2341                      ; 1000:21b5 ; -> 1000:2341

; ------------------------------------------------------------------------------
; FUNCTION: game_respawn_player  [Game / Lifecycle]
; ADDRESS:  1000:21b8  (Length: 0x3a bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Handles player death, decrements lives counter, checks for Game Over, or restarts player at room spawn point.
; ------------------------------------------------------------------------------
game_respawn_player:
    MOV byte ptr [0x8ad],0x1             ; 1000:21b8
    CALL 0x1000:3e98                     ; 1000:21bd ; -> 1000:3e98
    MOV AX,0x5                           ; 1000:21c0
    MOV BX,0x3                           ; 1000:21c3
    CALL 0x1000:1b8d                     ; 1000:21c6 ; -> 1000:1b8d
    MOV word ptr [0x6c6],0x28            ; 1000:21c9
    MOV word ptr [0x6c8],0x60            ; 1000:21cf
    MOV byte ptr [0x8ad],0x1             ; 1000:21d5
    CALL 0x1000:18e0                     ; 1000:21da ; -> 1000:18e0
    CALL 0x1000:3e1f                     ; 1000:21dd ; -> 1000:3e1f
    MOV byte ptr [0x897],0x0             ; 1000:21e0
    MOV byte ptr [0x291],0x1             ; 1000:21e5
    MOV byte ptr [0x89c],0x0             ; 1000:21ea
    JMP 0x1000:2341                      ; 1000:21ef ; -> 1000:2341

; ------------------------------------------------------------------------------
; FUNCTION: game_victory_sequence  [Game / Sequence]
; ADDRESS:  1000:21f2  (Length: 0x13 bytes)
; CALLERS:  FUN_1000_20fe
; PURPOSE:  Handles the end-game victory sequence, awards bonus points, and displays the ending cutscene.
; ------------------------------------------------------------------------------
game_victory_sequence:
    MOV AX,CS:[0x9300]                   ; 1000:21f2 ; -> 1000:9300
    CALL 0x1000:3f0a                     ; 1000:21f6 ; -> 1000:3f0a
    MOV AX,0x5                           ; 1000:21f9
    CALL 0x1000:027a                     ; 1000:21fc ; -> 1000:027a
    CALL 0x1000:65ef                     ; 1000:21ff ; -> 1000:65ef
    JMP 0x1000:0185                      ; 1000:2202 ; -> 1000:0185

; ------------------------------------------------------------------------------
; FUNCTION: game_over_sequence  [Game / Sequence]
; ADDRESS:  1000:2205  (Length: 0xb bytes)
; CALLERS:  FUN_1000_35de
; PURPOSE:  Handles Game Over sequence, plays game over jingle, and transitions to high score entry.
; ------------------------------------------------------------------------------
game_over_sequence:
    MOV AX,0x1                           ; 1000:2205
    LEA BX,[0x93cc]                      ; 1000:2208
    MOV CX,0x9                           ; 1000:220c
    INT3                                 ; 1000:220f

; ------------------------------------------------------------------------------
; FUNCTION: game_switch_room  [Game / Streaming]
; ADDRESS:  1000:2253  (Length: 0x28 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Processes room transition doorways, saves room state, loads destination room, and spawns player.
; ------------------------------------------------------------------------------
game_switch_room:
    MOV byte ptr [0x8ad],0x1             ; 1000:2253
    MOV word ptr [0x6c6],0x28            ; 1000:2258
    CALL 0x1000:3e98                     ; 1000:225e ; -> 1000:3e98
    MOV BX,word ptr [0x21a]              ; 1000:2261
    SHL BX,0x1                           ; 1000:2265
    SHL BX,0x1                           ; 1000:2267
    ADD BX,word ptr [0x93a8]             ; 1000:2269
    MOV AX,word ptr [BX]                 ; 1000:226d
    MOV BX,word ptr [BX + 0x2]           ; 1000:226f
    CALL 0x1000:1b8d                     ; 1000:2272 ; -> 1000:1b8d
    MOV CX,0x0                           ; 1000:2275
    JMP 0x1000:2164                      ; 1000:2278 ; -> 1000:2164

; ------------------------------------------------------------------------------
; FUNCTION: spr_animate_player  [Sprites / Player]
; ADDRESS:  1000:227b  (Length: 0xa8 bytes)
; CALLERS:  FUN_1000_2323, FUN_1000_409c
; PURPOSE:  Updates Comic's walking, jumping, and shooting animation frame counters based on velocity.
; ------------------------------------------------------------------------------
spr_animate_player:
    CALL 0x1000:3aa0                     ; 1000:227b ; -> 1000:3aa0
    CALL 0x1000:3a92                     ; 1000:227e ; -> 1000:3a92
    MOV byte ptr [0x8ed],0xc             ; 1000:2281
    CMP word ptr [0x222],0x205           ; 1000:2286
    JGE 0x1000:2294                      ; 1000:228c ; -> 1000:2294
    MOV word ptr [0x222],0x205           ; 1000:228e
    MOV AX,0x120                         ; 1000:2294
    MOV BX,0x90                          ; 1000:2297
    MOV CX,word ptr [0x222]              ; 1000:229a
    CALL 0x1000:3a5f                     ; 1000:229e ; -> 1000:3a5f
    MOV AX,[0x210]                       ; 1000:22a1
    MOV [0x6c6],AX                       ; 1000:22a4
    MOV AX,[0x212]                       ; 1000:22a7
    MOV [0x6c8],AX                       ; 1000:22aa
    MOV AL,[0x218]                       ; 1000:22ad
    MOV [0x8ad],AL                       ; 1000:22b0
    MOV AL,[0x219]                       ; 1000:22b3
    MOV [0x89d],AL                       ; 1000:22b6
    XOR AX,AX                            ; 1000:22b9
    MOV [0x892],AX                       ; 1000:22bb
    MOV [0x894],AX                       ; 1000:22be
    MOV [0x897],AL                       ; 1000:22c1
    MOV [0x8a3],AL                       ; 1000:22c4
    MOV [0x8a6],AL                       ; 1000:22c7
    MOV [0x8a8],AL                       ; 1000:22ca
    MOV [0x91e],AL                       ; 1000:22cd
    MOV [0x921],AL                       ; 1000:22d0
    MOV [0x89a],AL                       ; 1000:22d3
    MOV [0x89e],AL                       ; 1000:22d6
    MOV [0x8b1],AL                       ; 1000:22d9
    MOV [0x8b0],AL                       ; 1000:22dc
    MOV [0x8ec],AL                       ; 1000:22df
    MOV [0x291],AL                       ; 1000:22e2
    MOV [0x89c],AL                       ; 1000:22e5
    MOV [0x93b],AL                       ; 1000:22e8
    MOV byte ptr [0x89b],0x1             ; 1000:22eb
    MOV AX,[0x6fa]                       ; 1000:22f0
    MOV BX,word ptr [0x6fc]              ; 1000:22f3
    MOV word ptr [0x6fc],0xffff          ; 1000:22f7
    CALL 0x1000:1b8d                     ; 1000:22fd ; -> 1000:1b8d
    CALL 0x1000:18e0                     ; 1000:2300 ; -> 1000:18e0
    MOV BL,byte ptr [0x896]              ; 1000:2303
    XOR BH,BH                            ; 1000:2307
    MOV byte ptr [0x896],0x0             ; 1000:2309
    CALL 0x1000:3a17                     ; 1000:230e ; -> 1000:3a17
    CMP byte ptr [0x220],0xff            ; 1000:2311
    JNZ 0x1000:2320                      ; 1000:2316 ; -> 1000:2320
    MOV byte ptr [0x220],0x0             ; 1000:2318
    CALL 0x1000:3e1f                     ; 1000:231d ; -> 1000:3e1f
    JMP 0x1000:2341                      ; 1000:2320 ; -> 1000:2341

; ------------------------------------------------------------------------------
; FUNCTION: spr_set_player_frame  [Sprites / Player]
; ADDRESS:  1000:2323  (Length: 0x1e bytes)
; CALLERS:  FUN_1000_6668
; PURPOSE:  Sets specific sprite frame index for Comic based on current stance and facing direction.
; ------------------------------------------------------------------------------
spr_set_player_frame:
    MOV AX,[0x6fa]                       ; 1000:2323
    MOV BX,0xffff                        ; 1000:2326
    MOV word ptr [0x6fa],0xffff          ; 1000:2329
    CALL 0x1000:1b8d                     ; 1000:232f ; -> 1000:1b8d
    MOV word ptr [0x210],0x288           ; 1000:2332
    MOV word ptr [0x212],0x70            ; 1000:2338
    JMP 0x1000:227b                      ; 1000:233e ; -> 1000:227b

; ------------------------------------------------------------------------------
; FUNCTION: game_main_loop  [Game / Core]
; ADDRESS:  1000:2341  (Length: 0x6b3 bytes)
; CALLERS:  FUN_1000_21b8, FUN_1000_227b, FUN_1000_20fe
; PURPOSE:  Master gameplay loop. Polls user input, updates physics, checks collisions, updates monsters/projectiles, blits sprites, flips video pages, and synchronizes frames.
; ------------------------------------------------------------------------------
game_main_loop:
    PUSH ES                              ; 1000:2341
    XOR CX,CX                            ; 1000:2342
    MOV ES,CX                            ; 1000:2344
    MOV CL,byte ptr ES:[0x41a]           ; 1000:2346
    MOV byte ptr ES:[0x41c],CL           ; 1000:234b
    POP ES                               ; 1000:2350
    INC byte ptr [0x962b]                ; 1000:2351
    MOV word ptr [0x8e8],SP              ; 1000:2355
    MOV CH,byte ptr [0x8af]              ; 1000:2359
    INC CH                               ; 1000:235d
    CMP CH,0x4                           ; 1000:235f
    JNZ 0x1000:2366                      ; 1000:2362 ; -> 1000:2366
    MOV CH,0x1                           ; 1000:2364
    MOV byte ptr [0x8af],CH              ; 1000:2366
    MOV byte ptr [0x8ae],0x0             ; 1000:236a
    CMP byte ptr [0x962a],0x0            ; 1000:236f
    JZ 0x1000:237a                       ; 1000:2374 ; -> 1000:237a
    DEC byte ptr [0x962a]                ; 1000:2376
    CMP byte ptr [0x8ec],0x0             ; 1000:237a
    JZ 0x1000:2384                       ; 1000:237f ; -> 1000:2384
    JMP 0x1000:275c                      ; 1000:2381 ; -> 1000:275c
    CMP byte ptr [0x921],0x0             ; 1000:2384
    JZ 0x1000:238e                       ; 1000:2389 ; -> 1000:238e
    JMP 0x1000:275c                      ; 1000:238b ; -> 1000:275c
    CMP byte ptr [0x924],0x1             ; 1000:238e
    JLE 0x1000:2398                      ; 1000:2393 ; -> 1000:2398
    JMP 0x1000:3657                      ; 1000:2395 ; -> 1000:3657
    CMP byte ptr [0x92c],0x1             ; 1000:2398
    JLE 0x1000:23a2                      ; 1000:239d ; -> 1000:23a2
    JMP 0x1000:386f                      ; 1000:239f ; -> 1000:386f
    CMP byte ptr [0x89d],0x0             ; 1000:23a2
    JZ 0x1000:23ac                       ; 1000:23a7 ; -> 1000:23ac
    JMP 0x1000:2edc                      ; 1000:23a9 ; -> 1000:2edc
    CMP byte ptr [0x896],0x2             ; 1000:23ac
    JNZ 0x1000:23e0                      ; 1000:23b1 ; -> 1000:23e0
    MOV AX,0xaabe                        ; 1000:23b3
    CMP word ptr [0x222],0x0             ; 1000:23b6
    JZ 0x1000:23c7                       ; 1000:23bb ; -> 1000:23c7
    MOV AX,0xb7b6                        ; 1000:23bd
    CMP word ptr [0x6fa],0xe             ; 1000:23c0
    JNZ 0x1000:23e0                      ; 1000:23c5 ; -> 1000:23e0
    CMP byte ptr CS:[0x25a],0x1          ; 1000:23c7 ; -> 1000:025a
    JZ 0x1000:23d7                       ; 1000:23cd ; -> 1000:23d7
    CMP byte ptr CS:[0x25d],0x1          ; 1000:23cf ; -> 1000:025d
    JNZ 0x1000:23e0                      ; 1000:23d5 ; -> 1000:23e0
    CALL 0x1000:3610                     ; 1000:23d7 ; -> 1000:3610
    MOV byte ptr CS:[0x25a],0x0          ; 1000:23da ; -> 1000:025a
    CMP byte ptr [0x897],0x0             ; 1000:23e0
    JZ 0x1000:23ea                       ; 1000:23e5 ; -> 1000:23ea
    JMP 0x1000:2a10                      ; 1000:23e7 ; -> 1000:2a10
    CMP byte ptr [0x8a3],0x0             ; 1000:23ea
    JZ 0x1000:23f4                       ; 1000:23ef ; -> 1000:23f4
    JMP 0x1000:3107                      ; 1000:23f1 ; -> 1000:3107
    CMP byte ptr [0x8a7],0x0             ; 1000:23f4
    JZ 0x1000:23fe                       ; 1000:23f9 ; -> 1000:23fe
    JMP 0x1000:34a9                      ; 1000:23fb ; -> 1000:34a9
    CMP byte ptr [0x92c],0x1             ; 1000:23fe
    JNZ 0x1000:2416                      ; 1000:2403 ; -> 1000:2416
    MOV AX,[0x6c6]                       ; 1000:2405
    SUB AX,word ptr [0x930]              ; 1000:2408
    JL 0x1000:2416                       ; 1000:240c ; -> 1000:2416
    CMP AX,0x18                          ; 1000:240e
    JG 0x1000:2416                       ; 1000:2411 ; -> 1000:2416
    JMP 0x1000:385f                      ; 1000:2413 ; -> 1000:385f
    MOV AX,[0x6c6]                       ; 1000:2416
    ADD AX,0x8                           ; 1000:2419
    MOV BX,word ptr [0x6c8]              ; 1000:241c
    ADD BX,0x1f                          ; 1000:2420
    CALL 0x1000:1cfe                     ; 1000:2423 ; -> 1000:1cfe
    CMP AX,word ptr [0x294]              ; 1000:2426
    JLE 0x1000:2435                      ; 1000:242a ; -> 1000:2435
    CMP AX,word ptr [0x296]              ; 1000:242c
    JG 0x1000:2435                       ; 1000:2430 ; -> 1000:2435
    JMP 0x1000:2dc6                      ; 1000:2432 ; -> 1000:2dc6
    CMP byte ptr [0x8a6],0x0             ; 1000:2435
    JZ 0x1000:243f                       ; 1000:243a ; -> 1000:243f
    JMP 0x1000:330f                      ; 1000:243c ; -> 1000:330f
    CMP byte ptr CS:[0x25c],0x1          ; 1000:243f ; -> 1000:025c
    JNZ 0x1000:24a3                      ; 1000:2445 ; -> 1000:24a3
    CMP byte ptr [0x898],0x0             ; 1000:2447
    JZ 0x1000:24a3                       ; 1000:244c ; -> 1000:24a3
    MOV byte ptr [0x897],0x1             ; 1000:244e
    CMP byte ptr CS:[0x25b],0x1          ; 1000:2453 ; -> 1000:025b
    JZ 0x1000:247a                       ; 1000:2459 ; -> 1000:247a
    CMP byte ptr [0x9628],0x1            ; 1000:245b
    JNZ 0x1000:246c                      ; 1000:2460 ; -> 1000:246c
    MOV AL,[0x898]                       ; 1000:2462
    SHR AL,0x1                           ; 1000:2465
    INC AL                               ; 1000:2467
    MOV [0x898],AL                       ; 1000:2469
    MOV AX,0x1                           ; 1000:246c
    LEA BX,[0x96de]                      ; 1000:246f
    MOV CX,0x0                           ; 1000:2473
    INT3                                 ; 1000:2476
    MOV byte ptr [0x897],0x1             ; 1000:247a
    MOV byte ptr [0x898],0x0             ; 1000:247f
    MOV word ptr [0x894],0x5             ; 1000:2484
    MOV word ptr [0x892],0x0             ; 1000:248a
    MOV byte ptr [0x89a],0x2             ; 1000:2490
    MOV AX,0x1                           ; 1000:2495
    LEA BX,[0x96f6]                      ; 1000:2498
    MOV CX,0x0                           ; 1000:249c
    INT3                                 ; 1000:249f
    CMP byte ptr [0x962a],0x0            ; 1000:24a3
    JNZ 0x1000:24b2                      ; 1000:24a8 ; -> 1000:24b2
    CMP byte ptr CS:[0x25a],0x1          ; 1000:24aa ; -> 1000:025a
    JZ 0x1000:24b5                       ; 1000:24b0 ; -> 1000:24b5
    JMP 0x1000:2548                      ; 1000:24b2 ; -> 1000:2548
    CMP word ptr [0x6fa],0x5             ; 1000:24b5
    JNZ 0x1000:24e8                      ; 1000:24ba ; -> 1000:24e8
    CMP word ptr [0x6fc],0x3             ; 1000:24bc
    JNZ 0x1000:24e8                      ; 1000:24c1 ; -> 1000:24e8
    CMP word ptr [0x6c8],0x70            ; 1000:24c3
    JNZ 0x1000:24e8                      ; 1000:24c8 ; -> 1000:24e8
    CMP word ptr [0x6c6],0x270           ; 1000:24ca
    JL 0x1000:24e8                       ; 1000:24d0 ; -> 1000:24e8
    CMP word ptr [0x6c6],0x2a0           ; 1000:24d2
    JG 0x1000:24e8                       ; 1000:24d8 ; -> 1000:24e8
    CMP byte ptr [0x220],0x5             ; 1000:24da
    JGE 0x1000:24e8                      ; 1000:24df ; -> 1000:24e8
    MOV SP,word ptr [0x8e8]              ; 1000:24e1
    JMP 0x1000:6668                      ; 1000:24e5 ; -> 1000:6668
    MOV CX,word ptr [0x6ee]              ; 1000:24e8
    MOV SI,word ptr [0x6f0]              ; 1000:24ec
    OR CX,CX                             ; 1000:24f0
    JZ 0x1000:2548                       ; 1000:24f2 ; -> 1000:2548
    MOV AX,[0x6c6]                       ; 1000:24f4
    SUB AX,word ptr CS:[SI]              ; 1000:24f7
    JL 0x1000:2541                       ; 1000:24fa ; -> 1000:2541
    CMP AX,word ptr CS:[SI + 0x4]        ; 1000:24fc
    JG 0x1000:2541                       ; 1000:2500 ; -> 1000:2541
    MOV AX,[0x6c8]                       ; 1000:2502
    CMP AX,word ptr CS:[SI + 0x2]        ; 1000:2505
    JNZ 0x1000:2541                      ; 1000:2509 ; -> 1000:2541
    MOV BX,word ptr CS:[SI + 0x10]       ; 1000:250b
    OR BX,BX                             ; 1000:250f
    JZ 0x1000:2535                       ; 1000:2511 ; -> 1000:2535
    MOV AX,word ptr [BX + 0x2]           ; 1000:2513
    MOV BX,word ptr [BX]                 ; 1000:2516
    OR BX,BX                             ; 1000:2518
    JZ 0x1000:2535                       ; 1000:251a ; -> 1000:2535
    CMP AX,word ptr CS:[BX + 0x8]        ; 1000:251c
    JNZ 0x1000:2528                      ; 1000:2520 ; -> 1000:2528
    CMP word ptr CS:[BX],-0x1            ; 1000:2522
    JZ 0x1000:252d                       ; 1000:2526 ; -> 1000:252d
    CALL 0x1000:3610                     ; 1000:2528 ; -> 1000:3610
    JMP 0x1000:2548                      ; 1000:252b ; -> 1000:2548
    MOV BX,word ptr CS:[SI + 0x10]       ; 1000:252d
    MOV word ptr [BX],0x0                ; 1000:2531
    MOV SP,word ptr [0x8e8]              ; 1000:2535
    MOV byte ptr [0x962a],0x5            ; 1000:2539
    JMP 0x1000:20fe                      ; 1000:253e ; -> 1000:20fe
    ADD SI,0x12                          ; 1000:2541
    DEC CL                               ; 1000:2544
    JNZ 0x1000:24f4                      ; 1000:2546 ; -> 1000:24f4
    MOV word ptr [0x892],0x0             ; 1000:2548
    CMP byte ptr CS:[0x258],0x1          ; 1000:254e ; -> 1000:0258
    JNZ 0x1000:255f                      ; 1000:2554 ; -> 1000:255f
    MOV word ptr [0x892],0xfffb          ; 1000:2556
    CALL 0x1000:2bdc                     ; 1000:255c ; -> 1000:2bdc
    CMP byte ptr CS:[0x259],0x1          ; 1000:255f ; -> 1000:0259
    JNZ 0x1000:2570                      ; 1000:2565 ; -> 1000:2570
    MOV word ptr [0x892],0x5             ; 1000:2567
    CALL 0x1000:2c39                     ; 1000:256d ; -> 1000:2c39
    MOV AX,[0x6c6]                       ; 1000:2570
    MOV BX,word ptr [0x6c8]              ; 1000:2573
    ADD BX,0x20                          ; 1000:2577
    MOV DI,word ptr [0x294]              ; 1000:257a
    CALL 0x1000:2c9d                     ; 1000:257e ; -> 1000:2c9d
    JC 0x1000:25ac                       ; 1000:2581 ; -> 1000:25ac
    MOV byte ptr [0x897],0x1             ; 1000:2583
    MOV byte ptr [0x898],0x0             ; 1000:2588
    MOV word ptr [0x894],0x5             ; 1000:258d
    CMP word ptr [0x892],0x0             ; 1000:2593
    JL 0x1000:25a4                       ; 1000:2598 ; -> 1000:25a4
    JZ 0x1000:25ba                       ; 1000:259a ; -> 1000:25ba
    MOV word ptr [0x892],0x1             ; 1000:259c
    JMP 0x1000:25ba                      ; 1000:25a2 ; -> 1000:25ba
    MOV word ptr [0x892],0xffff          ; 1000:25a4
    JMP 0x1000:25ba                      ; 1000:25aa ; -> 1000:25ba
    CMP byte ptr [0x89c],0x1             ; 1000:25ac
    JNZ 0x1000:25ba                      ; 1000:25b1 ; -> 1000:25ba
    MOV SP,word ptr [0x8e8]              ; 1000:25b3
    JMP 0x1000:21b8                      ; 1000:25b7 ; -> 1000:21b8
    CMP byte ptr [0x897],0x0             ; 1000:25ba
    JNZ 0x1000:2621                      ; 1000:25bf ; -> 1000:2621
    MOV byte ptr [0x9628],0x0            ; 1000:25c1
    CMP word ptr [0x6fa],0x9             ; 1000:25c6
    JNZ 0x1000:2621                      ; 1000:25cb ; -> 1000:2621
    CMP word ptr CS:[0x12d0],-0x1        ; 1000:25cd ; -> 1000:12d0
    JZ 0x1000:2621                       ; 1000:25d3 ; -> 1000:2621
    MOV AX,[0x6c6]                       ; 1000:25d5
    MOV BX,word ptr [0x6c8]              ; 1000:25d8
    ADD BX,0x20                          ; 1000:25dc
    CALL 0x1000:1cfe                     ; 1000:25df ; -> 1000:1cfe
    CMP AX,0x1c                          ; 1000:25e2
    JL 0x1000:25ec                       ; 1000:25e5 ; -> 1000:25ec
    CMP AX,0x1e                          ; 1000:25e7
    JLE 0x1000:2601                      ; 1000:25ea ; -> 1000:2601
    TEST word ptr [0x6c6],0x8            ; 1000:25ec
    JZ 0x1000:2621                       ; 1000:25f2 ; -> 1000:2621
    INC BX                               ; 1000:25f4
    MOV AL,byte ptr [BX]                 ; 1000:25f5
    CMP AX,0x1c                          ; 1000:25f7
    JL 0x1000:2621                       ; 1000:25fa ; -> 1000:2621
    CMP AX,0x1e                          ; 1000:25fc
    JG 0x1000:2621                       ; 1000:25ff ; -> 1000:2621
    MOV byte ptr [0x9628],0x1            ; 1000:2601
    MOV AL,[0x8ae]                       ; 1000:2606
    PUSH AX                              ; 1000:2609
    CMP byte ptr [0x9629],0x0            ; 1000:260a
    JL 0x1000:261a                       ; 1000:260f ; -> 1000:261a
    CALL 0x1000:2c54                     ; 1000:2611 ; -> 1000:2c54
    POP AX                               ; 1000:2614
    MOV [0x8ae],AL                       ; 1000:2615
    JMP 0x1000:2621                      ; 1000:2618 ; -> 1000:2621
    CALL 0x1000:2bf7                     ; 1000:261a ; -> 1000:2bf7
    POP AX                               ; 1000:261d
    MOV [0x8ae],AL                       ; 1000:261e
    CMP word ptr [0x6fa],0x5             ; 1000:2621
    JNZ 0x1000:2652                      ; 1000:2626 ; -> 1000:2652
    CMP word ptr [0x6fc],0x3             ; 1000:2628
    JNZ 0x1000:2652                      ; 1000:262d ; -> 1000:2652
    CMP word ptr [0x6c8],0x60            ; 1000:262f
    JNZ 0x1000:2652                      ; 1000:2634 ; -> 1000:2652
    CMP word ptr [0x6c6],0x28            ; 1000:2636
    JNZ 0x1000:2652                      ; 1000:263b ; -> 1000:2652
    CMP byte ptr [0x291],0x1             ; 1000:263d
    JZ 0x1000:2657                       ; 1000:2642 ; -> 1000:2657
    CMP byte ptr [0x220],0x5             ; 1000:2644
    JGE 0x1000:2657                      ; 1000:2649 ; -> 1000:2657
    MOV SP,word ptr [0x8e8]              ; 1000:264b
    JMP 0x1000:2253                      ; 1000:264f ; -> 1000:2253
    MOV byte ptr [0x291],0x0             ; 1000:2652
    CMP byte ptr [0x8a8],0x0             ; 1000:2657
    JNZ 0x1000:267f                      ; 1000:265c ; -> 1000:267f
    MOV AL,CS:[0x61]                     ; 1000:265e ; -> 1000:0061
    CMP byte ptr CS:[0x257],AL           ; 1000:2662 ; -> 1000:0257
    JNZ 0x1000:267f                      ; 1000:2667 ; -> 1000:267f
    MOV byte ptr CS:[0x61],0x0           ; 1000:2669 ; -> 1000:0061
    CMP byte ptr [0x897],0x0             ; 1000:266f
    JNZ 0x1000:267f                      ; 1000:2674 ; -> 1000:267f
    CALL 0x1000:46e6                     ; 1000:2676 ; -> 1000:46e6
    MOV byte ptr CS:[0x61],0x0           ; 1000:2679 ; -> 1000:0061
    CMP byte ptr [0x924],0x0             ; 1000:267f
    JZ 0x1000:2689                       ; 1000:2684 ; -> 1000:2689
    CALL 0x1000:36c0                     ; 1000:2686 ; -> 1000:36c0
    CMP byte ptr [0x897],0x1             ; 1000:2689
    JZ 0x1000:26b1                       ; 1000:268e ; -> 1000:26b1
    CMP byte ptr CS:[0x25c],0x1          ; 1000:2690 ; -> 1000:025c
    JZ 0x1000:26b1                       ; 1000:2696 ; -> 1000:26b1
    MOV AL,[0x899]                       ; 1000:2698
    CMP word ptr CS:[0xe5c],-0x1         ; 1000:269b ; -> 1000:0e5c
    JNZ 0x1000:26ae                      ; 1000:26a1 ; -> 1000:26ae
    CMP word ptr CS:[0xe64],0xb094       ; 1000:26a3 ; -> 1000:0e64
    JNZ 0x1000:26ae                      ; 1000:26aa ; -> 1000:26ae
    INC AL                               ; 1000:26ac
    MOV [0x898],AL                       ; 1000:26ae
    CMP byte ptr CS:[0x25d],0x1          ; 1000:26b1 ; -> 1000:025d
    JZ 0x1000:26be                       ; 1000:26b7 ; -> 1000:26be
    CALL 0x1000:3ace                     ; 1000:26b9 ; -> 1000:3ace
    JMP 0x1000:2714                      ; 1000:26bc ; -> 1000:2714
    CMP byte ptr [0x896],0x0             ; 1000:26be
    JZ 0x1000:26d8                       ; 1000:26c3 ; -> 1000:26d8
    CALL 0x1000:3ace                     ; 1000:26c5 ; -> 1000:3ace
    CMP byte ptr [0x896],0x3             ; 1000:26c8
    JZ 0x1000:2708                       ; 1000:26cd ; -> 1000:2708
    CMP byte ptr [0x896],0x1             ; 1000:26cf
    JZ 0x1000:26f6                       ; 1000:26d4 ; -> 1000:26f6
    JMP 0x1000:2714                      ; 1000:26d6 ; -> 1000:2714
    CMP byte ptr [0x8ea],0x0             ; 1000:26d8
    JZ 0x1000:2714                       ; 1000:26dd ; -> 1000:2714
    CMP byte ptr [0x21c],0x0             ; 1000:26df
    JNZ 0x1000:26ee                      ; 1000:26e4 ; -> 1000:26ee
    MOV AX,0xa09c                        ; 1000:26e6
    CALL 0x1000:3610                     ; 1000:26e9 ; -> 1000:3610
    JMP 0x1000:26f1                      ; 1000:26ec ; -> 1000:26f1
    CALL 0x1000:5ceb                     ; 1000:26ee ; -> 1000:5ceb
    CALL 0x1000:3b12                     ; 1000:26f1 ; -> 1000:3b12
    JMP 0x1000:2714                      ; 1000:26f4 ; -> 1000:2714
    MOV AL,[0x897]                       ; 1000:26f6
    OR AL,byte ptr [0x89d]               ; 1000:26f9
    OR AL,byte ptr [0x8a3]               ; 1000:26fd
    JNZ 0x1000:2714                      ; 1000:2701 ; -> 1000:2714
    CALL 0x1000:30c6                     ; 1000:2703 ; -> 1000:30c6
    JMP 0x1000:2714                      ; 1000:2706 ; -> 1000:2714
    MOV AL,[0x897]                       ; 1000:2708
    OR AL,byte ptr [0x89d]               ; 1000:270b
    JNZ 0x1000:2714                      ; 1000:270f ; -> 1000:2714
    CALL 0x1000:3468                     ; 1000:2711 ; -> 1000:3468
    MOV AL,CS:[0x61]                     ; 1000:2714 ; -> 1000:0061
    CMP byte ptr CS:[0x250],AL           ; 1000:2718 ; -> 1000:0250
    JNZ 0x1000:2728                      ; 1000:271d ; -> 1000:2728
    MOV byte ptr CS:[0x61],0x0           ; 1000:271f ; -> 1000:0061
    CALL 0x1000:39ad                     ; 1000:2725 ; -> 1000:39ad
    MOV AL,CS:[0x61]                     ; 1000:2728 ; -> 1000:0061
    CMP byte ptr CS:[0x255],AL           ; 1000:272c ; -> 1000:0255
    JNZ 0x1000:2741                      ; 1000:2731 ; -> 1000:2741
    MOV byte ptr CS:[0x61],0x0           ; 1000:2733 ; -> 1000:0061
    MOV AX,0xffff                        ; 1000:2739
    CALL 0x1000:327d                     ; 1000:273c ; -> 1000:327d
    JMP 0x1000:2754                      ; 1000:273f ; -> 1000:2754
    CMP byte ptr CS:[0x256],AL           ; 1000:2741 ; -> 1000:0256
    JNZ 0x1000:2754                      ; 1000:2746 ; -> 1000:2754
    MOV byte ptr CS:[0x61],0x0           ; 1000:2748 ; -> 1000:0061
    MOV AX,0x1                           ; 1000:274e
    CALL 0x1000:327d                     ; 1000:2751 ; -> 1000:327d
    CMP byte ptr CS:[0x25d],0x1          ; 1000:2754 ; -> 1000:025d
    JNZ 0x1000:275c                      ; 1000:275a ; -> 1000:275c
    CMP byte ptr [0x93b],0x0             ; 1000:275c
    JZ 0x1000:2769                       ; 1000:2761 ; -> 1000:2769
    DEC byte ptr [0x93b]                 ; 1000:2763
    JMP 0x1000:2770                      ; 1000:2767 ; -> 1000:2770
    CMP byte ptr [0x93a],0x1             ; 1000:2769
    JNZ 0x1000:2794                      ; 1000:276e ; -> 1000:2794
    TEST byte ptr [0x962b],0x1           ; 1000:2770
    JNZ 0x1000:2788                      ; 1000:2775 ; -> 1000:2788
    ADD word ptr [0x6c4],0x2             ; 1000:2777
    MOV AX,[0x6c4]                       ; 1000:277c
    ADD AX,0xa0                          ; 1000:277f
    CMP AX,word ptr [0x29e]              ; 1000:2782
    JLE 0x1000:2794                      ; 1000:2786 ; -> 1000:2794
    SUB word ptr [0x6c4],0x2             ; 1000:2788
    JNC 0x1000:2794                      ; 1000:278d ; -> 1000:2794
    ADD word ptr [0x6c4],0x2             ; 1000:278f
    CALL 0x1000:1d2c                     ; 1000:2794 ; -> 1000:1d2c
    CALL 0x1000:437b                     ; 1000:2797 ; -> 1000:437b
    CALL 0x1000:5d5f                     ; 1000:279a ; -> 1000:5d5f
    CALL 0x1000:4b25                     ; 1000:279d ; -> 1000:4b25
    CMP byte ptr [0x8ec],0x0             ; 1000:27a0
    JZ 0x1000:27f7                       ; 1000:27a5 ; -> 1000:27f7
    DEC byte ptr [0x8ec]                 ; 1000:27a7
    JNZ 0x1000:27b0                      ; 1000:27ab ; -> 1000:27b0
    JMP 0x1000:29e3                      ; 1000:27ad ; -> 1000:29e3
    CMP byte ptr [0x8ec],0x1             ; 1000:27b0
    JG 0x1000:27ba                       ; 1000:27b5 ; -> 1000:27ba
    JMP 0x1000:2902                      ; 1000:27b7 ; -> 1000:2902
    MOV SI,0x9                           ; 1000:27ba
    MOV AL,[0x8ec]                       ; 1000:27bd
    XOR AH,AH                            ; 1000:27c0
    SUB SI,AX                            ; 1000:27c2
    SHL SI,0x1                           ; 1000:27c4
    MOV AX,[0x6c6]                       ; 1000:27c6
    MOV BX,word ptr [0x6c8]              ; 1000:27c9
    CMP byte ptr [0x89d],0x1             ; 1000:27cd
    JZ 0x1000:27e1                       ; 1000:27d2 ; -> 1000:27e1
    CMP byte ptr [0x92c],0x2             ; 1000:27d4
    JZ 0x1000:27e1                       ; 1000:27d9 ; -> 1000:27e1
    ADD SI,0x48a4                        ; 1000:27db
    JMP 0x1000:27e5                      ; 1000:27df ; -> 1000:27e5
    ADD SI,0x48b4                        ; 1000:27e1
    MOV DS,word ptr CS:[0x5e]            ; 1000:27e5 ; -> 1000:005e
    MOV SI,word ptr [SI]                 ; 1000:27ea
    MOV DS,word ptr CS:[0x5c]            ; 1000:27ec ; -> 1000:005c
    CALL 0x1000:7dbb                     ; 1000:27f1 ; -> 1000:7dbb
    JMP 0x1000:2902                      ; 1000:27f4 ; -> 1000:2902
    CMP byte ptr [0x924],0x1             ; 1000:27f7
    JLE 0x1000:2838                      ; 1000:27fc ; -> 1000:2838
    CMP byte ptr [0x925],0x2             ; 1000:27fe
    JZ 0x1000:2826                       ; 1000:2803 ; -> 1000:2826
    MOV SI,word ptr CS:[0x8dbe]          ; 1000:2805 ; -> 1000:8dbe
    CMP byte ptr [0x925],0x1             ; 1000:280a
    JZ 0x1000:2816                       ; 1000:280f ; -> 1000:2816
    MOV SI,word ptr CS:[0x8dc2]          ; 1000:2811 ; -> 1000:8dc2
    MOV AX,[0x928]                       ; 1000:2816
    MOV BX,word ptr [0x92a]              ; 1000:2819
    ADD BX,0x6                           ; 1000:281d
    CALL 0x1000:7dbb                     ; 1000:2820 ; -> 1000:7dbb
    JMP 0x1000:2902                      ; 1000:2823 ; -> 1000:2902
    MOV SI,word ptr CS:[0x8dc0]          ; 1000:2826 ; -> 1000:8dc0
    MOV AX,[0x928]                       ; 1000:282b
    MOV BX,word ptr [0x92a]              ; 1000:282e
    CALL 0x1000:7dbb                     ; 1000:2832 ; -> 1000:7dbb
    JMP 0x1000:2902                      ; 1000:2835 ; -> 1000:2902
    CMP byte ptr [0x92c],0x1             ; 1000:2838
    JLE 0x1000:2874                      ; 1000:283d ; -> 1000:2874
    CMP byte ptr [0x92d],0x0             ; 1000:283f
    JZ 0x1000:2862                       ; 1000:2844 ; -> 1000:2862
    MOV SI,word ptr CS:[0xa990]          ; 1000:2846 ; -> 1000:a990
    JG 0x1000:2852                       ; 1000:284b ; -> 1000:2852
    MOV SI,word ptr CS:[0xa98e]          ; 1000:284d ; -> 1000:a98e
    MOV AX,[0x930]                       ; 1000:2852
    MOV BX,word ptr [0x932]              ; 1000:2855
    ADD BX,0x3                           ; 1000:2859
    CALL 0x1000:7dbb                     ; 1000:285c ; -> 1000:7dbb
    JMP 0x1000:2902                      ; 1000:285f ; -> 1000:2902
    MOV SI,word ptr CS:[0xa98c]          ; 1000:2862 ; -> 1000:a98c
    MOV AX,[0x930]                       ; 1000:2867
    MOV BX,word ptr [0x932]              ; 1000:286a
    CALL 0x1000:7dbb                     ; 1000:286e ; -> 1000:7dbb
    JMP 0x1000:2902                      ; 1000:2871 ; -> 1000:2902
    MOV SI,0x4882                        ; 1000:2874
    CMP byte ptr [0x8ad],0x1             ; 1000:2877
    JZ 0x1000:2881                       ; 1000:287c ; -> 1000:2881
    MOV SI,0x4860                        ; 1000:287e
    MOV AL,[0x8ae]                       ; 1000:2881
    OR AL,AL                             ; 1000:2884
    JZ 0x1000:28ec                       ; 1000:2886 ; -> 1000:28ec
    DEC AL                               ; 1000:2888
    JZ 0x1000:28a0                       ; 1000:288a ; -> 1000:28a0
    DEC AL                               ; 1000:288c
    JZ 0x1000:28ab                       ; 1000:288e ; -> 1000:28ab
    DEC AL                               ; 1000:2890
    JZ 0x1000:28b0                       ; 1000:2892 ; -> 1000:28b0
    DEC AL                               ; 1000:2894
    JZ 0x1000:28c3                       ; 1000:2896 ; -> 1000:28c3
    DEC AL                               ; 1000:2898
    JZ 0x1000:28de                       ; 1000:289a ; -> 1000:28de
    DEC AL                               ; 1000:289c
    JZ 0x1000:28be                       ; 1000:289e ; -> 1000:28be
    MOV AL,[0x8af]                       ; 1000:28a0
    XOR AH,AH                            ; 1000:28a3
    SHL AX,0x1                           ; 1000:28a5
    ADD SI,AX                            ; 1000:28a7
    JMP 0x1000:28ec                      ; 1000:28a9 ; -> 1000:28ec
    ADD SI,0x8                           ; 1000:28ab
    JMP 0x1000:28ec                      ; 1000:28ae ; -> 1000:28ec
    ADD SI,0x8                           ; 1000:28b0
    MOV AL,[0x8af]                       ; 1000:28b3
    XOR AH,AH                            ; 1000:28b6
    SHL AX,0x1                           ; 1000:28b8
    ADD SI,AX                            ; 1000:28ba
    JMP 0x1000:28ec                      ; 1000:28bc ; -> 1000:28ec
    ADD SI,0x1a                          ; 1000:28be
    JMP 0x1000:28c6                      ; 1000:28c1 ; -> 1000:28c6
    ADD SI,0xe                           ; 1000:28c3
    MOV AL,[0x8af]                       ; 1000:28c6
    XOR AH,AH                            ; 1000:28c9
    SHL AX,0x1                           ; 1000:28cb
    ADD SI,AX                            ; 1000:28cd
    CMP byte ptr [0x8a5],0x1             ; 1000:28cf
    JZ 0x1000:28ec                       ; 1000:28d4 ; -> 1000:28ec
    MOV AX,[0x6c6]                       ; 1000:28d6
    SUB AX,0x8                           ; 1000:28d9
    JMP 0x1000:28ef                      ; 1000:28dc ; -> 1000:28ef
    ADD SI,0x14                          ; 1000:28de
    MOV AL,[0x8af]                       ; 1000:28e1
    XOR AH,AH                            ; 1000:28e4
    SHL AX,0x1                           ; 1000:28e6
    ADD SI,AX                            ; 1000:28e8
    JMP 0x1000:28ec                      ; 1000:28ea ; -> 1000:28ec
    MOV AX,[0x6c6]                       ; 1000:28ec
    MOV BX,word ptr [0x6c8]              ; 1000:28ef
    MOV DS,word ptr CS:[0x5e]            ; 1000:28f3 ; -> 1000:005e
    MOV SI,word ptr [SI]                 ; 1000:28f8 ; -> 260a:4868
    MOV DS,word ptr CS:[0x5c]            ; 1000:28fa ; -> 1000:005c
    CALL 0x1000:7dbb                     ; 1000:28ff ; -> 1000:7dbb
    CMP byte ptr [0x89e],0x0             ; 1000:2902 ; -> 34fe:089e
    JZ 0x1000:291a                       ; 1000:2907 ; -> 1000:291a
    DEC byte ptr [0x89e]                 ; 1000:2909 ; -> 34fe:089e
    MOV SI,0x48d6                        ; 1000:290d
    MOV AX,[0x89f]                       ; 1000:2910 ; -> 34fe:089f
    MOV BX,word ptr [0x8a1]              ; 1000:2913 ; -> 34fe:08a1
    CALL 0x1000:7dbb                     ; 1000:2917 ; -> 1000:7dbb
    CMP byte ptr [0x8b1],0x0             ; 1000:291a
    JZ 0x1000:2924                       ; 1000:291f ; -> 1000:2924
    CALL 0x1000:31eb                     ; 1000:2921 ; -> 1000:31eb
    CMP byte ptr [0x8a8],0x0             ; 1000:2924
    JZ 0x1000:2953                       ; 1000:2929 ; -> 1000:2953
    DEC byte ptr [0x8a8]                 ; 1000:292b
    JZ 0x1000:294a                       ; 1000:292f ; -> 1000:294a
    TEST byte ptr [0x962b],0x1           ; 1000:2931
    JZ 0x1000:2953                       ; 1000:2936 ; -> 1000:2953
    MOV BX,word ptr [0x8ab]              ; 1000:2938
    MOV SI,word ptr [BX + 0x6]           ; 1000:293c
    MOV AX,[0x8a9]                       ; 1000:293f
    MOV BX,word ptr [BX + 0x2]           ; 1000:2942
    CALL 0x1000:7dbb                     ; 1000:2945 ; -> 1000:7dbb
    JMP 0x1000:2953                      ; 1000:2948 ; -> 1000:2953
    MOV BX,word ptr [0x8ab]              ; 1000:294a
    MOV AX,[0x8a9]                       ; 1000:294e
    MOV word ptr [BX],AX                 ; 1000:2951
    CMP byte ptr [0x91e],0x0             ; 1000:2953
    JZ 0x1000:297f                       ; 1000:2958 ; -> 1000:297f
    CMP byte ptr [0x921],0x1             ; 1000:295a
    JZ 0x1000:297f                       ; 1000:295f ; -> 1000:297f
    DEC byte ptr [0x91e]                 ; 1000:2961
    JZ 0x1000:297f                       ; 1000:2965 ; -> 1000:297f
    TEST byte ptr [0x962b],0x1           ; 1000:2967
    JZ 0x1000:297f                       ; 1000:296c ; -> 1000:297f
    MOV SI,word ptr [0x91f]              ; 1000:296e
    MOV AX,[0x6c6]                       ; 1000:2972
    MOV BX,word ptr [0x6c8]              ; 1000:2975
    SUB BX,0x10                          ; 1000:2979
    CALL 0x1000:7dbb                     ; 1000:297c ; -> 1000:7dbb
    CMP byte ptr [0x924],0x1             ; 1000:297f
    JNZ 0x1000:2989                      ; 1000:2984 ; -> 1000:2989
    CALL 0x1000:378e                     ; 1000:2986 ; -> 1000:378e
    CMP byte ptr [0x92c],0x1             ; 1000:2989
    JL 0x1000:29a8                       ; 1000:298e ; -> 1000:29a8
    JZ 0x1000:2999                       ; 1000:2990 ; -> 1000:2999
    CMP byte ptr [0x8ec],0x0             ; 1000:2992
    JZ 0x1000:29a8                       ; 1000:2997 ; -> 1000:29a8
    MOV AX,[0x930]                       ; 1000:2999
    MOV BX,word ptr [0x932]              ; 1000:299c
    MOV SI,word ptr CS:[0xa98a]          ; 1000:29a0 ; -> 1000:a98a
    CALL 0x1000:7dbb                     ; 1000:29a5 ; -> 1000:7dbb
    CMP byte ptr [0x921],0x0             ; 1000:29a8
    JZ 0x1000:29c9                       ; 1000:29ad ; -> 1000:29c9
    MOV SI,word ptr [0x922]              ; 1000:29af
    CALL 0x1000:4206                     ; 1000:29b3 ; -> 1000:4206
    CMP byte ptr CS:[0x61],0x1c          ; 1000:29b6 ; -> 1000:0061
    JNZ 0x1000:29c9                      ; 1000:29bc ; -> 1000:29c9
    MOV byte ptr CS:[0x61],0x0           ; 1000:29be ; -> 1000:0061
    MOV byte ptr [0x921],0x0             ; 1000:29c4
    CMP byte ptr [0x8ed],0x0             ; 1000:29c9
    JZ 0x1000:29d7                       ; 1000:29ce ; -> 1000:29d7
    DEC byte ptr [0x8ed]                 ; 1000:29d0
    CALL 0x1000:3b51                     ; 1000:29d4 ; -> 1000:3b51
    CALL 0x1000:774e                     ; 1000:29d7 ; -> 1000:774e
    MOV AX,[0x6c8]                       ; 1000:29da
    CMP AX,word ptr [0x29e]              ; 1000:29dd
    JLE 0x1000:2a07                      ; 1000:29e1 ; -> 1000:2a07
    MOV SP,word ptr [0x8e8]              ; 1000:29e3
    MOV AX,0x1                           ; 1000:29e7
    MOV BX,0x969a                        ; 1000:29ea
    MOV CX,0xa                           ; 1000:29ed
    INT3                                 ; 1000:29f0
    MOV AX,0x1                           ; 1000:2a07
    CALL 0x1000:027a                     ; 1000:2a0a ; -> 1000:027a
    JMP 0x1000:2341                      ; 1000:2a0d ; -> 1000:2341

; ------------------------------------------------------------------------------
; FUNCTION: player_update_physics  [Player / Physics]
; ADDRESS:  1000:2a10  (Length: 0x17c bytes)
; CALLERS:  FUN_1000_330f, FUN_1000_2341, FUN_1000_3657
; PURPOSE:  Processes player horizontal velocity, acceleration, jump mechanics, gravity, ladder climbing, and swimming.
; ------------------------------------------------------------------------------
player_update_physics:
    CMP byte ptr [0x898],0x0             ; 1000:2a10
    JZ 0x1000:2a31                       ; 1000:2a15 ; -> 1000:2a31
    DEC byte ptr [0x898]                 ; 1000:2a17
    JZ 0x1000:2a31                       ; 1000:2a1b ; -> 1000:2a31
    CMP byte ptr CS:[0x25c],0x1          ; 1000:2a1d ; -> 1000:025c
    JNZ 0x1000:2a2c                      ; 1000:2a23 ; -> 1000:2a2c
    SUB word ptr [0x894],0x7             ; 1000:2a25
    JMP 0x1000:2a31                      ; 1000:2a2a ; -> 1000:2a31
    MOV byte ptr [0x898],0x0             ; 1000:2a2c
    MOV AX,[0x894]                       ; 1000:2a31
    MOV CX,AX                            ; 1000:2a34
    ADD AX,word ptr [0x6c8]              ; 1000:2a36
    JGE 0x1000:2a3e                      ; 1000:2a3a ; -> 1000:2a3e
    XOR AX,AX                            ; 1000:2a3c
    MOV [0x6c8],AX                       ; 1000:2a3e
    MOV word ptr [0x9620],CX             ; 1000:2a41
    ADD CX,0x5                           ; 1000:2a45
    CMP CX,0x17                          ; 1000:2a48
    JLE 0x1000:2a50                      ; 1000:2a4b ; -> 1000:2a50
    MOV CX,0x17                          ; 1000:2a4d
    MOV word ptr [0x894],CX              ; 1000:2a50
    MOV CX,word ptr [0x892]              ; 1000:2a54
    CMP byte ptr CS:[0x258],0x1          ; 1000:2a58 ; -> 1000:0258
    JNZ 0x1000:2a6e                      ; 1000:2a5e ; -> 1000:2a6e
    MOV byte ptr [0x8ad],0x2             ; 1000:2a60
    DEC CX                               ; 1000:2a65
    CMP CX,-0x5                          ; 1000:2a66
    JGE 0x1000:2a6e                      ; 1000:2a69 ; -> 1000:2a6e
    MOV CX,0xfffb                        ; 1000:2a6b
    CMP byte ptr CS:[0x259],0x1          ; 1000:2a6e ; -> 1000:0259
    JNZ 0x1000:2a84                      ; 1000:2a74 ; -> 1000:2a84
    MOV byte ptr [0x8ad],0x1             ; 1000:2a76
    INC CX                               ; 1000:2a7b
    CMP CX,0x5                           ; 1000:2a7c
    JLE 0x1000:2a84                      ; 1000:2a7f ; -> 1000:2a84
    MOV CX,0x5                           ; 1000:2a81
    MOV word ptr [0x892],CX              ; 1000:2a84
    OR CX,CX                             ; 1000:2a88
    JZ 0x1000:2a9e                       ; 1000:2a8a ; -> 1000:2a9e
    JG 0x1000:2a97                       ; 1000:2a8c ; -> 1000:2a97
    INC word ptr [0x892]                 ; 1000:2a8e
    CALL 0x1000:2bf7                     ; 1000:2a92 ; -> 1000:2bf7
    JMP 0x1000:2a9e                      ; 1000:2a95 ; -> 1000:2a9e
    DEC word ptr [0x892]                 ; 1000:2a97
    CALL 0x1000:2c54                     ; 1000:2a9b ; -> 1000:2c54
    CMP word ptr [0x894],0x0             ; 1000:2a9e
    JG 0x1000:2ad2                       ; 1000:2aa3 ; -> 1000:2ad2
    MOV AX,[0x6c6]                       ; 1000:2aa5
    MOV BX,word ptr [0x6c8]              ; 1000:2aa8
    MOV DI,word ptr [0x292]              ; 1000:2aac
    MOV byte ptr [0x8b0],0x2             ; 1000:2ab0
    CALL 0x1000:2c9d                     ; 1000:2ab5 ; -> 1000:2c9d
    MOV byte ptr [0x8b0],0x0             ; 1000:2ab8
    JNC 0x1000:2ad2                      ; 1000:2abd ; -> 1000:2ad2
    MOV word ptr [0x894],0x0             ; 1000:2abf
    AND word ptr [0x6c8],0xfff0          ; 1000:2ac5
    ADD word ptr [0x6c8],0xa             ; 1000:2aca
    JMP 0x1000:2b4c                      ; 1000:2acf ; -> 1000:2b4c
    CMP word ptr [0x9620],0x0            ; 1000:2ad2
    JLE 0x1000:2b4c                      ; 1000:2ad7 ; -> 1000:2b4c
    CMP byte ptr [0x924],0x1             ; 1000:2ad9
    JNZ 0x1000:2ae8                      ; 1000:2ade ; -> 1000:2ae8
    CALL 0x1000:362a                     ; 1000:2ae0 ; -> 1000:362a
    JNC 0x1000:2ae8                      ; 1000:2ae3 ; -> 1000:2ae8
    JMP 0x1000:2b73                      ; 1000:2ae5 ; -> 1000:2b73
    MOV DI,word ptr [0x294]              ; 1000:2ae8
    CMP byte ptr [0x89a],0x0             ; 1000:2aec
    JZ 0x1000:2afb                       ; 1000:2af1 ; -> 1000:2afb
    DEC byte ptr [0x89a]                 ; 1000:2af3
    MOV DI,word ptr [0x292]              ; 1000:2af7
    MOV AX,[0x6c6]                       ; 1000:2afb
    MOV BX,word ptr [0x6c8]              ; 1000:2afe
    ADD BX,0x20                          ; 1000:2b02
    MOV byte ptr [0x8b0],0x1             ; 1000:2b05
    CALL 0x1000:2c9d                     ; 1000:2b0a ; -> 1000:2c9d
    MOV byte ptr [0x8b0],0x0             ; 1000:2b0d
    JC 0x1000:2b23                       ; 1000:2b12 ; -> 1000:2b23
    CMP AX,word ptr [0x296]              ; 1000:2b14
    JG 0x1000:2b2c                       ; 1000:2b18 ; -> 1000:2b2c
    CMP AX,word ptr [0x294]              ; 1000:2b1a
    JLE 0x1000:2b2c                      ; 1000:2b1e ; -> 1000:2b2c
    JMP 0x1000:2dc6                      ; 1000:2b20 ; -> 1000:2dc6
    CMP byte ptr [0x89b],0x1             ; 1000:2b23
    JNZ 0x1000:2b31                      ; 1000:2b28 ; -> 1000:2b31
    JMP 0x1000:2b78                      ; 1000:2b2a ; -> 1000:2b78
    MOV byte ptr [0x89b],0x1             ; 1000:2b2c
    MOV AX,[0x6c6]                       ; 1000:2b31
    MOV BX,word ptr [0x6c8]              ; 1000:2b34
    ADD BX,0x28                          ; 1000:2b38
    MOV byte ptr [0x8b0],0x1             ; 1000:2b3b
    CALL 0x1000:2c9d                     ; 1000:2b40 ; -> 1000:2c9d
    MOV byte ptr [0x8b0],0x0             ; 1000:2b43
    JC 0x1000:2b73                       ; 1000:2b48 ; -> 1000:2b73
    JMP 0x1000:2b6b                      ; 1000:2b4a ; -> 1000:2b6b
    MOV AX,[0x6c6]                       ; 1000:2b4c
    MOV BX,word ptr [0x6c8]              ; 1000:2b4f
    ADD BX,0x20                          ; 1000:2b53
    MOV DI,word ptr [0x294]              ; 1000:2b56
    CALL 0x1000:2c9d                     ; 1000:2b5a ; -> 1000:2c9d
    JNC 0x1000:2b66                      ; 1000:2b5d ; -> 1000:2b66
    MOV byte ptr [0x89b],0x0             ; 1000:2b5f
    JMP 0x1000:2b6b                      ; 1000:2b64 ; -> 1000:2b6b
    MOV byte ptr [0x89b],0x1             ; 1000:2b66
    MOV byte ptr [0x8ae],0x2             ; 1000:2b6b
    JMP 0x1000:2b88                      ; 1000:2b70 ; -> 1000:2b88
    ADD word ptr [0x6c8],0x8             ; 1000:2b73
    AND word ptr [0x6c8],0xfff0          ; 1000:2b78
    MOV byte ptr [0x897],0x0             ; 1000:2b7d
    MOV word ptr [0x894],0x0             ; 1000:2b82
    CALL 0x1000:2b8e                     ; 1000:2b88 ; -> 1000:2b8e
    JMP 0x1000:267f                      ; 1000:2b8b ; -> 1000:267f

; ------------------------------------------------------------------------------
; FUNCTION: player_check_ground_collision  [Player / Collision]
; ADDRESS:  1000:2b8e  (Length: 0x4e bytes)
; CALLERS:  FUN_1000_385f, FUN_1000_2a10, FUN_1000_3657
; PURPOSE:  Checks for solid ground tiles below player feet to handle landing and ground movement.
; ------------------------------------------------------------------------------
player_check_ground_collision:
    MOV AX,[0x6c8]                       ; 1000:2b8e
    MOV BX,word ptr [0x6c4]              ; 1000:2b91
    SUB AX,BX                            ; 1000:2b95
    CMP AX,0x30                          ; 1000:2b97
    JL 0x1000:2ba2                       ; 1000:2b9a ; -> 1000:2ba2
    CMP AX,0x50                          ; 1000:2b9c
    JG 0x1000:2bbb                       ; 1000:2b9f ; -> 1000:2bbb
    RET                                  ; 1000:2ba1
    OR BX,BX                             ; 1000:2ba2
    JZ 0x1000:2ba1                       ; 1000:2ba4 ; -> 1000:2ba1
    ADD BX,AX                            ; 1000:2ba6
    SUB BX,0x30                          ; 1000:2ba8
    JL 0x1000:2bb3                       ; 1000:2bab ; -> 1000:2bb3
    MOV word ptr [0x6c4],BX              ; 1000:2bad
    JMP 0x1000:2ba1                      ; 1000:2bb1 ; -> 1000:2ba1
    MOV word ptr [0x6c4],0x0             ; 1000:2bb3
    JMP 0x1000:2ba1                      ; 1000:2bb9 ; -> 1000:2ba1
    MOV CX,word ptr [0x29e]              ; 1000:2bbb
    SUB CX,0xa0                          ; 1000:2bbf
    CMP BX,CX                            ; 1000:2bc3
    JZ 0x1000:2ba1                       ; 1000:2bc5 ; -> 1000:2ba1
    ADD BX,AX                            ; 1000:2bc7
    SUB BX,0x50                          ; 1000:2bc9
    CMP BX,CX                            ; 1000:2bcc
    JG 0x1000:2bd6                       ; 1000:2bce ; -> 1000:2bd6
    MOV word ptr [0x6c4],BX              ; 1000:2bd0
    JMP 0x1000:2ba1                      ; 1000:2bd4 ; -> 1000:2ba1
    MOV word ptr [0x6c4],CX              ; 1000:2bd6
    JMP 0x1000:2ba1                      ; 1000:2bda ; -> 1000:2ba1

; ------------------------------------------------------------------------------
; FUNCTION: player_check_ceiling_collision  [Player / Collision]
; ADDRESS:  1000:2bdc  (Length: 0x1b bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Detects collisions with ceiling tiles above Comic's head during jumps.
; ------------------------------------------------------------------------------
player_check_ceiling_collision:
    CMP byte ptr [0x8ad],0x2             ; 1000:2bdc
    JZ 0x1000:2bea                       ; 1000:2be1 ; -> 1000:2bea
    MOV byte ptr [0x8ad],0x2             ; 1000:2be3
    JMP 0x1000:2c38                      ; 1000:2be8 ; -> 1000:2c38
    CMP byte ptr [0x9628],0x1            ; 1000:2bea
    JNZ 0x1000:2bf7                      ; 1000:2bef ; -> 1000:2bf7
    MOV byte ptr [0x8ae],0x1             ; 1000:2bf1
    RET                                  ; 1000:2bf6

; ------------------------------------------------------------------------------
; FUNCTION: player_check_wall_collision_left  [Player / Collision]
; ADDRESS:  1000:2bf7  (Length: 0x3a bytes)
; CALLERS:  FUN_1000_330f, FUN_1000_2a10, FUN_1000_2341
; PURPOSE:  Detects collisions with solid walls to Comic's left side.
; ------------------------------------------------------------------------------
player_check_wall_collision_left:
    MOV byte ptr [0x9629],0xff           ; 1000:2bf7
    MOV AX,[0x6c6]                       ; 1000:2bfc
    OR AX,AX                             ; 1000:2bff
    JNZ 0x1000:2c09                      ; 1000:2c01 ; -> 1000:2c09
    CALL 0x1000:35de                     ; 1000:2c03 ; -> 1000:35de
    JMP 0x1000:2c32                      ; 1000:2c06 ; -> 1000:2c32
    SUB AX,0x8                           ; 1000:2c09
    PUSH AX                              ; 1000:2c0c
    CALL 0x1000:2d61                     ; 1000:2c0d ; -> 1000:2d61
    POP AX                               ; 1000:2c10
    JC 0x1000:2c32                       ; 1000:2c11 ; -> 1000:2c32
    MOV [0x6c6],AX                       ; 1000:2c13
    MOV byte ptr [0x8ae],0x1             ; 1000:2c16
    SUB AX,word ptr [0x6c2]              ; 1000:2c1b
    CMP AX,0x50                          ; 1000:2c1f
    JGE 0x1000:2c38                      ; 1000:2c22 ; -> 1000:2c38
    CMP word ptr [0x6c2],0x0             ; 1000:2c24
    JZ 0x1000:2c38                       ; 1000:2c29 ; -> 1000:2c38
    SUB word ptr [0x6c2],0x8             ; 1000:2c2b
    JMP 0x1000:2c38                      ; 1000:2c30 ; -> 1000:2c38

; ------------------------------------------------------------------------------
; FUNCTION: player_stop_x  [Player / Physics]
; ADDRESS:  1000:2c32  (Length: 0x7 bytes)
; CALLERS:  FUN_1000_2bf7, FUN_1000_3026, FUN_1000_2c54
; PURPOSE:  Halts horizontal velocity when walking into an obstruction.
; ------------------------------------------------------------------------------
player_stop_x:
    MOV word ptr [0x892],0x0             ; 1000:2c32
    RET                                  ; 1000:2c38

; ------------------------------------------------------------------------------
; FUNCTION: player_check_wall_collision_right  [Player / Collision]
; ADDRESS:  1000:2c39  (Length: 0x1b bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Detects collisions with solid walls to Comic's right side.
; ------------------------------------------------------------------------------
player_check_wall_collision_right:
    CMP byte ptr [0x8ad],0x1             ; 1000:2c39
    JZ 0x1000:2c47                       ; 1000:2c3e ; -> 1000:2c47
    MOV byte ptr [0x8ad],0x1             ; 1000:2c40
    JMP 0x1000:2c38                      ; 1000:2c45 ; -> 1000:2c38
    CMP byte ptr [0x9628],0x1            ; 1000:2c47
    JNZ 0x1000:2c54                      ; 1000:2c4c ; -> 1000:2c54
    MOV byte ptr [0x8ae],0x1             ; 1000:2c4e
    RET                                  ; 1000:2c53

; ------------------------------------------------------------------------------
; FUNCTION: player_check_climbing  [Player / Physics]
; ADDRESS:  1000:2c54  (Length: 0x49 bytes)
; CALLERS:  FUN_1000_330f, FUN_1000_2a10, FUN_1000_2341
; PURPOSE:  Checks if Comic is on a ladder tile and processes vertical climbing input.
; ------------------------------------------------------------------------------
player_check_climbing:
    MOV byte ptr [0x9629],0x1            ; 1000:2c54
    MOV AX,[0x6c6]                       ; 1000:2c59
    MOV BX,AX                            ; 1000:2c5c
    ADD BX,0x10                          ; 1000:2c5e
    CMP BX,word ptr [0x29c]              ; 1000:2c61
    JL 0x1000:2c6c                       ; 1000:2c65 ; -> 1000:2c6c
    CALL 0x1000:35de                     ; 1000:2c67 ; -> 1000:35de
    JMP 0x1000:2c32                      ; 1000:2c6a ; -> 1000:2c32
    ADD AX,0x8                           ; 1000:2c6c
    PUSH AX                              ; 1000:2c6f
    ADD AX,0xf                           ; 1000:2c70
    CALL 0x1000:2d61                     ; 1000:2c73 ; -> 1000:2d61
    POP AX                               ; 1000:2c76
    JC 0x1000:2c32                       ; 1000:2c77 ; -> 1000:2c32
    MOV [0x6c6],AX                       ; 1000:2c79
    MOV byte ptr [0x8ae],0x1             ; 1000:2c7c
    SUB AX,word ptr [0x6c2]              ; 1000:2c81
    CMP AX,0x70                          ; 1000:2c85
    JLE 0x1000:2c38                      ; 1000:2c88 ; -> 1000:2c38
    MOV AX,[0x6c2]                       ; 1000:2c8a
    ADD AX,0xd0                          ; 1000:2c8d
    CMP AX,word ptr [0x29c]              ; 1000:2c90
    JZ 0x1000:2c38                       ; 1000:2c94 ; -> 1000:2c38
    ADD word ptr [0x6c2],0x8             ; 1000:2c96
    JMP 0x1000:2c38                      ; 1000:2c9b ; -> 1000:2c38

; ------------------------------------------------------------------------------
; FUNCTION: player_check_swimming  [Player / Physics]
; ADDRESS:  1000:2c9d  (Length: 0x69 bytes)
; CALLERS:  FUN_1000_330f, FUN_1000_2a10, FUN_1000_2341
; PURPOSE:  Checks for water tiles; applies fluid buoyancy, swimming controls, and checks for underwater boots.
; ------------------------------------------------------------------------------
player_check_swimming:
    PUSH BP                              ; 1000:2c9d
    MOV BP,SP                            ; 1000:2c9e
    SUB SP,0x2                           ; 1000:2ca0
    MOV word ptr [BP + -0x2],0x0         ; 1000:2ca3 ; -> Stack[-0x4]
    PUSH AX                              ; 1000:2ca8
    PUSH BX                              ; 1000:2ca9
    CALL 0x1000:1cfe                     ; 1000:2caa ; -> 1000:1cfe
    POP DX                               ; 1000:2cad
    POP CX                               ; 1000:2cae
    CMP AX,0x100                         ; 1000:2caf
    JZ 0x1000:2cfa                       ; 1000:2cb2 ; -> 1000:2cfa
    CMP AX,DI                            ; 1000:2cb4
    JG 0x1000:2cc7                       ; 1000:2cb6 ; -> 1000:2cc7
    MOV word ptr [BP + -0x2],0x1         ; 1000:2cb8 ; -> Stack[-0x4]
    CMP byte ptr [0x8b0],0x1             ; 1000:2cbd
    JL 0x1000:2cd1                       ; 1000:2cc2 ; -> 1000:2cd1
    CALL 0x1000:2d06                     ; 1000:2cc4 ; -> 1000:2d06
    CMP byte ptr [0x8b0],0x2             ; 1000:2cc7
    JL 0x1000:2cd1                       ; 1000:2ccc ; -> 1000:2cd1
    CALL 0x1000:2d31                     ; 1000:2cce ; -> 1000:2d31
    TEST CX,0xc                          ; 1000:2cd1
    JZ 0x1000:2cfa                       ; 1000:2cd5 ; -> 1000:2cfa
    INC BX                               ; 1000:2cd7
    ADD CX,0x10                          ; 1000:2cd8
    MOV AL,byte ptr [BX]                 ; 1000:2cdb
    CMP AX,DI                            ; 1000:2cdd
    JG 0x1000:2cf0                       ; 1000:2cdf ; -> 1000:2cf0
    MOV word ptr [BP + -0x2],0x1         ; 1000:2ce1 ; -> Stack[-0x4]
    CMP byte ptr [0x8b0],0x1             ; 1000:2ce6
    JL 0x1000:2cfa                       ; 1000:2ceb ; -> 1000:2cfa
    CALL 0x1000:2d06                     ; 1000:2ced ; -> 1000:2d06
    CMP byte ptr [0x8b0],0x2             ; 1000:2cf0
    JL 0x1000:2cfa                       ; 1000:2cf5 ; -> 1000:2cfa
    CALL 0x1000:2d31                     ; 1000:2cf7 ; -> 1000:2d31
    CLC                                  ; 1000:2cfa
    CMP word ptr [BP + -0x2],0x0         ; 1000:2cfb ; -> Stack[-0x4]
    JZ 0x1000:2d02                       ; 1000:2cff ; -> 1000:2d02
    STC                                  ; 1000:2d01
    MOV SP,BP                            ; 1000:2d02
    POP BP                               ; 1000:2d04
    RET                                  ; 1000:2d05

; ------------------------------------------------------------------------------
; FUNCTION: player_move_left  [Player / Movement]
; ADDRESS:  1000:2d06  (Length: 0x2b bytes)
; CALLERS:  FUN_1000_2c9d, FUN_1000_5d5f
; PURPOSE:  Applies leftward acceleration to player and sets facing direction to left.
; ------------------------------------------------------------------------------
player_move_left:
    PUSH AX                              ; 1000:2d06
    CMP AL,byte ptr [0x6d6]              ; 1000:2d07
    JL 0x1000:2d2f                       ; 1000:2d0b ; -> 1000:2d2f
    CMP AL,byte ptr [0x6d7]              ; 1000:2d0d
    JG 0x1000:2d2f                       ; 1000:2d11 ; -> 1000:2d2f
    SUB AL,byte ptr [0x6d6]              ; 1000:2d13
    MOV SI,0x6d8                         ; 1000:2d17
    ADD SI,AX                            ; 1000:2d1a
    MOV AL,byte ptr [SI]                 ; 1000:2d1c
    PUSH BX                              ; 1000:2d1e
    PUSH CX                              ; 1000:2d1f
    PUSH DX                              ; 1000:2d20
    PUSH DI                              ; 1000:2d21
    AND CX,0xfff0                        ; 1000:2d22
    AND DX,0xfff0                        ; 1000:2d25
    CALL 0x1000:3181                     ; 1000:2d28 ; -> 1000:3181
    POP DI                               ; 1000:2d2b
    POP DX                               ; 1000:2d2c
    POP CX                               ; 1000:2d2d
    POP BX                               ; 1000:2d2e
    POP AX                               ; 1000:2d2f
    RET                                  ; 1000:2d30

; ------------------------------------------------------------------------------
; FUNCTION: player_move_right  [Player / Movement]
; ADDRESS:  1000:2d31  (Length: 0x30 bytes)
; CALLERS:  FUN_1000_2c9d
; PURPOSE:  Applies rightward acceleration to player and sets facing direction to right.
; ------------------------------------------------------------------------------
player_move_right:
    PUSH AX                              ; 1000:2d31
    CMP AL,byte ptr [0x6e2]              ; 1000:2d32
    JL 0x1000:2d5f                       ; 1000:2d36 ; -> 1000:2d5f
    CMP AL,byte ptr [0x6e3]              ; 1000:2d38
    JG 0x1000:2d5f                       ; 1000:2d3c ; -> 1000:2d5f
    SUB AL,byte ptr [0x6e2]              ; 1000:2d3e
    MOV SI,0x6e4                         ; 1000:2d42
    ADD SI,AX                            ; 1000:2d45
    MOV AL,byte ptr [SI]                 ; 1000:2d47
    MOV word ptr [BP + -0x2],0x1         ; 1000:2d49
    PUSH BX                              ; 1000:2d4e
    PUSH CX                              ; 1000:2d4f
    PUSH DX                              ; 1000:2d50
    PUSH DI                              ; 1000:2d51
    AND CX,0xfff0                        ; 1000:2d52
    AND DX,0xfff0                        ; 1000:2d55
    CALL 0x1000:3181                     ; 1000:2d58 ; -> 1000:3181
    POP DI                               ; 1000:2d5b
    POP DX                               ; 1000:2d5c
    POP CX                               ; 1000:2d5d
    POP BX                               ; 1000:2d5e
    POP AX                               ; 1000:2d5f
    RET                                  ; 1000:2d60

; ------------------------------------------------------------------------------
; FUNCTION: player_do_jump  [Player / Physics]
; ADDRESS:  1000:2d61  (Length: 0x65 bytes)
; CALLERS:  FUN_1000_2bf7, FUN_1000_2c54
; PURPOSE:  Initiates jump state, applies vertical impulse (enhanced if jump boots are held), and plays jump sound.
; ------------------------------------------------------------------------------
player_do_jump:
    MOV BX,word ptr [0x6c8]              ; 1000:2d61
    ADD BX,0xd                           ; 1000:2d65
    PUSH AX                              ; 1000:2d68
    CALL 0x1000:1cfe                     ; 1000:2d69 ; -> 1000:1cfe
    POP CX                               ; 1000:2d6c
    SHR CX,0x1                           ; 1000:2d6d
    SHR CX,0x1                           ; 1000:2d6f
    SHR CX,0x1                           ; 1000:2d71
    SHR CX,0x1                           ; 1000:2d73
    CMP AX,word ptr [0x292]              ; 1000:2d75
    JLE 0x1000:2d9f                      ; 1000:2d79 ; -> 1000:2d9f
    ADD BX,word ptr [0x298]              ; 1000:2d7b
    MOV AL,byte ptr [BX]                 ; 1000:2d7f
    CMP AX,word ptr [0x292]              ; 1000:2d81
    JG 0x1000:2da1                       ; 1000:2d85 ; -> 1000:2da1
    SUB BX,CX                            ; 1000:2d87
    MOV CX,word ptr [0x6c6]              ; 1000:2d89
    SHR CX,0x1                           ; 1000:2d8d
    SHR CX,0x1                           ; 1000:2d8f
    SHR CX,0x1                           ; 1000:2d91
    SHR CX,0x1                           ; 1000:2d93
    ADD BX,CX                            ; 1000:2d95
    MOV AL,byte ptr [BX]                 ; 1000:2d97
    CMP AX,word ptr [0x292]              ; 1000:2d99
    JLE 0x1000:2dc4                      ; 1000:2d9d ; -> 1000:2dc4
    STC                                  ; 1000:2d9f
    RET                                  ; 1000:2da0
    CMP AX,word ptr [0x294]              ; 1000:2da1
    JG 0x1000:2dc4                       ; 1000:2da5 ; -> 1000:2dc4
    SUB BX,CX                            ; 1000:2da7
    MOV CX,word ptr [0x6c6]              ; 1000:2da9
    SHR CX,0x1                           ; 1000:2dad
    SHR CX,0x1                           ; 1000:2daf
    SHR CX,0x1                           ; 1000:2db1
    SHR CX,0x1                           ; 1000:2db3
    ADD BX,CX                            ; 1000:2db5
    MOV AL,byte ptr [BX]                 ; 1000:2db7
    CMP AX,word ptr [0x294]              ; 1000:2db9
    JLE 0x1000:2dc4                      ; 1000:2dbd ; -> 1000:2dc4
    MOV byte ptr [0x89b],0x0             ; 1000:2dbf
    CLC                                  ; 1000:2dc4
    RET                                  ; 1000:2dc5

; ------------------------------------------------------------------------------
; FUNCTION: player_stop_jump  [Player / Physics]
; ADDRESS:  1000:2dc6  (Length: 0xa bytes)
; CALLERS:  FUN_1000_330f, FUN_1000_2a10, FUN_1000_2341
; PURPOSE:  Trims jump velocity when jump key is released early for variable jump height.
; ------------------------------------------------------------------------------
player_stop_jump:
    MOV AX,0x1                           ; 1000:2dc6
    MOV BX,0x965e                        ; 1000:2dc9
    MOV CX,0x5                           ; 1000:2dcc
    INT3                                 ; 1000:2dcf

; ------------------------------------------------------------------------------
; FUNCTION: player_process_controls  [Player / Input]
; ADDRESS:  1000:2edc  (Length: 0x110 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Translates raw keyboard/joystick input vectors into player movement, jumping, and shooting actions.
; ------------------------------------------------------------------------------
player_process_controls:
    MOV AX,[0x894]                       ; 1000:2edc
    MOV CX,AX                            ; 1000:2edf
    ADD AX,word ptr [0x6c8]              ; 1000:2ee1
    CMP byte ptr CS:[0x25a],0x1          ; 1000:2ee5 ; -> 1000:025a
    JNZ 0x1000:2ef0                      ; 1000:2eeb ; -> 1000:2ef0
    SUB AX,0x2                           ; 1000:2eed
    OR AX,AX                             ; 1000:2ef0
    JGE 0x1000:2ef6                      ; 1000:2ef2 ; -> 1000:2ef6
    XOR AX,AX                            ; 1000:2ef4
    PUSH AX                              ; 1000:2ef6
    MOV BX,AX                            ; 1000:2ef7
    MOV AX,[0x6c6]                       ; 1000:2ef9
    MOV DI,word ptr [0x292]              ; 1000:2efc
    CMP word ptr [0x894],0x0             ; 1000:2f00
    JZ 0x1000:2f2d                       ; 1000:2f05 ; -> 1000:2f2d
    JL 0x1000:2f1b                       ; 1000:2f07 ; -> 1000:2f1b
    ADD BX,0xf                           ; 1000:2f09
    CALL 0x1000:2fee                     ; 1000:2f0c ; -> 1000:2fee
    JNC 0x1000:2f2d                      ; 1000:2f0f ; -> 1000:2f2d
    POP AX                               ; 1000:2f11
    AND AX,0xfff0                        ; 1000:2f12
    MOV [0x6c8],AX                       ; 1000:2f15
    JMP 0x1000:2f31                      ; 1000:2f18 ; -> 1000:2f31
    CALL 0x1000:2fee                     ; 1000:2f1b ; -> 1000:2fee
    JNC 0x1000:2f2d                      ; 1000:2f1e ; -> 1000:2f2d
    POP AX                               ; 1000:2f20
    AND AX,0xfff0                        ; 1000:2f21
    ADD AX,0x10                          ; 1000:2f24
    MOV [0x6c8],AX                       ; 1000:2f27
    JMP 0x1000:2f31                      ; 1000:2f2a ; -> 1000:2f31
    POP AX                               ; 1000:2f2d
    MOV [0x6c8],AX                       ; 1000:2f2e
    MOV CX,word ptr [0x894]              ; 1000:2f31
    SUB CX,0x5                           ; 1000:2f35
    CMP CX,-0x4                          ; 1000:2f38
    JGE 0x1000:2f40                      ; 1000:2f3b ; -> 1000:2f40
    MOV CX,0xfffc                        ; 1000:2f3d
    MOV word ptr [0x894],CX              ; 1000:2f40
    MOV CX,word ptr [0x892]              ; 1000:2f44
    CMP byte ptr CS:[0x258],0x1          ; 1000:2f48 ; -> 1000:0258
    JNZ 0x1000:2f5e                      ; 1000:2f4e ; -> 1000:2f5e
    MOV byte ptr [0x8ad],0x2             ; 1000:2f50
    DEC CX                               ; 1000:2f55
    CMP CX,-0x5                          ; 1000:2f56
    JGE 0x1000:2f5e                      ; 1000:2f59 ; -> 1000:2f5e
    MOV CX,0xfffb                        ; 1000:2f5b
    CMP byte ptr CS:[0x259],0x1          ; 1000:2f5e ; -> 1000:0259
    JNZ 0x1000:2f74                      ; 1000:2f64 ; -> 1000:2f74
    MOV byte ptr [0x8ad],0x1             ; 1000:2f66
    INC CX                               ; 1000:2f6b
    CMP CX,0x5                           ; 1000:2f6c
    JLE 0x1000:2f74                      ; 1000:2f6f ; -> 1000:2f74
    MOV CX,0x5                           ; 1000:2f71
    MOV word ptr [0x892],CX              ; 1000:2f74
    OR CX,CX                             ; 1000:2f78
    JZ 0x1000:2f8e                       ; 1000:2f7a ; -> 1000:2f8e
    JG 0x1000:2f87                       ; 1000:2f7c ; -> 1000:2f87
    INC word ptr [0x892]                 ; 1000:2f7e
    CALL 0x1000:3026                     ; 1000:2f82 ; -> 1000:3026
    JMP 0x1000:2f8e                      ; 1000:2f85 ; -> 1000:2f8e
    DEC word ptr [0x892]                 ; 1000:2f87
    CALL 0x1000:305e                     ; 1000:2f8b ; -> 1000:305e
    CMP byte ptr CS:[0x25b],0x1          ; 1000:2f8e ; -> 1000:025b
    JNZ 0x1000:2fa9                      ; 1000:2f94 ; -> 1000:2fa9
    MOV CX,word ptr [0x894]              ; 1000:2f96
    ADD CX,0x7                           ; 1000:2f9a
    CMP CX,0x4                           ; 1000:2f9d
    JLE 0x1000:2fa5                      ; 1000:2fa0 ; -> 1000:2fa5
    MOV CX,0x4                           ; 1000:2fa2
    MOV word ptr [0x894],CX              ; 1000:2fa5
    MOV AX,[0x6c6]                       ; 1000:2fa9
    ADD AX,0x10                          ; 1000:2fac
    MOV BX,word ptr [0x6c8]              ; 1000:2faf
    CALL 0x1000:1cfe                     ; 1000:2fb3 ; -> 1000:1cfe
    CMP AX,word ptr [0x296]              ; 1000:2fb6
    JLE 0x1000:2fd1                      ; 1000:2fba ; -> 1000:2fd1
    AND word ptr [0x6c8],0xfff0          ; 1000:2fbc
    ADD word ptr [0x6c8],0xc             ; 1000:2fc1
    CMP byte ptr CS:[0x25c],0x1          ; 1000:2fc6 ; -> 1000:025c
    JNZ 0x1000:2fd1                      ; 1000:2fcc ; -> 1000:2fd1
    JMP 0x1000:2e4f                      ; 1000:2fce ; -> 1000:2e4f
    MOV BX,word ptr [0x6c8]              ; 1000:2fd1
    MOV AX,[0x6c6]                       ; 1000:2fd5
    ADD AX,0x10                          ; 1000:2fd8
    ADD BX,0x8                           ; 1000:2fdb
    CALL 0x1000:3019                     ; 1000:2fde ; -> 1000:3019
    JC 0x1000:2fe6                       ; 1000:2fe1 ; -> 1000:2fe6
    JMP 0x1000:2e96                      ; 1000:2fe3 ; -> 1000:2e96
    MOV byte ptr [0x8ae],0x3             ; 1000:2fe6
    JMP 0x1000:2b88                      ; 1000:2feb ; -> 1000:2b88

; ------------------------------------------------------------------------------
; FUNCTION: player_take_damage  [Player / Health]
; ADDRESS:  1000:2fee  (Length: 0x2b bytes)
; CALLERS:  FUN_1000_2edc
; PURPOSE:  Deducts shield points, sets invulnerability timer, plays damage sound effect, and triggers death if shields reach zero.
; ------------------------------------------------------------------------------
player_take_damage:
    CMP BX,word ptr [0x29e]              ; 1000:2fee
    JGE 0x1000:3011                      ; 1000:2ff2 ; -> 1000:3011
    PUSH AX                              ; 1000:2ff4
    CALL 0x1000:1cfe                     ; 1000:2ff5 ; -> 1000:1cfe
    POP CX                               ; 1000:2ff8
    CMP AX,DI                            ; 1000:2ff9
    JLE 0x1000:3011                      ; 1000:2ffb ; -> 1000:3011
    MOV AL,byte ptr [BX + 0x1]           ; 1000:2ffd
    CMP AX,DI                            ; 1000:3000
    JLE 0x1000:3011                      ; 1000:3002 ; -> 1000:3011
    TEST CX,0xc                          ; 1000:3004
    JZ 0x1000:3015                       ; 1000:3008 ; -> 1000:3015
    MOV AL,byte ptr [BX + 0x2]           ; 1000:300a
    CMP AX,DI                            ; 1000:300d
    JG 0x1000:3015                       ; 1000:300f ; -> 1000:3015
    XOR AX,AX                            ; 1000:3011
    STC                                  ; 1000:3013
    RET                                  ; 1000:3014
    XOR AX,AX                            ; 1000:3015
    CLC                                  ; 1000:3017
    RET                                  ; 1000:3018

; ------------------------------------------------------------------------------
; FUNCTION: player_add_health  [Player / Health]
; ADDRESS:  1000:3019  (Length: 0xd bytes)
; CALLERS:  FUN_1000_2edc
; PURPOSE:  Increases player shield points up to maximum capacity and plays recharge sound.
; ------------------------------------------------------------------------------
player_add_health:
    CALL 0x1000:1cfe                     ; 1000:3019 ; -> 1000:1cfe
    CMP AX,word ptr [0x296]              ; 1000:301c
    JG 0x1000:3024                       ; 1000:3020 ; -> 1000:3024
    STC                                  ; 1000:3022
    RET                                  ; 1000:3023
    CLC                                  ; 1000:3024
    RET                                  ; 1000:3025

; ------------------------------------------------------------------------------
; FUNCTION: player_check_hazard_collision  [Player / Collision]
; ADDRESS:  1000:3026  (Length: 0x31 bytes)
; CALLERS:  FUN_1000_2edc
; PURPOSE:  Checks player bounding box against environmental hazards (spikes, lava, acid).
; ------------------------------------------------------------------------------
player_check_hazard_collision:
    MOV AX,[0x6c6]                       ; 1000:3026
    OR AX,AX                             ; 1000:3029
    JNZ 0x1000:3033                      ; 1000:302b ; -> 1000:3033
    CALL 0x1000:35de                     ; 1000:302d ; -> 1000:35de
    JMP 0x1000:2c32                      ; 1000:3030 ; -> 1000:2c32
    SUB AX,0x8                           ; 1000:3033
    PUSH AX                              ; 1000:3036
    CALL 0x1000:309d                     ; 1000:3037 ; -> 1000:309d
    POP AX                               ; 1000:303a
    JC 0x1000:3057                       ; 1000:303b ; -> 1000:3057
    MOV [0x6c6],AX                       ; 1000:303d
    SUB AX,word ptr [0x6c2]              ; 1000:3040
    CMP AX,0x50                          ; 1000:3044
    JGE 0x1000:305d                      ; 1000:3047 ; -> 1000:305d
    CMP word ptr [0x6c2],0x0             ; 1000:3049
    JZ 0x1000:305d                       ; 1000:304e ; -> 1000:305d
    SUB word ptr [0x6c2],0x8             ; 1000:3050
    JMP 0x1000:305d                      ; 1000:3055 ; -> 1000:305d

; ------------------------------------------------------------------------------
; FUNCTION: player_zero_score_delta  [Player / Score]
; ADDRESS:  1000:3057  (Length: 0x7 bytes)
; CALLERS:  FUN_1000_305e
; PURPOSE:  Resets per-frame score delta accumulator.
; ------------------------------------------------------------------------------
player_zero_score_delta:
    MOV word ptr [0x892],0x0             ; 1000:3057
    RET                                  ; 1000:305d

; ------------------------------------------------------------------------------
; FUNCTION: player_check_crush  [Player / Collision]
; ADDRESS:  1000:305e  (Length: 0x3f bytes)
; CALLERS:  FUN_1000_2edc
; PURPOSE:  Checks if Comic is crushed between moving platforms or solid blocks.
; ------------------------------------------------------------------------------
player_check_crush:
    MOV AX,[0x6c6]                       ; 1000:305e
    MOV BX,AX                            ; 1000:3061
    ADD BX,0x20                          ; 1000:3063
    CMP BX,word ptr [0x29c]              ; 1000:3066
    JL 0x1000:3071                       ; 1000:306a ; -> 1000:3071
    CALL 0x1000:35de                     ; 1000:306c ; -> 1000:35de
    JMP 0x1000:3057                      ; 1000:306f ; -> 1000:3057
    ADD AX,0x8                           ; 1000:3071
    PUSH AX                              ; 1000:3074
    ADD AX,0x1f                          ; 1000:3075
    CALL 0x1000:309d                     ; 1000:3078 ; -> 1000:309d
    POP AX                               ; 1000:307b
    JC 0x1000:3057                       ; 1000:307c ; -> 1000:3057
    MOV [0x6c6],AX                       ; 1000:307e
    SUB AX,word ptr [0x6c2]              ; 1000:3081
    CMP AX,0x60                          ; 1000:3085
    JLE 0x1000:305d                      ; 1000:3088 ; -> 1000:305d
    MOV AX,[0x6c2]                       ; 1000:308a
    ADD AX,0xd0                          ; 1000:308d
    CMP AX,word ptr [0x29c]              ; 1000:3090
    JZ 0x1000:305d                       ; 1000:3094 ; -> 1000:305d
    ADD word ptr [0x6c2],0x8             ; 1000:3096
    JMP 0x1000:305d                      ; 1000:309b ; -> 1000:305d

; ------------------------------------------------------------------------------
; FUNCTION: player_fall_death  [Player / Lifecycle]
; ADDRESS:  1000:309d  (Length: 0x29 bytes)
; CALLERS:  FUN_1000_3026, FUN_1000_305e
; PURPOSE:  Handles player falling into bottomless pit / lethal hazard.
; ------------------------------------------------------------------------------
player_fall_death:
    MOV DI,word ptr [0x294]              ; 1000:309d
    MOV BX,word ptr [0x6c8]              ; 1000:30a1
    CALL 0x1000:1cfe                     ; 1000:30a5 ; -> 1000:1cfe
    CMP AX,DI                            ; 1000:30a8
    JLE 0x1000:30c2                      ; 1000:30aa ; -> 1000:30c2
    MOV CX,word ptr [0x6c8]              ; 1000:30ac
    AND CX,0xf                           ; 1000:30b0
    CMP CX,0x4                           ; 1000:30b3
    JLE 0x1000:30c4                      ; 1000:30b6 ; -> 1000:30c4
    ADD BX,word ptr [0x298]              ; 1000:30b8
    MOV AL,byte ptr [BX]                 ; 1000:30bc
    CMP AX,DI                            ; 1000:30be
    JG 0x1000:30c4                       ; 1000:30c0 ; -> 1000:30c4
    STC                                  ; 1000:30c2
    RET                                  ; 1000:30c3
    CLC                                  ; 1000:30c4
    RET                                  ; 1000:30c5

; ------------------------------------------------------------------------------
; FUNCTION: player_fire_projectile  [Weapons / Fire]
; ADDRESS:  1000:30c6  (Length: 0x41 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Spawns player blast fireball, checks weapon power level (normal, corkscrew, fireball), deducts ammo/power, and plays sound.
; ------------------------------------------------------------------------------
player_fire_projectile:
    CMP byte ptr [0x8ad],0x1             ; 1000:30c6
    JZ 0x1000:30db                       ; 1000:30cb ; -> 1000:30db
    CMP word ptr [0x6c6],0x8             ; 1000:30cd
    JL 0x1000:3106                       ; 1000:30d2 ; -> 1000:3106
    MOV byte ptr [0x8a5],0x2             ; 1000:30d4
    JMP 0x1000:30ec                      ; 1000:30d9 ; -> 1000:30ec
    MOV AX,[0x29c]                       ; 1000:30db
    SUB AX,0x18                          ; 1000:30de
    CMP word ptr [0x6c6],AX              ; 1000:30e1
    JG 0x1000:3106                       ; 1000:30e5 ; -> 1000:3106
    MOV byte ptr [0x8a5],0x1             ; 1000:30e7
    MOV byte ptr [0x8a3],0x1             ; 1000:30ec
    MOV byte ptr [0x8a4],0x3             ; 1000:30f1
    MOV byte ptr [0x8af],0x1             ; 1000:30f6
    MOV byte ptr [0x8ae],0x4             ; 1000:30fb
    MOV word ptr [0x892],0x0             ; 1000:3100
    RET                                  ; 1000:3106

; ------------------------------------------------------------------------------
; FUNCTION: proj_init_trajectory  [Weapons / Projectiles]
; ADDRESS:  1000:3107  (Length: 0x7a bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Calculates initial velocity vector, sprite angle, and lifetime for spawned projectile.
; ------------------------------------------------------------------------------
proj_init_trajectory:
    CMP byte ptr [0x8a4],0x3             ; 1000:3107
    JNZ 0x1000:3123                      ; 1000:310c ; -> 1000:3123
    CMP byte ptr CS:[0x25d],0x1          ; 1000:310e ; -> 1000:025d
    JNZ 0x1000:3123                      ; 1000:3114 ; -> 1000:3123
    MOV byte ptr [0x8af],0x1             ; 1000:3116
    MOV byte ptr [0x8ae],0x4             ; 1000:311b
    JMP 0x1000:267f                      ; 1000:3120 ; -> 1000:267f
    DEC byte ptr [0x8a4]                 ; 1000:3123
    JNZ 0x1000:3135                      ; 1000:3127 ; -> 1000:3135
    MOV byte ptr [0x8a3],0x0             ; 1000:3129
    MOV byte ptr [0x8ae],0x0             ; 1000:312e
    JMP 0x1000:317e                      ; 1000:3133 ; -> 1000:317e
    MOV AX,[0x6c6]                       ; 1000:3135
    TEST AX,0x8                          ; 1000:3138
    JNZ 0x1000:3179                      ; 1000:313b ; -> 1000:3179
    SUB AX,0x10                          ; 1000:313d
    CMP byte ptr [0x8a5],0x1             ; 1000:3140
    JNZ 0x1000:314a                      ; 1000:3145 ; -> 1000:314a
    ADD AX,0x20                          ; 1000:3147
    MOV BX,word ptr [0x6c8]              ; 1000:314a
    CMP byte ptr [0x8a4],0x2             ; 1000:314e
    JZ 0x1000:3158                       ; 1000:3153 ; -> 1000:3158
    ADD BX,0x10                          ; 1000:3155
    PUSH AX                              ; 1000:3158
    PUSH BX                              ; 1000:3159
    CALL 0x1000:1cfe                     ; 1000:315a ; -> 1000:1cfe
    POP DX                               ; 1000:315d
    POP CX                               ; 1000:315e
    CMP AL,byte ptr [0x6ca]              ; 1000:315f
    JL 0x1000:3179                       ; 1000:3163 ; -> 1000:3179
    CMP AL,byte ptr [0x6cb]              ; 1000:3165
    JG 0x1000:3179                       ; 1000:3169 ; -> 1000:3179
    SUB AL,byte ptr [0x6ca]              ; 1000:316b
    MOV SI,0x6cc                         ; 1000:316f
    ADD SI,AX                            ; 1000:3172
    MOV AL,byte ptr [SI]                 ; 1000:3174
    CALL 0x1000:3181                     ; 1000:3176 ; -> 1000:3181
    MOV byte ptr [0x8ae],0x4             ; 1000:3179
    JMP 0x1000:267f                      ; 1000:317e ; -> 1000:267f

; ------------------------------------------------------------------------------
; FUNCTION: proj_spawn_burst  [Weapons / Projectiles]
; ADDRESS:  1000:3181  (Length: 0x69 bytes)
; CALLERS:  FUN_1000_3107, FUN_1000_2d06, FUN_1000_2d31
; PURPOSE:  Spawns multiple projectile burst when upgraded weapons (corkscrew / spread blast) are active.
; ------------------------------------------------------------------------------
proj_spawn_burst:
    CMP byte ptr [0x8b1],0x6             ; 1000:3181
    JZ 0x1000:31ea                       ; 1000:3186 ; -> 1000:31ea
    MOV SI,0x8b2                         ; 1000:3188
    MOV DI,0x6                           ; 1000:318b
    CMP word ptr [SI + 0x5],CX           ; 1000:318e
    JNZ 0x1000:319b                      ; 1000:3191 ; -> 1000:319b
    CMP word ptr [SI + 0x7],DX           ; 1000:3193
    JNZ 0x1000:319b                      ; 1000:3196 ; -> 1000:319b
    JMP 0x1000:31ea                      ; 1000:3198 ; -> 1000:31ea
    ADD SI,0x9                           ; 1000:319b
    DEC DI                               ; 1000:319e
    JNZ 0x1000:318e                      ; 1000:319f ; -> 1000:318e
    MOV SI,0x8b2                         ; 1000:31a1
    CMP byte ptr [SI],0x0                ; 1000:31a4
    JZ 0x1000:31ae                       ; 1000:31a7 ; -> 1000:31ae
    ADD SI,0x9                           ; 1000:31a9
    JMP 0x1000:31a4                      ; 1000:31ac ; -> 1000:31a4
    INC byte ptr [0x8b1]                 ; 1000:31ae
    MOV byte ptr [SI],0x1                ; 1000:31b2
    MOV byte ptr [SI + 0x1],AL           ; 1000:31b5
    MOV word ptr [SI + 0x2],BX           ; 1000:31b8
    MOV byte ptr [SI + 0x4],0x0          ; 1000:31bb
    MOV word ptr [SI + 0x5],CX           ; 1000:31bf
    MOV word ptr [SI + 0x7],DX           ; 1000:31c2
    MOV AX,CX                            ; 1000:31c5
    MOV BX,DX                            ; 1000:31c7
    MOV CL,byte ptr [SI + 0x1]           ; 1000:31c9
    XOR CH,CH                            ; 1000:31cc
    PUSH AX                              ; 1000:31ce
    PUSH BX                              ; 1000:31cf
    PUSH CX                              ; 1000:31d0
    CALL 0x1000:1e09                     ; 1000:31d1 ; -> 1000:1e09
    POP CX                               ; 1000:31d4
    POP BX                               ; 1000:31d5
    POP AX                               ; 1000:31d6
    CMP CX,word ptr [0x292]              ; 1000:31d7
    JLE 0x1000:31e0                      ; 1000:31db ; -> 1000:31e0
    CALL 0x1000:4579                     ; 1000:31dd ; -> 1000:4579
    MOV AX,0x1                           ; 1000:31e0
    MOV BX,0x968e                        ; 1000:31e3
    MOV CX,0x5                           ; 1000:31e6
    INT3                                 ; 1000:31e9
    RET                                  ; 1000:31ea

; ------------------------------------------------------------------------------
; FUNCTION: proj_update_all  [Weapons / Projectiles]
; ADDRESS:  1000:31eb  (Length: 0x92 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Updates all active player projectiles: advances position, checks tile collision, and triggers explosions.
; ------------------------------------------------------------------------------
proj_update_all:
    MOV DI,0x8b2                         ; 1000:31eb
    MOV word ptr [0x9620],0x0            ; 1000:31ee
    CMP byte ptr [DI],0x0                ; 1000:31f4 ; -> 34fe:08bb
    JZ 0x1000:326d                       ; 1000:31f7 ; -> 1000:326d
    INC word ptr [0x9620]                ; 1000:31f9 ; -> 34fe:9620
    MOV SI,0x9622                        ; 1000:31fd
    MOV BL,byte ptr [DI]                 ; 1000:3200 ; -> 34fe:08bb
    XOR BH,BH                            ; 1000:3202
    DEC BL                               ; 1000:3204
    SHL BX,0x1                           ; 1000:3206
    MOV SI,word ptr [BX + SI]            ; 1000:3208
    MOV AL,byte ptr [DI + 0x4]           ; 1000:320a ; -> 34fe:08bf
    XOR AH,AH                            ; 1000:320d
    SHL AX,0x1                           ; 1000:320f
    ADD SI,AX                            ; 1000:3211
    MOV AX,word ptr [DI + 0x5]           ; 1000:3213 ; -> 34fe:08c0
    SUB AX,word ptr [0x6c2]              ; 1000:3216 ; -> 34fe:06c2
    JL 0x1000:3247                       ; 1000:321a ; -> 1000:3247
    CMP AX,0xc0                          ; 1000:321c
    JG 0x1000:3247                       ; 1000:321f ; -> 1000:3247
    ADD AX,0x8                           ; 1000:3221
    MOV BX,word ptr [DI + 0x7]           ; 1000:3224
    SUB BX,word ptr [0x6c4]              ; 1000:3227
    JL 0x1000:3247                       ; 1000:322b ; -> 1000:3247
    CMP BX,0x90                          ; 1000:322d
    JG 0x1000:3247                       ; 1000:3231 ; -> 1000:3247
    ADD BX,0x8                           ; 1000:3233
    PUSH DI                              ; 1000:3236
    MOV DS,word ptr CS:[0x5e]            ; 1000:3237 ; -> 1000:005e
    MOV SI,word ptr [SI]                 ; 1000:323c
    CALL 0x1000:77a3                     ; 1000:323e ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:3241 ; -> 1000:005c
    POP DI                               ; 1000:3246
    INC byte ptr [DI + 0x4]              ; 1000:3247 ; -> 34fe:08b6
    CMP byte ptr [DI + 0x4],0x3          ; 1000:324a ; -> 34fe:08b6
    JNZ 0x1000:326d                      ; 1000:324e ; -> 1000:326d
    CMP byte ptr [DI],0x1                ; 1000:3250 ; -> 34fe:08b2
    JNZ 0x1000:325d                      ; 1000:3253 ; -> 1000:325d
    MOV BX,word ptr [DI + 0x2]           ; 1000:3255 ; -> 34fe:08b4
    MOV AL,byte ptr [DI + 0x1]           ; 1000:3258 ; -> 34fe:08b3
    MOV byte ptr [BX],AL                 ; 1000:325b
    MOV byte ptr [DI],0x0                ; 1000:325d ; -> 34fe:08b2
    MOV word ptr [DI + 0x5],0xffff       ; 1000:3260 ; -> 34fe:08b7
    DEC byte ptr [0x8b1]                 ; 1000:3265 ; -> 34fe:08b1
    DEC word ptr [0x9620]                ; 1000:3269 ; -> 34fe:9620
    ADD DI,0x9                           ; 1000:326d
    MOV AX,[0x9620]                      ; 1000:3270 ; -> 34fe:9620
    CMP AL,byte ptr [0x8b1]              ; 1000:3273 ; -> 34fe:08b1
    JZ 0x1000:327c                       ; 1000:3277 ; -> 1000:327c
    JMP 0x1000:31f4                      ; 1000:3279 ; -> 1000:31f4
    RET                                  ; 1000:327c

; ------------------------------------------------------------------------------
; FUNCTION: proj_check_monster_hit  [Weapons / Collision]
; ADDRESS:  1000:327d  (Length: 0x92 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Tests intersection between player projectiles and monster bounding boxes; damages monsters.
; ------------------------------------------------------------------------------
proj_check_monster_hit:
    CMP byte ptr [0x89d],0x1             ; 1000:327d
    JZ 0x1000:3290                       ; 1000:3282 ; -> 1000:3290
    CMP byte ptr [0x924],0x2             ; 1000:3284
    JZ 0x1000:3290                       ; 1000:3289 ; -> 1000:3290
    CMP byte ptr [0x92c],0x2             ; 1000:328b
    JZ 0x1000:330e                       ; 1000:3290 ; -> 1000:330e
    MOV BL,byte ptr [0x896]              ; 1000:3292
    XOR BH,BH                            ; 1000:3296
    MOV AH,BL                            ; 1000:3298
    ADD AH,AL                            ; 1000:329a
    CMP AH,0x4                           ; 1000:329c
    JGE 0x1000:330e                      ; 1000:329f ; -> 1000:330e
    CMP AH,0x0                           ; 1000:32a1
    JL 0x1000:330e                       ; 1000:32a4 ; -> 1000:330e
    JZ 0x1000:32cb                       ; 1000:32a6 ; -> 1000:32cb
    CMP AH,0x3                           ; 1000:32a8
    JNZ 0x1000:32b6                      ; 1000:32ab ; -> 1000:32b6
    CMP byte ptr [0x21f],0x1             ; 1000:32ad
    JZ 0x1000:32cb                       ; 1000:32b2 ; -> 1000:32cb
    JMP 0x1000:329a                      ; 1000:32b4 ; -> 1000:329a
    CMP AH,0x2                           ; 1000:32b6
    JNZ 0x1000:32c4                      ; 1000:32b9 ; -> 1000:32c4
    CMP byte ptr [0x21e],0x1             ; 1000:32bb
    JZ 0x1000:32cb                       ; 1000:32c0 ; -> 1000:32cb
    JMP 0x1000:329a                      ; 1000:32c2 ; -> 1000:329a
    CMP byte ptr [0x21d],0x1             ; 1000:32c4
    JNZ 0x1000:329a                      ; 1000:32c9 ; -> 1000:329a
    MOV byte ptr [0x8a3],0x0             ; 1000:32cb
    MOV byte ptr [0x896],AH              ; 1000:32d0
    CALL 0x1000:3a17                     ; 1000:32d4 ; -> 1000:3a17
    CMP byte ptr [0x896],0x2             ; 1000:32d7
    JNZ 0x1000:32f8                      ; 1000:32dc ; -> 1000:32f8
    CMP word ptr [0x222],0x0             ; 1000:32de
    JZ 0x1000:330e                       ; 1000:32e3 ; -> 1000:330e
    CMP word ptr [0x6fa],0xe             ; 1000:32e5
    JZ 0x1000:330e                       ; 1000:32ea ; -> 1000:330e
    MOV byte ptr [0x897],0x0             ; 1000:32ec
    MOV byte ptr [0x8a6],0x1             ; 1000:32f1
    JMP 0x1000:330e                      ; 1000:32f6 ; -> 1000:330e
    CMP byte ptr [0x8a6],0x1             ; 1000:32f8
    JNZ 0x1000:330e                      ; 1000:32fd ; -> 1000:330e
    MOV byte ptr [0x8a6],0x0             ; 1000:32ff
    MOV byte ptr [0x897],0x1             ; 1000:3304
    MOV byte ptr [0x898],0x0             ; 1000:3309
    RET                                  ; 1000:330e

; ------------------------------------------------------------------------------
; FUNCTION: ent_update_all  [Entities / AI]
; ADDRESS:  1000:330f  (Length: 0x158 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Iterates through all active monsters: updates movement AI, animation frames, and tests contact with Comic.
; ------------------------------------------------------------------------------
ent_update_all:
    CMP word ptr [0x6fa],0xe             ; 1000:330f
    JZ 0x1000:331d                       ; 1000:3314 ; -> 1000:331d
    CMP word ptr [0x222],0x0             ; 1000:3316
    JNZ 0x1000:332f                      ; 1000:331b ; -> 1000:332f
    MOV byte ptr [0x8a6],0x0             ; 1000:331d
    MOV byte ptr [0x897],0x1             ; 1000:3322
    MOV byte ptr [0x898],0x0             ; 1000:3327
    JMP 0x1000:2a10                      ; 1000:332c ; -> 1000:2a10
    TEST byte ptr [0x962b],0x3           ; 1000:332f
    JNZ 0x1000:3341                      ; 1000:3334 ; -> 1000:3341
    CMP word ptr CS:[0x14d4],-0x1        ; 1000:3336 ; -> 1000:14d4
    JZ 0x1000:3341                       ; 1000:333c ; -> 1000:3341
    CALL 0x1000:48aa                     ; 1000:333e ; -> 1000:48aa
    CMP byte ptr CS:[0x25d],0x1          ; 1000:3341 ; -> 1000:025d
    JZ 0x1000:3351                       ; 1000:3347 ; -> 1000:3351
    CMP byte ptr CS:[0x25a],0x1          ; 1000:3349 ; -> 1000:025a
    JNZ 0x1000:3363                      ; 1000:334f ; -> 1000:3363
    SUB word ptr [0x894],0x2             ; 1000:3351
    CMP word ptr [0x894],-0xc            ; 1000:3356
    JGE 0x1000:3363                      ; 1000:335b ; -> 1000:3363
    MOV word ptr [0x894],0xfff4          ; 1000:335d
    MOV AX,[0x894]                       ; 1000:3363
    MOV CX,AX                            ; 1000:3366
    ADD AX,word ptr [0x6c8]              ; 1000:3368
    JGE 0x1000:3372                      ; 1000:336c ; -> 1000:3372
    XOR AX,AX                            ; 1000:336e
    XOR CX,CX                            ; 1000:3370
    MOV [0x6c8],AX                       ; 1000:3372
    MOV word ptr [0x9620],CX             ; 1000:3375
    ADD CX,0x1                           ; 1000:3379
    CMP CX,0x8                           ; 1000:337c
    JLE 0x1000:3384                      ; 1000:337f ; -> 1000:3384
    MOV CX,0x8                           ; 1000:3381
    MOV word ptr [0x894],CX              ; 1000:3384
    MOV CX,word ptr [0x892]              ; 1000:3388
    CMP byte ptr CS:[0x258],0x1          ; 1000:338c ; -> 1000:0258
    JNZ 0x1000:33a2                      ; 1000:3392 ; -> 1000:33a2
    MOV byte ptr [0x8ad],0x2             ; 1000:3394
    DEC CX                               ; 1000:3399
    CMP CX,-0x5                          ; 1000:339a
    JGE 0x1000:33a2                      ; 1000:339d ; -> 1000:33a2
    MOV CX,0xfffb                        ; 1000:339f
    CMP byte ptr CS:[0x259],0x1          ; 1000:33a2 ; -> 1000:0259
    JNZ 0x1000:33b8                      ; 1000:33a8 ; -> 1000:33b8
    MOV byte ptr [0x8ad],0x1             ; 1000:33aa
    INC CX                               ; 1000:33af
    CMP CX,0x5                           ; 1000:33b0
    JLE 0x1000:33b8                      ; 1000:33b3 ; -> 1000:33b8
    MOV CX,0x5                           ; 1000:33b5
    MOV word ptr [0x892],CX              ; 1000:33b8
    OR CX,CX                             ; 1000:33bc
    JZ 0x1000:33d2                       ; 1000:33be ; -> 1000:33d2
    JG 0x1000:33cb                       ; 1000:33c0 ; -> 1000:33cb
    INC word ptr [0x892]                 ; 1000:33c2
    CALL 0x1000:2bf7                     ; 1000:33c6 ; -> 1000:2bf7
    JMP 0x1000:33d2                      ; 1000:33c9 ; -> 1000:33d2
    DEC word ptr [0x892]                 ; 1000:33cb
    CALL 0x1000:2c54                     ; 1000:33cf ; -> 1000:2c54
    CMP word ptr [0x894],0x0             ; 1000:33d2
    JG 0x1000:3405                       ; 1000:33d7 ; -> 1000:3405
    MOV AX,[0x6c6]                       ; 1000:33d9
    MOV BX,word ptr [0x6c8]              ; 1000:33dc
    MOV DI,word ptr [0x292]              ; 1000:33e0
    MOV byte ptr [0x8b0],0x2             ; 1000:33e4
    CALL 0x1000:2c9d                     ; 1000:33e9 ; -> 1000:2c9d
    MOV byte ptr [0x8b0],0x0             ; 1000:33ec
    JNC 0x1000:3405                      ; 1000:33f1 ; -> 1000:3405
    MOV word ptr [0x894],0x0             ; 1000:33f3
    AND word ptr [0x6c8],0xfff0          ; 1000:33f9
    ADD word ptr [0x6c8],0xa             ; 1000:33fe
    JMP 0x1000:3453                      ; 1000:3403 ; -> 1000:3453
    CMP word ptr [0x9620],0x0            ; 1000:3405
    JLE 0x1000:3453                      ; 1000:340a ; -> 1000:3453
    MOV DI,word ptr [0x294]              ; 1000:340c
    CMP byte ptr CS:[0x25b],0x0          ; 1000:3410 ; -> 1000:025b
    JZ 0x1000:341c                       ; 1000:3416 ; -> 1000:341c
    MOV DI,word ptr [0x292]              ; 1000:3418
    MOV AX,[0x6c6]                       ; 1000:341c
    MOV BX,word ptr [0x6c8]              ; 1000:341f
    ADD BX,0x20                          ; 1000:3423
    CALL 0x1000:2c9d                     ; 1000:3426 ; -> 1000:2c9d
    JC 0x1000:3455                       ; 1000:3429 ; -> 1000:3455
    CMP AX,word ptr [0x296]              ; 1000:342b
    JG 0x1000:343a                       ; 1000:342f ; -> 1000:343a
    CMP AX,word ptr [0x294]              ; 1000:3431
    JLE 0x1000:343a                      ; 1000:3435 ; -> 1000:343a
    JMP 0x1000:2dc6                      ; 1000:3437 ; -> 1000:2dc6
    MOV AX,[0x6c8]                       ; 1000:343a
    ADD AX,0x28                          ; 1000:343d
    CMP AX,word ptr [0x29e]              ; 1000:3440
    JL 0x1000:3460                       ; 1000:3444 ; -> 1000:3460
    CMP AX,word ptr [0x29e]              ; 1000:3446
    SUB AX,0x20                          ; 1000:344a
    MOV [0x6c8],AX                       ; 1000:344d
    JMP 0x1000:3455                      ; 1000:3450 ; -> 1000:3455
    JMP 0x1000:3460                      ; 1000:3453 ; -> 1000:3460
    AND word ptr [0x6c8],0xfff0          ; 1000:3455
    MOV word ptr [0x894],0x0             ; 1000:345a
    MOV byte ptr [0x8ae],0x5             ; 1000:3460
    JMP 0x1000:2b88                      ; 1000:3465 ; -> 1000:2b88

; ------------------------------------------------------------------------------
; FUNCTION: ent_spawn_enemy  [Entities / Spawning]
; ADDRESS:  1000:3468  (Length: 0x41 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Spawns a new monster into an available entity slot from room spawn tables.
; ------------------------------------------------------------------------------
ent_spawn_enemy:
    CMP byte ptr [0x8ad],0x1             ; 1000:3468
    JZ 0x1000:347d                       ; 1000:346d ; -> 1000:347d
    CMP word ptr [0x6c6],0x8             ; 1000:346f
    JL 0x1000:34a8                       ; 1000:3474 ; -> 1000:34a8
    MOV byte ptr [0x8a5],0x2             ; 1000:3476
    JMP 0x1000:348e                      ; 1000:347b ; -> 1000:348e
    MOV AX,[0x29c]                       ; 1000:347d
    SUB AX,0x18                          ; 1000:3480
    CMP word ptr [0x6c6],AX              ; 1000:3483
    JG 0x1000:34a8                       ; 1000:3487 ; -> 1000:34a8
    MOV byte ptr [0x8a5],0x1             ; 1000:3489
    MOV byte ptr [0x8a7],0x1             ; 1000:348e
    MOV byte ptr [0x8a4],0x3             ; 1000:3493
    MOV byte ptr [0x8af],0x1             ; 1000:3498
    MOV byte ptr [0x8ae],0x6             ; 1000:349d
    MOV word ptr [0x892],0x0             ; 1000:34a2
    RET                                  ; 1000:34a8

; ------------------------------------------------------------------------------
; FUNCTION: ent_update_ai  [Entities / AI]
; ADDRESS:  1000:34a9  (Length: 0x133 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Processes AI behavioral patterns: patrollers, jumpers, fliers, projectiles, and boss attacks.
; ------------------------------------------------------------------------------
ent_update_ai:
    DEC byte ptr [0x8a4]                 ; 1000:34a9
    JNZ 0x1000:34bc                      ; 1000:34ad ; -> 1000:34bc
    MOV byte ptr [0x8a7],0x0             ; 1000:34af
    MOV byte ptr [0x8ae],0x0             ; 1000:34b4
    JMP 0x1000:358e                      ; 1000:34b9 ; -> 1000:358e
    CMP byte ptr [0x8a4],0x1             ; 1000:34bc
    JZ 0x1000:34c6                       ; 1000:34c1 ; -> 1000:34c6
    JMP 0x1000:3589                      ; 1000:34c3 ; -> 1000:3589
    MOV DI,word ptr [0x6fe]              ; 1000:34c6
    OR DI,DI                             ; 1000:34ca
    JNZ 0x1000:34d1                      ; 1000:34cc ; -> 1000:34d1
    JMP 0x1000:3589                      ; 1000:34ce ; -> 1000:3589
    MOV AX,[0x6c6]                       ; 1000:34d1
    SUB AX,0x10                          ; 1000:34d4
    CMP byte ptr [0x8a5],0x1             ; 1000:34d7
    JNZ 0x1000:34e1                      ; 1000:34dc ; -> 1000:34e1
    ADD AX,0x20                          ; 1000:34de
    MOV BX,word ptr [0x6c8]              ; 1000:34e1
    ADD BX,0x10                          ; 1000:34e5
    MOV SI,0x700                         ; 1000:34e8
    TEST word ptr [SI + 0x4],0x8000      ; 1000:34eb
    JNZ 0x1000:34fd                      ; 1000:34f0 ; -> 1000:34fd
    CMP AX,word ptr [SI]                 ; 1000:34f2
    JNZ 0x1000:34fd                      ; 1000:34f4 ; -> 1000:34fd
    CMP BX,word ptr [SI + 0x2]           ; 1000:34f6
    JNZ 0x1000:34fd                      ; 1000:34f9 ; -> 1000:34fd
    JMP 0x1000:3506                      ; 1000:34fb ; -> 1000:3506
    ADD SI,0x8                           ; 1000:34fd
    DEC DI                               ; 1000:3500
    JNZ 0x1000:34eb                      ; 1000:3501 ; -> 1000:34eb
    JMP 0x1000:3589                      ; 1000:3503 ; -> 1000:3589
    MOV BX,word ptr [SI + 0x6]           ; 1000:3506
    CMP BX,0xafee                        ; 1000:3509
    JNZ 0x1000:3519                      ; 1000:350d ; -> 1000:3519
    CMP byte ptr [0x220],0x5             ; 1000:350f
    JL 0x1000:3519                       ; 1000:3514 ; -> 1000:3519
    JMP 0x1000:3591                      ; 1000:3516 ; -> 1000:3591
    MOV DI,0x946                         ; 1000:3519
    MOV CX,word ptr [0x944]              ; 1000:351c
    CMP BX,word ptr [DI]                 ; 1000:3520
    JZ 0x1000:352c                       ; 1000:3522 ; -> 1000:352c
    ADD DI,0x6                           ; 1000:3524
    LOOP 0x1000:3520                     ; 1000:3527 ; -> 1000:3520
    JMP 0x1000:3589                      ; 1000:3529 ; -> 1000:3589
    MOV BX,word ptr [DI + 0x2]           ; 1000:352c
    MOV word ptr [SI + 0x6],BX           ; 1000:352f
    MOV AX,word ptr [SI + 0x4]           ; 1000:3532
    AND AX,0x7fff                        ; 1000:3535
    MOV SI,0xaf0                         ; 1000:3538
    MOV CX,AX                            ; 1000:353b
    SHL AX,0x1                           ; 1000:353d
    ADD AX,CX                            ; 1000:353f
    SHL AX,0x1                           ; 1000:3541
    SHL AX,0x1                           ; 1000:3543
    ADD SI,AX                            ; 1000:3545
    MOV AX,word ptr [DI + 0x4]           ; 1000:3547
    MOV word ptr CS:[SI + 0x8],BX        ; 1000:354a
    MOV word ptr CS:[SI + 0xa],AX        ; 1000:354e
    CMP byte ptr [0x8b1],0x6             ; 1000:3552
    JZ 0x1000:357f                       ; 1000:3557 ; -> 1000:357f
    MOV DI,0x8b2                         ; 1000:3559
    CMP byte ptr [DI],0x0                ; 1000:355c
    JZ 0x1000:3566                       ; 1000:355f ; -> 1000:3566
    ADD DI,0x9                           ; 1000:3561
    JMP 0x1000:355c                      ; 1000:3564 ; -> 1000:355c
    INC byte ptr [0x8b1]                 ; 1000:3566
    MOV byte ptr [DI],0x2                ; 1000:356a
    MOV byte ptr [DI + 0x4],0x0          ; 1000:356d
    MOV AX,word ptr CS:[SI + 0x4]        ; 1000:3571
    MOV word ptr [DI + 0x5],AX           ; 1000:3575
    MOV AX,word ptr CS:[SI + 0x6]        ; 1000:3578
    MOV word ptr [DI + 0x7],AX           ; 1000:357c
    MOV AX,0x1                           ; 1000:357f
    MOV BX,0x9a25                        ; 1000:3582
    MOV CX,0x2                           ; 1000:3585
    INT3                                 ; 1000:3588
    MOV byte ptr [0x8ae],0x6             ; 1000:3589
    JMP 0x1000:267f                      ; 1000:358e ; -> 1000:267f
    MOV AX,word ptr [SI + 0x4]           ; 1000:3591
    AND AX,0x7fff                        ; 1000:3594
    MOV DI,0xaf0                         ; 1000:3597
    MOV CX,AX                            ; 1000:359a
    SHL AX,0x1                           ; 1000:359c
    ADD AX,CX                            ; 1000:359e
    SHL AX,0x1                           ; 1000:35a0
    SHL AX,0x1                           ; 1000:35a2
    ADD DI,AX                            ; 1000:35a4
    MOV word ptr CS:[DI],0xfffe          ; 1000:35a6
    MOV AX,word ptr [SI]                 ; 1000:35ab
    MOV BX,word ptr [SI + 0x2]           ; 1000:35ad
    MOV word ptr [SI],0xffff             ; 1000:35b0
    SUB BX,0x10                          ; 1000:35b4
    CALL 0x1000:401b                     ; 1000:35b7 ; -> 1000:401b
    MOV byte ptr [0x8a7],0x0             ; 1000:35ba
    MOV byte ptr [0x8ae],0x0             ; 1000:35bf
    MOV byte ptr [0x93b],0x14            ; 1000:35c4
    INC byte ptr [0x226]                 ; 1000:35c9
    CMP byte ptr [0x226],0x6             ; 1000:35cd
    JNZ 0x1000:35db                      ; 1000:35d2 ; -> 1000:35db
    MOV SP,word ptr [0x8e8]              ; 1000:35d4
    JMP 0x1000:409c                      ; 1000:35d8 ; -> 1000:409c
    JMP 0x1000:267f                      ; 1000:35db ; -> 1000:267f

; ------------------------------------------------------------------------------
; FUNCTION: ent_check_player_contact  [Entities / Collision]
; ADDRESS:  1000:35de  (Length: 0x32 bytes)
; CALLERS:  FUN_1000_2bf7, FUN_1000_385f, FUN_1000_3026, FUN_1000_2c54, FUN_1000_305e, FUN_1000_3657
; PURPOSE:  Tests collision between player bounding box and enemy body; deals damage on contact.
; ------------------------------------------------------------------------------
ent_check_player_contact:
    MOV CX,word ptr [0x6f2]              ; 1000:35de
    MOV SI,word ptr [0x6f4]              ; 1000:35e2
    OR CX,CX                             ; 1000:35e6
    JZ 0x1000:360f                       ; 1000:35e8 ; -> 1000:360f
    MOV AX,[0x6c6]                       ; 1000:35ea
    CMP AX,word ptr CS:[SI]              ; 1000:35ed
    JNZ 0x1000:3608                      ; 1000:35f0 ; -> 1000:3608
    MOV AX,[0x6c8]                       ; 1000:35f2
    CMP AX,word ptr CS:[SI + 0x2]        ; 1000:35f5
    JL 0x1000:3608                       ; 1000:35f9 ; -> 1000:3608
    CMP AX,word ptr CS:[SI + 0x4]        ; 1000:35fb
    JG 0x1000:3608                       ; 1000:35ff ; -> 1000:3608
    MOV SP,word ptr [0x8e8]              ; 1000:3601
    JMP 0x1000:2205                      ; 1000:3605 ; -> 1000:2205
    ADD SI,0xc                           ; 1000:3608
    DEC CL                               ; 1000:360b
    JNZ 0x1000:35ea                      ; 1000:360d ; -> 1000:35ea
    RET                                  ; 1000:360f

; ------------------------------------------------------------------------------
; FUNCTION: ent_destroy  [Entities / Lifecycle]
; ADDRESS:  1000:3610  (Length: 0x1a bytes)
; CALLERS:  FUN_1000_6806, FUN_1000_2341
; PURPOSE:  Destroys enemy, spawns explosion effect, plays destruction sound, and adds points to score.
; ------------------------------------------------------------------------------
ent_destroy:
    CMP byte ptr [0x91e],0x0             ; 1000:3610
    JNZ 0x1000:3629                      ; 1000:3615 ; -> 1000:3629
    MOV [0x91f],AX                       ; 1000:3617
    MOV byte ptr [0x91e],0x8             ; 1000:361a
    MOV AX,0x1                           ; 1000:361f
    MOV BX,0x96ae                        ; 1000:3622
    MOV CX,0x5                           ; 1000:3625
    INT3                                 ; 1000:3628
    RET                                  ; 1000:3629

; ------------------------------------------------------------------------------
; FUNCTION: ent_check_tile_collision  [Entities / Collision]
; ADDRESS:  1000:362a  (Length: 0x2d bytes)
; CALLERS:  FUN_1000_2a10
; PURPOSE:  Checks monster collision against solid walls and platforms in the room.
; ------------------------------------------------------------------------------
ent_check_tile_collision:
    MOV AX,[0x6c6]                       ; 1000:362a
    SUB AX,word ptr [0x928]              ; 1000:362d
    JL 0x1000:3655                       ; 1000:3631 ; -> 1000:3655
    CMP AX,0x10                          ; 1000:3633
    JG 0x1000:3655                       ; 1000:3636 ; -> 1000:3655
    MOV AX,[0x92a]                       ; 1000:3638
    SUB AX,word ptr [0x6c8]              ; 1000:363b
    JGE 0x1000:3643                      ; 1000:363f ; -> 1000:3643
    NEG AX                               ; 1000:3641
    CMP AX,0xc                           ; 1000:3643
    JG 0x1000:3655                       ; 1000:3646 ; -> 1000:3655
    MOV byte ptr [0x924],0x2             ; 1000:3648
    MOV word ptr [0x926],0x8             ; 1000:364d
    STC                                  ; 1000:3653
    RET                                  ; 1000:3654
    CLC                                  ; 1000:3655
    RET                                  ; 1000:3656

; ------------------------------------------------------------------------------
; FUNCTION: ent_turn_around  [Entities / AI]
; ADDRESS:  1000:3657  (Length: 0x69 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Reverses monster movement direction upon reaching platform edges or walls.
; ------------------------------------------------------------------------------
ent_turn_around:
    CMP byte ptr [0x896],0x0             ; 1000:3657
    JZ 0x1000:366c                       ; 1000:365c ; -> 1000:366c
    XOR BH,BH                            ; 1000:365e
    MOV BL,byte ptr [0x896]              ; 1000:3660
    MOV byte ptr [0x896],0x0             ; 1000:3664
    CALL 0x1000:3a17                     ; 1000:3669 ; -> 1000:3a17
    CMP byte ptr CS:[0x25c],0x1          ; 1000:366c ; -> 1000:025c
    JNZ 0x1000:368e                      ; 1000:3672 ; -> 1000:368e
    CMP byte ptr [0x898],0x0             ; 1000:3674
    JZ 0x1000:368e                       ; 1000:3679 ; -> 1000:368e
    MOV byte ptr [0x897],0x1             ; 1000:367b
    MOV word ptr [0x892],0x5             ; 1000:3680
    MOV byte ptr [0x924],0x1             ; 1000:3686
    JMP 0x1000:2a10                      ; 1000:368b ; -> 1000:2a10
    MOV AX,[0x6c6]                       ; 1000:368e
    ADD AX,0x10                          ; 1000:3691
    CMP AX,word ptr [0x29c]              ; 1000:3694
    JNZ 0x1000:369d                      ; 1000:3698 ; -> 1000:369d
    CALL 0x1000:35de                     ; 1000:369a ; -> 1000:35de
    MOV AX,[0x6c6]                       ; 1000:369d
    SUB AX,word ptr [0x6c2]              ; 1000:36a0
    CMP AX,0x70                          ; 1000:36a4
    JLE 0x1000:36ba                      ; 1000:36a7 ; -> 1000:36ba
    MOV AX,[0x6c2]                       ; 1000:36a9
    ADD AX,0xd0                          ; 1000:36ac
    CMP AX,word ptr [0x29c]              ; 1000:36af
    JZ 0x1000:36ba                       ; 1000:36b3 ; -> 1000:36ba
    ADD word ptr [0x6c2],0x8             ; 1000:36b5
    CALL 0x1000:2b8e                     ; 1000:36ba ; -> 1000:2b8e
    JMP 0x1000:267f                      ; 1000:36bd ; -> 1000:267f

; ------------------------------------------------------------------------------
; FUNCTION: item_collect_pickup  [Items / Pickups]
; ADDRESS:  1000:36c0  (Length: 0xcd bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Handles item collection: gems, door keys, teleport wand, boots, lantern, potion, and treasures.
; ------------------------------------------------------------------------------
item_collect_pickup:
    MOV AX,[0x926]                       ; 1000:36c0
    ADD AX,word ptr [0x928]              ; 1000:36c3
    MOV [0x928],AX                       ; 1000:36c7
    MOV BX,word ptr [0x92a]              ; 1000:36ca
    PUSH AX                              ; 1000:36ce
    PUSH BX                              ; 1000:36cf
    ADD AX,0x10                          ; 1000:36d0
    ADD BX,0x10                          ; 1000:36d3
    CALL 0x1000:1cfe                     ; 1000:36d6 ; -> 1000:1cfe
    CMP AX,0x1b                          ; 1000:36d9
    JNZ 0x1000:36e3                      ; 1000:36dc ; -> 1000:36e3
    POP BX                               ; 1000:36de
    POP AX                               ; 1000:36df
    JMP 0x1000:371d                      ; 1000:36e0 ; -> 1000:371d
    POP BX                               ; 1000:36e3
    POP AX                               ; 1000:36e4
    CMP byte ptr [0x925],0x2             ; 1000:36e5
    JZ 0x1000:36ef                       ; 1000:36ea ; -> 1000:36ef
    ADD AX,0x10                          ; 1000:36ec
    ADD BX,0x20                          ; 1000:36ef
    CALL 0x1000:1cfe                     ; 1000:36f2 ; -> 1000:1cfe
    CMP AX,0x1a                          ; 1000:36f5
    JZ 0x1000:3716                       ; 1000:36f8 ; -> 1000:3716
    CMP AX,0x1b                          ; 1000:36fa
    JZ 0x1000:371d                       ; 1000:36fd ; -> 1000:371d
    CMP AX,0x19                          ; 1000:36ff
    JG 0x1000:3724                       ; 1000:3702 ; -> 1000:3724
    INC byte ptr [0x925]                 ; 1000:3704
    CMP byte ptr [0x925],0x4             ; 1000:3708
    JZ 0x1000:3724                       ; 1000:370d ; -> 1000:3724
    MOV byte ptr [0x925],0x2             ; 1000:370f
    JMP 0x1000:3731                      ; 1000:3714 ; -> 1000:3731
    MOV byte ptr [0x925],0x1             ; 1000:3716
    JMP 0x1000:3731                      ; 1000:371b ; -> 1000:3731
    MOV byte ptr [0x925],0x3             ; 1000:371d
    JMP 0x1000:3731                      ; 1000:3722 ; -> 1000:3731
    CMP byte ptr [0x925],0x1             ; 1000:3724
    JLE 0x1000:372f                      ; 1000:3729 ; -> 1000:372f
    DEC byte ptr [0x925]                 ; 1000:372b
    JMP 0x1000:3731                      ; 1000:372f ; -> 1000:3731
    MOV AX,[0x926]                       ; 1000:3731
    CMP byte ptr [0x925],0x1             ; 1000:3734
    JNZ 0x1000:373f                      ; 1000:3739 ; -> 1000:373f
    ADD word ptr [0x92a],AX              ; 1000:373b
    CMP byte ptr [0x925],0x3             ; 1000:373f
    JNZ 0x1000:374a                      ; 1000:3744 ; -> 1000:374a
    SUB word ptr [0x92a],AX              ; 1000:3746
    CMP byte ptr [0x924],0x2             ; 1000:374a
    JL 0x1000:378d                       ; 1000:374f ; -> 1000:378d
    MOV byte ptr [0x8ad],0x1             ; 1000:3751
    MOV AX,[0x928]                       ; 1000:3756
    ADD AX,0x8                           ; 1000:3759
    MOV [0x6c6],AX                       ; 1000:375c
    MOV BX,word ptr [0x92a]              ; 1000:375f
    MOV word ptr [0x6c8],BX              ; 1000:3763
    ADD AX,0x8                           ; 1000:3767
    CALL 0x1000:1cfe                     ; 1000:376a ; -> 1000:1cfe
    CMP AX,word ptr [0x292]              ; 1000:376d
    JG 0x1000:378d                       ; 1000:3771 ; -> 1000:378d
    MOV byte ptr [0x924],0x1             ; 1000:3773
    MOV byte ptr [0x897],0x1             ; 1000:3778
    MOV byte ptr [0x898],0x0             ; 1000:377d
    MOV word ptr [0x892],0xfffb          ; 1000:3782
    MOV byte ptr [0x8ae],0x2             ; 1000:3788
    RET                                  ; 1000:378d

; ------------------------------------------------------------------------------
; FUNCTION: hud_render_all  [HUD / Display]
; ADDRESS:  1000:378e  (Length: 0xd1 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Draws complete on-screen HUD: score digits, shield bar, extra lives, active weapon, keys, and inventory items.
; ------------------------------------------------------------------------------
hud_render_all:
    CMP byte ptr [0x925],0x2             ; 1000:378e
    JZ 0x1000:37b8                       ; 1000:3793 ; -> 1000:37b8
    MOV SI,word ptr CS:[0x8db8]          ; 1000:3795 ; -> 1000:8db8
    CMP byte ptr [0x925],0x3             ; 1000:379a
    JNZ 0x1000:37a6                      ; 1000:379f ; -> 1000:37a6
    MOV SI,word ptr CS:[0x8dbc]          ; 1000:37a1 ; -> 1000:8dbc
    MOV AX,[0x928]                       ; 1000:37a6
    MOV BX,word ptr [0x92a]              ; 1000:37a9
    ADD BX,0x6                           ; 1000:37ad
    CALL 0x1000:7dbb                     ; 1000:37b0 ; -> 1000:7dbb
    OR AX,AX                             ; 1000:37b3
    JZ 0x1000:37cc                       ; 1000:37b5 ; -> 1000:37cc
    RET                                  ; 1000:37b7
    MOV SI,word ptr CS:[0x8dba]          ; 1000:37b8 ; -> 1000:8dba
    MOV AX,[0x928]                       ; 1000:37bd
    MOV BX,word ptr [0x92a]              ; 1000:37c0
    CALL 0x1000:7dbb                     ; 1000:37c4 ; -> 1000:7dbb
    OR AX,AX                             ; 1000:37c7
    JZ 0x1000:37cc                       ; 1000:37c9 ; -> 1000:37cc
    RET                                  ; 1000:37cb
    MOV AX,[0x928]                       ; 1000:37cc
    CMP AX,word ptr [0x6c6]              ; 1000:37cf
    JLE 0x1000:3810                      ; 1000:37d3 ; -> 1000:3810
    CMP word ptr [0x6fc],0x1             ; 1000:37d5
    JNZ 0x1000:37f5                      ; 1000:37da ; -> 1000:37f5
    MOV word ptr [0x928],0x0             ; 1000:37dc
    MOV word ptr [0x92a],0x20            ; 1000:37e2
    MOV word ptr [0x926],0x8             ; 1000:37e8
    MOV byte ptr [0x925],0x2             ; 1000:37ee
    JMP 0x1000:3811                      ; 1000:37f3 ; -> 1000:3811
    CMP word ptr [0x6fc],0x0             ; 1000:37f5
    JNZ 0x1000:3810                      ; 1000:37fa ; -> 1000:3810
    MOV word ptr [0x928],0xb0            ; 1000:37fc
    MOV word ptr [0x92a],0xe0            ; 1000:3802
    MOV word ptr [0x926],0x0             ; 1000:3808
    JMP 0x1000:3811                      ; 1000:380e ; -> 1000:3811
    RET                                  ; 1000:3810
    MOV AX,[0x6c6]                       ; 1000:3811
    CMP AX,0xac0                         ; 1000:3814
    JGE 0x1000:3810                      ; 1000:3817 ; -> 1000:3810
    SUB AX,0x140                         ; 1000:3819
    CMP word ptr [0x928],AX              ; 1000:381c
    JGE 0x1000:3810                      ; 1000:3820 ; -> 1000:3810
    MOV word ptr [0x926],0x8             ; 1000:3822
    PUSH AX                              ; 1000:3828
    ADD word ptr [0x928],0x10            ; 1000:3829
    SUB word ptr [0x92a],0x10            ; 1000:382e
    MOV AX,[0x928]                       ; 1000:3833
    ADD AX,0x10                          ; 1000:3836
    MOV BX,word ptr [0x92a]              ; 1000:3839
    ADD BX,0x20                          ; 1000:383d
    CALL 0x1000:1cfe                     ; 1000:3840 ; -> 1000:1cfe
    CMP AX,0x1b                          ; 1000:3843
    JLE 0x1000:385c                      ; 1000:3846 ; -> 1000:385c
    ADD word ptr [0x92a],0x10            ; 1000:3848
    MOV AX,[0x92a]                       ; 1000:384d
    CMP AX,word ptr [0x29e]              ; 1000:3850
    JL 0x1000:3833                       ; 1000:3854 ; -> 1000:3833
    MOV word ptr [0x92a],0x0             ; 1000:3856
    POP AX                               ; 1000:385c
    JMP 0x1000:381c                      ; 1000:385d ; -> 1000:381c

; ------------------------------------------------------------------------------
; FUNCTION: hud_update_score  [HUD / Score]
; ADDRESS:  1000:385f  (Length: 0x14a bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Adds points to total score, handles high score check, and triggers score display redraw.
; ------------------------------------------------------------------------------
hud_update_score:
    MOV word ptr [0x92e],0x0             ; 1000:385f
    MOV byte ptr [0x8a6],0x0             ; 1000:3865
    MOV byte ptr [0x92c],0x2             ; 1000:386a
    CMP byte ptr [0x896],0x0             ; 1000:386f
    JZ 0x1000:3884                       ; 1000:3874 ; -> 1000:3884
    XOR BH,BH                            ; 1000:3876
    MOV BL,byte ptr [0x896]              ; 1000:3878
    MOV byte ptr [0x896],0x0             ; 1000:387c
    CALL 0x1000:3a17                     ; 1000:3881 ; -> 1000:3a17
    ADD word ptr [0x92e],0x2             ; 1000:3884
    CMP word ptr [0x92e],0xc             ; 1000:3889
    JLE 0x1000:3896                      ; 1000:388e ; -> 1000:3896
    MOV word ptr [0x92e],0xc             ; 1000:3890
    MOV AX,[0x930]                       ; 1000:3896
    ADD AX,0x8                           ; 1000:3899
    MOV [0x930],AX                       ; 1000:389c
    MOV BX,word ptr [0x932]              ; 1000:389f
    ADD BX,word ptr [0x92e]              ; 1000:38a3
    PUSH BX                              ; 1000:38a7
    ADD AX,0x10                          ; 1000:38a8
    ADD BX,0x20                          ; 1000:38ab
    CALL 0x1000:1cfe                     ; 1000:38ae ; -> 1000:1cfe
    SUB BX,word ptr [0x298]              ; 1000:38b1
    MOV CL,byte ptr [BX]                 ; 1000:38b5
    XOR CH,CH                            ; 1000:38b7
    SUB BX,word ptr [0x298]              ; 1000:38b9
    MOV DL,byte ptr [BX]                 ; 1000:38bd
    XOR DH,DH                            ; 1000:38bf
    POP BX                               ; 1000:38c1
    CMP AL,0x16                          ; 1000:38c2
    JNZ 0x1000:38ce                      ; 1000:38c4 ; -> 1000:38ce
    ADD BX,0x10                          ; 1000:38c6
    AND BX,0xfff0                        ; 1000:38c9
    JMP 0x1000:38ec                      ; 1000:38cc ; -> 1000:38ec
    CMP CL,0x20                          ; 1000:38ce
    JZ 0x1000:3915                       ; 1000:38d1 ; -> 1000:3915
    CMP AX,word ptr [0x292]              ; 1000:38d3
    JG 0x1000:3925                       ; 1000:38d7 ; -> 1000:3925
    AND BX,0xfff0                        ; 1000:38d9
    MOV word ptr [0x92e],0x1             ; 1000:38dc
    CMP CL,0x1f                          ; 1000:38e2
    JZ 0x1000:3925                       ; 1000:38e5 ; -> 1000:3925
    CMP CL,0x16                          ; 1000:38e7
    JNZ 0x1000:3900                      ; 1000:38ea ; -> 1000:3900
    PUSH BX                              ; 1000:38ec
    MOV byte ptr [0x8ec],0xa             ; 1000:38ed
    MOV AX,0x1                           ; 1000:38f2
    MOV BX,0x9a4d                        ; 1000:38f5
    MOV CX,0x9                           ; 1000:38f8
    INT3                                 ; 1000:38fb
    MOV word ptr [0x92e],0x0             ; 1000:3900
    CMP byte ptr CS:[0x25c],0x1          ; 1000:3906 ; -> 1000:025c
    JNZ 0x1000:3925                      ; 1000:390c ; -> 1000:3925
    CMP byte ptr [0x898],0x0             ; 1000:390e
    JZ 0x1000:3925                       ; 1000:3913 ; -> 1000:3925
    AND BX,0xfff0                        ; 1000:3915
    CMP word ptr [0x92e],0x0             ; 1000:3918
    JL 0x1000:3925                       ; 1000:391d ; -> 1000:3925
    MOV word ptr [0x92e],0xfff0          ; 1000:391f
    CMP word ptr [0x92e],0x0             ; 1000:3925
    JGE 0x1000:393c                      ; 1000:392a ; -> 1000:393c
    CMP CX,word ptr [0x292]              ; 1000:392c
    JG 0x1000:393c                       ; 1000:3930 ; -> 1000:393c
    MOV BX,word ptr [0x932]              ; 1000:3932
    MOV word ptr [0x92e],0x0             ; 1000:3936
    MOV word ptr [0x932],BX              ; 1000:393c
    MOV AX,[0x92e]                       ; 1000:3940
    MOV [0x92d],AL                       ; 1000:3943
    MOV AL,[0x899]                       ; 1000:3946
    MOV [0x898],AL                       ; 1000:3949
    CMP byte ptr CS:[0x25c],0x1          ; 1000:394c ; -> 1000:025c
    JNZ 0x1000:3959                      ; 1000:3952 ; -> 1000:3959
    MOV byte ptr [0x898],0x0             ; 1000:3954
    MOV AX,[0x930]                       ; 1000:3959
    ADD AX,0x8                           ; 1000:395c
    MOV [0x6c6],AX                       ; 1000:395f
    MOV BX,word ptr [0x932]              ; 1000:3962
    MOV word ptr [0x6c8],BX              ; 1000:3966
    CMP byte ptr [0x8ec],0xa             ; 1000:396a
    JNZ 0x1000:397b                      ; 1000:396f ; -> 1000:397b
    ADD word ptr [0x6c8],0x10            ; 1000:3971
    SUB word ptr [0x6c6],0x8             ; 1000:3976
    MOV AX,[0x6c6]                       ; 1000:397b
    ADD AX,0x10                          ; 1000:397e
    CMP AX,word ptr [0x29c]              ; 1000:3981
    JNZ 0x1000:398a                      ; 1000:3985 ; -> 1000:398a
    CALL 0x1000:35de                     ; 1000:3987 ; -> 1000:35de
    MOV AX,[0x6c6]                       ; 1000:398a
    SUB AX,word ptr [0x6c2]              ; 1000:398d
    CMP AX,0x70                          ; 1000:3991
    JLE 0x1000:39a7                      ; 1000:3994 ; -> 1000:39a7
    MOV AX,[0x6c2]                       ; 1000:3996
    ADD AX,0xd0                          ; 1000:3999
    CMP AX,word ptr [0x29c]              ; 1000:399c
    JZ 0x1000:39a7                       ; 1000:39a0 ; -> 1000:39a7
    ADD word ptr [0x6c2],0x8             ; 1000:39a2
    CALL 0x1000:2b8e                     ; 1000:39a7 ; -> 1000:2b8e
    JMP 0x1000:267f                      ; 1000:39aa ; -> 1000:267f

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_lives  [HUD / Display]
; ADDRESS:  1000:39ad  (Length: 0xb bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Renders Comic head icons representing remaining lives.
; ------------------------------------------------------------------------------
hud_draw_lives:
    MOV AX,0x1                           ; 1000:39ad
    LEA BX,[0xaac0]                      ; 1000:39b0
    MOV CX,0x4                           ; 1000:39b4
    INT3                                 ; 1000:39b7

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_item_icon  [HUD / Display]
; ADDRESS:  1000:3a17  (Length: 0x48 bytes)
; CALLERS:  FUN_1000_227b, FUN_1000_385f, FUN_1000_3657, FUN_1000_327d
; PURPOSE:  Draws specific item graphic icon into inventory box.
; ------------------------------------------------------------------------------
hud_draw_item_icon:
    LEA AX,[0x9710]                      ; 1000:3a17
    SHL BX,0x1                           ; 1000:3a1b
    ADD BX,AX                            ; 1000:3a1d
    MOV BX,word ptr [BX]                 ; 1000:3a1f
    MOV AX,0x100                         ; 1000:3a21
    MOV DS,word ptr CS:[0x5e]            ; 1000:3a24 ; -> 1000:005e
    LEA SI,[0x98c0]                      ; 1000:3a29
    MOV SI,word ptr [SI + 0x2]           ; 1000:3a2d ; -> 260a:98c2
    CALL 0x1000:790c                     ; 1000:3a30 ; -> 1000:790c
    MOV DS,word ptr CS:[0x5c]            ; 1000:3a33 ; -> 1000:005c
    XOR BH,BH                            ; 1000:3a38
    MOV BL,byte ptr [0x896]              ; 1000:3a3a ; -> 34fe:0896
    LEA AX,[0x9710]                      ; 1000:3a3e
    SHL BX,0x1                           ; 1000:3a42
    ADD BX,AX                            ; 1000:3a44
    MOV BX,word ptr [BX]                 ; 1000:3a46
    MOV AX,0x100                         ; 1000:3a48
    MOV DS,word ptr CS:[0x5e]            ; 1000:3a4b ; -> 1000:005e
    LEA SI,[0x98c0]                      ; 1000:3a50
    MOV SI,word ptr [SI]                 ; 1000:3a54 ; -> 260a:98c0
    CALL 0x1000:790c                     ; 1000:3a56 ; -> 1000:790c
    MOV DS,word ptr CS:[0x5c]            ; 1000:3a59 ; -> 1000:005c
    RET                                  ; 1000:3a5e

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_shield_bar  [HUD / Display]
; ADDRESS:  1000:3a5f  (Length: 0x33 bytes)
; CALLERS:  FUN_1000_6da4, FUN_1000_48aa, FUN_1000_227b
; PURPOSE:  Draws health/shield energy meter using color-coded bar segments.
; ------------------------------------------------------------------------------
hud_draw_shield_bar:
    MOV DS,word ptr CS:[0x5e]            ; 1000:3a5f ; -> 1000:005e
    PUSH AX                              ; 1000:3a64
    PUSH BX                              ; 1000:3a65
    PUSH CX                              ; 1000:3a66
    MOV SI,0x98c4                        ; 1000:3a67
    MOV CL,CH                            ; 1000:3a6a
    XOR CH,CH                            ; 1000:3a6c
    SHL CX,0x1                           ; 1000:3a6e
    ADD SI,CX                            ; 1000:3a70
    MOV SI,word ptr [SI]                 ; 1000:3a72
    CALL 0x1000:790c                     ; 1000:3a74 ; -> 1000:790c
    POP CX                               ; 1000:3a77
    POP BX                               ; 1000:3a78
    POP AX                               ; 1000:3a79
    ADD AX,0x8                           ; 1000:3a7a
    LEA SI,[0x98c4]                      ; 1000:3a7d
    XOR CH,CH                            ; 1000:3a81
    SHL CX,0x1                           ; 1000:3a83
    ADD SI,CX                            ; 1000:3a85
    MOV SI,word ptr [SI]                 ; 1000:3a87
    CALL 0x1000:790c                     ; 1000:3a89 ; -> 1000:790c
    MOV DS,word ptr CS:[0x5c]            ; 1000:3a8c ; -> 1000:005c
    RET                                  ; 1000:3a91

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_keys  [HUD / Display]
; ADDRESS:  1000:3a92  (Length: 0xd bytes)
; CALLERS:  FUN_1000_227b
; PURPOSE:  Draws collected key icons in HUD inventory bar.
; ------------------------------------------------------------------------------
hud_draw_keys:
    MOV byte ptr [0x8eb],0x0             ; 1000:3a92
    MOV AX,0x100                         ; 1000:3a97
    MOV BX,0x43                          ; 1000:3a9a
    JMP 0x1000:3aab                      ; 1000:3a9d ; -> 1000:3aab

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_digits  [HUD / Display]
; ADDRESS:  1000:3aa0  (Length: 0x2e bytes)
; CALLERS:  FUN_1000_227b
; PURPOSE:  Renders decimal digit string on screen using custom EGA numeric font.
; ------------------------------------------------------------------------------
hud_draw_digits:
    MOV byte ptr [0x8ea],0x0             ; 1000:3aa0
    MOV AX,0x100                         ; 1000:3aa5
    MOV BX,0x2a                          ; 1000:3aa8
    MOV CX,0x6                           ; 1000:3aab
    MOV DS,word ptr CS:[0x5e]            ; 1000:3aae ; -> 1000:005e
    MOV SI,0x98d8                        ; 1000:3ab3
    MOV SI,word ptr [SI]                 ; 1000:3ab6 ; -> 260a:98d8
    PUSH AX                              ; 1000:3ab8
    PUSH BX                              ; 1000:3ab9
    PUSH CX                              ; 1000:3aba
    PUSH SI                              ; 1000:3abb
    CALL 0x1000:790c                     ; 1000:3abc ; -> 1000:790c
    POP SI                               ; 1000:3abf
    POP CX                               ; 1000:3ac0
    POP BX                               ; 1000:3ac1
    POP AX                               ; 1000:3ac2
    ADD AX,0x8                           ; 1000:3ac3
    LOOP 0x1000:3ab8                     ; 1000:3ac6 ; -> 1000:3ab8
    MOV DS,word ptr CS:[0x5c]            ; 1000:3ac8 ; -> 1000:005c
    RET                                  ; 1000:3acd

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_number_word  [HUD / Display]
; ADDRESS:  1000:3ace  (Length: 0x44 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Converts 16-bit word value into decimal representation and draws it.
; ------------------------------------------------------------------------------
hud_draw_number_word:
    MOV AL,[0x8ea]                       ; 1000:3ace
    XOR AH,AH                            ; 1000:3ad1
    CMP AX,0x17                          ; 1000:3ad3
    JZ 0x1000:3b11                       ; 1000:3ad6 ; -> 1000:3b11
    INC byte ptr [0x8ea]                 ; 1000:3ad8
    MOV BX,AX                            ; 1000:3adc
    DEC BX                               ; 1000:3ade
    SHR BX,0x1                           ; 1000:3adf
    SHR AX,0x1                           ; 1000:3ae1
    CMP AX,BX                            ; 1000:3ae3
    JZ 0x1000:3b11                       ; 1000:3ae5 ; -> 1000:3b11
    MOV DX,AX                            ; 1000:3ae7
    SHR AX,0x1                           ; 1000:3ae9
    AND DX,0x1                           ; 1000:3aeb
    SHL AX,0x1                           ; 1000:3aee
    SHL AX,0x1                           ; 1000:3af0
    SHL AX,0x1                           ; 1000:3af2
    ADD AX,0x100                         ; 1000:3af4
    MOV BX,0x2a                          ; 1000:3af7
    INC DX                               ; 1000:3afa
    SHL DX,0x1                           ; 1000:3afb
    MOV DS,word ptr CS:[0x5e]            ; 1000:3afd ; -> 1000:005e
    MOV SI,0x98d8                        ; 1000:3b02
    ADD SI,DX                            ; 1000:3b05
    MOV SI,word ptr [SI]                 ; 1000:3b07
    CALL 0x1000:790c                     ; 1000:3b09 ; -> 1000:790c
    MOV DS,word ptr CS:[0x5c]            ; 1000:3b0c ; -> 1000:005c
    RET                                  ; 1000:3b11

; ------------------------------------------------------------------------------
; FUNCTION: hud_draw_char  [HUD / Display]
; ADDRESS:  1000:3b12  (Length: 0x3f bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Draws single 8x8 font glyph to HUD.
; ------------------------------------------------------------------------------
hud_draw_char:
    MOV AL,[0x8ea]                       ; 1000:3b12
    OR AL,AL                             ; 1000:3b15
    JZ 0x1000:3b50                       ; 1000:3b17 ; -> 1000:3b50
    DEC byte ptr [0x8ea]                 ; 1000:3b19
    XOR AH,AH                            ; 1000:3b1d
    DEC AX                               ; 1000:3b1f
    TEST AX,0x1                          ; 1000:3b20
    JNZ 0x1000:3b50                      ; 1000:3b23 ; -> 1000:3b50
    SHR AX,0x1                           ; 1000:3b25
    MOV DX,AX                            ; 1000:3b27
    SHR AX,0x1                           ; 1000:3b29
    AND DX,0x1                           ; 1000:3b2b
    SHL AX,0x1                           ; 1000:3b2e
    SHL AX,0x1                           ; 1000:3b30
    SHL AX,0x1                           ; 1000:3b32
    ADD AX,0x100                         ; 1000:3b34
    MOV BX,0x2a                          ; 1000:3b37
    SHL DX,0x1                           ; 1000:3b3a
    MOV DS,word ptr CS:[0x5e]            ; 1000:3b3c ; -> 1000:005e
    MOV SI,0x98d8                        ; 1000:3b41
    ADD SI,DX                            ; 1000:3b44
    MOV SI,word ptr [SI]                 ; 1000:3b46
    CALL 0x1000:790c                     ; 1000:3b48 ; -> 1000:790c
    MOV DS,word ptr CS:[0x5c]            ; 1000:3b4b ; -> 1000:005c
    RET                                  ; 1000:3b50

; ------------------------------------------------------------------------------
; FUNCTION: hud_flash_item  [HUD / Animation]
; ADDRESS:  1000:3b51  (Length: 0x3b bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Animates flashing highlight around newly acquired inventory item.
; ------------------------------------------------------------------------------
hud_flash_item:
    CMP byte ptr [0x8eb],0xc             ; 1000:3b51
    JZ 0x1000:3b8b                       ; 1000:3b56 ; -> 1000:3b8b
    MOV AL,[0x8eb]                       ; 1000:3b58
    INC byte ptr [0x8eb]                 ; 1000:3b5b
    XOR AH,AH                            ; 1000:3b5f
    MOV DX,AX                            ; 1000:3b61
    SHR AX,0x1                           ; 1000:3b63
    AND DX,0x1                           ; 1000:3b65
    SHL AX,0x1                           ; 1000:3b68
    SHL AX,0x1                           ; 1000:3b6a
    SHL AX,0x1                           ; 1000:3b6c
    ADD AX,0x100                         ; 1000:3b6e
    MOV BX,0x43                          ; 1000:3b71
    INC DX                               ; 1000:3b74
    SHL DX,0x1                           ; 1000:3b75
    MOV DS,word ptr CS:[0x5e]            ; 1000:3b77 ; -> 1000:005e
    MOV SI,0x98d8                        ; 1000:3b7c
    ADD SI,DX                            ; 1000:3b7f
    MOV SI,word ptr [SI]                 ; 1000:3b81
    CALL 0x1000:790c                     ; 1000:3b83 ; -> 1000:790c
    MOV DS,word ptr CS:[0x5c]            ; 1000:3b86 ; -> 1000:005c
    RET                                  ; 1000:3b8b

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_pickup  [Audio / SFX]
; ADDRESS:  1000:3bc6  (Length: 0xb bytes)
; CALLERS:  FUN_1000_20fe
; PURPOSE:  Plays item pickup sound effect via PC speaker.
; ------------------------------------------------------------------------------
snd_play_pickup:
    MOV AX,0x1                           ; 1000:3bc6
    LEA BX,[0x965e]                      ; 1000:3bc9
    MOV CX,0x9                           ; 1000:3bcd
    INT3                                 ; 1000:3bd0

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_blast  [Audio / SFX]
; ADDRESS:  1000:3c13  (Length: 0x5a bytes)
; CALLERS:  FUN_1000_20fe
; PURPOSE:  Plays laser/fireball blasting sound effect.
; ------------------------------------------------------------------------------
snd_play_blast:
    PUSH BP                              ; 1000:3c13
    MOV BP,SP                            ; 1000:3c14
    SUB SP,0xc                           ; 1000:3c16
    PUSH SI                              ; 1000:3c19
    MOV AX,word ptr CS:[SI]              ; 1000:3c1a
    SUB AX,word ptr [0x6c2]              ; 1000:3c1d
    ADD AX,0x8                           ; 1000:3c21
    MOV word ptr [BP + -0x2],AX          ; 1000:3c24 ; -> Stack[-0x4]
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:3c27
    SUB AX,word ptr [0x6c4]              ; 1000:3c2b
    ADD AX,0x8                           ; 1000:3c2f
    MOV word ptr [BP + -0x4],AX          ; 1000:3c32 ; -> Stack[-0x6]
    XOR AH,AH                            ; 1000:3c35
    MOV AL,byte ptr CS:[SI + 0x6]        ; 1000:3c37
    MOV word ptr [BP + -0x6],AX          ; 1000:3c3b ; -> Stack[-0x8]
    MOV AL,byte ptr CS:[SI + 0x7]        ; 1000:3c3e
    MOV word ptr [BP + -0x8],AX          ; 1000:3c42 ; -> Stack[-0xa]
    MOV AL,byte ptr CS:[SI + 0x8]        ; 1000:3c45
    MOV word ptr [BP + -0xa],AX          ; 1000:3c49 ; -> Stack[-0xc]
    MOV AL,byte ptr CS:[SI + 0x9]        ; 1000:3c4c
    MOV word ptr [BP + -0xc],AX          ; 1000:3c50 ; -> Stack[-0xe]
    CALL 0x1000:1d2c                     ; 1000:3c53 ; -> 1000:1d2c
    CALL 0x1000:4b25                     ; 1000:3c56 ; -> 1000:4b25
    CALL 0x1000:3db6                     ; 1000:3c59 ; -> 1000:3db6
    MOV AX,0x1                           ; 1000:3c5c
    CALL 0x1000:027a                     ; 1000:3c5f ; -> 1000:027a
    LEA BX,[0x973c]                      ; 1000:3c62
    MOV AX,0x1                           ; 1000:3c66
    MOV CX,0x9                           ; 1000:3c69
    INT3                                 ; 1000:3c6c

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_hit  [Audio / SFX]
; ADDRESS:  1000:3cb1  (Length: 0x4b bytes)
; CALLERS:  FUN_1000_20fe
; PURPOSE:  Plays enemy hit / player damage sound effect.
; ------------------------------------------------------------------------------
snd_play_hit:
    PUSH BP                              ; 1000:3cb1
    MOV BP,SP                            ; 1000:3cb2
    SUB SP,0xc                           ; 1000:3cb4
    PUSH SI                              ; 1000:3cb7
    MOV AX,word ptr CS:[SI]              ; 1000:3cb8
    SUB AX,word ptr [0x6c2]              ; 1000:3cbb
    ADD AX,0x8                           ; 1000:3cbf
    MOV word ptr [BP + -0x2],AX          ; 1000:3cc2 ; -> Stack[-0x4]
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:3cc5
    SUB AX,word ptr [0x6c4]              ; 1000:3cc9
    ADD AX,0x8                           ; 1000:3ccd
    MOV word ptr [BP + -0x4],AX          ; 1000:3cd0 ; -> Stack[-0x6]
    XOR AH,AH                            ; 1000:3cd3
    MOV AL,byte ptr CS:[SI + 0x6]        ; 1000:3cd5
    MOV word ptr [BP + -0x6],AX          ; 1000:3cd9 ; -> Stack[-0x8]
    MOV AL,byte ptr CS:[SI + 0x7]        ; 1000:3cdc
    MOV word ptr [BP + -0x8],AX          ; 1000:3ce0 ; -> Stack[-0xa]
    MOV AL,byte ptr CS:[SI + 0x8]        ; 1000:3ce3
    MOV word ptr [BP + -0xa],AX          ; 1000:3ce7 ; -> Stack[-0xc]
    MOV AL,byte ptr CS:[SI + 0x9]        ; 1000:3cea
    MOV word ptr [BP + -0xc],AX          ; 1000:3cee ; -> Stack[-0xe]
    LEA BX,[0x973c]                      ; 1000:3cf1
    MOV AX,0x1                           ; 1000:3cf5
    MOV CX,0x9                           ; 1000:3cf8
    INT3                                 ; 1000:3cfb

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_jump  [Audio / SFX]
; ADDRESS:  1000:3db6  (Length: 0xd bytes)
; CALLERS:  FUN_1000_3c13, FUN_1000_3e98, FUN_1000_3e1f, FUN_1000_409c
; PURPOSE:  Plays jump sound effect.
; ------------------------------------------------------------------------------
snd_play_jump:
    CALL 0x1000:437b                     ; 1000:3db6 ; -> 1000:437b
    CALL 0x1000:774e                     ; 1000:3db9 ; -> 1000:774e
    MOV AX,0x1                           ; 1000:3dbc
    CALL 0x1000:027a                     ; 1000:3dbf ; -> 1000:027a
    RET                                  ; 1000:3dc2

; ------------------------------------------------------------------------------
; FUNCTION: spr_render_explosion  [Sprites / Animation]
; ADDRESS:  1000:3dfb  (Length: 0x24 bytes)
; CALLERS:  FUN_1000_3e98, FUN_1000_409c
; PURPOSE:  Animates explosion particle burst when enemies or hazards are destroyed.
; ------------------------------------------------------------------------------
spr_render_explosion:
    MOV SI,0x4882                        ; 1000:3dfb
    CMP byte ptr [0x8ad],0x1             ; 1000:3dfe
    JZ 0x1000:3e08                       ; 1000:3e03 ; -> 1000:3e08
    MOV SI,0x4860                        ; 1000:3e05
    MOV AX,[0x6c6]                       ; 1000:3e08
    MOV BX,word ptr [0x6c8]              ; 1000:3e0b
    MOV DS,word ptr CS:[0x5e]            ; 1000:3e0f ; -> 1000:005e
    MOV SI,word ptr [SI]                 ; 1000:3e14 ; -> 260a:4860
    CALL 0x1000:7dbb                     ; 1000:3e16 ; -> 1000:7dbb
    MOV DS,word ptr CS:[0x5c]            ; 1000:3e19 ; -> 1000:005c
    RET                                  ; 1000:3e1e

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_door_open  [Audio / SFX]
; ADDRESS:  1000:3e1f  (Length: 0x29 bytes)
; CALLERS:  FUN_1000_21b8, FUN_1000_227b
; PURPOSE:  Plays door opening / key unlock sound effect.
; ------------------------------------------------------------------------------
snd_play_door_open:
    PUSH BP                              ; 1000:3e1f
    MOV BP,SP                            ; 1000:3e20
    SUB SP,0x4                           ; 1000:3e22
    MOV byte ptr [0x8ec],0xa             ; 1000:3e25
    MOV word ptr [BP + -0x2],0xa         ; 1000:3e2a ; -> Stack[-0x4]
    CALL 0x1000:1d2c                     ; 1000:3e2f ; -> 1000:1d2c
    CALL 0x1000:4b25                     ; 1000:3e32 ; -> 1000:4b25
    CALL 0x1000:3db6                     ; 1000:3e35 ; -> 1000:3db6
    DEC word ptr [BP + -0x2]             ; 1000:3e38 ; -> Stack[-0x4]
    JNZ 0x1000:3e2f                      ; 1000:3e3b ; -> 1000:3e2f
    MOV AX,0x1                           ; 1000:3e3d
    LEA BX,[0x9764]                      ; 1000:3e40
    MOV CX,0x9                           ; 1000:3e44
    INT3                                 ; 1000:3e47

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_teleport  [Audio / SFX]
; ADDRESS:  1000:3e98  (Length: 0x2c bytes)
; CALLERS:  FUN_1000_2253, FUN_1000_21b8
; PURPOSE:  Plays teleportation portal sound effect.
; ------------------------------------------------------------------------------
snd_play_teleport:
    PUSH BP                              ; 1000:3e98
    MOV BP,SP                            ; 1000:3e99
    SUB SP,0x4                           ; 1000:3e9b
    MOV byte ptr [0x8ec],0xa             ; 1000:3e9e
    MOV word ptr [BP + -0x2],0xa         ; 1000:3ea3 ; -> Stack[-0x4]
    CALL 0x1000:1d2c                     ; 1000:3ea8 ; -> 1000:1d2c
    CALL 0x1000:4b25                     ; 1000:3eab ; -> 1000:4b25
    CALL 0x1000:3dfb                     ; 1000:3eae ; -> 1000:3dfb
    CALL 0x1000:3db6                     ; 1000:3eb1 ; -> 1000:3db6
    DEC word ptr [BP + -0x2]             ; 1000:3eb4 ; -> Stack[-0x4]
    JNZ 0x1000:3ea8                      ; 1000:3eb7 ; -> 1000:3ea8
    MOV AX,0x1                           ; 1000:3eb9
    LEA BX,[0x97cc]                      ; 1000:3ebc
    MOV CX,0x9                           ; 1000:3ec0
    INT3                                 ; 1000:3ec3

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_level_music  [Audio / Music]
; ADDRESS:  1000:3f0a  (Length: 0x2f bytes)
; CALLERS:  FUN_1000_21f2, FUN_1000_20fe
; PURPOSE:  Initializes background theme tune for current game zone.
; ------------------------------------------------------------------------------
snd_play_level_music:
    PUSH BP                              ; 1000:3f0a
    MOV BP,SP                            ; 1000:3f0b
    SUB SP,0x8                           ; 1000:3f0d
    PUSH SI                              ; 1000:3f10
    MOV word ptr [BP + -0x6],AX          ; 1000:3f11 ; -> Stack[-0x8]
    MOV AX,word ptr CS:[SI]              ; 1000:3f14
    SUB AX,word ptr [0x6c2]              ; 1000:3f17
    ADD AX,0x8                           ; 1000:3f1b
    MOV word ptr [BP + -0x2],AX          ; 1000:3f1e ; -> Stack[-0x4]
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:3f21
    SUB AX,word ptr [0x6c4]              ; 1000:3f25
    ADD AX,0x8                           ; 1000:3f29
    MOV word ptr [BP + -0x4],AX          ; 1000:3f2c ; -> Stack[-0x6]
    MOV BX,0x9834                        ; 1000:3f2f
    MOV AX,0x1                           ; 1000:3f32
    MOV CX,0x9                           ; 1000:3f35
    INT3                                 ; 1000:3f38

; ------------------------------------------------------------------------------
; FUNCTION: math_random_byte  [Math / PRNG]
; ADDRESS:  1000:401b  (Length: 0x17 bytes)
; CALLERS:  FUN_1000_34a9
; PURPOSE:  Generates pseudo-random byte using linear congruential algorithm.
; ------------------------------------------------------------------------------
math_random_byte:
    PUSH BP                              ; 1000:401b
    MOV BP,SP                            ; 1000:401c
    SUB SP,0x8                           ; 1000:401e
    PUSH SI                              ; 1000:4021
    MOV word ptr [BP + -0x2],AX          ; 1000:4022 ; -> Stack[-0x4]
    MOV word ptr [BP + -0x4],BX          ; 1000:4025 ; -> Stack[-0x6]
    MOV BX,0x9894                        ; 1000:4028
    MOV AX,0x1                           ; 1000:402b
    MOV CX,0x9                           ; 1000:402e
    INT3                                 ; 1000:4031

; ------------------------------------------------------------------------------
; FUNCTION: math_calc_distance  [Math / Geometry]
; ADDRESS:  1000:4073  (Length: 0x29 bytes)
; CALLERS:  FUN_1000_409c
; PURPOSE:  Calculates Manhattan/Euclidean distance approximation between two entity positions.
; ------------------------------------------------------------------------------
math_calc_distance:
    TEST AX,0x1                          ; 1000:4073
    JNZ 0x1000:4089                      ; 1000:4076 ; -> 1000:4089
    ADD word ptr [0x6c4],0x2             ; 1000:4078
    MOV AX,[0x6c4]                       ; 1000:407d
    ADD AX,0xa0                          ; 1000:4080
    CMP AX,word ptr [0x29e]              ; 1000:4083
    JLE 0x1000:4095                      ; 1000:4087 ; -> 1000:4095
    SUB word ptr [0x6c4],0x2             ; 1000:4089
    JNC 0x1000:4095                      ; 1000:408e ; -> 1000:4095
    ADD word ptr [0x6c4],0x2             ; 1000:4090
    CALL 0x1000:1d2c                     ; 1000:4095 ; -> 1000:1d2c
    CALL 0x1000:4b25                     ; 1000:4098 ; -> 1000:4b25
    RET                                  ; 1000:409b

; ------------------------------------------------------------------------------
; FUNCTION: boss_update_state  [Boss / AI]
; ADDRESS:  1000:409c  (Length: 0x166 bytes)
; CALLERS:  FUN_1000_34a9
; PURPOSE:  Updates boss state machine, attack phases, invulnerability periods, and health.
; ------------------------------------------------------------------------------
boss_update_state:
    PUSH BP                              ; 1000:409c
    MOV BP,SP                            ; 1000:409d
    SUB SP,0xa                           ; 1000:409f
    MOV word ptr [BP + -0x2],0xc0        ; 1000:40a2 ; -> Stack[-0x4]
    MOV word ptr [BP + -0x8],0x0         ; 1000:40a7 ; -> Stack[-0xa]
    MOV word ptr [BP + -0xa],0x0         ; 1000:40ac ; -> Stack[-0xc]
    MOV AX,word ptr [BP + -0x8]          ; 1000:40b1 ; -> Stack[-0xa]
    CALL 0x1000:4073                     ; 1000:40b4 ; -> 1000:4073
    CALL 0x1000:3dfb                     ; 1000:40b7 ; -> 1000:3dfb
    TEST word ptr [BP + -0x8],0x1        ; 1000:40ba ; -> Stack[-0xa]
    JNZ 0x1000:40c5                      ; 1000:40bf ; -> 1000:40c5
    XOR word ptr [BP + -0xa],0x4         ; 1000:40c1 ; -> Stack[-0xc]
    MOV word ptr [BP + -0x4],0x9718      ; 1000:40c5 ; -> Stack[-0x6]
    MOV word ptr [BP + -0x6],0x0         ; 1000:40ca ; -> Stack[-0x8]
    XOR word ptr [BP + -0xa],0x4         ; 1000:40cf ; -> Stack[-0xc]
    MOV SI,word ptr [BP + -0x4]          ; 1000:40d3 ; -> Stack[-0x6]
    MOV AX,[0x6c6]                       ; 1000:40d6 ; -> 34fe:06c6
    ADD AX,word ptr [SI]                 ; 1000:40d9 ; -> 34fe:971c
    SUB AX,word ptr [BP + -0xa]          ; 1000:40db ; -> Stack[-0xc]
    MOV BX,word ptr [0x6c8]              ; 1000:40de ; -> 34fe:06c8
    ADD BX,word ptr [SI + 0x2]           ; 1000:40e2 ; -> 34fe:971e
    ADD BX,word ptr [BP + -0x2]          ; 1000:40e5 ; -> Stack[-0x4]
    MOV SI,word ptr CS:[0x9535]          ; 1000:40e8 ; -> 1000:9535
    MOV DS,word ptr CS:[0x5e]            ; 1000:40ed ; -> 1000:005e
    CALL 0x1000:7dbb                     ; 1000:40f2 ; -> 1000:7dbb
    MOV DS,word ptr CS:[0x5c]            ; 1000:40f5 ; -> 1000:005c
    ADD word ptr [BP + -0x4],0x4         ; 1000:40fa ; -> Stack[-0x6]
    INC word ptr [BP + -0x6]             ; 1000:40fe ; -> Stack[-0x8]
    CMP word ptr [BP + -0x6],0x9         ; 1000:4101 ; -> Stack[-0x8]
    JL 0x1000:40cf                       ; 1000:4105 ; -> 1000:40cf
    MOV word ptr [BP + -0x4],0x9718      ; 1000:4107 ; -> Stack[-0x6]
    MOV word ptr [BP + -0x6],0x0         ; 1000:410c ; -> Stack[-0x8]
    XOR word ptr [BP + -0xa],0x4         ; 1000:4111 ; -> Stack[-0xc]
    MOV SI,word ptr [BP + -0x4]          ; 1000:4115 ; -> Stack[-0x6]
    MOV AX,[0x6c6]                       ; 1000:4118 ; -> 34fe:06c6
    ADD AX,word ptr [SI]                 ; 1000:411b ; -> 34fe:9718
    SUB AX,word ptr [BP + -0xa]          ; 1000:411d ; -> Stack[-0xc]
    MOV BX,word ptr [0x6c8]              ; 1000:4120 ; -> 34fe:06c8
    ADD BX,word ptr [SI + 0x2]           ; 1000:4124 ; -> 34fe:971a
    ADD BX,word ptr [BP + -0x2]          ; 1000:4127 ; -> Stack[-0x4]
    SUB BX,0x60                          ; 1000:412a
    MOV SI,word ptr CS:[0x9535]          ; 1000:412d ; -> 1000:9535
    MOV DS,word ptr CS:[0x5e]            ; 1000:4132 ; -> 1000:005e
    CALL 0x1000:7dbb                     ; 1000:4137 ; -> 1000:7dbb
    MOV DS,word ptr CS:[0x5c]            ; 1000:413a ; -> 1000:005c
    ADD word ptr [BP + -0x4],0x4         ; 1000:413f ; -> Stack[-0x6]
    INC word ptr [BP + -0x6]             ; 1000:4143 ; -> Stack[-0x8]
    CMP word ptr [BP + -0x6],0x9         ; 1000:4146 ; -> Stack[-0x8]
    JL 0x1000:4111                       ; 1000:414a ; -> 1000:4111
    CALL 0x1000:3db6                     ; 1000:414c ; -> 1000:3db6
    SUB word ptr [BP + -0x2],0x8         ; 1000:414f ; -> Stack[-0x4]
    INC word ptr [BP + -0x8]             ; 1000:4153 ; -> Stack[-0xa]
    CMP word ptr [BP + -0x8],0x32        ; 1000:4156 ; -> Stack[-0xa]
    JZ 0x1000:415f                       ; 1000:415a ; -> 1000:415f
    JMP 0x1000:40b1                      ; 1000:415c ; -> 1000:40b1
    MOV word ptr [BP + -0x2],0x50        ; 1000:415f ; -> Stack[-0x4]
    MOV word ptr [BP + -0x8],0x0         ; 1000:4164 ; -> Stack[-0xa]
    MOV AX,word ptr [BP + -0x8]          ; 1000:4169 ; -> Stack[-0xa]
    CALL 0x1000:4073                     ; 1000:416c ; -> 1000:4073
    CALL 0x1000:3dfb                     ; 1000:416f ; -> 1000:3dfb
    MOV AX,[0x6c6]                       ; 1000:4172 ; -> 34fe:06c6
    SUB AX,0x8                           ; 1000:4175
    MOV BX,word ptr [0x6c8]              ; 1000:4178 ; -> 34fe:06c8
    ADD BX,word ptr [BP + -0x2]          ; 1000:417c ; -> Stack[-0x4]
    MOV SI,word ptr CS:[0x9535]          ; 1000:417f ; -> 1000:9535
    MOV DS,word ptr CS:[0x5e]            ; 1000:4184 ; -> 1000:005e
    CALL 0x1000:7dbb                     ; 1000:4189 ; -> 1000:7dbb
    MOV DS,word ptr CS:[0x5c]            ; 1000:418c ; -> 1000:005c
    CALL 0x1000:3db6                     ; 1000:4191 ; -> 1000:3db6
    SUB word ptr [BP + -0x2],0x8         ; 1000:4194 ; -> Stack[-0x4]
    INC word ptr [BP + -0x8]             ; 1000:4198 ; -> Stack[-0xa]
    CMP word ptr [BP + -0x2],-0x8        ; 1000:419b ; -> Stack[-0x4]
    JG 0x1000:4169                       ; 1000:419f ; -> 1000:4169
    MOV AX,word ptr [BP + -0x8]          ; 1000:41a1 ; -> Stack[-0xa]
    CALL 0x1000:4073                     ; 1000:41a4 ; -> 1000:4073
    CALL 0x1000:3dfb                     ; 1000:41a7 ; -> 1000:3dfb
    MOV AX,[0x6c6]                       ; 1000:41aa ; -> 34fe:06c6
    SUB AX,0x8                           ; 1000:41ad
    MOV BX,word ptr [0x6c8]              ; 1000:41b0 ; -> 34fe:06c8
    MOV SI,word ptr CS:[0x9535]          ; 1000:41b4 ; -> 1000:9535
    MOV DS,word ptr CS:[0x5e]            ; 1000:41b9 ; -> 1000:005e
    CALL 0x1000:7dbb                     ; 1000:41be ; -> 1000:7dbb
    MOV DS,word ptr CS:[0x5c]            ; 1000:41c1 ; -> 1000:005c
    CALL 0x1000:3db6                     ; 1000:41c6 ; -> 1000:3db6
    INC word ptr [BP + -0x8]             ; 1000:41c9 ; -> Stack[-0xa]
    SUB word ptr [0x6c8],0x8             ; 1000:41cc ; -> 34fe:06c8
    CMP word ptr [0x6c8],-0x20           ; 1000:41d1 ; -> 34fe:06c8
    JG 0x1000:41a1                       ; 1000:41d6 ; -> 1000:41a1
    MOV byte ptr [0x220],0x6             ; 1000:41d8 ; -> 34fe:0220
    MOV AX,0x0                           ; 1000:41dd
    MOV BX,0x7                           ; 1000:41e0
    MOV word ptr [0x210],0xa0            ; 1000:41e3 ; -> 34fe:0210
    MOV word ptr [0x212],0x0             ; 1000:41e9 ; -> 34fe:0212
    MOV word ptr [0x6fa],0xffff          ; 1000:41ef ; -> 34fe:06fa
    MOV word ptr [0x6fc],BX              ; 1000:41f5 ; -> 34fe:06fc
    MOV BX,0xffff                        ; 1000:41f9
    CALL 0x1000:1b8d                     ; 1000:41fc ; -> 1000:1b8d
    JMP 0x1000:227b                      ; 1000:41ff ; -> 1000:227b

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_window_box  [UI / Windows]
; ADDRESS:  1000:4206  (Length: 0xce bytes)
; CALLERS:  FUN_1000_660b, FUN_1000_2341
; PURPOSE:  Draws bordered UI dialog window with drop shadow and title header.
; ------------------------------------------------------------------------------
ui_draw_window_box:
    PUSH BP                              ; 1000:4206
    MOV BP,SP                            ; 1000:4207
    SUB SP,0x8                           ; 1000:4209
    LODSB SI                             ; 1000:420c
    XOR AH,AH                            ; 1000:420d
    MOV word ptr [BP + -0x2],AX          ; 1000:420f ; -> Stack[-0x4]
    SHL AX,0x1                           ; 1000:4212
    SHL AX,0x1                           ; 1000:4214
    SHL AX,0x1                           ; 1000:4216
    MOV CX,AX                            ; 1000:4218
    LODSB SI                             ; 1000:421a
    XOR AH,AH                            ; 1000:421b
    MOV word ptr [BP + -0x4],AX          ; 1000:421d ; -> Stack[-0x6]
    SHL AX,0x1                           ; 1000:4220
    SHL AX,0x1                           ; 1000:4222
    SHL AX,0x1                           ; 1000:4224
    MOV DX,AX                            ; 1000:4226
    MOV AX,0xd0                          ; 1000:4228
    SUB AX,CX                            ; 1000:422b
    JC 0x1000:4239                       ; 1000:422d ; -> 1000:4239
    SHR AX,0x1                           ; 1000:422f
    AND AX,0xfff8                        ; 1000:4231
    ADD AX,0x8                           ; 1000:4234
    JMP 0x1000:423b                      ; 1000:4237 ; -> 1000:423b
    XOR AX,AX                            ; 1000:4239
    MOV BX,0xa0                          ; 1000:423b
    SUB BX,DX                            ; 1000:423e
    JC 0x1000:424c                       ; 1000:4240 ; -> 1000:424c
    SHR BX,0x1                           ; 1000:4242
    AND BX,0xfff8                        ; 1000:4244
    ADD BX,0x8                           ; 1000:4247
    JMP 0x1000:424e                      ; 1000:424a ; -> 1000:424e
    XOR BX,BX                            ; 1000:424c
    MOV word ptr [BP + -0x6],AX          ; 1000:424e ; -> Stack[-0x8]
    MOV word ptr [BP + -0x8],BX          ; 1000:4251 ; -> Stack[-0xa]
    MOV DI,word ptr CS:[0x773f]          ; 1000:4254 ; -> 1000:773f
    CALL 0x1000:7ac8                     ; 1000:4259 ; -> 1000:7ac8
    MOV AX,0x9                           ; 1000:425c
    CALL 0x1000:4339                     ; 1000:425f ; -> 1000:4339
    SUB word ptr [BP + -0x4],0x2         ; 1000:4262 ; -> Stack[-0x6]
    MOV CX,word ptr [BP + -0x8]          ; 1000:4266 ; -> Stack[-0xa]
    MOV BX,word ptr [BP + -0x6]          ; 1000:4269 ; -> Stack[-0x8]
    MOV AL,0x22                          ; 1000:426c
    CALL 0x1000:42e8                     ; 1000:426e ; -> 1000:42e8
    MOV DX,word ptr [BP + -0x2]          ; 1000:4271 ; -> Stack[-0x4]
    SUB DX,0x2                           ; 1000:4274
    MOV AL,0x23                          ; 1000:4277
    CALL 0x1000:42e8                     ; 1000:4279 ; -> 1000:42e8
    DEC DX                               ; 1000:427c
    JNZ 0x1000:4279                      ; 1000:427d ; -> 1000:4279
    MOV AL,0x24                          ; 1000:427f
    CALL 0x1000:42e8                     ; 1000:4281 ; -> 1000:42e8
    ADD CX,0x8                           ; 1000:4284
    MOV BX,word ptr [BP + -0x6]          ; 1000:4287 ; -> Stack[-0x8]
    MOV AL,0x25                          ; 1000:428a
    CALL 0x1000:42e8                     ; 1000:428c ; -> 1000:42e8
    MOV AX,0x7                           ; 1000:428f
    CALL 0x1000:4339                     ; 1000:4292 ; -> 1000:4339
    MOV DX,word ptr [BP + -0x2]          ; 1000:4295 ; -> Stack[-0x4]
    SUB DX,0x2                           ; 1000:4298
    LODSB SI                             ; 1000:429b
    CALL 0x1000:42e8                     ; 1000:429c ; -> 1000:42e8
    DEC DX                               ; 1000:429f
    JNZ 0x1000:429b                      ; 1000:42a0 ; -> 1000:429b
    MOV AX,0x9                           ; 1000:42a2
    CALL 0x1000:4339                     ; 1000:42a5 ; -> 1000:4339
    MOV AL,0x26                          ; 1000:42a8
    CALL 0x1000:42e8                     ; 1000:42aa ; -> 1000:42e8
    DEC word ptr [BP + -0x4]             ; 1000:42ad ; -> Stack[-0x6]
    JNZ 0x1000:4284                      ; 1000:42b0 ; -> 1000:4284
    ADD CX,0x8                           ; 1000:42b2
    MOV BX,word ptr [BP + -0x6]          ; 1000:42b5 ; -> Stack[-0x8]
    MOV AL,0x3a                          ; 1000:42b8
    CALL 0x1000:42e8                     ; 1000:42ba ; -> 1000:42e8
    MOV DX,word ptr [BP + -0x2]          ; 1000:42bd ; -> Stack[-0x4]
    SUB DX,0x2                           ; 1000:42c0
    MOV AL,0x3b                          ; 1000:42c3
    CALL 0x1000:42e8                     ; 1000:42c5 ; -> 1000:42e8
    DEC DX                               ; 1000:42c8
    JNZ 0x1000:42c5                      ; 1000:42c9 ; -> 1000:42c5
    MOV AL,0x3c                          ; 1000:42cb
    CALL 0x1000:42e8                     ; 1000:42cd ; -> 1000:42e8
    MOV SP,BP                            ; 1000:42d0
    POP BP                               ; 1000:42d2
    RET                                  ; 1000:42d3

; ------------------------------------------------------------------------------
; FUNCTION: ui_clear_window  [UI / Windows]
; ADDRESS:  1000:42d4  (Length: 0x14 bytes)
; CALLERS:  FUN_1000_6f47, FUN_1000_677a, FUN_1000_7057, FUN_1000_61c8
; PURPOSE:  Clears interior area of dialog box with background color.
; ------------------------------------------------------------------------------
ui_clear_window:
    PUSH BX                              ; 1000:42d4
    MOV CX,BX                            ; 1000:42d5
    MOV BX,AX                            ; 1000:42d7
    CLD                                  ; 1000:42d9
    LODSB SI                             ; 1000:42da
    OR AL,AL                             ; 1000:42db
    JZ 0x1000:42e4                       ; 1000:42dd ; -> 1000:42e4
    CALL 0x1000:42e8                     ; 1000:42df ; -> 1000:42e8
    JMP 0x1000:42da                      ; 1000:42e2 ; -> 1000:42da
    MOV AX,BX                            ; 1000:42e4
    POP BX                               ; 1000:42e6
    RET                                  ; 1000:42e7

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_window_border  [UI / Windows]
; ADDRESS:  1000:42e8  (Length: 0x51 bytes)
; CALLERS:  FUN_1000_6f47, FUN_1000_4206, FUN_1000_7057, FUN_1000_42d4, FUN_1000_61c8
; PURPOSE:  Blits decorative border tiles and corner pieces around dialog box.
; ------------------------------------------------------------------------------
ui_draw_window_border:
    PUSH AX                              ; 1000:42e8
    PUSH BX                              ; 1000:42e9
    PUSH CX                              ; 1000:42ea
    PUSH DX                              ; 1000:42eb
    PUSH SI                              ; 1000:42ec
    SHR BX,0x1                           ; 1000:42ed
    SHR BX,0x1                           ; 1000:42ef
    SHR BX,0x1                           ; 1000:42f1
    MOV DX,CX                            ; 1000:42f3
    SHL CX,0x1                           ; 1000:42f5
    SHL CX,0x1                           ; 1000:42f7
    SHL CX,0x1                           ; 1000:42f9
    SHL DX,0x1                           ; 1000:42fb
    SHL DX,0x1                           ; 1000:42fd
    SHL DX,0x1                           ; 1000:42ff
    SHL DX,0x1                           ; 1000:4301
    SHL DX,0x1                           ; 1000:4303
    ADD CX,DX                            ; 1000:4305
    ADD BX,CX                            ; 1000:4307
    MOV DI,word ptr CS:[0x773f]          ; 1000:4309 ; -> 1000:773f
    ADD DI,BX                            ; 1000:430e
    XOR AH,AH                            ; 1000:4310
    SUB AX,0x20                          ; 1000:4312
    MOV SI,0xa6f4                        ; 1000:4315
    ADD SI,AX                            ; 1000:4318
    MOV CX,0x8                           ; 1000:431a
    MOV DS,word ptr CS:[0x5e]            ; 1000:431d ; -> 1000:005e
    MOVSB ES:DI,SI                       ; 1000:4322
    ADD SI,0x3a                          ; 1000:4323
    ADD DI,0x27                          ; 1000:4326
    LOOP 0x1000:4322                     ; 1000:4329 ; -> 1000:4322
    MOV DS,word ptr CS:[0x5c]            ; 1000:432b ; -> 1000:005c
    POP SI                               ; 1000:4330
    POP DX                               ; 1000:4331
    POP CX                               ; 1000:4332
    POP BX                               ; 1000:4333
    POP AX                               ; 1000:4334
    ADD BX,0x8                           ; 1000:4335
    RET                                  ; 1000:4338

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_write_mode_0  [Video / Registers]
; ADDRESS:  1000:4339  (Length: 0x12 bytes)
; CALLERS:  FUN_1000_6f47, FUN_1000_4206, FUN_1000_677a, FUN_1000_7057, FUN_1000_61c8
; PURPOSE:  Configures EGA Graphics Controller to Write Mode 0 for standard memory writes.
; ------------------------------------------------------------------------------
vid_set_write_mode_0:
    PUSH DX                              ; 1000:4339
    MOV AH,AL                            ; 1000:433a
    MOV AL,0x2                           ; 1000:433c
    MOV DX,0x3c4                         ; 1000:433e
    OUT DX,AL                            ; 1000:4341
    INC DX                               ; 1000:4342
    XCHG AL,AH                           ; 1000:4343
    OUT DX,AL                            ; 1000:4345
    DEC DX                               ; 1000:4346
    XCHG AL,AH                           ; 1000:4347
    POP DX                               ; 1000:4349
    RET                                  ; 1000:434a

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_text_string  [UI / Text]
; ADDRESS:  1000:437b  (Length: 0xac bytes)
; CALLERS:  FUN_1000_3db6, FUN_1000_2341, FUN_1000_46e6
; PURPOSE:  Blits text string to EGA display buffer at pixel coordinate (X, Y) with color attribute.
; ------------------------------------------------------------------------------
ui_draw_text_string:
    MOV DI,word ptr [0x6fe]              ; 1000:437b
    OR DI,DI                             ; 1000:437f
    JNZ 0x1000:4384                      ; 1000:4381 ; -> 1000:4384
    RET                                  ; 1000:4383
    MOV AX,[0x6c2]                       ; 1000:4384
    MOV BX,0xc0                          ; 1000:4387
    ADD BX,AX                            ; 1000:438a
    MOV CX,word ptr [0x6c4]              ; 1000:438c
    MOV DX,0x9f                          ; 1000:4390
    ADD DX,CX                            ; 1000:4393
    SUB CX,0xf                           ; 1000:4395
    LEA SI,[0x700]                       ; 1000:4398
    TEST word ptr [SI + 0x4],0x8000      ; 1000:439c
    JNZ 0x1000:43b7                      ; 1000:43a1 ; -> 1000:43b7
    CMP AX,word ptr [SI]                 ; 1000:43a3
    JG 0x1000:43b7                       ; 1000:43a5 ; -> 1000:43b7
    CMP BX,word ptr [SI]                 ; 1000:43a7
    JL 0x1000:43b7                       ; 1000:43a9 ; -> 1000:43b7
    CMP CX,word ptr [SI + 0x2]           ; 1000:43ab
    JG 0x1000:43b7                       ; 1000:43ae ; -> 1000:43b7
    CMP DX,word ptr [SI + 0x2]           ; 1000:43b0
    JL 0x1000:43b7                       ; 1000:43b3 ; -> 1000:43b7
    JMP 0x1000:43be                      ; 1000:43b5 ; -> 1000:43be
    ADD SI,0x8                           ; 1000:43b7
    DEC DI                               ; 1000:43ba
    JNZ 0x1000:439c                      ; 1000:43bb ; -> 1000:439c
    RET                                  ; 1000:43bd
    PUSH AX                              ; 1000:43be
    PUSH BX                              ; 1000:43bf
    PUSH CX                              ; 1000:43c0
    PUSH DX                              ; 1000:43c1
    PUSH SI                              ; 1000:43c2
    PUSH DI                              ; 1000:43c3
    MOV CX,0xf                           ; 1000:43c4
    MOV DX,0x1f                          ; 1000:43c7
    CMP byte ptr [0x89d],0x1             ; 1000:43ca
    JNZ 0x1000:43d3                      ; 1000:43cf ; -> 1000:43d3
    XCHG DX,CX                           ; 1000:43d1
    MOV AX,word ptr [SI]                 ; 1000:43d3
    ADD AX,0xf                           ; 1000:43d5
    CMP AX,word ptr [0x6c6]              ; 1000:43d8
    JL 0x1000:440a                       ; 1000:43dc ; -> 1000:440a
    SUB AX,0xf                           ; 1000:43de
    SUB AX,CX                            ; 1000:43e1
    CMP AX,word ptr [0x6c6]              ; 1000:43e3
    JG 0x1000:440a                       ; 1000:43e7 ; -> 1000:440a
    MOV BX,word ptr [SI + 0x2]           ; 1000:43e9
    ADD BX,0xf                           ; 1000:43ec
    CMP BX,word ptr [0x6c8]              ; 1000:43ef
    JL 0x1000:440a                       ; 1000:43f3 ; -> 1000:440a
    SUB BX,0xf                           ; 1000:43f5
    SUB BX,DX                            ; 1000:43f8
    CMP BX,word ptr [0x6c8]              ; 1000:43fa
    JG 0x1000:440a                       ; 1000:43fe ; -> 1000:440a
    CMP byte ptr [0x8ec],0x0             ; 1000:4400
    JNZ 0x1000:440a                      ; 1000:4405 ; -> 1000:440a
    JMP 0x1000:441d                      ; 1000:4407 ; -> 1000:441d
    MOV AX,word ptr [SI]                 ; 1000:440a
    MOV BX,word ptr [SI + 0x2]           ; 1000:440c
    MOV SI,word ptr [SI + 0x6]           ; 1000:440f
    CALL 0x1000:7dbb                     ; 1000:4412 ; -> 1000:7dbb
    POP DI                               ; 1000:4415
    POP SI                               ; 1000:4416
    POP DX                               ; 1000:4417
    POP CX                               ; 1000:4418
    POP BX                               ; 1000:4419
    POP AX                               ; 1000:441a
    JMP 0x1000:43b7                      ; 1000:441b ; -> 1000:43b7
    MOV AX,0x1                           ; 1000:441d
    LEA BX,[0x98e0]                      ; 1000:4420
    MOV CX,0x3                           ; 1000:4424
    INT3                                 ; 1000:4427

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_dialog_message  [UI / Windows]
; ADDRESS:  1000:451c  (Length: 0x5d bytes)
; CALLERS:  FUN_1000_46e6
; PURPOSE:  Displays centered dialog box with text prompt and waits for user keypress.
; ------------------------------------------------------------------------------
ui_draw_dialog_message:
    MOV CX,AX                            ; 1000:451c
    SHL CX,0x1                           ; 1000:451e
    LEA BX,[0x239]                       ; 1000:4520
    ADD BX,CX                            ; 1000:4524
    MOV CX,word ptr [BX]                 ; 1000:4526
    CMP CX,-0x1                          ; 1000:4528
    JNZ 0x1000:4547                      ; 1000:452b ; -> 1000:4547
    SHL AX,0x1                           ; 1000:452d
    SHL AX,0x1                           ; 1000:452f
    SHL AX,0x1                           ; 1000:4531
    SHL AX,0x1                           ; 1000:4533
    ADD AX,0x8                           ; 1000:4535
    MOV BX,0xb1                          ; 1000:4538
    MOV CX,0x10                          ; 1000:453b
    MOV DX,0x10                          ; 1000:453e
    CALL 0x1000:7aab                     ; 1000:4541 ; -> 1000:7aab
    XOR DI,DI                            ; 1000:4544
    RET                                  ; 1000:4546
    LEA DI,[0xaf0]                       ; 1000:4547
    MOV BX,CX                            ; 1000:454b
    SHL CX,0x1                           ; 1000:454d
    ADD CX,BX                            ; 1000:454f
    SHL CX,0x1                           ; 1000:4551
    SHL CX,0x1                           ; 1000:4553
    ADD DI,CX                            ; 1000:4555
    PUSH DI                              ; 1000:4557
    SHL AX,0x1                           ; 1000:4558
    SHL AX,0x1                           ; 1000:455a
    SHL AX,0x1                           ; 1000:455c
    SHL AX,0x1                           ; 1000:455e
    ADD AX,0x8                           ; 1000:4560
    MOV BX,0xb1                          ; 1000:4563
    MOV SI,word ptr CS:[DI + 0x8]        ; 1000:4566
    MOV DS,word ptr CS:[0x5e]            ; 1000:456a ; -> 1000:005e
    CALL 0x1000:78c6                     ; 1000:456f ; -> 1000:78c6
    MOV DS,word ptr CS:[0x5c]            ; 1000:4572 ; -> 1000:005c
    POP DI                               ; 1000:4577
    RET                                  ; 1000:4578

; ------------------------------------------------------------------------------
; FUNCTION: ui_erase_dialog  [UI / Windows]
; ADDRESS:  1000:4579  (Length: 0x22 bytes)
; CALLERS:  FUN_1000_3181
; PURPOSE:  Restores original background graphics obscured by a temporary dialog box.
; ------------------------------------------------------------------------------
ui_erase_dialog:
    MOV DI,word ptr [0x6fe]              ; 1000:4579
    OR DI,DI                             ; 1000:457d
    JNZ 0x1000:4582                      ; 1000:457f ; -> 1000:4582
    RET                                  ; 1000:4581
    LEA SI,[0x700]                       ; 1000:4582
    CMP AX,word ptr [SI]                 ; 1000:4586
    JNZ 0x1000:4594                      ; 1000:4588 ; -> 1000:4594
    CMP BX,word ptr [SI + 0x2]           ; 1000:458a
    JNZ 0x1000:4594                      ; 1000:458d ; -> 1000:4594
    AND word ptr [SI + 0x4],0x7fff       ; 1000:458f
    ADD SI,0x8                           ; 1000:4594
    DEC DI                               ; 1000:4597
    JNZ 0x1000:4586                      ; 1000:4598 ; -> 1000:4586
    RET                                  ; 1000:459a

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_dialog_beep  [Audio / SFX]
; ADDRESS:  1000:45a6  (Length: 0xb bytes)
; CALLERS:  FUN_1000_4cc1
; PURPOSE:  Plays UI cursor navigation beep sound.
; ------------------------------------------------------------------------------
snd_play_dialog_beep:
    MOV AX,0x1                           ; 1000:45a6
    LEA BX,[0x9904]                      ; 1000:45a9
    MOV CX,0x9                           ; 1000:45ad
    INT3                                 ; 1000:45b0

; ------------------------------------------------------------------------------
; FUNCTION: game_pause_handler  [Game / State]
; ADDRESS:  1000:46e6  (Length: 0x16f bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Handles Game Pause state: displays 'GAME PAUSED' banner, pauses timers, and polls Q key to quit.
; ------------------------------------------------------------------------------
game_pause_handler:
    CALL 0x1000:1d2c                     ; 1000:46e6 ; -> 1000:1d2c
    CALL 0x1000:437b                     ; 1000:46e9 ; -> 1000:437b
    CALL 0x1000:774e                     ; 1000:46ec ; -> 1000:774e
    CALL 0x1000:1d2c                     ; 1000:46ef ; -> 1000:1d2c
    CALL 0x1000:437b                     ; 1000:46f2 ; -> 1000:437b
    MOV AX,[0x890]                       ; 1000:46f5 ; -> 34fe:0890
    SHL AX,0x1                           ; 1000:46f8
    SHL AX,0x1                           ; 1000:46fa
    SHL AX,0x1                           ; 1000:46fc
    SHL AX,0x1                           ; 1000:46fe
    ADD AX,0x8                           ; 1000:4700
    MOV BX,0xb1                          ; 1000:4703
    LEA SI,[0xa334]                      ; 1000:4706
    MOV DS,word ptr CS:[0x5e]            ; 1000:470a ; -> 1000:005e
    CALL 0x1000:77a3                     ; 1000:470f ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:4712 ; -> 1000:005c
    CALL 0x1000:774e                     ; 1000:4717 ; -> 1000:774e
    XOR AL,AL                            ; 1000:471a
    MOV CS:[0x258],AL                    ; 1000:471c ; -> 1000:0258
    MOV CS:[0x259],AL                    ; 1000:4720 ; -> 1000:0259
    MOV CS:[0x25d],AL                    ; 1000:4724 ; -> 1000:025d
    MOV CS:[0x61],AL                     ; 1000:4728 ; -> 1000:0061
    CMP byte ptr CS:[0x258],0x1          ; 1000:472c ; -> 1000:0258
    JZ 0x1000:475c                       ; 1000:4732 ; -> 1000:475c
    CMP byte ptr CS:[0x259],0x1          ; 1000:4734 ; -> 1000:0259
    JZ 0x1000:476f                       ; 1000:473a ; -> 1000:476f
    CMP byte ptr CS:[0x25d],0x1          ; 1000:473c ; -> 1000:025d
    JZ 0x1000:4783                       ; 1000:4742 ; -> 1000:4783
    MOV AL,CS:[0x61]                     ; 1000:4744 ; -> 1000:0061
    CMP AL,byte ptr CS:[0x257]           ; 1000:4748 ; -> 1000:0257
    JNZ 0x1000:472c                      ; 1000:474d ; -> 1000:472c
    MOV AX,[0x890]                       ; 1000:474f ; -> 34fe:0890
    CALL 0x1000:451c                     ; 1000:4752 ; -> 1000:451c
    MOV AX,0x1                           ; 1000:4755
    CALL 0x1000:027a                     ; 1000:4758 ; -> 1000:027a
    RET                                  ; 1000:475b
    CMP word ptr [0x890],0x0             ; 1000:475c ; -> 34fe:0890
    JZ 0x1000:471a                       ; 1000:4761 ; -> 1000:471a
    MOV AX,[0x890]                       ; 1000:4763 ; -> 34fe:0890
    CALL 0x1000:451c                     ; 1000:4766 ; -> 1000:451c
    DEC word ptr [0x890]                 ; 1000:4769 ; -> 34fe:0890
    JMP 0x1000:46f5                      ; 1000:476d ; -> 1000:46f5
    CMP word ptr [0x890],0xc             ; 1000:476f ; -> 34fe:0890
    JZ 0x1000:471a                       ; 1000:4774 ; -> 1000:471a
    MOV AX,[0x890]                       ; 1000:4776 ; -> 34fe:0890
    CALL 0x1000:451c                     ; 1000:4779 ; -> 1000:451c
    INC word ptr [0x890]                 ; 1000:477c ; -> 34fe:0890
    JMP 0x1000:46f5                      ; 1000:4780 ; -> 1000:46f5
    MOV AX,[0x890]                       ; 1000:4783 ; -> 34fe:0890
    CALL 0x1000:451c                     ; 1000:4786 ; -> 1000:451c
    OR DI,DI                             ; 1000:4789
    JNZ 0x1000:479a                      ; 1000:478b ; -> 1000:479a
    MOV byte ptr CS:[0x25d],0x0          ; 1000:478d ; -> 1000:025d
    MOV AX,0x1                           ; 1000:4793
    CALL 0x1000:027a                     ; 1000:4796 ; -> 1000:027a
    RET                                  ; 1000:4799
    CMP word ptr CS:[DI + 0x8],0xb3d2    ; 1000:479a
    JNZ 0x1000:47dd                      ; 1000:47a0 ; -> 1000:47dd
    CMP word ptr [0x6fa],0xb             ; 1000:47a2 ; -> 34fe:06fa
    JNZ 0x1000:47dd                      ; 1000:47a7 ; -> 1000:47dd
    CMP word ptr [0x6fc],0x7             ; 1000:47a9 ; -> 34fe:06fc
    JNZ 0x1000:47dd                      ; 1000:47ae ; -> 1000:47dd
    CMP word ptr [0x6c6],0x250           ; 1000:47b0 ; -> 34fe:06c6
    JNZ 0x1000:47dd                      ; 1000:47b6 ; -> 1000:47dd
    CMP word ptr [0x6c8],0x50            ; 1000:47b8 ; -> 34fe:06c8
    JNZ 0x1000:47dd                      ; 1000:47bd ; -> 1000:47dd
    MOV byte ptr [0x221],0x1             ; 1000:47bf ; -> 34fe:0221
    MOV AX,0x1                           ; 1000:47c4
    LEA BX,[0x99a4]                      ; 1000:47c7
    MOV CX,0x9                           ; 1000:47cb
    INT3                                 ; 1000:47ce
    MOV AX,[0x6fa]                       ; 1000:47dd ; -> 34fe:06fa
    MOV word ptr CS:[DI],AX              ; 1000:47e0
    MOV AX,[0x6fc]                       ; 1000:47e3 ; -> 34fe:06fc
    MOV word ptr CS:[DI + 0x2],AX        ; 1000:47e6
    MOV AX,[0x6c6]                       ; 1000:47ea ; -> 34fe:06c6
    MOV word ptr CS:[DI + 0x4],AX        ; 1000:47ed
    MOV AX,[0x6c8]                       ; 1000:47f1 ; -> 34fe:06c8
    ADD AX,0x10                          ; 1000:47f4
    MOV word ptr CS:[DI + 0x6],AX        ; 1000:47f7
    MOV SI,0x700                         ; 1000:47fb
    MOV CX,word ptr [0x6fe]              ; 1000:47fe ; -> 34fe:06fe
    OR CX,CX                             ; 1000:4802
    JZ 0x1000:4819                       ; 1000:4804 ; -> 1000:4819
    CMP word ptr [SI],-0x1               ; 1000:4806 ; -> 34fe:0700
    JZ 0x1000:481d                       ; 1000:4809 ; -> 1000:481d
    ADD SI,0x8                           ; 1000:480b
    DEC CX                               ; 1000:480e
    JNZ 0x1000:4806                      ; 1000:480f ; -> 1000:4806
    CMP word ptr [0x6fe],0x32            ; 1000:4811 ; -> 34fe:06fe
    JL 0x1000:4819                       ; 1000:4816 ; -> 1000:4819
    RET                                  ; 1000:4818
    INC word ptr [0x6fe]                 ; 1000:4819 ; -> 34fe:06fe
    MOV word ptr [0x8ab],SI              ; 1000:481d ; -> 34fe:08ab
    MOV AX,word ptr CS:[DI + 0x4]        ; 1000:4821
    MOV [0x8a9],AX                       ; 1000:4825 ; -> 34fe:08a9
    MOV word ptr [SI],0xffff             ; 1000:4828 ; -> 34fe:0708
    MOV AX,word ptr CS:[DI + 0x6]        ; 1000:482c
    MOV word ptr [SI + 0x2],AX           ; 1000:4830 ; -> 34fe:070a
    MOV AX,word ptr CS:[DI + 0x8]        ; 1000:4833
    MOV word ptr [SI + 0x6],AX           ; 1000:4837 ; -> 34fe:070e
    MOV AX,[0x890]                       ; 1000:483a ; -> 34fe:0890
    MOV CX,AX                            ; 1000:483d
    SHL CX,0x1                           ; 1000:483f
    LEA BX,[0x239]                       ; 1000:4841
    ADD BX,CX                            ; 1000:4845
    MOV CX,word ptr [BX]                 ; 1000:4847
    MOV word ptr [SI + 0x4],CX           ; 1000:4849 ; -> 34fe:070c
    MOV word ptr [BX],0xffff             ; 1000:484c
    CALL 0x1000:451c                     ; 1000:4850 ; -> 1000:451c
    MOV byte ptr [0x8a8],0x14            ; 1000:4853 ; -> 34fe:08a8
    MOV AX,0x1                           ; 1000:4858
    LEA BX,[0x98fc]                      ; 1000:485b
    MOV CX,0x4                           ; 1000:485f
    INT3                                 ; 1000:4862

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_chest_open  [Audio / SFX]
; ADDRESS:  1000:4870  (Length: 0xb bytes)
; CALLERS:  FUN_1000_6806
; PURPOSE:  Plays treasure chest unlock sound effect.
; ------------------------------------------------------------------------------
snd_play_chest_open:
    MOV AX,0x1                           ; 1000:4870
    LEA BX,[0x98e0]                      ; 1000:4873
    MOV CX,0x3                           ; 1000:4877
    INT3                                 ; 1000:487a

; ------------------------------------------------------------------------------
; FUNCTION: item_check_has_item  [Items / Inventory]
; ADDRESS:  1000:489e  (Length: 0xc bytes)
; CALLERS:  FUN_1000_48aa
; PURPOSE:  Checks if specified item flag is set in player's inventory bitmask.
; ------------------------------------------------------------------------------
item_check_has_item:
    JCXZ 0x1000:48a9                     ; 1000:489e ; -> 1000:48a9
    DEC CL                               ; 1000:48a0
    JGE 0x1000:48a9                      ; 1000:48a2 ; -> 1000:48a9
    ADD CL,0xa                           ; 1000:48a4
    DEC CH                               ; 1000:48a7
    RET                                  ; 1000:48a9

; ------------------------------------------------------------------------------
; FUNCTION: item_add_to_inventory  [Items / Inventory]
; ADDRESS:  1000:48aa  (Length: 0x15 bytes)
; CALLERS:  FUN_1000_330f
; PURPOSE:  Sets item flag in player inventory bitmask and triggers item pickup notification.
; ------------------------------------------------------------------------------
item_add_to_inventory:
    MOV CX,word ptr [0x222]              ; 1000:48aa
    CALL 0x1000:489e                     ; 1000:48ae ; -> 1000:489e
    MOV word ptr [0x222],CX              ; 1000:48b1
    MOV AX,0x120                         ; 1000:48b5
    MOV BX,0x90                          ; 1000:48b8
    CALL 0x1000:3a5f                     ; 1000:48bb ; -> 1000:3a5f
    RET                                  ; 1000:48be

; ------------------------------------------------------------------------------
; FUNCTION: item_remove_from_inventory  [Items / Inventory]
; ADDRESS:  1000:48d4  (Length: 0x19 bytes)
; CALLERS:  FUN_1000_4cc1, FUN_1000_49ad
; PURPOSE:  Clears item flag from player inventory bitmask upon item consumption.
; ------------------------------------------------------------------------------
item_remove_from_inventory:
    MOV CX,word ptr [0x6fe]              ; 1000:48d4
    JCXZ 0x1000:48ec                     ; 1000:48d8 ; -> 1000:48ec
    MOV SI,0x700                         ; 1000:48da
    CMP word ptr [SI + 0x2],0x8000       ; 1000:48dd
    JC 0x1000:48e7                       ; 1000:48e2 ; -> 1000:48e7
    NEG word ptr [SI + 0x2]              ; 1000:48e4
    ADD SI,0x8                           ; 1000:48e7
    LOOP 0x1000:48dd                     ; 1000:48ea ; -> 1000:48dd
    RET                                  ; 1000:48ec

; ------------------------------------------------------------------------------
; FUNCTION: spr_render_monster  [Sprites / Monsters]
; ADDRESS:  1000:49ad  (Length: 0xc3 bytes)
; CALLERS:  FUN_1000_18e0
; PURPOSE:  Renders monster sprite with planar EGA color plane writes and transparency masking.
; ------------------------------------------------------------------------------
spr_render_monster:
    PUSH BP                              ; 1000:49ad
    MOV BP,SP                            ; 1000:49ae
    SUB SP,0x4                           ; 1000:49b0
    MOV byte ptr [0x9a22],0x0            ; 1000:49b3
    MOV byte ptr [0x9a24],0x0            ; 1000:49b8
    MOV BX,word ptr [0x6fa]              ; 1000:49bd
    ADD BX,0x229                         ; 1000:49c1
    MOV AL,byte ptr [BX]                 ; 1000:49c5
    MOV [0x9a23],AL                      ; 1000:49c7
    CMP AL,0x1                           ; 1000:49ca
    JZ 0x1000:49d1                       ; 1000:49cc ; -> 1000:49d1
    CALL 0x1000:48d4                     ; 1000:49ce ; -> 1000:48d4
    MOV SI,word ptr [0x6f8]              ; 1000:49d1
    MOV CX,word ptr [0x6f6]              ; 1000:49d5
    OR CX,CX                             ; 1000:49d9
    JZ 0x1000:49f6                       ; 1000:49db ; -> 1000:49f6
    MOV byte ptr CS:[SI + 0x6],0x0       ; 1000:49dd
    MOV AX,word ptr CS:[SI]              ; 1000:49e2
    MOV word ptr CS:[SI + 0x8],AX        ; 1000:49e5
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:49e9
    MOV word ptr CS:[SI + 0xa],AX        ; 1000:49ed
    ADD SI,0xc                           ; 1000:49f1
    LOOP 0x1000:49dd                     ; 1000:49f4 ; -> 1000:49dd
    MOV word ptr [BP + -0x2],0x0         ; 1000:49f6 ; -> Stack[-0x4]
    MOV SI,0x48ed                        ; 1000:49fb
    MOV DI,word ptr [0x6f8]              ; 1000:49fe
    MOV AX,[0x6f6]                       ; 1000:4a02
    MOV word ptr [BP + -0x4],AX          ; 1000:4a05 ; -> Stack[-0x6]
    OR AX,AX                             ; 1000:4a08
    JZ 0x1000:4a5a                       ; 1000:4a0a ; -> 1000:4a5a
    MOV CX,word ptr [0x6c2]              ; 1000:4a0c
    ADD CX,0xd0                          ; 1000:4a10
    MOV DX,word ptr [0x6c4]              ; 1000:4a14
    ADD DX,0xa0                          ; 1000:4a18
    MOV BX,word ptr CS:[DI + 0x4]        ; 1000:4a1c
    MOV AX,word ptr CS:[DI + 0x8]        ; 1000:4a20
    CMP AX,word ptr [0x6c2]              ; 1000:4a24
    JL 0x1000:4a52                       ; 1000:4a28 ; -> 1000:4a52
    ADD AX,word ptr CS:[BX]              ; 1000:4a2a
    CMP AX,CX                            ; 1000:4a2d
    JG 0x1000:4a52                       ; 1000:4a2f ; -> 1000:4a52
    MOV AX,word ptr CS:[DI + 0xa]        ; 1000:4a31
    CMP AX,DX                            ; 1000:4a35
    JGE 0x1000:4a52                      ; 1000:4a37 ; -> 1000:4a52
    ADD AX,word ptr CS:[BX + 0x2]        ; 1000:4a39
    CMP AX,word ptr [0x6c4]              ; 1000:4a3d
    JLE 0x1000:4a52                      ; 1000:4a41 ; -> 1000:4a52
    CALL 0x1000:4a70                     ; 1000:4a43 ; -> 1000:4a70
    ADD SI,0x20                          ; 1000:4a46
    INC word ptr [BP + -0x2]             ; 1000:4a49 ; -> Stack[-0x4]
    CMP word ptr [BP + -0x2],0x6         ; 1000:4a4c ; -> Stack[-0x4]
    JZ 0x1000:4a6c                       ; 1000:4a50 ; -> 1000:4a6c
    ADD DI,0xc                           ; 1000:4a52
    DEC word ptr [BP + -0x4]             ; 1000:4a55 ; -> Stack[-0x6]
    JNZ 0x1000:4a0c                      ; 1000:4a58 ; -> 1000:4a0c
    MOV word ptr CS:[SI + 0x6],0x0       ; 1000:4a5a ; -> 1000:4913
    ADD SI,0x20                          ; 1000:4a60
    INC word ptr [BP + -0x2]             ; 1000:4a63 ; -> Stack[-0x4]
    CMP word ptr [BP + -0x2],0x6         ; 1000:4a66 ; -> Stack[-0x4]
    JNZ 0x1000:4a5a                      ; 1000:4a6a ; -> 1000:4a5a
    MOV SP,BP                            ; 1000:4a6c
    POP BP                               ; 1000:4a6e
    RET                                  ; 1000:4a6f

; ------------------------------------------------------------------------------
; FUNCTION: spr_get_monster_frame  [Sprites / Animation]
; ADDRESS:  1000:4a70  (Length: 0x73 bytes)
; CALLERS:  FUN_1000_4b46, FUN_1000_49ad
; PURPOSE:  Looks up graphic offset for monster animation frame from sprite catalog.
; ------------------------------------------------------------------------------
spr_get_monster_frame:
    MOV CX,0x8                           ; 1000:4a70
    PUSH BX                              ; 1000:4a73
    PUSH SI                              ; 1000:4a74
    MOV AX,word ptr CS:[BX]              ; 1000:4a75
    ADD BX,0x2                           ; 1000:4a78
    MOV word ptr CS:[SI],AX              ; 1000:4a7b
    ADD SI,0x2                           ; 1000:4a7e
    LOOP 0x1000:4a75                     ; 1000:4a81 ; -> 1000:4a75
    POP SI                               ; 1000:4a83
    POP BX                               ; 1000:4a84
    MOV AX,word ptr CS:[DI + 0x8]        ; 1000:4a85
    MOV word ptr CS:[SI + 0x10],AX       ; 1000:4a89
    MOV AX,word ptr CS:[DI + 0xa]        ; 1000:4a8d
    MOV word ptr CS:[SI + 0x12],AX       ; 1000:4a91
    XOR AX,AX                            ; 1000:4a95
    MOV word ptr CS:[SI + 0x14],AX       ; 1000:4a97
    MOV word ptr CS:[SI + 0x16],AX       ; 1000:4a9b
    MOV word ptr CS:[SI + 0x1c],AX       ; 1000:4a9f
    MOV word ptr CS:[SI + 0x1a],AX       ; 1000:4aa3
    MOV word ptr CS:[SI + 0x1e],DI       ; 1000:4aa7
    MOV AX,[0x9a20]                      ; 1000:4aab
    XOR word ptr [0x9a20],0x3            ; 1000:4aae
    MOV word ptr CS:[SI + 0x18],AX       ; 1000:4ab3
    MOV byte ptr CS:[DI + 0x6],0x1       ; 1000:4ab7
    MOV AX,word ptr CS:[SI + 0x4]        ; 1000:4abc
    CMP AH,0x1                           ; 1000:4ac0
    JNZ 0x1000:4ade                      ; 1000:4ac3 ; -> 1000:4ade
    CMP byte ptr [0x9a23],0x1            ; 1000:4ac5
    JZ 0x1000:4ad4                       ; 1000:4aca ; -> 1000:4ad4
    MOV word ptr CS:[SI + 0x10],0xffff   ; 1000:4acc
    JMP 0x1000:4ade                      ; 1000:4ad2 ; -> 1000:4ade
    CMP byte ptr [0x9a22],0x0            ; 1000:4ad4
    JNZ 0x1000:4ade                      ; 1000:4ad9 ; -> 1000:4ade
    MOV [0x9a22],AL                      ; 1000:4adb
    INC byte ptr [0x9a24]                ; 1000:4ade
    RET                                  ; 1000:4ae2

; ------------------------------------------------------------------------------
; FUNCTION: vid_flip_buffers  [Video / Double Buffering]
; ADDRESS:  1000:4b25  (Length: 0x21 bytes)
; CALLERS:  FUN_1000_3c13, FUN_1000_4073, FUN_1000_3e98, FUN_1000_3e1f, FUN_1000_2341
; PURPOSE:  Toggles active and visible video page offsets for flicker-free double buffering.
; ------------------------------------------------------------------------------
vid_flip_buffers:
    PUSH BP                              ; 1000:4b25
    MOV BP,SP                            ; 1000:4b26
    SUB SP,0x4                           ; 1000:4b28
    MOV SI,0x48ed                        ; 1000:4b2b
    MOV DI,word ptr [0x6f8]              ; 1000:4b2e
    MOV AX,[0x6f6]                       ; 1000:4b32
    MOV word ptr [BP + -0x2],AX          ; 1000:4b35 ; -> Stack[-0x4]
    MOV CX,0x6                           ; 1000:4b38
    PUSH CX                              ; 1000:4b3b
    CMP word ptr CS:[SI + 0x6],0x0       ; 1000:4b3c ; -> 1000:48f3
    JZ 0x1000:4b59                       ; 1000:4b41 ; -> 1000:4b59
    JMP 0x1000:4f15                      ; 1000:4b43 ; -> 1000:4f15

; ------------------------------------------------------------------------------
; FUNCTION: spr_blit_masked  [Video / Sprites]
; ADDRESS:  1000:4b46  (Length: 0xbf bytes)
; CALLERS:  FUN_1000_5090
; PURPOSE:  Low-level masked EGA sprite blitter using latch registers and bitmasking.
; ------------------------------------------------------------------------------
spr_blit_masked:
    OR SI,SI                             ; 1000:4b46
    JZ 0x1000:4b55                       ; 1000:4b48 ; -> 1000:4b55
    POP CX                               ; 1000:4b4a
    ADD SI,0x20                          ; 1000:4b4b
    LOOP 0x1000:4b3b                     ; 1000:4b4e ; -> 1000:4b3b
    XOR SI,SI                            ; 1000:4b50
    JMP 0x1000:4b59                      ; 1000:4b52 ; -> 1000:4b59
    MOV SP,BP                            ; 1000:4b55
    POP BP                               ; 1000:4b57
    RET                                  ; 1000:4b58
    CMP word ptr [BP + -0x2],0x0         ; 1000:4b59
    JZ 0x1000:4b46                       ; 1000:4b5d ; -> 1000:4b46
    TEST word ptr CS:[DI + 0x6],0x700    ; 1000:4b5f
    JZ 0x1000:4b74                       ; 1000:4b65 ; -> 1000:4b74
    CMP byte ptr CS:[DI + 0x7],0x1       ; 1000:4b67
    JZ 0x1000:4b7e                       ; 1000:4b6c ; -> 1000:4b7e
    DEC byte ptr CS:[DI + 0x7]           ; 1000:4b6e
    JMP 0x1000:4b7e                      ; 1000:4b72 ; -> 1000:4b7e
    CMP word ptr CS:[DI + 0x6],0x1       ; 1000:4b74
    JNZ 0x1000:4b7e                      ; 1000:4b79 ; -> 1000:4b7e
    JMP 0x1000:4bfd                      ; 1000:4b7b ; -> 1000:4bfd
    MOV CX,word ptr [0x6c2]              ; 1000:4b7e
    ADD CX,0xd0                          ; 1000:4b82
    MOV DX,word ptr [0x6c4]              ; 1000:4b86
    ADD DX,0xa0                          ; 1000:4b8a
    MOV BX,word ptr CS:[DI + 0x4]        ; 1000:4b8e
    MOV AX,word ptr CS:[DI + 0x8]        ; 1000:4b92
    CMP AX,word ptr [0x6c2]              ; 1000:4b96
    JL 0x1000:4bf8                       ; 1000:4b9a ; -> 1000:4bf8
    ADD AX,word ptr CS:[BX]              ; 1000:4b9c
    CMP AX,CX                            ; 1000:4b9f
    JG 0x1000:4bf8                       ; 1000:4ba1 ; -> 1000:4bf8
    MOV AX,word ptr CS:[DI + 0xa]        ; 1000:4ba3
    CMP AX,DX                            ; 1000:4ba7
    JGE 0x1000:4bf8                      ; 1000:4ba9 ; -> 1000:4bf8
    ADD AX,word ptr CS:[BX + 0x2]        ; 1000:4bab
    CMP AX,word ptr [0x6c4]              ; 1000:4baf
    JLE 0x1000:4bf8                      ; 1000:4bb3 ; -> 1000:4bf8
    TEST word ptr CS:[DI + 0x6],0x700    ; 1000:4bb5
    JNZ 0x1000:4bc6                      ; 1000:4bbb ; -> 1000:4bc6
    CMP byte ptr CS:[DI + 0x6],0x0       ; 1000:4bbd
    JNZ 0x1000:4bfd                      ; 1000:4bc2 ; -> 1000:4bfd
    JMP 0x1000:4be1                      ; 1000:4bc4 ; -> 1000:4be1
    CMP byte ptr CS:[DI + 0x7],0x1       ; 1000:4bc6
    JNZ 0x1000:4bfd                      ; 1000:4bcb ; -> 1000:4bfd
    CMP byte ptr [0x9a23],0x1            ; 1000:4bcd
    JNZ 0x1000:4bfd                      ; 1000:4bd2 ; -> 1000:4bfd
    CMP byte ptr [0x9a24],0x5            ; 1000:4bd4
    JGE 0x1000:4bfd                      ; 1000:4bd9 ; -> 1000:4bfd
    MOV word ptr CS:[DI + 0x6],0x600     ; 1000:4bdb
    OR SI,SI                             ; 1000:4be1
    JNZ 0x1000:4bec                      ; 1000:4be3 ; -> 1000:4bec
    MOV byte ptr CS:[DI + 0x6],0x2       ; 1000:4be5
    JMP 0x1000:4bfd                      ; 1000:4bea ; -> 1000:4bfd
    CALL 0x1000:4a70                     ; 1000:4bec ; -> 1000:4a70
    ADD DI,0xc                           ; 1000:4bef
    DEC word ptr [BP + -0x2]             ; 1000:4bf2
    JMP 0x1000:4f15                      ; 1000:4bf5 ; -> 1000:4f15
    MOV byte ptr CS:[DI + 0x6],0x0       ; 1000:4bf8
    ADD DI,0xc                           ; 1000:4bfd
    DEC word ptr [BP + -0x2]             ; 1000:4c00
    JMP 0x1000:4b59                      ; 1000:4c03 ; -> 1000:4b59

; ------------------------------------------------------------------------------
; FUNCTION: math_clamp_coords  [Math / Geometry]
; ADDRESS:  1000:4c06  (Length: 0x1f bytes)
; CALLERS:  FUN_1000_4f15
; PURPOSE:  Clamps entity X/Y coordinates to stay within room bounding box.
; ------------------------------------------------------------------------------
math_clamp_coords:
    MOV word ptr CS:[SI + 0x6],0x0       ; 1000:4c06
    MOV BX,word ptr CS:[SI + 0x1e]       ; 1000:4c0c
    MOV byte ptr CS:[BX + 0x6],0x2       ; 1000:4c10
    MOV AX,word ptr CS:[BX]              ; 1000:4c15
    MOV word ptr CS:[BX + 0x8],AX        ; 1000:4c18
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:4c1c
    MOV word ptr CS:[BX + 0xa],AX        ; 1000:4c20
    RET                                  ; 1000:4c24

; ------------------------------------------------------------------------------
; FUNCTION: map_scroll_camera  [Tilemap / Camera]
; ADDRESS:  1000:4c25  (Length: 0x80 bytes)
; CALLERS:  FUN_1000_4cc1, FUN_1000_4f15
; PURPOSE:  Calculates camera scroll offsets relative to player position to keep Comic on screen.
; ------------------------------------------------------------------------------
map_scroll_camera:
    MOV word ptr CS:[SI + 0x6],0x0       ; 1000:4c25
    CMP byte ptr [0x8b1],0x6             ; 1000:4c2b
    JZ 0x1000:4c83                       ; 1000:4c30 ; -> 1000:4c83
    MOV DI,0x8b2                         ; 1000:4c32
    CMP byte ptr [DI],0x0                ; 1000:4c35
    JZ 0x1000:4c3f                       ; 1000:4c38 ; -> 1000:4c3f
    ADD DI,0x9                           ; 1000:4c3a
    JMP 0x1000:4c35                      ; 1000:4c3d ; -> 1000:4c35
    INC byte ptr [0x8b1]                 ; 1000:4c3f
    MOV byte ptr [DI],0x2                ; 1000:4c43
    MOV byte ptr [DI + 0x4],0x0          ; 1000:4c46
    MOV AX,word ptr CS:[SI]              ; 1000:4c4a
    SUB AX,0x10                          ; 1000:4c4d
    SAR AX,0x1                           ; 1000:4c50
    ADD AX,word ptr CS:[SI + 0x10]       ; 1000:4c52
    MOV word ptr [DI + 0x5],AX           ; 1000:4c56
    MOV AX,word ptr CS:[SI + 0x12]       ; 1000:4c59
    MOV word ptr [DI + 0x7],AX           ; 1000:4c5d
    CMP word ptr CS:[SI + 0x2],0x10      ; 1000:4c60
    JLE 0x1000:4c83                      ; 1000:4c65 ; -> 1000:4c83
    SUB word ptr CS:[SI + 0x2],0x10      ; 1000:4c67
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:4c6c
    ADD word ptr CS:[SI + 0x12],AX       ; 1000:4c70
    CMP word ptr CS:[SI + 0x2],0x10      ; 1000:4c74
    JLE 0x1000:4c81                      ; 1000:4c79 ; -> 1000:4c81
    MOV word ptr CS:[SI + 0x2],0x10      ; 1000:4c7b
    JMP 0x1000:4c25                      ; 1000:4c81 ; -> 1000:4c25
    MOV BX,word ptr CS:[SI + 0x1e]       ; 1000:4c83
    MOV byte ptr CS:[BX + 0x6],0x2       ; 1000:4c87
    MOV AX,word ptr CS:[BX]              ; 1000:4c8c
    MOV word ptr CS:[BX + 0x8],AX        ; 1000:4c8f
    MOV AX,word ptr CS:[BX + 0x2]        ; 1000:4c93
    MOV word ptr CS:[BX + 0xa],AX        ; 1000:4c97
    MOV AX,0x1                           ; 1000:4c9b
    MOV BX,0x9a25                        ; 1000:4c9e
    MOV CX,0x2                           ; 1000:4ca1
    INT3                                 ; 1000:4ca4

; ------------------------------------------------------------------------------
; FUNCTION: map_check_room_bounds  [Tilemap / Streaming]
; ADDRESS:  1000:4cc1  (Length: 0x7d bytes)
; CALLERS:  FUN_1000_4f15
; PURPOSE:  Tests if player has crossed room boundaries to trigger screen transition.
; ------------------------------------------------------------------------------
map_check_room_bounds:
    MOV BX,DI                            ; 1000:4cc1
    CMP byte ptr [0x8b1],0x6             ; 1000:4cc3
    JZ 0x1000:4cf3                       ; 1000:4cc8 ; -> 1000:4cf3
    MOV DI,0x8b2                         ; 1000:4cca
    CMP byte ptr [DI],0x0                ; 1000:4ccd
    JZ 0x1000:4cd7                       ; 1000:4cd0 ; -> 1000:4cd7
    ADD DI,0x9                           ; 1000:4cd2
    JMP 0x1000:4ccd                      ; 1000:4cd5 ; -> 1000:4ccd
    INC byte ptr [0x8b1]                 ; 1000:4cd7
    MOV byte ptr [DI],0x2                ; 1000:4cdb
    MOV byte ptr [DI + 0x4],0x2          ; 1000:4cde
    MOV AX,word ptr [BX]                 ; 1000:4ce2
    SUB AX,0x4                           ; 1000:4ce4
    MOV word ptr [DI + 0x5],AX           ; 1000:4ce7
    MOV AX,word ptr [BX + 0x2]           ; 1000:4cea
    SUB AX,0x4                           ; 1000:4ced
    MOV word ptr [DI + 0x7],AX           ; 1000:4cf0
    MOV word ptr [BX],0xffff             ; 1000:4cf3
    CMP byte ptr [0x9a22],0x1            ; 1000:4cf7
    JNZ 0x1000:4d30                      ; 1000:4cfc ; -> 1000:4d30
    MOV BX,word ptr [0x6fa]              ; 1000:4cfe
    ADD BX,0x229                         ; 1000:4d02
    MOV byte ptr [BX],0x0                ; 1000:4d06
    MOV byte ptr [0x9a23],0x0            ; 1000:4d09
    MOV CX,0x6                           ; 1000:4d0e
    MOV BX,0x48ed                        ; 1000:4d11
    MOV word ptr CS:[BX + 0x6],0x0       ; 1000:4d14 ; -> 1000:48f3
    ADD BX,0x20                          ; 1000:4d1a
    LOOP 0x1000:4d14                     ; 1000:4d1d ; -> 1000:4d14
    PUSH SI                              ; 1000:4d1f
    CALL 0x1000:45a6                     ; 1000:4d20 ; -> 1000:45a6
    CALL 0x1000:48d4                     ; 1000:4d23 ; -> 1000:48d4
    POP SI                               ; 1000:4d26
    MOV AX,0x5                           ; 1000:4d27
    CALL 0x1000:6de7                     ; 1000:4d2a ; -> 1000:6de7
    JMP 0x1000:4c25                      ; 1000:4d2d ; -> 1000:4c25
    DEC byte ptr [0x9a22]                ; 1000:4d30
    MOV AX,0x1                           ; 1000:4d34
    MOV BX,0x9a31                        ; 1000:4d37
    MOV CX,0x2                           ; 1000:4d3a
    INT3                                 ; 1000:4d3d

; ------------------------------------------------------------------------------
; FUNCTION: boss_render_sprites  [Boss / Rendering]
; ADDRESS:  1000:4d41  (Length: 0x187 bytes)
; CALLERS:  FUN_1000_4f15
; PURPOSE:  Renders multi-segment animated boss monster graphics.
; ------------------------------------------------------------------------------
boss_render_sprites:
    CMP byte ptr CS:[SI + 0x4],0x0       ; 1000:4d41
    JNZ 0x1000:4d65                      ; 1000:4d46 ; -> 1000:4d65
    CMP byte ptr CS:[SI + 0x5],0x80      ; 1000:4d48
    JZ 0x1000:4d5f                       ; 1000:4d4d ; -> 1000:4d5f
    MOV AL,[0x89d]                       ; 1000:4d4f
    OR AL,byte ptr [0x8a7]               ; 1000:4d52
    OR AL,byte ptr [0x8a3]               ; 1000:4d56
    JNZ 0x1000:4d62                      ; 1000:4d5a ; -> 1000:4d62
    JMP 0x1000:4e29                      ; 1000:4d5c ; -> 1000:4e29
    CALL 0x1000:6806                     ; 1000:4d5f ; -> 1000:6806
    JMP 0x1000:5090                      ; 1000:4d62 ; -> 1000:5090
    CMP byte ptr [0x8ec],0x0             ; 1000:4d65
    JNZ 0x1000:4d62                      ; 1000:4d6a ; -> 1000:4d62
    CMP byte ptr CS:[SI + 0x5],0x1       ; 1000:4d6c
    JZ 0x1000:4dd2                       ; 1000:4d71 ; -> 1000:4dd2
    MOV word ptr CS:[SI + 0x6],0x0       ; 1000:4d73
    CMP byte ptr [0x8b1],0x6             ; 1000:4d79
    JZ 0x1000:4dd2                       ; 1000:4d7e ; -> 1000:4dd2
    MOV DI,0x8b2                         ; 1000:4d80
    CMP byte ptr [DI],0x0                ; 1000:4d83
    JZ 0x1000:4d8d                       ; 1000:4d86 ; -> 1000:4d8d
    ADD DI,0x9                           ; 1000:4d88
    JMP 0x1000:4d83                      ; 1000:4d8b ; -> 1000:4d83
    INC byte ptr [0x8b1]                 ; 1000:4d8d
    MOV byte ptr [DI],0x3                ; 1000:4d91
    MOV byte ptr [DI + 0x4],0x0          ; 1000:4d94
    MOV AX,word ptr CS:[SI]              ; 1000:4d98
    SUB AX,0x10                          ; 1000:4d9b
    SAR AX,0x1                           ; 1000:4d9e
    ADD AX,word ptr CS:[SI + 0x10]       ; 1000:4da0
    MOV word ptr [DI + 0x5],AX           ; 1000:4da4
    MOV AX,word ptr CS:[SI + 0x12]       ; 1000:4da7
    MOV word ptr [DI + 0x7],AX           ; 1000:4dab
    CMP word ptr CS:[SI + 0x2],0x10      ; 1000:4dae
    JLE 0x1000:4dd2                      ; 1000:4db3 ; -> 1000:4dd2
    SUB word ptr CS:[SI + 0x2],0x10      ; 1000:4db5
    MOV AX,word ptr CS:[SI + 0x2]        ; 1000:4dba
    ADD word ptr CS:[SI + 0x12],AX       ; 1000:4dbe
    CMP word ptr CS:[SI + 0x2],0x10      ; 1000:4dc2
    JLE 0x1000:4dcf                      ; 1000:4dc7 ; -> 1000:4dcf
    MOV word ptr CS:[SI + 0x2],0x10      ; 1000:4dc9
    JMP 0x1000:4d41                      ; 1000:4dcf ; -> 1000:4d41
    MOV AX,0x1                           ; 1000:4dd2
    MOV BX,0x9a3d                        ; 1000:4dd5
    MOV CX,0x8                           ; 1000:4dd8
    INT3                                 ; 1000:4ddb
    CMP byte ptr CS:[SI + 0x5],0x7f      ; 1000:4e29
    JZ 0x1000:4e52                       ; 1000:4e2e ; -> 1000:4e52
    CMP byte ptr CS:[SI + 0x5],0x7e      ; 1000:4e30
    JZ 0x1000:4e84                       ; 1000:4e35 ; -> 1000:4e84
    CMP byte ptr CS:[SI + 0x5],0x7d      ; 1000:4e37
    JZ 0x1000:4ead                       ; 1000:4e3c ; -> 1000:4ead
    CMP byte ptr CS:[SI + 0x5],0x7c      ; 1000:4e3e
    JZ 0x1000:4e4f                       ; 1000:4e43 ; -> 1000:4e4f
    CMP byte ptr CS:[SI + 0x5],0x7b      ; 1000:4e45
    JNZ 0x1000:4e4f                      ; 1000:4e4a ; -> 1000:4e4f
    JMP 0x1000:4ede                      ; 1000:4e4c ; -> 1000:4ede
    JMP 0x1000:5090                      ; 1000:4e4f ; -> 1000:5090
    MOV AL,[0x8a6]                       ; 1000:4e52
    OR AL,byte ptr [0x897]               ; 1000:4e55
    JNZ 0x1000:4e75                      ; 1000:4e59 ; -> 1000:4e75
    MOV byte ptr [0x897],0x1             ; 1000:4e5b
    MOV byte ptr [0x898],0x0             ; 1000:4e60
    MOV AX,word ptr CS:[SI + 0x14]       ; 1000:4e65
    MOV [0x892],AX                       ; 1000:4e69
    MOV word ptr [0x894],0xfff8          ; 1000:4e6c
    JMP 0x1000:5090                      ; 1000:4e72 ; -> 1000:5090
    MOV AX,word ptr CS:[SI + 0x14]       ; 1000:4e75
    MOV [0x892],AX                       ; 1000:4e79
    SUB word ptr [0x894],0x8             ; 1000:4e7c
    JMP 0x1000:5090                      ; 1000:4e81 ; -> 1000:5090
    MOV AL,[0x8a6]                       ; 1000:4e84
    XOR AL,0x1                           ; 1000:4e87
    MOV [0x897],AL                       ; 1000:4e89
    MOV byte ptr [0x898],0x0             ; 1000:4e8c
    MOV word ptr [0x892],0xfff8          ; 1000:4e91
    CMP word ptr CS:[SI + 0x18],0x2      ; 1000:4e97
    JZ 0x1000:4ea4                       ; 1000:4e9c ; -> 1000:4ea4
    MOV word ptr [0x892],0x8             ; 1000:4e9e
    MOV word ptr [0x894],0xfff4          ; 1000:4ea4
    JMP 0x1000:5090                      ; 1000:4eaa ; -> 1000:5090
    MOV AX,word ptr CS:[SI + 0x12]       ; 1000:4ead
    SUB AX,word ptr [0x6c8]              ; 1000:4eb1
    CMP AX,0x6                           ; 1000:4eb5
    JLE 0x1000:4edb                      ; 1000:4eb8 ; -> 1000:4edb
    MOV AL,[0x8a6]                       ; 1000:4eba
    XOR AL,0x1                           ; 1000:4ebd
    MOV [0x897],AL                       ; 1000:4ebf
    MOV byte ptr [0x898],0x0             ; 1000:4ec2
    MOV AX,word ptr CS:[SI + 0x16]       ; 1000:4ec7
    MOV [0x894],AX                       ; 1000:4ecb
    MOV word ptr [0x892],0x0             ; 1000:4ece
    MOV AX,word ptr CS:[SI + 0x10]       ; 1000:4ed4
    MOV [0x6c6],AX                       ; 1000:4ed8
    JMP 0x1000:5090                      ; 1000:4edb ; -> 1000:5090
    MOV AL,[0x8a6]                       ; 1000:4ede
    XOR AL,0x1                           ; 1000:4ee1
    MOV [0x897],AL                       ; 1000:4ee3
    MOV byte ptr [0x898],0x0             ; 1000:4ee6
    MOV word ptr [0x892],0xfff8          ; 1000:4eeb
    MOV AX,word ptr CS:[SI + 0x18]       ; 1000:4ef1
    MOV word ptr CS:[SI + 0x18],0x1      ; 1000:4ef5
    CMP AX,0x2                           ; 1000:4efb
    JZ 0x1000:4f0c                       ; 1000:4efe ; -> 1000:4f0c
    MOV word ptr CS:[SI + 0x18],0x2      ; 1000:4f00
    MOV word ptr [0x892],0x8             ; 1000:4f06
    MOV word ptr [0x894],0xfffd          ; 1000:4f0c
    JMP 0x1000:5090                      ; 1000:4f12 ; -> 1000:5090

; ------------------------------------------------------------------------------
; FUNCTION: ent_render_all  [Entities / Rendering]
; ADDRESS:  1000:4f15  (Length: 0x17b bytes)
; CALLERS:  FUN_1000_4b46, FUN_1000_4b25
; PURPOSE:  Iterates through all active monsters, items, and projectiles, rendering their sprites.
; ------------------------------------------------------------------------------
ent_render_all:
    PUSH DI                              ; 1000:4f15
    CMP byte ptr [0x921],0x0             ; 1000:4f16
    JNZ 0x1000:4f2c                      ; 1000:4f1b ; -> 1000:4f2c
    MOV AX,word ptr CS:[SI + 0x6]        ; 1000:4f1d
    DEC AX                               ; 1000:4f21
    SHL AX,0x1                           ; 1000:4f22
    MOV BX,0x4ae3                        ; 1000:4f24
    ADD BX,AX                            ; 1000:4f27
    CALL word ptr CS:[BX]                ; 1000:4f29
    CMP word ptr CS:[SI + 0x6],0x0       ; 1000:4f2c
    JNZ 0x1000:4f3c                      ; 1000:4f31 ; -> 1000:4f3c
    POP DI                               ; 1000:4f33
    DEC byte ptr [0x9a24]                ; 1000:4f34
    POP CX                               ; 1000:4f38
    JMP 0x1000:4b3b                      ; 1000:4f39 ; -> 1000:4b3b
    MOV CX,word ptr [0x6c2]              ; 1000:4f3c
    ADD CX,0xd0                          ; 1000:4f40
    MOV DX,word ptr [0x6c4]              ; 1000:4f44
    ADD DX,0xa0                          ; 1000:4f48
    MOV AX,word ptr CS:[SI + 0x10]       ; 1000:4f4c
    CMP AX,word ptr [0x6c2]              ; 1000:4f50
    JL 0x1000:4f6f                       ; 1000:4f54 ; -> 1000:4f6f
    ADD AX,word ptr CS:[SI]              ; 1000:4f56
    CMP AX,CX                            ; 1000:4f59
    JG 0x1000:4f6f                       ; 1000:4f5b ; -> 1000:4f6f
    MOV AX,word ptr CS:[SI + 0x12]       ; 1000:4f5d
    CMP AX,DX                            ; 1000:4f61
    JGE 0x1000:4f6f                      ; 1000:4f63 ; -> 1000:4f6f
    ADD AX,word ptr CS:[SI + 0x2]        ; 1000:4f65
    CMP AX,word ptr [0x6c4]              ; 1000:4f69
    JG 0x1000:4fd0                       ; 1000:4f6d ; -> 1000:4fd0
    MOV AX,word ptr CS:[SI + 0x6]        ; 1000:4f6f
    MOV word ptr CS:[SI + 0x6],0x0       ; 1000:4f73
    MOV BX,word ptr CS:[SI + 0x1e]       ; 1000:4f79
    TEST word ptr CS:[BX + 0x6],0xff00   ; 1000:4f7d
    JNZ 0x1000:4fc2                      ; 1000:4f83 ; -> 1000:4fc2
    CMP AX,0x13                          ; 1000:4f85
    JZ 0x1000:4fa3                       ; 1000:4f88 ; -> 1000:4fa3
    CMP AX,0x14                          ; 1000:4f8a
    JZ 0x1000:4fa3                       ; 1000:4f8d ; -> 1000:4fa3
    CMP AX,0x18                          ; 1000:4f8f
    JZ 0x1000:4fa3                       ; 1000:4f92 ; -> 1000:4fa3
    CMP AX,0x1b                          ; 1000:4f94
    JZ 0x1000:4fa8                       ; 1000:4f97 ; -> 1000:4fa8
    CMP AX,0x1c                          ; 1000:4f99
    JZ 0x1000:4fa8                       ; 1000:4f9c ; -> 1000:4fa8
    CMP AX,0x15                          ; 1000:4f9e
    JNZ 0x1000:4fb2                      ; 1000:4fa1 ; -> 1000:4fb2
    CALL 0x1000:4c06                     ; 1000:4fa3 ; -> 1000:4c06
    JMP 0x1000:4fc7                      ; 1000:4fa6 ; -> 1000:4fc7
    CALL 0x1000:4c06                     ; 1000:4fa8 ; -> 1000:4c06
    MOV byte ptr CS:[BX + 0x6],0x1       ; 1000:4fab
    JMP 0x1000:4fc7                      ; 1000:4fb0 ; -> 1000:4fc7
    MOV AX,word ptr CS:[SI + 0x10]       ; 1000:4fb2
    MOV word ptr CS:[BX + 0x8],AX        ; 1000:4fb6
    MOV AX,word ptr CS:[SI + 0x12]       ; 1000:4fba
    MOV word ptr CS:[BX + 0xa],AX        ; 1000:4fbe
    MOV byte ptr CS:[BX + 0x6],0x0       ; 1000:4fc2
    POP DI                               ; 1000:4fc7
    DEC byte ptr [0x9a24]                ; 1000:4fc8
    POP CX                               ; 1000:4fcc
    JMP 0x1000:4b3b                      ; 1000:4fcd ; -> 1000:4b3b
    MOV CL,byte ptr [0x21c]              ; 1000:4fd0
    OR CL,CL                             ; 1000:4fd4
    JZ 0x1000:5031                       ; 1000:4fd6 ; -> 1000:5031
    XOR CH,CH                            ; 1000:4fd8
    MOV DI,0x8ee                         ; 1000:4fda
    MOV AX,word ptr [DI]                 ; 1000:4fdd
    CMP AX,0xffff                        ; 1000:4fdf
    JZ 0x1000:502c                       ; 1000:4fe2 ; -> 1000:502c
    ADD AX,0xc                           ; 1000:4fe4
    SUB AX,word ptr CS:[SI + 0x10]       ; 1000:4fe7
    JLE 0x1000:502c                      ; 1000:4feb ; -> 1000:502c
    SUB AX,0x10                          ; 1000:4fed
    CMP AX,word ptr CS:[SI]              ; 1000:4ff0
    JGE 0x1000:502c                      ; 1000:4ff3 ; -> 1000:502c
    MOV AX,word ptr [DI + 0x2]           ; 1000:4ff5
    ADD AX,0x8                           ; 1000:4ff8
    SUB AX,word ptr CS:[SI + 0x12]       ; 1000:4ffb
    JLE 0x1000:502c                      ; 1000:4fff ; -> 1000:502c
    SUB AX,0x8                           ; 1000:5001
    CMP AX,word ptr CS:[SI + 0x2]        ; 1000:5004
    JGE 0x1000:502c                      ; 1000:5008 ; -> 1000:502c
    MOV AX,word ptr CS:[SI + 0x4]        ; 1000:500a
    TEST AX,0x8000                       ; 1000:500e
    JNZ 0x1000:502c                      ; 1000:5011 ; -> 1000:502c
    CMP AH,0x1                           ; 1000:5013
    JNZ 0x1000:501b                      ; 1000:5016 ; -> 1000:501b
    JMP 0x1000:4cc1                      ; 1000:5018 ; -> 1000:4cc1
    CMP AH,0x7c                          ; 1000:501b
    JZ 0x1000:502c                       ; 1000:501e ; -> 1000:502c
    CMP AH,0x7b                          ; 1000:5020
    JZ 0x1000:502c                       ; 1000:5023 ; -> 1000:502c
    MOV word ptr [DI],0xffff             ; 1000:5025
    JMP 0x1000:4c25                      ; 1000:5029 ; -> 1000:4c25
    ADD DI,0x8                           ; 1000:502c
    LOOP 0x1000:4fdd                     ; 1000:502f ; -> 1000:4fdd
    CMP byte ptr [0x89d],0x1             ; 1000:5031
    JZ 0x1000:5064                       ; 1000:5036 ; -> 1000:5064
    MOV AX,[0x6c6]                       ; 1000:5038
    ADD AX,0xf                           ; 1000:503b
    SUB AX,word ptr CS:[SI + 0x10]       ; 1000:503e
    JLE 0x1000:5090                      ; 1000:5042 ; -> 1000:5090
    SUB AX,0xe                           ; 1000:5044
    CMP AX,word ptr CS:[SI]              ; 1000:5047
    JGE 0x1000:5090                      ; 1000:504a ; -> 1000:5090
    MOV AX,[0x6c8]                       ; 1000:504c
    ADD AX,0x20                          ; 1000:504f
    SUB AX,word ptr CS:[SI + 0x12]       ; 1000:5052
    JLE 0x1000:5090                      ; 1000:5056 ; -> 1000:5090
    SUB AX,0x1e                          ; 1000:5058
    CMP AX,word ptr CS:[SI + 0x2]        ; 1000:505b
    JGE 0x1000:5090                      ; 1000:505f ; -> 1000:5090
    JMP 0x1000:4d41                      ; 1000:5061 ; -> 1000:4d41
    MOV AX,[0x6c6]                       ; 1000:5064
    ADD AX,0x1e                          ; 1000:5067
    SUB AX,word ptr CS:[SI + 0x10]       ; 1000:506a
    JLE 0x1000:5090                      ; 1000:506e ; -> 1000:5090
    SUB AX,0x1c                          ; 1000:5070
    CMP AX,word ptr CS:[SI]              ; 1000:5073
    JGE 0x1000:5090                      ; 1000:5076 ; -> 1000:5090
    MOV AX,[0x6c8]                       ; 1000:5078
    ADD AX,0xf                           ; 1000:507b
    SUB AX,word ptr CS:[SI + 0x12]       ; 1000:507e
    JLE 0x1000:5090                      ; 1000:5082 ; -> 1000:5090
    SUB AX,0xe                           ; 1000:5084
    CMP AX,word ptr CS:[SI + 0x2]        ; 1000:5087
    JGE 0x1000:5090                      ; 1000:508b ; -> 1000:5090
    JMP 0x1000:4d41                      ; 1000:508d ; -> 1000:4d41

; ------------------------------------------------------------------------------
; FUNCTION: spr_render_water_ripple  [Tilemap / Animation]
; ADDRESS:  1000:5090  (Length: 0x56 bytes)
; CALLERS:  FUN_1000_4d41
; PURPOSE:  Renders animated surface ripple effect over water tiles.
; ------------------------------------------------------------------------------
spr_render_water_ripple:
    MOV AX,word ptr CS:[SI + 0x1a]       ; 1000:5090
    INC AX                               ; 1000:5094
    MOV BX,word ptr CS:[SI + 0x14]       ; 1000:5095
    OR BX,word ptr CS:[SI + 0x16]        ; 1000:5099
    JZ 0x1000:50b0                       ; 1000:509d ; -> 1000:50b0
    CMP AX,word ptr CS:[SI + 0xa]        ; 1000:509f
    JL 0x1000:50a7                       ; 1000:50a3 ; -> 1000:50a7
    XOR AX,AX                            ; 1000:50a5
    MOV CX,word ptr CS:[SI + 0x8]        ; 1000:50a7
    ADD CX,AX                            ; 1000:50ab
    JMP 0x1000:50ba                      ; 1000:50ad ; -> 1000:50ba
    CMP AX,word ptr CS:[SI + 0x8]        ; 1000:50b0
    JL 0x1000:50b8                       ; 1000:50b4 ; -> 1000:50b8
    XOR AX,AX                            ; 1000:50b6
    MOV CX,AX                            ; 1000:50b8
    MOV word ptr CS:[SI + 0x1a],AX       ; 1000:50ba
    MOV BX,word ptr CS:[SI + 0xc]        ; 1000:50be
    CMP word ptr CS:[SI + 0x18],0x2      ; 1000:50c2
    JZ 0x1000:50cd                       ; 1000:50c7 ; -> 1000:50cd
    MOV BX,word ptr CS:[SI + 0xe]        ; 1000:50c9
    SHL CX,0x1                           ; 1000:50cd
    ADD BX,CX                            ; 1000:50cf
    MOV DX,word ptr CS:[BX]              ; 1000:50d1
    MOV AX,word ptr CS:[SI + 0x10]       ; 1000:50d4
    MOV BX,word ptr CS:[SI + 0x12]       ; 1000:50d8
    PUSH SI                              ; 1000:50dc
    MOV SI,DX                            ; 1000:50dd
    CALL 0x1000:7dbb                     ; 1000:50df ; -> 1000:7dbb
    POP SI                               ; 1000:50e2
    POP DI                               ; 1000:50e3
    JMP 0x1000:4b46                      ; 1000:50e4 ; -> 1000:4b46

; ------------------------------------------------------------------------------
; FUNCTION: map_update_animated_tiles  [Tilemap / Animation]
; ADDRESS:  1000:5ceb  (Length: 0x71 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Advances animation frame counters for animated room tiles (water, lava, torches).
; ------------------------------------------------------------------------------
map_update_animated_tiles:
    MOV CL,byte ptr [0x21c]              ; 1000:5ceb
    XOR CH,CH                            ; 1000:5cef
    LEA SI,[0x8ee]                       ; 1000:5cf1
    CMP word ptr [SI],-0x1               ; 1000:5cf5
    JNZ 0x1000:5d58                      ; 1000:5cf8 ; -> 1000:5d58
    MOV AX,[0x6c6]                       ; 1000:5cfa
    ADD AX,0x4                           ; 1000:5cfd
    MOV word ptr [SI],AX                 ; 1000:5d00
    MOV AX,0xfff0                        ; 1000:5d02
    CMP byte ptr [0x8ad],0x2             ; 1000:5d05
    JZ 0x1000:5d18                       ; 1000:5d0a ; -> 1000:5d18
    NEG AX                               ; 1000:5d0c
    CMP byte ptr [0x89d],0x1             ; 1000:5d0e
    JNZ 0x1000:5d18                      ; 1000:5d13 ; -> 1000:5d18
    ADD word ptr [SI],0x10               ; 1000:5d15
    MOV word ptr [SI + 0x4],AX           ; 1000:5d18
    MOV AX,[0x6c8]                       ; 1000:5d1b
    ADD AX,0x4                           ; 1000:5d1e
    CMP byte ptr [0x89d],0x1             ; 1000:5d21
    JZ 0x1000:5d2b                       ; 1000:5d26 ; -> 1000:5d2b
    ADD AX,0x4                           ; 1000:5d28
    MOV word ptr [SI + 0x2],AX           ; 1000:5d2b
    MOV AX,[0x9a9c]                      ; 1000:5d2e
    MOV word ptr [SI + 0x6],AX           ; 1000:5d31
    XOR word ptr [0x9a9c],0x4            ; 1000:5d34
    SUB word ptr [SI + 0x6],0xc          ; 1000:5d39
    CMP word ptr CS:[0x127c],-0x1        ; 1000:5d3d ; -> 1000:127c
    JZ 0x1000:5d4a                       ; 1000:5d43 ; -> 1000:5d4a
    MOV word ptr [SI + 0x6],0xfff4       ; 1000:5d45
    MOV AX,0x1                           ; 1000:5d4a
    LEA BX,[0x9a80]                      ; 1000:5d4d
    MOV CX,0x1                           ; 1000:5d51
    INT3                                 ; 1000:5d54
    ADD SI,0x8                           ; 1000:5d58
    LOOP 0x1000:5cf5                     ; 1000:5d5b ; -> 1000:5cf5
    CLC                                  ; 1000:5d5d
    RET                                  ; 1000:5d5e

; ------------------------------------------------------------------------------
; FUNCTION: map_redraw_dirty_tiles  [Tilemap / Rendering]
; ADDRESS:  1000:5d5f  (Length: 0xf3 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Redraws tiles marked dirty to restore background behind moving sprites.
; ------------------------------------------------------------------------------
map_redraw_dirty_tiles:
    MOV CL,byte ptr [0x21c]              ; 1000:5d5f
    OR CL,CL                             ; 1000:5d63
    JNZ 0x1000:5d68                      ; 1000:5d65 ; -> 1000:5d68
    RET                                  ; 1000:5d67
    XOR CH,CH                            ; 1000:5d68
    LEA SI,[0x8ee]                       ; 1000:5d6a
    PUSH CX                              ; 1000:5d6e
    CMP word ptr [SI],-0x1               ; 1000:5d6f ; -> 34fe:08f6
    JZ 0x1000:5df3                       ; 1000:5d72 ; -> 1000:5df3
    MOV AX,word ptr [SI + 0x4]           ; 1000:5d74 ; -> 34fe:08fa
    ADD word ptr [SI],AX                 ; 1000:5d77 ; -> 34fe:08f6
    CMP word ptr CS:[0x1018],-0x1        ; 1000:5d79 ; -> 1000:1018
    JNZ 0x1000:5da7                      ; 1000:5d7f ; -> 1000:5da7
    MOV BX,word ptr [SI + 0x6]           ; 1000:5d81 ; -> 34fe:08fc
    CMP BX,0x0                           ; 1000:5d84
    JGE 0x1000:5d8f                      ; 1000:5d87 ; -> 1000:5d8f
    ADD word ptr [SI + 0x6],0xc          ; 1000:5d89 ; -> 34fe:08fc
    JMP 0x1000:5da7                      ; 1000:5d8d ; -> 1000:5da7
    MOV CX,BX                            ; 1000:5d8f
    SHL BX,0x1                           ; 1000:5d91
    ADD BX,0x9a8c                        ; 1000:5d93
    MOV BX,word ptr [BX]                 ; 1000:5d97
    ADD word ptr [SI + 0x2],BX           ; 1000:5d99 ; -> 34fe:08f8
    INC CX                               ; 1000:5d9c
    CMP CX,0x8                           ; 1000:5d9d
    JNZ 0x1000:5da4                      ; 1000:5da0 ; -> 1000:5da4
    XOR CX,CX                            ; 1000:5da2
    MOV word ptr [SI + 0x6],CX           ; 1000:5da4 ; -> 34fe:08fc
    MOV AX,word ptr [SI]                 ; 1000:5da7 ; -> 34fe:08f6
    MOV BX,word ptr [SI + 0x2]           ; 1000:5da9 ; -> 34fe:08f8
    ADD AX,0x3                           ; 1000:5dac
    ADD BX,0x3                           ; 1000:5daf
    CALL 0x1000:1cfe                     ; 1000:5db2 ; -> 1000:1cfe
    MOV CX,AX                            ; 1000:5db5
    MOV DX,BX                            ; 1000:5db7
    CMP AX,word ptr [0x292]              ; 1000:5db9
    JLE 0x1000:5e04                      ; 1000:5dbd ; -> 1000:5e04
    MOV AX,word ptr [SI]                 ; 1000:5dbf
    SUB AX,word ptr [0x6c2]              ; 1000:5dc1
    JL 0x1000:5dfd                       ; 1000:5dc5 ; -> 1000:5dfd
    CMP AX,0xc8                          ; 1000:5dc7
    JG 0x1000:5dfd                       ; 1000:5dca ; -> 1000:5dfd
    MOV BX,word ptr [SI + 0x2]           ; 1000:5dcc
    SUB BX,word ptr [0x6c4]              ; 1000:5dcf
    JL 0x1000:5dfd                       ; 1000:5dd3 ; -> 1000:5dfd
    CMP BX,0x98                          ; 1000:5dd5
    JG 0x1000:5dfd                       ; 1000:5dd9 ; -> 1000:5dfd
    ADD AX,0x8                           ; 1000:5ddb
    ADD BX,0x8                           ; 1000:5dde
    PUSH SI                              ; 1000:5de1
    MOV SI,0xa4a6                        ; 1000:5de2
    MOV DS,word ptr CS:[0x5e]            ; 1000:5de5 ; -> 1000:005e
    CALL 0x1000:77a3                     ; 1000:5dea ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:5ded ; -> 1000:005c
    POP SI                               ; 1000:5df2
    POP CX                               ; 1000:5df3
    ADD SI,0x8                           ; 1000:5df4
    DEC CX                               ; 1000:5df7
    JZ 0x1000:5e03                       ; 1000:5df8 ; -> 1000:5e03
    JMP 0x1000:5d6e                      ; 1000:5dfa ; -> 1000:5d6e
    MOV word ptr [SI],0xffff             ; 1000:5dfd ; -> 34fe:08ee
    JMP 0x1000:5df3                      ; 1000:5e01 ; -> 1000:5df3
    RET                                  ; 1000:5e03
    MOV AX,word ptr [SI]                 ; 1000:5e04
    SUB AX,word ptr [0x6c2]              ; 1000:5e06
    JL 0x1000:5dfd                       ; 1000:5e0a ; -> 1000:5dfd
    CMP AX,0xc8                          ; 1000:5e0c
    JG 0x1000:5dfd                       ; 1000:5e0f ; -> 1000:5dfd
    MOV BX,word ptr [SI + 0x2]           ; 1000:5e11
    SUB BX,word ptr [0x6c4]              ; 1000:5e14
    JL 0x1000:5dfd                       ; 1000:5e18 ; -> 1000:5dfd
    CMP BX,0x98                          ; 1000:5e1a
    JG 0x1000:5dfd                       ; 1000:5e1e ; -> 1000:5dfd
    PUSH AX                              ; 1000:5e20
    PUSH BX                              ; 1000:5e21
    PUSH SI                              ; 1000:5e22
    MOV AX,CX                            ; 1000:5e23
    MOV BX,DX                            ; 1000:5e25
    MOV CX,word ptr [SI]                 ; 1000:5e27
    MOV DX,word ptr [SI + 0x2]           ; 1000:5e29
    ADD CX,0x3                           ; 1000:5e2c
    ADD DX,0x3                           ; 1000:5e2f
    CALL 0x1000:2d06                     ; 1000:5e32 ; -> 1000:2d06
    POP SI                               ; 1000:5e35
    POP BX                               ; 1000:5e36
    POP AX                               ; 1000:5e37
    ADD AX,0x8                           ; 1000:5e38
    ADD BX,0x8                           ; 1000:5e3b
    PUSH SI                              ; 1000:5e3e
    MOV SI,0xa4d4                        ; 1000:5e3f
    MOV DS,word ptr CS:[0x5e]            ; 1000:5e42 ; -> 1000:005e
    CALL 0x1000:77a3                     ; 1000:5e47 ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:5e4a ; -> 1000:005c
    POP SI                               ; 1000:5e4f
    JMP 0x1000:5dfd                      ; 1000:5e50 ; -> 1000:5dfd

; ------------------------------------------------------------------------------
; FUNCTION: sys_main_dispatcher  [System / Dispatcher]
; ADDRESS:  1000:5e52  (Length: 0x32 bytes)
; CALLERS:  entry
; PURPOSE:  Top-level game dispatcher. Initializes PRNG from DOS time, loads configuration, checks high scores, and runs main menu loop.
; ------------------------------------------------------------------------------
sys_main_dispatcher:
    MOV AH,0x2c                          ; 1000:5e52
    INT 0x21                             ; 1000:5e54
    MOV DL,CL                            ; 1000:5e56
    MOV word ptr [0xaba9],DX             ; 1000:5e58
    CALL 0x1000:6545                     ; 1000:5e5c ; -> 1000:6545
    MOV DX,0x9c73                        ; 1000:5e5f
    CALL 0x1000:633c                     ; 1000:5e62 ; -> 1000:633c
    MOV word ptr [0x9c8c],0xffff         ; 1000:5e65
    JMP 0x1000:5e73                      ; 1000:5e6b ; -> 1000:5e73
    CALL 0x1000:62a4                     ; 1000:5e73 ; -> 1000:62a4
    CALL 0x1000:6df4                     ; 1000:5e76 ; -> 1000:6df4
    OR AX,AX                             ; 1000:5e79
    JZ 0x1000:5e80                       ; 1000:5e7b ; -> 1000:5e80
    CALL 0x1000:631a                     ; 1000:5e7d ; -> 1000:631a
    MOV AX,0x1                           ; 1000:5e80
    MOV BX,0x9d3f                        ; 1000:5e83
    MOV CX,0x1                           ; 1000:5e86
    INT3                                 ; 1000:5e89

; ------------------------------------------------------------------------------
; FUNCTION: input_poll_devices  [Input / Polling]
; ADDRESS:  1000:61c8  (Length: 0xdb bytes)
; PURPOSE:  Polls keyboard buffer and joystick states to update the current input vector.
; ------------------------------------------------------------------------------
input_poll_devices:
    PUSH BP                              ; 1000:61c8
    MOV BP,SP                            ; 1000:61c9
    SUB SP,0x8                           ; 1000:61cb
    PUSH ES                              ; 1000:61ce
    XOR CX,CX                            ; 1000:61cf
    MOV ES,CX                            ; 1000:61d1
    MOV CL,byte ptr ES:[0x41a]           ; 1000:61d3
    MOV byte ptr ES:[0x41c],CL           ; 1000:61d8
    POP ES                               ; 1000:61dd
    MOV byte ptr CS:[0x60],0x1           ; 1000:61de ; -> 1000:0060
    MOV word ptr [BP + -0x2],0x48        ; 1000:61e4 ; -> Stack[-0x4]
    MOV CX,word ptr [0x9d3d]             ; 1000:61e9
    SHL CX,0x1                           ; 1000:61ed
    SHL CX,0x1                           ; 1000:61ef
    SHL CX,0x1                           ; 1000:61f1
    SHL CX,0x1                           ; 1000:61f3
    ADD CX,0x40                          ; 1000:61f5
    MOV word ptr [BP + -0x4],CX          ; 1000:61f8 ; -> Stack[-0x6]
    MOV SI,word ptr [0x9d3d]             ; 1000:61fb
    SHL SI,0x1                           ; 1000:61ff
    ADD SI,word ptr [0x9d3b]             ; 1000:6201
    MOV SI,word ptr [SI]                 ; 1000:6205
    MOV word ptr [BP + -0x6],SI          ; 1000:6207 ; -> Stack[-0x8]
    MOV word ptr [BP + -0x8],0x0         ; 1000:620a ; -> Stack[-0xa]
    MOV SI,word ptr [BP + -0x6]          ; 1000:620f ; -> Stack[-0x8]
    ADD SI,word ptr [BP + -0x8]          ; 1000:6212 ; -> Stack[-0xa]
    MOV byte ptr [SI],0x0                ; 1000:6215
    MOV AX,word ptr [BP + -0x2]          ; 1000:6218 ; -> Stack[-0x4]
    MOV BX,word ptr [BP + -0x4]          ; 1000:621b ; -> Stack[-0x6]
    MOV CX,0xa0                          ; 1000:621e
    MOV DX,0x8                           ; 1000:6221
    MOV DI,word ptr CS:[0x773f]          ; 1000:6224 ; -> 1000:773f
    CALL 0x1000:7ac8                     ; 1000:6229 ; -> 1000:7ac8
    MOV AX,0x7                           ; 1000:622c
    CALL 0x1000:4339                     ; 1000:622f ; -> 1000:4339
    MOV AX,word ptr [BP + -0x2]          ; 1000:6232 ; -> Stack[-0x4]
    MOV BX,word ptr [BP + -0x4]          ; 1000:6235 ; -> Stack[-0x6]
    MOV SI,word ptr [BP + -0x6]          ; 1000:6238 ; -> Stack[-0x8]
    CALL 0x1000:42d4                     ; 1000:623b ; -> 1000:42d4
    MOV CX,BX                            ; 1000:623e
    MOV BX,AX                            ; 1000:6240
    MOV AL,0x2b                          ; 1000:6242
    CALL 0x1000:42e8                     ; 1000:6244 ; -> 1000:42e8
    CALL 0x1000:774e                     ; 1000:6247 ; -> 1000:774e
    XOR AX,AX                            ; 1000:624a
    INT 0x16                             ; 1000:624c
    OR AL,AL                             ; 1000:624e
    JZ 0x1000:620f                       ; 1000:6250 ; -> 1000:620f
    CMP AL,0x1b                          ; 1000:6252
    JNZ 0x1000:6260                      ; 1000:6254 ; -> 1000:6260
    MOV SI,word ptr [BP + -0x6]          ; 1000:6256 ; -> Stack[-0x8]
    MOV byte ptr [SI],0x0                ; 1000:6259
    STC                                  ; 1000:625c
    JMP 0x1000:629a                      ; 1000:625d ; -> 1000:629a
    CMP AL,0x8                           ; 1000:6260
    JZ 0x1000:628d                       ; 1000:6262 ; -> 1000:628d
    CMP AL,0xd                           ; 1000:6264
    JZ 0x1000:6299                       ; 1000:6266 ; -> 1000:6299
    CMP AL,0x61                          ; 1000:6268
    JL 0x1000:626e                       ; 1000:626a ; -> 1000:626e
    SUB AL,0x20                          ; 1000:626c
    CMP AL,0x20                          ; 1000:626e
    JZ 0x1000:627a                       ; 1000:6270 ; -> 1000:627a
    CMP AL,0x30                          ; 1000:6272
    JL 0x1000:620f                       ; 1000:6274 ; -> 1000:620f
    CMP AL,0x5a                          ; 1000:6276
    JG 0x1000:620f                       ; 1000:6278 ; -> 1000:620f
    CMP word ptr [BP + -0x8],0xf         ; 1000:627a ; -> Stack[-0xa]
    JZ 0x1000:620f                       ; 1000:627e ; -> 1000:620f
    MOV SI,word ptr [BP + -0x6]          ; 1000:6280 ; -> Stack[-0x8]
    ADD SI,word ptr [BP + -0x8]          ; 1000:6283 ; -> Stack[-0xa]
    MOV byte ptr [SI],AL                 ; 1000:6286
    INC word ptr [BP + -0x8]             ; 1000:6288 ; -> Stack[-0xa]
    JMP 0x1000:620f                      ; 1000:628b ; -> 1000:620f
    CMP word ptr [BP + -0x8],0x0         ; 1000:628d ; -> Stack[-0xa]
    JZ 0x1000:6296                       ; 1000:6291 ; -> 1000:6296
    DEC word ptr [BP + -0x8]             ; 1000:6293 ; -> Stack[-0xa]
    JMP 0x1000:620f                      ; 1000:6296 ; -> 1000:620f
    CLC                                  ; 1000:6299
    MOV byte ptr CS:[0x60],0x0           ; 1000:629a ; -> 1000:0060
    MOV SP,BP                            ; 1000:62a0
    POP BP                               ; 1000:62a2
    RET                                  ; 1000:62a3

; ------------------------------------------------------------------------------
; FUNCTION: file_load_keys_def  [File I/O / Config]
; ADDRESS:  1000:62a4  (Length: 0x75 bytes)
; CALLERS:  FUN_1000_5e52
; PURPOSE:  Loads custom key configuration bindings from 'KEYS.DEF' file.
; ------------------------------------------------------------------------------
file_load_keys_def:
    MOV DX,0x9c7c                        ; 1000:62a4
    MOV AX,0x3d00                        ; 1000:62a7
    INT 0x21                             ; 1000:62aa
    JNC 0x1000:62b1                      ; 1000:62ac ; -> 1000:62b1
    JMP 0x1000:62c4                      ; 1000:62ae ; -> 1000:62c4
    MOV BX,AX                            ; 1000:62b1
    MOV DX,0x9c94                        ; 1000:62b3
    MOV CX,0x66                          ; 1000:62b6
    MOV AX,0x3f00                        ; 1000:62b9
    INT 0x21                             ; 1000:62bc
    MOV AX,0x3e00                        ; 1000:62be
    INT 0x21                             ; 1000:62c1
    RET                                  ; 1000:62c3
    MOV word ptr [0x9c94],0x0            ; 1000:62c4
    MOV byte ptr [0x9c96],0x0            ; 1000:62ca
    MOV byte ptr [0x9ca6],0x0            ; 1000:62cf
    MOV byte ptr [0x9cb6],0x0            ; 1000:62d4
    MOV byte ptr [0x9cc6],0x0            ; 1000:62d9
    MOV byte ptr [0x9cd6],0x0            ; 1000:62de
    MOV word ptr [0x9ce8],0x0            ; 1000:62e3
    MOV word ptr [0x9cea],0x0            ; 1000:62e9
    MOV word ptr [0x9cec],0x0            ; 1000:62ef
    MOV word ptr [0x9cf8],0x0            ; 1000:62f5
    MOV word ptr [0x9cee],0x0            ; 1000:62fb
    MOV word ptr [0x9cf0],0x0            ; 1000:6301
    MOV word ptr [0x9cf2],0x0            ; 1000:6307
    MOV word ptr [0x9cf4],0x0            ; 1000:630d
    MOV word ptr [0x9cf6],0x0            ; 1000:6313
    RET                                  ; 1000:6319

; ------------------------------------------------------------------------------
; FUNCTION: file_save_high_scores  [File I/O / High Scores]
; ADDRESS:  1000:631a  (Length: 0x22 bytes)
; CALLERS:  FUN_1000_5e52
; PURPOSE:  Saves high score table entries to disk.
; ------------------------------------------------------------------------------
file_save_high_scores:
    MOV DX,0x9c7c                        ; 1000:631a
    XOR CX,CX                            ; 1000:631d
    MOV AX,0x3c00                        ; 1000:631f
    INT 0x21                             ; 1000:6322
    JNC 0x1000:6329                      ; 1000:6324 ; -> 1000:6329
    JMP 0x1000:0202                      ; 1000:6326 ; -> 1000:0202
    MOV BX,AX                            ; 1000:6329
    MOV DX,0x9c94                        ; 1000:632b
    MOV CX,0x66                          ; 1000:632e
    MOV AX,0x4000                        ; 1000:6331
    INT 0x21                             ; 1000:6334
    MOV AX,0x3e00                        ; 1000:6336
    INT 0x21                             ; 1000:6339
    RET                                  ; 1000:633b

; ------------------------------------------------------------------------------
; FUNCTION: file_load_high_scores  [File I/O / High Scores]
; ADDRESS:  1000:633c  (Length: 0x33 bytes)
; CALLERS:  FUN_1000_5e52
; PURPOSE:  Loads high score table entries from disk.
; ------------------------------------------------------------------------------
file_load_high_scores:
    XOR CX,CX                            ; 1000:633c
    MOV AX,0x3c00                        ; 1000:633e
    INT 0x21                             ; 1000:6341
    JNC 0x1000:6348                      ; 1000:6343 ; -> 1000:6348
    JMP 0x1000:0202                      ; 1000:6345 ; -> 1000:0202
    MOV BX,AX                            ; 1000:6348
    PUSH DS                              ; 1000:634a
    MOV DX,0xaf0                         ; 1000:634b
    MOV CX,word ptr [0x942]              ; 1000:634e
    MOV AX,CS                            ; 1000:6352
    MOV DS,AX                            ; 1000:6354
    MOV AX,0x4000                        ; 1000:6356
    INT 0x21                             ; 1000:6359
    POP DS                               ; 1000:635b
    MOV CX,0x28f                         ; 1000:635c
    MOV DX,0x210                         ; 1000:635f
    SUB CX,DX                            ; 1000:6362
    MOV AX,0x4000                        ; 1000:6364
    INT 0x21                             ; 1000:6367
    MOV AX,0x3e00                        ; 1000:6369
    INT 0x21                             ; 1000:636c
    RET                                  ; 1000:636e

; ------------------------------------------------------------------------------
; FUNCTION: sys_generate_system_hash  [System / Security]
; ADDRESS:  1000:6545  (Length: 0xaa bytes)
; CALLERS:  FUN_1000_5e52
; PURPOSE:  Hashes VGA/System BIOS ROM bytes to generate system signature for copy protection and seed PRNG.
; ------------------------------------------------------------------------------
sys_generate_system_hash:
    PUSH ES                              ; 1000:6545
    MOV AX,[0xaba9]                      ; 1000:6546
    MOV [0x9ce6],AX                      ; 1000:6549
    MOV AX,0xc000                        ; 1000:654c
    MOV ES,AX                            ; 1000:654f
    XOR BX,BX                            ; 1000:6551
    XOR DX,DX                            ; 1000:6553
    MOV CX,0x40                          ; 1000:6555
    ADD DX,word ptr ES:[BX]              ; 1000:6558
    ADD BX,0x2                           ; 1000:655b
    LOOP 0x1000:6558                     ; 1000:655e ; -> 1000:6558
    MOV word ptr [0xabab],DX             ; 1000:6560
    MOV BX,0xf000                        ; 1000:6564
    MOV ES,BX                            ; 1000:6567
    XOR DX,DX                            ; 1000:6569
    MOV CX,0x40                          ; 1000:656b
    ADD DX,word ptr ES:[BX]              ; 1000:656e ; -> f000:f000
    ADD BX,0x2                           ; 1000:6571
    LOOP 0x1000:656e                     ; 1000:6574 ; -> 1000:656e
    MOV word ptr [0xabad],DX             ; 1000:6576
    MOV BX,0xff80                        ; 1000:657a
    XOR DX,DX                            ; 1000:657d
    MOV CX,0x40                          ; 1000:657f
    ADD DX,word ptr ES:[BX]              ; 1000:6582 ; -> f000:ff80
    ADD BX,0x2                           ; 1000:6585
    LOOP 0x1000:6582                     ; 1000:6588 ; -> 1000:6582
    MOV word ptr [0xabaf],DX             ; 1000:658a
    POP ES                               ; 1000:658e
    MOV BX,word ptr [0xaba9]             ; 1000:658f
    MOV AX,[0xabab]                      ; 1000:6593
    MOV CX,word ptr [0xabad]             ; 1000:6596
    MOV DX,word ptr [0xabaf]             ; 1000:659a
    ADD BX,AX                            ; 1000:659e
    ADD BX,CX                            ; 1000:65a0
    ADD BX,DX                            ; 1000:65a2
    MOV AX,BX                            ; 1000:65a4
    MOV CL,0x5                           ; 1000:65a6
    SHR AX,CL                            ; 1000:65a8
    ADD BX,AX                            ; 1000:65aa
    AND BX,0x7ff                         ; 1000:65ac
    PUSH BX                              ; 1000:65b0
    MOV AX,BX                            ; 1000:65b1
    MOV CL,0x4                           ; 1000:65b3
    SHR BX,CL                            ; 1000:65b5
    AND AX,0xf                           ; 1000:65b7
    MOV [0xabb1],AX                      ; 1000:65ba
    MOV word ptr [0xabb3],BX             ; 1000:65bd
    POP BX                               ; 1000:65c1
    ADD BX,0x1234                        ; 1000:65c2
    MOV AX,BX                            ; 1000:65c6
    MOV CX,BX                            ; 1000:65c8
    MOV DX,BX                            ; 1000:65ca
    SHL BX,0x1                           ; 1000:65cc
    SHL BX,0x1                           ; 1000:65ce
    SHL BX,0x1                           ; 1000:65d0
    SHL BX,0x1                           ; 1000:65d2
    SHL AX,0x1                           ; 1000:65d4
    SHL AX,0x1                           ; 1000:65d6
    SHL AX,0x1                           ; 1000:65d8
    SHL CX,0x1                           ; 1000:65da
    ADD BX,AX                            ; 1000:65dc
    ADD BX,CX                            ; 1000:65de
    ADD BX,DX                            ; 1000:65e0
    TEST BX,0x40                         ; 1000:65e2
    JZ 0x1000:65ea                       ; 1000:65e6 ; -> 1000:65ea
    NEG BX                               ; 1000:65e8
    MOV word ptr [0xabbb],BX             ; 1000:65ea
    RET                                  ; 1000:65ee

; ------------------------------------------------------------------------------
; FUNCTION: file_check_config_exists  [File I/O / Config]
; ADDRESS:  1000:65ef  (Length: 0xe bytes)
; CALLERS:  FUN_1000_21f2
; PURPOSE:  Checks if configuration file 'FR.CFG' exists on disk.
; ------------------------------------------------------------------------------
file_check_config_exists:
    PUSH SI                              ; 1000:65ef
    PUSH DI                              ; 1000:65f0
    MOV SI,0x9bdf                        ; 1000:65f1
    MOV DI,0x9bd3                        ; 1000:65f4
    CALL 0x1000:660b                     ; 1000:65f7 ; -> 1000:660b
    POP DI                               ; 1000:65fa
    POP SI                               ; 1000:65fb
    RET                                  ; 1000:65fc

; ------------------------------------------------------------------------------
; FUNCTION: file_save_config  [File I/O / Config]
; ADDRESS:  1000:660b  (Length: 0x29 bytes)
; CALLERS:  FUN_1000_65ef
; PURPOSE:  Writes audio, input, and video configuration settings to 'FR.CFG'.
; ------------------------------------------------------------------------------
file_save_config:
    MOV DX,DI                            ; 1000:660b
    MOV AX,0x3d00                        ; 1000:660d
    INT 0x21                             ; 1000:6610
    JC 0x1000:661c                       ; 1000:6612 ; -> 1000:661c
    MOV BX,AX                            ; 1000:6614
    MOV AX,0x3e00                        ; 1000:6616
    INT 0x21                             ; 1000:6619
    RET                                  ; 1000:661b
    PUSH SI                              ; 1000:661c
    PUSH DI                              ; 1000:661d
    CALL 0x1000:4206                     ; 1000:661e ; -> 1000:4206
    CALL 0x1000:774e                     ; 1000:6621 ; -> 1000:774e
    POP DI                               ; 1000:6624
    POP SI                               ; 1000:6625
    CALL 0x1000:0599                     ; 1000:6626 ; -> 1000:0599
    CMP byte ptr CS:[0x61],0x1           ; 1000:6629 ; -> 1000:0061
    JNZ 0x1000:660b                      ; 1000:662f ; -> 1000:660b
    JMP 0x1000:0215                      ; 1000:6631 ; -> 1000:0215

; ------------------------------------------------------------------------------
; FUNCTION: game_save_load_menu  [Game / Saves]
; ADDRESS:  1000:6668  (Length: 0x112 bytes)
; CALLERS:  FUN_1000_2341
; PURPOSE:  Save/Load game slot interface. Handles saving and loading games to 'GAME.000' through 'GAME.003'.
; ------------------------------------------------------------------------------
game_save_load_menu:
    MOV DX,0x9ea8                        ; 1000:6668
    MOV AX,0x3d00                        ; 1000:666b
    INT 0x21                             ; 1000:666e
    JNC 0x1000:6675                      ; 1000:6670 ; -> 1000:6675
    JMP 0x1000:0202                      ; 1000:6672 ; -> 1000:0202
    MOV BX,AX                            ; 1000:6675
    MOV DX,0x9a8                         ; 1000:6677
    MOV CX,0x7fff                        ; 1000:667a
    MOV AX,0x3f00                        ; 1000:667d
    INT 0x21                             ; 1000:6680
    MOV AX,0x3e00                        ; 1000:6682
    INT 0x21                             ; 1000:6685
    MOV AX,0x8                           ; 1000:6687
    MOV BX,0x8                           ; 1000:668a
    MOV SI,0x9a8                         ; 1000:668d
    CALL 0x1000:790c                     ; 1000:6690 ; -> 1000:790c
    MOV DX,0x9eb1                        ; 1000:6693
    MOV AX,0x3d00                        ; 1000:6696
    INT 0x21                             ; 1000:6699
    JNC 0x1000:66a0                      ; 1000:669b ; -> 1000:66a0
    JMP 0x1000:0202                      ; 1000:669d ; -> 1000:0202
    MOV BX,AX                            ; 1000:66a0
    MOV DX,0x9a8                         ; 1000:66a2
    MOV CX,0x7fff                        ; 1000:66a5
    MOV AX,0x3f00                        ; 1000:66a8
    INT 0x21                             ; 1000:66ab
    MOV AX,0x3e00                        ; 1000:66ad
    INT 0x21                             ; 1000:66b0
    MOV BX,0x6634                        ; 1000:66b2
    CLI                                  ; 1000:66b5
    MOV word ptr CS:[0x278],BX           ; 1000:66b6 ; -> 1000:0278
    MOV word ptr CS:[BX + 0x2],0x0       ; 1000:66bb ; -> 1000:6636
    MOV word ptr CS:[BX + 0x4],0x0       ; 1000:66c1 ; -> 1000:6638
    MOV word ptr CS:[BX + 0xa],0x0       ; 1000:66c7 ; -> 1000:663e
    MOV word ptr CS:[BX + 0xc],0x0       ; 1000:66cd ; -> 1000:6640
    MOV word ptr CS:[BX + 0x12],0x0      ; 1000:66d3 ; -> 1000:6646
    MOV word ptr CS:[BX + 0x14],0x0      ; 1000:66d9 ; -> 1000:6648
    STI                                  ; 1000:66df
    CALL 0x1000:677a                     ; 1000:66e0 ; -> 1000:677a
    CALL 0x1000:0599                     ; 1000:66e3 ; -> 1000:0599
    CMP byte ptr CS:[0x25a],0x1          ; 1000:66e6 ; -> 1000:025a
    JZ 0x1000:6729                       ; 1000:66ec ; -> 1000:6729
    CMP byte ptr CS:[0x25b],0x1          ; 1000:66ee ; -> 1000:025b
    JZ 0x1000:6737                       ; 1000:66f4 ; -> 1000:6737
    CMP byte ptr CS:[0x258],0x1          ; 1000:66f6 ; -> 1000:0258
    JZ 0x1000:6745                       ; 1000:66fc ; -> 1000:6745
    CMP byte ptr CS:[0x259],0x1          ; 1000:66fe ; -> 1000:0259
    JZ 0x1000:6759                       ; 1000:6704 ; -> 1000:6759
    CMP byte ptr CS:[0x25b],0x1          ; 1000:6706 ; -> 1000:025b
    MOV AL,CS:[0x25c]                    ; 1000:670c ; -> 1000:025c
    OR AL,byte ptr CS:[0x25d]            ; 1000:6710 ; -> 1000:025d
    JNZ 0x1000:6771                      ; 1000:6715 ; -> 1000:6771
    CMP byte ptr CS:[0x61],0x1           ; 1000:6717 ; -> 1000:0061
    JZ 0x1000:6771                       ; 1000:671d ; -> 1000:6771
    CMP byte ptr CS:[0x61],0x1c          ; 1000:671f ; -> 1000:0061
    JZ 0x1000:6771                       ; 1000:6725 ; -> 1000:6771
    JMP 0x1000:66e3                      ; 1000:6727 ; -> 1000:66e3
    CMP word ptr [0x21a],0x2             ; 1000:6729
    JLE 0x1000:66e3                      ; 1000:672e ; -> 1000:66e3
    SUB word ptr [0x21a],0x3             ; 1000:6730
    JMP 0x1000:66e0                      ; 1000:6735 ; -> 1000:66e0
    CMP word ptr [0x21a],0x3             ; 1000:6737
    JGE 0x1000:66e3                      ; 1000:673c ; -> 1000:66e3
    ADD word ptr [0x21a],0x3             ; 1000:673e
    JMP 0x1000:66e0                      ; 1000:6743 ; -> 1000:66e0
    CMP word ptr [0x21a],0x0             ; 1000:6745
    JZ 0x1000:66e3                       ; 1000:674a ; -> 1000:66e3
    CMP word ptr [0x21a],0x3             ; 1000:674c
    JZ 0x1000:66e3                       ; 1000:6751 ; -> 1000:66e3
    DEC word ptr [0x21a]                 ; 1000:6753
    JMP 0x1000:66e0                      ; 1000:6757 ; -> 1000:66e0
    CMP word ptr [0x21a],0x2             ; 1000:6759
    JZ 0x1000:676e                       ; 1000:675e ; -> 1000:676e
    CMP word ptr [0x21a],0x5             ; 1000:6760
    JZ 0x1000:676e                       ; 1000:6765 ; -> 1000:676e
    INC word ptr [0x21a]                 ; 1000:6767
    JMP 0x1000:66e0                      ; 1000:676b ; -> 1000:66e0
    JMP 0x1000:66e3                      ; 1000:676e ; -> 1000:66e3
    MOV byte ptr CS:[0x61],0x0           ; 1000:6771 ; -> 1000:0061
    JMP 0x1000:2323                      ; 1000:6777 ; -> 1000:2323

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_slot_info  [UI / Saves]
; ADDRESS:  1000:677a  (Length: 0x8c bytes)
; CALLERS:  FUN_1000_6668
; PURPOSE:  Displays metadata for a saved game slot (score, level, items).
; ------------------------------------------------------------------------------
ui_draw_slot_info:
    MOV CX,0x6                           ; 1000:677a
    MOV SI,0x9e90                        ; 1000:677d
    MOV AX,word ptr [SI]                 ; 1000:6780
    MOV BX,word ptr [SI + 0x2]           ; 1000:6782
    PUSH SI                              ; 1000:6785
    PUSH CX                              ; 1000:6786
    MOV SI,0x9a8                         ; 1000:6787
    ADD SI,0x4b6                         ; 1000:678a
    CALL 0x1000:77a3                     ; 1000:678e ; -> 1000:77a3
    POP CX                               ; 1000:6791
    POP SI                               ; 1000:6792
    ADD SI,0x4                           ; 1000:6793
    LOOP 0x1000:6780                     ; 1000:6796 ; -> 1000:6780
    MOV AX,0x88                          ; 1000:6798
    MOV BX,0x5a                          ; 1000:679b
    MOV CX,0x30                          ; 1000:679e
    MOV DX,0x8                           ; 1000:67a1
    MOV DI,word ptr CS:[0x773f]          ; 1000:67a4 ; -> 1000:773f
    CALL 0x1000:7ac8                     ; 1000:67a9 ; -> 1000:7ac8
    MOV AX,[0x21a]                       ; 1000:67ac
    SHL AX,0x1                           ; 1000:67af
    SHL AX,0x1                           ; 1000:67b1
    MOV SI,0x9e90                        ; 1000:67b3
    ADD SI,AX                            ; 1000:67b6
    MOV AX,word ptr [SI]                 ; 1000:67b8
    MOV BX,word ptr [SI + 0x2]           ; 1000:67ba
    MOV SI,0x9a8                         ; 1000:67bd
    CALL 0x1000:77a3                     ; 1000:67c0 ; -> 1000:77a3
    MOV AX,0x9                           ; 1000:67c3
    CALL 0x1000:4339                     ; 1000:67c6 ; -> 1000:4339
    MOV BX,word ptr [0x21a]              ; 1000:67c9
    SHL BX,0x1                           ; 1000:67cd
    ADD BX,0x9e84                        ; 1000:67cf
    MOV SI,word ptr [BX]                 ; 1000:67d3
    MOV AX,0x88                          ; 1000:67d5
    MOV BX,0x5a                          ; 1000:67d8
    CALL 0x1000:42d4                     ; 1000:67db ; -> 1000:42d4
    MOV BX,word ptr [0x21a]              ; 1000:67de
    SHL BX,0x1                           ; 1000:67e2
    ADD BX,0x98de                        ; 1000:67e4
    MOV DS,word ptr CS:[0x5e]            ; 1000:67e8 ; -> 1000:005e
    MOV SI,word ptr [BX]                 ; 1000:67ed
    MOV DI,word ptr CS:[0x773f]          ; 1000:67ef ; -> 1000:773f
    MOV AX,0x60                          ; 1000:67f4
    MOV BX,0x4c                          ; 1000:67f7
    CALL 0x1000:7927                     ; 1000:67fa ; -> 1000:7927
    MOV DS,word ptr CS:[0x5c]            ; 1000:67fd ; -> 1000:005c
    CALL 0x1000:774e                     ; 1000:6802 ; -> 1000:774e
    RET                                  ; 1000:6805

; ------------------------------------------------------------------------------
; FUNCTION: game_execute_script  [Game / Scripting]
; ADDRESS:  1000:6806  (Length: 0x583 bytes)
; CALLERS:  FUN_1000_4d41
; PURPOSE:  Interprets cutscene and scripted event commands during story sequences.
; ------------------------------------------------------------------------------
game_execute_script:
    MOV AL,[0x897]                       ; 1000:6806
    OR AL,byte ptr [0x921]               ; 1000:6809
    OR AL,byte ptr [0x8a6]               ; 1000:680d
    JZ 0x1000:6816                       ; 1000:6811 ; -> 1000:6816
    JMP 0x1000:6d74                      ; 1000:6813 ; -> 1000:6d74
    MOV BX,word ptr CS:[SI + 0x1e]       ; 1000:6816
    CMP BX,0x8976                        ; 1000:681a
    JNZ 0x1000:6823                      ; 1000:681e ; -> 1000:6823
    JMP 0x1000:694f                      ; 1000:6820 ; -> 1000:694f
    CMP BX,0x8abe                        ; 1000:6823
    JNZ 0x1000:682c                      ; 1000:6827 ; -> 1000:682c
    JMP 0x1000:6993                      ; 1000:6829 ; -> 1000:6993
    CMP BX,0x8aca                        ; 1000:682c
    JNZ 0x1000:6835                      ; 1000:6830 ; -> 1000:6835
    JMP 0x1000:6993                      ; 1000:6832 ; -> 1000:6993
    CMP BX,0x8ad6                        ; 1000:6835
    JNZ 0x1000:683e                      ; 1000:6839 ; -> 1000:683e
    JMP 0x1000:6993                      ; 1000:683b ; -> 1000:6993
    CMP BX,0x8c50                        ; 1000:683e
    JNZ 0x1000:6847                      ; 1000:6842 ; -> 1000:6847
    JMP 0x1000:69ca                      ; 1000:6844 ; -> 1000:69ca
    CMP BX,0x921b                        ; 1000:6847
    JNZ 0x1000:6850                      ; 1000:684b ; -> 1000:6850
    JMP 0x1000:6a24                      ; 1000:684d ; -> 1000:6a24
    CMP BX,0x930a                        ; 1000:6850
    JNZ 0x1000:6859                      ; 1000:6854 ; -> 1000:6859
    JMP 0x1000:6ab4                      ; 1000:6856 ; -> 1000:6ab4
    CMP BX,0x980d                        ; 1000:6859
    JNZ 0x1000:6862                      ; 1000:685d ; -> 1000:6862
    JMP 0x1000:6aca                      ; 1000:685f ; -> 1000:6aca
    CMP BX,0x9b42                        ; 1000:6862
    JNZ 0x1000:686b                      ; 1000:6866 ; -> 1000:686b
    JMP 0x1000:6aff                      ; 1000:6868 ; -> 1000:6aff
    CMP BX,0x9b80                        ; 1000:686b
    JNZ 0x1000:6874                      ; 1000:686f ; -> 1000:6874
    JMP 0x1000:6b15                      ; 1000:6871 ; -> 1000:6b15
    CMP BX,0x9b98                        ; 1000:6874
    JNZ 0x1000:687d                      ; 1000:6878 ; -> 1000:687d
    JMP 0x1000:6b2b                      ; 1000:687a ; -> 1000:6b2b
    CMP BX,0x9b8c                        ; 1000:687d
    JNZ 0x1000:6886                      ; 1000:6881 ; -> 1000:6886
    JMP 0x1000:6b41                      ; 1000:6883 ; -> 1000:6b41
    CMP BX,0x9ba6                        ; 1000:6886
    JNZ 0x1000:688f                      ; 1000:688a ; -> 1000:688f
    JMP 0x1000:6b57                      ; 1000:688c ; -> 1000:6b57
    CMP BX,0xb37f                        ; 1000:688f
    JNZ 0x1000:6898                      ; 1000:6893 ; -> 1000:6898
    JMP 0x1000:6b6d                      ; 1000:6895 ; -> 1000:6b6d
    CMP BX,0xb38d                        ; 1000:6898
    JNZ 0x1000:68a1                      ; 1000:689c ; -> 1000:68a1
    JMP 0x1000:6b83                      ; 1000:689e ; -> 1000:6b83
    CMP BX,0xb399                        ; 1000:68a1
    JNZ 0x1000:68aa                      ; 1000:68a5 ; -> 1000:68aa
    JMP 0x1000:6b99                      ; 1000:68a7 ; -> 1000:6b99
    CMP BX,0xb3a5                        ; 1000:68aa
    JNZ 0x1000:68b3                      ; 1000:68ae ; -> 1000:68b3
    JMP 0x1000:6baf                      ; 1000:68b0 ; -> 1000:6baf
    CMP BX,0xb98e                        ; 1000:68b3
    JNZ 0x1000:68bc                      ; 1000:68b7 ; -> 1000:68bc
    JMP 0x1000:6bc5                      ; 1000:68b9 ; -> 1000:6bc5
    CMP BX,0xc75c                        ; 1000:68bc
    JNZ 0x1000:68c5                      ; 1000:68c0 ; -> 1000:68c5
    JMP 0x1000:6bdb                      ; 1000:68c2 ; -> 1000:6bdb
    CMP BX,0xc8ea                        ; 1000:68c5
    JNZ 0x1000:68ce                      ; 1000:68c9 ; -> 1000:68ce
    JMP 0x1000:6bf1                      ; 1000:68cb ; -> 1000:6bf1
    CMP BX,0xcaa6                        ; 1000:68ce
    JNZ 0x1000:68d7                      ; 1000:68d2 ; -> 1000:68d7
    JMP 0x1000:6c07                      ; 1000:68d4 ; -> 1000:6c07
    CMP BX,0xd227                        ; 1000:68d7
    JNZ 0x1000:68e0                      ; 1000:68db ; -> 1000:68e0
    JMP 0x1000:6c1d                      ; 1000:68dd ; -> 1000:6c1d
    CMP BX,0xd2dd                        ; 1000:68e0
    JNZ 0x1000:68e9                      ; 1000:68e4 ; -> 1000:68e9
    JMP 0x1000:6c46                      ; 1000:68e6 ; -> 1000:6c46
    CMP BX,0xd2e9                        ; 1000:68e9
    JNZ 0x1000:68f2                      ; 1000:68ed ; -> 1000:68f2
    JMP 0x1000:6c5c                      ; 1000:68ef ; -> 1000:6c5c
    CMP BX,0xd393                        ; 1000:68f2
    JNZ 0x1000:68fb                      ; 1000:68f6 ; -> 1000:68fb
    JMP 0x1000:6c72                      ; 1000:68f8 ; -> 1000:6c72
    CMP BX,0xd3a1                        ; 1000:68fb
    JNZ 0x1000:6904                      ; 1000:68ff ; -> 1000:6904
    JMP 0x1000:6c88                      ; 1000:6901 ; -> 1000:6c88
    CMP BX,0xd3ad                        ; 1000:6904
    JNZ 0x1000:690d                      ; 1000:6908 ; -> 1000:690d
    JMP 0x1000:6c9e                      ; 1000:690a ; -> 1000:6c9e
    CMP BX,0xd59d                        ; 1000:690d
    JNZ 0x1000:6916                      ; 1000:6911 ; -> 1000:6916
    JMP 0x1000:6cb4                      ; 1000:6913 ; -> 1000:6cb4
    CMP BX,0xd5c3                        ; 1000:6916
    JNZ 0x1000:691f                      ; 1000:691a ; -> 1000:691f
    JMP 0x1000:6cca                      ; 1000:691c ; -> 1000:6cca
    CMP BX,0x8507                        ; 1000:691f
    JNZ 0x1000:6928                      ; 1000:6923 ; -> 1000:6928
    JMP 0x1000:6ce0                      ; 1000:6925 ; -> 1000:6ce0
    CMP BX,0x8513                        ; 1000:6928
    JNZ 0x1000:6931                      ; 1000:692c ; -> 1000:6931
    JMP 0x1000:6cf6                      ; 1000:692e ; -> 1000:6cf6
    CMP BX,0x851f                        ; 1000:6931
    JNZ 0x1000:693a                      ; 1000:6935 ; -> 1000:693a
    JMP 0x1000:6d0c                      ; 1000:6937 ; -> 1000:6d0c
    CMP BX,0x8537                        ; 1000:693a
    JNZ 0x1000:6943                      ; 1000:693e ; -> 1000:6943
    JMP 0x1000:6d22                      ; 1000:6940 ; -> 1000:6d22
    CMP BX,0x931a                        ; 1000:6943
    JNZ 0x1000:694c                      ; 1000:6947 ; -> 1000:694c
    JMP 0x1000:6d4b                      ; 1000:6949 ; -> 1000:6d4b
    JMP 0x1000:6d74                      ; 1000:694c ; -> 1000:6d74
    CMP byte ptr [0x220],0x0             ; 1000:694f
    JNZ 0x1000:6976                      ; 1000:6954 ; -> 1000:6976
    CMP word ptr CS:[SI + 0x1c],0x1      ; 1000:6956
    JZ 0x1000:696b                       ; 1000:695b ; -> 1000:696b
    JG 0x1000:6990                       ; 1000:695d ; -> 1000:6990
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:695f
    MOV AX,0x0                           ; 1000:6965
    JMP 0x1000:6d75                      ; 1000:6968 ; -> 1000:6d75
    MOV byte ptr [0x220],0x1             ; 1000:696b
    MOV AX,0x9                           ; 1000:6970
    JMP 0x1000:6d75                      ; 1000:6973 ; -> 1000:6d75
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6976
    JNZ 0x1000:6990                      ; 1000:697b ; -> 1000:6990
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:697d
    CMP byte ptr [0x220],0x2             ; 1000:6983
    JGE 0x1000:6990                      ; 1000:6988 ; -> 1000:6990
    MOV AX,0x1                           ; 1000:698a
    JMP 0x1000:6d75                      ; 1000:698d ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6990 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6993
    JNZ 0x1000:69c7                      ; 1000:6998 ; -> 1000:69c7
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:699a
    CMP byte ptr [0x220],0x2             ; 1000:69a0
    JGE 0x1000:69ad                      ; 1000:69a5 ; -> 1000:69ad
    MOV AX,0x2                           ; 1000:69a7
    JMP 0x1000:6d75                      ; 1000:69aa ; -> 1000:6d75
    CMP byte ptr [0x21d],0x0             ; 1000:69ad
    JZ 0x1000:69ba                       ; 1000:69b2 ; -> 1000:69ba
    MOV AX,0x7                           ; 1000:69b4
    JMP 0x1000:6d75                      ; 1000:69b7 ; -> 1000:6d75
    PUSH SI                              ; 1000:69ba
    PUSH DI                              ; 1000:69bb
    CALL 0x1000:4870                     ; 1000:69bc ; -> 1000:4870
    POP DI                               ; 1000:69bf
    POP SI                               ; 1000:69c0
    MOV AX,0x6                           ; 1000:69c1
    JMP 0x1000:6d75                      ; 1000:69c4 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:69c7 ; -> 1000:6d74
    CMP byte ptr [0x220],0x2             ; 1000:69ca
    JL 0x1000:69ea                       ; 1000:69cf ; -> 1000:69ea
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:69d1
    JNZ 0x1000:6a21                      ; 1000:69d6 ; -> 1000:6a21
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:69d8
    MOV AX,0xac0a                        ; 1000:69de
    CALL 0x1000:3610                     ; 1000:69e1 ; -> 1000:3610
    MOV AX,0x8                           ; 1000:69e4
    JMP 0x1000:6d75                      ; 1000:69e7 ; -> 1000:6d75
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:69ea
    JZ 0x1000:6a09                       ; 1000:69ef ; -> 1000:6a09
    CMP word ptr CS:[SI + 0x1c],0x1      ; 1000:69f1
    JZ 0x1000:6a15                       ; 1000:69f6 ; -> 1000:6a15
    MOV byte ptr [0x220],0x2             ; 1000:69f8
    MOV AX,0xac0a                        ; 1000:69fd
    CALL 0x1000:3610                     ; 1000:6a00 ; -> 1000:3610
    MOV AX,0x5                           ; 1000:6a03
    JMP 0x1000:6d75                      ; 1000:6a06 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6a09
    MOV AX,0x3                           ; 1000:6a0f
    JMP 0x1000:6d75                      ; 1000:6a12 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x2      ; 1000:6a15
    MOV AX,0x4                           ; 1000:6a1b
    JMP 0x1000:6d75                      ; 1000:6a1e ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6a21 ; -> 1000:6d74
    CMP byte ptr [0x220],0x3             ; 1000:6a24
    JL 0x1000:6a41                       ; 1000:6a29 ; -> 1000:6a41
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6a2b
    JNZ 0x1000:6a3e                      ; 1000:6a30 ; -> 1000:6a3e
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6a32
    MOV AX,0x5                           ; 1000:6a38
    JMP 0x1000:6d75                      ; 1000:6a3b ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6a3e ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6a41
    JZ 0x1000:6a84                       ; 1000:6a46 ; -> 1000:6a84
    CMP word ptr CS:[SI + 0x1c],0x1      ; 1000:6a48
    JZ 0x1000:6a90                       ; 1000:6a4d ; -> 1000:6a90
    CMP word ptr CS:[SI + 0x1c],0x2      ; 1000:6a4f
    JZ 0x1000:6a9c                       ; 1000:6a54 ; -> 1000:6a9c
    CMP word ptr CS:[SI + 0x1c],0x3      ; 1000:6a56
    JZ 0x1000:6aa8                       ; 1000:6a5b ; -> 1000:6aa8
    MOV byte ptr [0x220],0x3             ; 1000:6a5d
    MOV word ptr CS:[0x7ed5],0x4         ; 1000:6a62 ; -> 1000:7ed5
    MOV word ptr CS:[0x8067],0x4         ; 1000:6a69 ; -> 1000:8067
    MOV word ptr CS:[0x7ef9],0x4         ; 1000:6a70 ; -> 1000:7ef9
    MOV word ptr CS:[0x7efb],0x1         ; 1000:6a77 ; -> 1000:7efb
    MOV AX,0x4                           ; 1000:6a7e
    JMP 0x1000:6d75                      ; 1000:6a81 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6a84
    MOV AX,0x0                           ; 1000:6a8a
    JMP 0x1000:6d75                      ; 1000:6a8d ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x2      ; 1000:6a90
    MOV AX,0x1                           ; 1000:6a96
    JMP 0x1000:6d75                      ; 1000:6a99 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x3      ; 1000:6a9c
    MOV AX,0x2                           ; 1000:6aa2
    JMP 0x1000:6d75                      ; 1000:6aa5 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x4      ; 1000:6aa8
    MOV AX,0x3                           ; 1000:6aae
    JMP 0x1000:6d75                      ; 1000:6ab1 ; -> 1000:6d75
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6ab4
    JNZ 0x1000:6ac7                      ; 1000:6ab9 ; -> 1000:6ac7
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6abb
    MOV AX,0x0                           ; 1000:6ac1
    JMP 0x1000:6d75                      ; 1000:6ac4 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6ac7 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6aca
    JNZ 0x1000:6afc                      ; 1000:6acf ; -> 1000:6afc
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6ad1
    CMP byte ptr [0x220],0x4             ; 1000:6ad7
    JL 0x1000:6aeb                       ; 1000:6adc ; -> 1000:6aeb
    CMP byte ptr [0x220],0x5             ; 1000:6ade
    JL 0x1000:6af6                       ; 1000:6ae3 ; -> 1000:6af6
    MOV AX,0x2                           ; 1000:6ae5
    JMP 0x1000:6d75                      ; 1000:6ae8 ; -> 1000:6d75
    MOV byte ptr [0x220],0x4             ; 1000:6aeb
    MOV AX,0x0                           ; 1000:6af0
    JMP 0x1000:6d75                      ; 1000:6af3 ; -> 1000:6d75
    MOV AX,0x1                           ; 1000:6af6
    JMP 0x1000:6d75                      ; 1000:6af9 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6afc ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6aff
    JNZ 0x1000:6b12                      ; 1000:6b04 ; -> 1000:6b12
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b06
    MOV AX,0x0                           ; 1000:6b0c
    JMP 0x1000:6d75                      ; 1000:6b0f ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b12 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b15
    JNZ 0x1000:6b28                      ; 1000:6b1a ; -> 1000:6b28
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b1c
    MOV AX,0x1                           ; 1000:6b22
    JMP 0x1000:6d75                      ; 1000:6b25 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b28 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b2b
    JNZ 0x1000:6b3e                      ; 1000:6b30 ; -> 1000:6b3e
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b32
    MOV AX,0x2                           ; 1000:6b38
    JMP 0x1000:6d75                      ; 1000:6b3b ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b3e ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b41
    JNZ 0x1000:6b54                      ; 1000:6b46 ; -> 1000:6b54
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b48
    MOV AX,0x3                           ; 1000:6b4e
    JMP 0x1000:6d75                      ; 1000:6b51 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b54 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b57
    JNZ 0x1000:6b6a                      ; 1000:6b5c ; -> 1000:6b6a
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b5e
    MOV AX,0x4                           ; 1000:6b64
    JMP 0x1000:6d75                      ; 1000:6b67 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b6a ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b6d
    JNZ 0x1000:6b80                      ; 1000:6b72 ; -> 1000:6b80
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b74
    MOV AX,0x5                           ; 1000:6b7a
    JMP 0x1000:6d75                      ; 1000:6b7d ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b80 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b83
    JNZ 0x1000:6b96                      ; 1000:6b88 ; -> 1000:6b96
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6b8a
    MOV AX,0x6                           ; 1000:6b90
    JMP 0x1000:6d75                      ; 1000:6b93 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6b96 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6b99
    JNZ 0x1000:6bac                      ; 1000:6b9e ; -> 1000:6bac
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6ba0
    MOV AX,0x7                           ; 1000:6ba6
    JMP 0x1000:6d75                      ; 1000:6ba9 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6bac ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6baf
    JNZ 0x1000:6bc2                      ; 1000:6bb4 ; -> 1000:6bc2
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6bb6
    MOV AX,0x8                           ; 1000:6bbc
    JMP 0x1000:6d75                      ; 1000:6bbf ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6bc2 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6bc5
    JNZ 0x1000:6bd8                      ; 1000:6bca ; -> 1000:6bd8
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6bcc
    MOV AX,0x9                           ; 1000:6bd2
    JMP 0x1000:6d75                      ; 1000:6bd5 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6bd8 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6bdb
    JNZ 0x1000:6bee                      ; 1000:6be0 ; -> 1000:6bee
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6be2
    MOV AX,0x0                           ; 1000:6be8
    JMP 0x1000:6d75                      ; 1000:6beb ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6bee ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6bf1
    JNZ 0x1000:6c04                      ; 1000:6bf6 ; -> 1000:6c04
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6bf8
    MOV AX,0x1                           ; 1000:6bfe
    JMP 0x1000:6d75                      ; 1000:6c01 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c04 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c07
    JNZ 0x1000:6c1a                      ; 1000:6c0c ; -> 1000:6c1a
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6c0e
    MOV AX,0x2                           ; 1000:6c14
    JMP 0x1000:6d75                      ; 1000:6c17 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c1a ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x1      ; 1000:6c1d
    JG 0x1000:6c43                       ; 1000:6c22 ; -> 1000:6c43
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c24
    JG 0x1000:6c37                       ; 1000:6c29 ; -> 1000:6c37
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6c2b
    MOV AX,0x3                           ; 1000:6c31
    JMP 0x1000:6d75                      ; 1000:6c34 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x2      ; 1000:6c37
    MOV AX,0x4                           ; 1000:6c3d
    JMP 0x1000:6d75                      ; 1000:6c40 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c43 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c46
    JNZ 0x1000:6c59                      ; 1000:6c4b ; -> 1000:6c59
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6c4d
    MOV AX,0x5                           ; 1000:6c53
    JMP 0x1000:6d75                      ; 1000:6c56 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c59 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c5c
    JNZ 0x1000:6c6f                      ; 1000:6c61 ; -> 1000:6c6f
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6c63
    MOV AX,0x6                           ; 1000:6c69
    JMP 0x1000:6d75                      ; 1000:6c6c ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c6f ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c72
    JNZ 0x1000:6c85                      ; 1000:6c77 ; -> 1000:6c85
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6c79
    MOV AX,0x7                           ; 1000:6c7f
    JMP 0x1000:6d75                      ; 1000:6c82 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c85 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c88
    JNZ 0x1000:6c9b                      ; 1000:6c8d ; -> 1000:6c9b
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6c8f
    MOV AX,0x8                           ; 1000:6c95
    JMP 0x1000:6d75                      ; 1000:6c98 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6c9b ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6c9e
    JNZ 0x1000:6cb1                      ; 1000:6ca3 ; -> 1000:6cb1
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6ca5
    MOV AX,0x9                           ; 1000:6cab
    JMP 0x1000:6d75                      ; 1000:6cae ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6cb1 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6cb4
    JNZ 0x1000:6cc7                      ; 1000:6cb9 ; -> 1000:6cc7
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6cbb
    MOV AX,0xa                           ; 1000:6cc1
    JMP 0x1000:6d75                      ; 1000:6cc4 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6cc7 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6cca
    JNZ 0x1000:6cdd                      ; 1000:6ccf ; -> 1000:6cdd
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6cd1
    MOV AX,0xb                           ; 1000:6cd7
    JMP 0x1000:6d75                      ; 1000:6cda ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6cdd ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6ce0
    JNZ 0x1000:6cf3                      ; 1000:6ce5 ; -> 1000:6cf3
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6ce7
    MOV AX,0x1                           ; 1000:6ced
    JMP 0x1000:6d75                      ; 1000:6cf0 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6cf3 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6cf6
    JNZ 0x1000:6d09                      ; 1000:6cfb ; -> 1000:6d09
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6cfd
    MOV AX,0x2                           ; 1000:6d03
    JMP 0x1000:6d75                      ; 1000:6d06 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6d09 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6d0c
    JNZ 0x1000:6d1f                      ; 1000:6d11 ; -> 1000:6d1f
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6d13
    MOV AX,0x3                           ; 1000:6d19
    JMP 0x1000:6d75                      ; 1000:6d1c ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6d1f ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x1      ; 1000:6d22
    JG 0x1000:6d48                       ; 1000:6d27 ; -> 1000:6d48
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6d29
    JG 0x1000:6d3c                       ; 1000:6d2e ; -> 1000:6d3c
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6d30
    MOV AX,0x4                           ; 1000:6d36
    JMP 0x1000:6d75                      ; 1000:6d39 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x2      ; 1000:6d3c
    MOV AX,0x5                           ; 1000:6d42
    JMP 0x1000:6d75                      ; 1000:6d45 ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6d48 ; -> 1000:6d74
    CMP word ptr CS:[SI + 0x1c],0x1      ; 1000:6d4b
    JG 0x1000:6d71                       ; 1000:6d50 ; -> 1000:6d71
    CMP word ptr CS:[SI + 0x1c],0x0      ; 1000:6d52
    JG 0x1000:6d65                       ; 1000:6d57 ; -> 1000:6d65
    MOV word ptr CS:[SI + 0x1c],0x1      ; 1000:6d59
    MOV AX,0x6                           ; 1000:6d5f
    JMP 0x1000:6d75                      ; 1000:6d62 ; -> 1000:6d75
    MOV word ptr CS:[SI + 0x1c],0x2      ; 1000:6d65
    MOV AX,0x7                           ; 1000:6d6b
    JMP 0x1000:6d75                      ; 1000:6d6e ; -> 1000:6d75
    JMP 0x1000:6d74                      ; 1000:6d71 ; -> 1000:6d74
    RET                                  ; 1000:6d74
    SHL AX,0x1                           ; 1000:6d75
    ADD AX,0x9ec0                        ; 1000:6d77
    MOV BX,AX                            ; 1000:6d7a
    MOV AX,word ptr [BX]                 ; 1000:6d7c
    ADD AX,0x9ec0                        ; 1000:6d7e
    MOV [0x922],AX                       ; 1000:6d81
    MOV byte ptr [0x921],0x1             ; 1000:6d84
    MOV AX,0x1                           ; 1000:6d89
    LEA BX,[0xaac0]                      ; 1000:6d8c
    MOV CX,0x9                           ; 1000:6d90
    INT3                                 ; 1000:6d93

; ------------------------------------------------------------------------------
; FUNCTION: ui_prompt_yes_no  [UI / Dialogs]
; ADDRESS:  1000:6da4  (Length: 0x3f bytes)
; CALLERS:  FUN_1000_6de7
; PURPOSE:  Displays Yes/No prompt dialog box and waits for user confirmation.
; ------------------------------------------------------------------------------
ui_prompt_yes_no:
    MOV CX,word ptr [BX]                 ; 1000:6da4
    ADD CL,AL                            ; 1000:6da6
    CMP CL,0x9                           ; 1000:6da8
    JLE 0x1000:6db2                      ; 1000:6dab ; -> 1000:6db2
    SUB CL,0xa                           ; 1000:6dad
    INC CH                               ; 1000:6db0
    ADD CH,AH                            ; 1000:6db2
    XOR AX,AX                            ; 1000:6db4
    CMP CH,0x9                           ; 1000:6db6
    JLE 0x1000:6dc0                      ; 1000:6db9 ; -> 1000:6dc0
    SUB CH,0xa                           ; 1000:6dbb
    INC AL                               ; 1000:6dbe
    PUSH AX                              ; 1000:6dc0
    PUSH BX                              ; 1000:6dc1
    PUSH DX                              ; 1000:6dc2
    MOV word ptr [BX],CX                 ; 1000:6dc3
    MOV AX,DX                            ; 1000:6dc5
    MOV BX,0x10                          ; 1000:6dc7
    CALL 0x1000:3a5f                     ; 1000:6dca ; -> 1000:3a5f
    POP DX                               ; 1000:6dcd
    POP BX                               ; 1000:6dce
    POP AX                               ; 1000:6dcf
    CMP BX,0x934                         ; 1000:6dd0
    JZ 0x1000:6de0                       ; 1000:6dd4 ; -> 1000:6de0
    SUB BX,0x2                           ; 1000:6dd6
    SUB DX,0x10                          ; 1000:6dd9
    OR AX,AX                             ; 1000:6ddc
    JNZ 0x1000:6da4                      ; 1000:6dde ; -> 1000:6da4
    POP DI                               ; 1000:6de0
    POP SI                               ; 1000:6de1
    RET                                  ; 1000:6de2

; ------------------------------------------------------------------------------
; FUNCTION: snd_play_cursor_move  [Audio / SFX]
; ADDRESS:  1000:6de7  (Length: 0xd bytes)
; CALLERS:  FUN_1000_4cc1
; PURPOSE:  Plays menu cursor navigation tick sound.
; ------------------------------------------------------------------------------
snd_play_cursor_move:
    PUSH SI                              ; 1000:6de7
    PUSH DI                              ; 1000:6de8
    MOV BX,0x934                         ; 1000:6de9
    ADD BX,0x2                           ; 1000:6dec
    MOV DX,0x100                         ; 1000:6def
    JMP 0x1000:6da4                      ; 1000:6df2 ; -> 1000:6da4

; ------------------------------------------------------------------------------
; FUNCTION: ui_main_menu  [UI / Menu]
; ADDRESS:  1000:6df4  (Length: 0x153 bytes)
; CALLERS:  FUN_1000_5e52
; PURPOSE:  Main title menu loop: displays options (Play Game, Instructions, Story, Demo, High Scores, Key Config, Exit).
; ------------------------------------------------------------------------------
ui_main_menu:
    PUSH BP                              ; 1000:6df4
    MOV BP,SP                            ; 1000:6df5
    SUB SP,0x4                           ; 1000:6df7
    MOV AX,[0xabab]                      ; 1000:6dfa
    CMP AX,word ptr [0x9ce8]             ; 1000:6dfd
    JNZ 0x1000:6e19                      ; 1000:6e01 ; -> 1000:6e19
    MOV AX,[0xabad]                      ; 1000:6e03
    CMP AX,word ptr [0x9cea]             ; 1000:6e06
    JNZ 0x1000:6e19                      ; 1000:6e0a ; -> 1000:6e19
    MOV AX,[0xabaf]                      ; 1000:6e0c
    SUB AX,word ptr [0x9cec]             ; 1000:6e0f
    JNZ 0x1000:6e19                      ; 1000:6e13 ; -> 1000:6e19
    MOV SP,BP                            ; 1000:6e15
    POP BP                               ; 1000:6e17
    RET                                  ; 1000:6e18
    CALL 0x1000:6f47                     ; 1000:6e19 ; -> 1000:6f47
    CALL 0x1000:774e                     ; 1000:6e1c ; -> 1000:774e
    CALL 0x1000:6f47                     ; 1000:6e1f ; -> 1000:6f47
    CALL 0x1000:7057                     ; 1000:6e22 ; -> 1000:7057
    MOV AX,[0xabb1]                      ; 1000:6e25
    MOV word ptr [BP + -0x2],AX          ; 1000:6e28 ; -> Stack[-0x4]
    MOV BX,word ptr [0xabb3]             ; 1000:6e2b
    MOV word ptr [BP + -0x4],BX          ; 1000:6e2f ; -> Stack[-0x6]
    CALL 0x1000:774e                     ; 1000:6e32 ; -> 1000:774e
    MOV DX,0x25                          ; 1000:6e35
    MOV AX,[0xabb1]                      ; 1000:6e38
    MOV CX,word ptr [0xabb1]             ; 1000:6e3b
    AND CX,0xf                           ; 1000:6e3f
    XOR BX,BX                            ; 1000:6e42
    OR BX,CX                             ; 1000:6e44
    ADD BX,AX                            ; 1000:6e46
    XOR AX,DX                            ; 1000:6e48
    LOOP 0x1000:6e44                     ; 1000:6e4a ; -> 1000:6e44
    MOV SI,0xabb5                        ; 1000:6e4c
    MOV DX,word ptr [0xabbb]             ; 1000:6e4f
    XOR AX,AX                            ; 1000:6e53
    MOV BL,byte ptr [SI]                 ; 1000:6e55
    OR BL,BL                             ; 1000:6e57
    JZ 0x1000:6e71                       ; 1000:6e59 ; -> 1000:6e71
    SUB BL,0x30                          ; 1000:6e5b
    XOR BH,BH                            ; 1000:6e5e
    MOV CX,AX                            ; 1000:6e60
    SHL AX,0x1                           ; 1000:6e62
    SHL AX,0x1                           ; 1000:6e64
    SHL AX,0x1                           ; 1000:6e66
    SHL CX,0x1                           ; 1000:6e68
    ADD AX,CX                            ; 1000:6e6a
    ADD AX,BX                            ; 1000:6e6c
    INC SI                               ; 1000:6e6e
    JMP 0x1000:6e55                      ; 1000:6e6f ; -> 1000:6e55
    MOV [0x9cf8],AX                      ; 1000:6e71
    MOV word ptr [0xabb1],BX             ; 1000:6e74
    MOV word ptr [0xabb3],CX             ; 1000:6e78
    CALL 0x1000:774e                     ; 1000:6e7c ; -> 1000:774e
    MOV DX,0x25                          ; 1000:6e7f
    MOV AX,[0xabb1]                      ; 1000:6e82
    MOV CX,word ptr [0xabb1]             ; 1000:6e85
    AND CX,0xf                           ; 1000:6e89
    XOR BX,BX                            ; 1000:6e8c
    OR BX,CX                             ; 1000:6e8e
    ADD BX,AX                            ; 1000:6e90
    XOR AX,DX                            ; 1000:6e92
    LOOP 0x1000:6e8e                     ; 1000:6e94 ; -> 1000:6e8e
    MOV word ptr [0xabb1],DX             ; 1000:6e96
    MOV AX,[0xabb1]                      ; 1000:6e9a
    MOV BX,word ptr [0x9cf8]             ; 1000:6e9d
    MOV CX,word ptr [0xabb3]             ; 1000:6ea1
    MOV DX,word ptr [0xabbb]             ; 1000:6ea5
    XOR BX,CX                            ; 1000:6ea9
    XOR AX,BX                            ; 1000:6eab
    XOR DX,CX                            ; 1000:6ead
    XOR BX,AX                            ; 1000:6eaf
    XOR CX,AX                            ; 1000:6eb1
    XOR DX,AX                            ; 1000:6eb3
    XOR AX,CX                            ; 1000:6eb5
    XOR BX,DX                            ; 1000:6eb7
    MOV [0xabb1],AX                      ; 1000:6eb9
    MOV word ptr [0xabb3],CX             ; 1000:6ebc
    PUSH AX                              ; 1000:6ec0
    PUSH BX                              ; 1000:6ec1
    CALL 0x1000:774e                     ; 1000:6ec2 ; -> 1000:774e
    POP BX                               ; 1000:6ec5
    POP AX                               ; 1000:6ec6
    ADD AX,BX                            ; 1000:6ec7
    SUB AX,word ptr [0xabb1]             ; 1000:6ec9
    MOV [0xabb3],AX                      ; 1000:6ecd
    MOV BX,word ptr [0xaba9]             ; 1000:6ed0
    MOV AX,[0xabab]                      ; 1000:6ed4
    MOV CX,word ptr [0xabad]             ; 1000:6ed7
    MOV DX,word ptr [0xabaf]             ; 1000:6edb
    MOV word ptr [0x9ce6],BX             ; 1000:6edf
    MOV [0x9ce8],AX                      ; 1000:6ee3
    MOV word ptr [0x9cea],CX             ; 1000:6ee6
    MOV word ptr [0x9cec],DX             ; 1000:6eea
    XOR DX,AX                            ; 1000:6eee
    XOR BX,CX                            ; 1000:6ef0
    XOR BX,AX                            ; 1000:6ef2
    XOR DX,CX                            ; 1000:6ef4
    XOR AX,BX                            ; 1000:6ef6
    XOR AX,CX                            ; 1000:6ef8
    XOR CX,AX                            ; 1000:6efa
    XOR BX,DX                            ; 1000:6efc
    ADD CX,word ptr [0xabb3]             ; 1000:6efe
    MOV word ptr [0xabb1],CX             ; 1000:6f02
    CALL 0x1000:774e                     ; 1000:6f06 ; -> 1000:774e
    MOV DX,0x25                          ; 1000:6f09
    MOV AX,[0xabb1]                      ; 1000:6f0c
    MOV CX,word ptr [0xabb1]             ; 1000:6f0f
    AND CX,0xf                           ; 1000:6f13
    XOR BX,BX                            ; 1000:6f16
    OR BX,CX                             ; 1000:6f18
    ADD BX,AX                            ; 1000:6f1a
    XOR AX,DX                            ; 1000:6f1c
    LOOP 0x1000:6f18                     ; 1000:6f1e ; -> 1000:6f18
    MOV AX,[0xabb3]                      ; 1000:6f20
    OR AX,AX                             ; 1000:6f23
    MOV AX,word ptr [BP + -0x2]          ; 1000:6f25 ; -> Stack[-0x4]
    MOV BX,word ptr [BP + -0x4]          ; 1000:6f28 ; -> Stack[-0x6]
    MOV [0xabb1],AX                      ; 1000:6f2b
    MOV word ptr [0xabb3],BX             ; 1000:6f2e
    JZ 0x1000:6f37                       ; 1000:6f32 ; -> 1000:6f37
    JMP 0x1000:6e19                      ; 1000:6f34 ; -> 1000:6e19
    MOV SP,BP                            ; 1000:6f37
    POP BP                               ; 1000:6f39
    MOV AX,[0xabab]                      ; 1000:6f3a
    MOV BX,word ptr [0x9ce8]             ; 1000:6f3d
    XOR AX,BX                            ; 1000:6f41
    JNZ 0x1000:6f47                      ; 1000:6f43 ; -> 1000:6f47
    INC AX                               ; 1000:6f45
    RET                                  ; 1000:6f46

; ------------------------------------------------------------------------------
; FUNCTION: ui_high_scores_screen  [UI / High Scores]
; ADDRESS:  1000:6f47  (Length: 0x110 bytes)
; CALLERS:  FUN_1000_6df4
; PURPOSE:  Renders the Hall of Fame high scores screen.
; ------------------------------------------------------------------------------
ui_high_scores_screen:
    MOV AX,0x0                           ; 1000:6f47
    MOV BX,0x0                           ; 1000:6f4a
    MOV CX,0x140                         ; 1000:6f4d
    MOV DX,0xc8                          ; 1000:6f50
    MOV DI,word ptr CS:[0x773f]          ; 1000:6f53 ; -> 1000:773f
    CALL 0x1000:7ac8                     ; 1000:6f58 ; -> 1000:7ac8
    MOV AX,0x9                           ; 1000:6f5b
    CALL 0x1000:4339                     ; 1000:6f5e ; -> 1000:4339
    MOV CX,0x18                          ; 1000:6f61
    MOV BX,0x0                           ; 1000:6f64
    MOV AL,0x22                          ; 1000:6f67
    CALL 0x1000:42e8                     ; 1000:6f69 ; -> 1000:42e8
    MOV DX,0x26                          ; 1000:6f6c
    MOV AL,0x23                          ; 1000:6f6f
    CALL 0x1000:42e8                     ; 1000:6f71 ; -> 1000:42e8
    DEC DX                               ; 1000:6f74
    JNZ 0x1000:6f71                      ; 1000:6f75 ; -> 1000:6f71
    MOV AL,0x24                          ; 1000:6f77
    CALL 0x1000:42e8                     ; 1000:6f79 ; -> 1000:42e8
    MOV CX,0xc0                          ; 1000:6f7c
    MOV BX,0x0                           ; 1000:6f7f
    MOV AL,0x3a                          ; 1000:6f82
    CALL 0x1000:42e8                     ; 1000:6f84 ; -> 1000:42e8
    MOV DX,0x26                          ; 1000:6f87
    MOV AL,0x3b                          ; 1000:6f8a
    CALL 0x1000:42e8                     ; 1000:6f8c ; -> 1000:42e8
    DEC DX                               ; 1000:6f8f
    JNZ 0x1000:6f8c                      ; 1000:6f90 ; -> 1000:6f8c
    MOV AL,0x3c                          ; 1000:6f92
    CALL 0x1000:42e8                     ; 1000:6f94 ; -> 1000:42e8
    MOV DX,0x14                          ; 1000:6f97
    MOV CX,0x20                          ; 1000:6f9a
    MOV BX,0x0                           ; 1000:6f9d
    MOV AL,0x25                          ; 1000:6fa0
    CALL 0x1000:42e8                     ; 1000:6fa2 ; -> 1000:42e8
    MOV BX,0x138                         ; 1000:6fa5
    MOV AL,0x26                          ; 1000:6fa8
    CALL 0x1000:42e8                     ; 1000:6faa ; -> 1000:42e8
    ADD CX,0x8                           ; 1000:6fad
    DEC DX                               ; 1000:6fb0
    JNZ 0x1000:6f9d                      ; 1000:6fb1 ; -> 1000:6f9d
    MOV AX,0x7                           ; 1000:6fb3
    CALL 0x1000:4339                     ; 1000:6fb6 ; -> 1000:4339
    MOV AX,0x10                          ; 1000:6fb9
    MOV BX,0x0                           ; 1000:6fbc
    MOV SI,0xaae0                        ; 1000:6fbf
    CALL 0x1000:42d4                     ; 1000:6fc2 ; -> 1000:42d4
    MOV AX,0x50                          ; 1000:6fc5
    MOV BX,0x10                          ; 1000:6fc8
    MOV SI,0xab05                        ; 1000:6fcb
    CALL 0x1000:42d4                     ; 1000:6fce ; -> 1000:42d4
    MOV AX,0x10                          ; 1000:6fd1
    MOV BX,0x28                          ; 1000:6fd4
    MOV SI,0xab1b                        ; 1000:6fd7
    CALL 0x1000:42d4                     ; 1000:6fda ; -> 1000:42d4
    MOV AX,0x10                          ; 1000:6fdd
    MOV BX,0x30                          ; 1000:6fe0
    MOV SI,0xab40                        ; 1000:6fe3
    CALL 0x1000:42d4                     ; 1000:6fe6 ; -> 1000:42d4
    MOV AX,0x10                          ; 1000:6fe9
    MOV BX,0x38                          ; 1000:6fec
    MOV SI,0xab65                        ; 1000:6fef
    CALL 0x1000:42d4                     ; 1000:6ff2 ; -> 1000:42d4
    MOV AX,0x10                          ; 1000:6ff5
    MOV BX,0x40                          ; 1000:6ff8
    MOV SI,0xab8a                        ; 1000:6ffb
    CALL 0x1000:42d4                     ; 1000:6ffe ; -> 1000:42d4
    MOV AX,0x10                          ; 1000:7001
    MOV BX,0x58                          ; 1000:7004
    MOV SI,0xab97                        ; 1000:7007
    CALL 0x1000:42d4                     ; 1000:700a ; -> 1000:42d4
    MOV AX,[0xabb1]                      ; 1000:700d
    ADD AL,0x41                          ; 1000:7010
    MOV BX,0xa0                          ; 1000:7012
    MOV CX,0x58                          ; 1000:7015
    CALL 0x1000:42e8                     ; 1000:7018 ; -> 1000:42e8
    MOV AL,0x2c                          ; 1000:701b
    CALL 0x1000:42e8                     ; 1000:701d ; -> 1000:42e8
    MOV DX,word ptr [0xabb3]             ; 1000:7020
    XOR AX,AX                            ; 1000:7024
    CMP DX,0x64                          ; 1000:7026
    JL 0x1000:7032                       ; 1000:7029 ; -> 1000:7032
    INC AL                               ; 1000:702b
    SUB DX,0x64                          ; 1000:702d
    JMP 0x1000:7026                      ; 1000:7030 ; -> 1000:7026
    ADD AL,0x30                          ; 1000:7032
    CALL 0x1000:42e8                     ; 1000:7034 ; -> 1000:42e8
    XOR AX,AX                            ; 1000:7037
    CMP DX,0xa                           ; 1000:7039
    JL 0x1000:7045                       ; 1000:703c ; -> 1000:7045
    INC AL                               ; 1000:703e
    SUB DX,0xa                           ; 1000:7040
    JMP 0x1000:7039                      ; 1000:7043 ; -> 1000:7039
    ADD AL,0x30                          ; 1000:7045
    CALL 0x1000:42e8                     ; 1000:7047 ; -> 1000:42e8
    MOV AX,DX                            ; 1000:704a
    ADD AL,0x30                          ; 1000:704c
    CALL 0x1000:42e8                     ; 1000:704e ; -> 1000:42e8
    MOV AL,0x2d                          ; 1000:7051
    CALL 0x1000:42e8                     ; 1000:7053 ; -> 1000:42e8
    RET                                  ; 1000:7056

; ------------------------------------------------------------------------------
; FUNCTION: ui_instructions_screen  [UI / Instructions]
; ADDRESS:  1000:7057  (Length: 0xae bytes)
; CALLERS:  FUN_1000_6df4
; PURPOSE:  Displays scrolling gameplay instructions, item descriptions, and key controls.
; ------------------------------------------------------------------------------
ui_instructions_screen:
    PUSH BP                              ; 1000:7057
    MOV BP,SP                            ; 1000:7058
    SUB SP,0x8                           ; 1000:705a
    MOV word ptr [BP + -0x2],0xd8        ; 1000:705d ; -> Stack[-0x4]
    MOV word ptr [BP + -0x4],0x58        ; 1000:7062 ; -> Stack[-0x6]
    MOV SI,0xabb5                        ; 1000:7067
    MOV word ptr [BP + -0x6],SI          ; 1000:706a ; -> Stack[-0x8]
    MOV word ptr [BP + -0x8],0x0         ; 1000:706d ; -> Stack[-0xa]
    MOV SI,word ptr [BP + -0x6]          ; 1000:7072 ; -> Stack[-0x8]
    ADD SI,word ptr [BP + -0x8]          ; 1000:7075 ; -> Stack[-0xa]
    MOV byte ptr [SI],0x0                ; 1000:7078
    MOV AX,word ptr [BP + -0x2]          ; 1000:707b ; -> Stack[-0x4]
    MOV BX,word ptr [BP + -0x4]          ; 1000:707e ; -> Stack[-0x6]
    MOV CX,0x30                          ; 1000:7081
    MOV DX,0x8                           ; 1000:7084
    MOV DI,word ptr CS:[0x773f]          ; 1000:7087 ; -> 1000:773f
    CALL 0x1000:7ac8                     ; 1000:708c ; -> 1000:7ac8
    MOV AX,0x7                           ; 1000:708f
    CALL 0x1000:4339                     ; 1000:7092 ; -> 1000:4339
    MOV AX,word ptr [BP + -0x2]          ; 1000:7095 ; -> Stack[-0x4]
    MOV BX,word ptr [BP + -0x4]          ; 1000:7098 ; -> Stack[-0x6]
    MOV SI,word ptr [BP + -0x6]          ; 1000:709b ; -> Stack[-0x8]
    CALL 0x1000:42d4                     ; 1000:709e ; -> 1000:42d4
    MOV CX,BX                            ; 1000:70a1
    MOV BX,AX                            ; 1000:70a3
    MOV AL,0x2b                          ; 1000:70a5
    CALL 0x1000:42e8                     ; 1000:70a7 ; -> 1000:42e8
    CALL 0x1000:774e                     ; 1000:70aa ; -> 1000:774e
    CALL 0x1000:0599                     ; 1000:70ad ; -> 1000:0599
    MOV AL,CS:[0x61]                     ; 1000:70b0 ; -> 1000:0061
    CMP AL,0x1                           ; 1000:70b4
    JNZ 0x1000:70bb                      ; 1000:70b6 ; -> 1000:70bb
    JMP 0x1000:0215                      ; 1000:70b8 ; -> 1000:0215
    CMP AL,0xe                           ; 1000:70bb
    JZ 0x1000:70e3                       ; 1000:70bd ; -> 1000:70e3
    CMP AL,0x1c                          ; 1000:70bf
    JZ 0x1000:70ee                       ; 1000:70c1 ; -> 1000:70ee
    CMP AL,0xb                           ; 1000:70c3
    JZ 0x1000:70ce                       ; 1000:70c5 ; -> 1000:70ce
    JG 0x1000:70ad                       ; 1000:70c7 ; -> 1000:70ad
    ADD AL,0x2f                          ; 1000:70c9
    JMP 0x1000:70d0                      ; 1000:70cb ; -> 1000:70d0
    MOV AL,0x30                          ; 1000:70ce
    CMP word ptr [BP + -0x8],0x5         ; 1000:70d0 ; -> Stack[-0xa]
    JZ 0x1000:7072                       ; 1000:70d4 ; -> 1000:7072
    MOV SI,word ptr [BP + -0x6]          ; 1000:70d6 ; -> Stack[-0x8]
    ADD SI,word ptr [BP + -0x8]          ; 1000:70d9 ; -> Stack[-0xa]
    MOV byte ptr [SI],AL                 ; 1000:70dc
    INC word ptr [BP + -0x8]             ; 1000:70de ; -> Stack[-0xa]
    JMP 0x1000:7072                      ; 1000:70e1 ; -> 1000:7072
    CMP word ptr [BP + -0x8],0x0         ; 1000:70e3 ; -> Stack[-0xa]
    JZ 0x1000:70ec                       ; 1000:70e7 ; -> 1000:70ec
    DEC word ptr [BP + -0x8]             ; 1000:70e9 ; -> Stack[-0xa]
    JMP 0x1000:7072                      ; 1000:70ec ; -> 1000:7072
    MOV AX,word ptr [BP + -0x2]          ; 1000:70ee ; -> Stack[-0x4]
    MOV BX,word ptr [BP + -0x4]          ; 1000:70f1 ; -> Stack[-0x6]
    MOV CX,0x30                          ; 1000:70f4
    MOV DX,0x8                           ; 1000:70f7
    MOV DI,word ptr CS:[0x773f]          ; 1000:70fa ; -> 1000:773f
    CALL 0x1000:7ac8                     ; 1000:70ff ; -> 1000:7ac8
    MOV SP,BP                            ; 1000:7102
    POP BP                               ; 1000:7104
    RET                                  ; 1000:7105

; ------------------------------------------------------------------------------
; FUNCTION: ui_story_screen  [UI / Story]
; ADDRESS:  1000:7164  (Length: 0x9a bytes)
; CALLERS:  entry
; PURPOSE:  Presents illustrated storyline cutscene sequence explaining Captain Comic's mission on Planet Ogron.
; ------------------------------------------------------------------------------
ui_story_screen:
    MOV word ptr [0xabf3],SP             ; 1000:7164
    MOV byte ptr CS:[0x61],0x0           ; 1000:7168 ; -> 1000:0061
    PUSH DS                              ; 1000:716e
    MOV AX,0x3206                        ; 1000:716f
    MOV DS,AX                            ; 1000:7172
    MOV SI,0x1cd0                        ; 1000:7174
    MOV DI,0x6000                        ; 1000:7177
    CALL 0x1000:79c7                     ; 1000:717a ; -> 1000:79c7
    POP DS                               ; 1000:717d
    MOV DI,word ptr CS:[0x773f]          ; 1000:717e ; -> 1000:773f
    MOV SI,0x6000                        ; 1000:7183
    CALL 0x1000:7765                     ; 1000:7186 ; -> 1000:7765
    CALL 0x1000:774e                     ; 1000:7189 ; -> 1000:774e
    MOV DX,0xabc0                        ; 1000:718c
    CALL 0x1000:015d                     ; 1000:718f ; -> 1000:015d
    MOV DX,0xabf7                        ; 1000:7192
    CALL 0x1000:2022                     ; 1000:7195 ; -> 1000:2022
    MOV AX,0x12                          ; 1000:7198
    CALL 0x1000:7278                     ; 1000:719b ; -> 1000:7278
    CALL 0x1000:7255                     ; 1000:719e ; -> 1000:7255
    MOV AX,0x48                          ; 1000:71a1
    MOV BX,0x60                          ; 1000:71a4
    MOV SI,0x1722                        ; 1000:71a7
    CALL 0x1000:7261                     ; 1000:71aa ; -> 1000:7261
    MOV AX,0x1b                          ; 1000:71ad
    CALL 0x1000:7278                     ; 1000:71b0 ; -> 1000:7278
    CALL 0x1000:7255                     ; 1000:71b3 ; -> 1000:7255
    CALL 0x1000:774e                     ; 1000:71b6 ; -> 1000:774e
    MOV AX,0x12                          ; 1000:71b9
    CALL 0x1000:7278                     ; 1000:71bc ; -> 1000:7278
    MOV AX,0x100                         ; 1000:71bf
    MOV BX,0x0                           ; 1000:71c2
    MOV SI,0x13d4                        ; 1000:71c5
    MOV CX,0x6                           ; 1000:71c8
    CALL 0x1000:71fe                     ; 1000:71cb ; -> 1000:71fe
    MOV SI,0x1086                        ; 1000:71ce
    MOV CX,0x6                           ; 1000:71d1
    CALL 0x1000:71fe                     ; 1000:71d4 ; -> 1000:71fe
    MOV SI,0xd38                         ; 1000:71d7
    MOV CX,0x5                           ; 1000:71da
    CALL 0x1000:721b                     ; 1000:71dd ; -> 1000:721b
    MOV SI,0x9ea                         ; 1000:71e0
    MOV CX,0x5                           ; 1000:71e3
    CALL 0x1000:721b                     ; 1000:71e6 ; -> 1000:721b
    MOV SI,0x69c                         ; 1000:71e9
    MOV CX,0x5                           ; 1000:71ec
    CALL 0x1000:7238                     ; 1000:71ef ; -> 1000:7238
    MOV SI,0x34e                         ; 1000:71f2
    MOV CX,0x5                           ; 1000:71f5
    CALL 0x1000:7238                     ; 1000:71f8 ; -> 1000:7238
    JMP 0x1000:7298                      ; 1000:71fb ; -> 1000:7298

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_story_page_1  [UI / Story]
; ADDRESS:  1000:71fe  (Length: 0x1d bytes)
; CALLERS:  FUN_1000_7164
; PURPOSE:  Renders page 1 of story prologue artwork and narrative.
; ------------------------------------------------------------------------------
ui_draw_story_page_1:
    PUSH CX                              ; 1000:71fe
    PUSH SI                              ; 1000:71ff
    PUSH AX                              ; 1000:7200
    PUSH BX                              ; 1000:7201
    CALL 0x1000:7255                     ; 1000:7202 ; -> 1000:7255
    POP BX                               ; 1000:7205
    POP AX                               ; 1000:7206
    POP SI                               ; 1000:7207
    SUB AX,0x4                           ; 1000:7208
    ADD BX,0x3                           ; 1000:720b
    PUSH SI                              ; 1000:720e
    PUSH AX                              ; 1000:720f
    PUSH BX                              ; 1000:7210
    CALL 0x1000:7261                     ; 1000:7211 ; -> 1000:7261
    POP BX                               ; 1000:7214
    POP AX                               ; 1000:7215
    POP SI                               ; 1000:7216
    POP CX                               ; 1000:7217
    LOOP 0x1000:71fe                     ; 1000:7218 ; -> 1000:71fe
    RET                                  ; 1000:721a

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_story_page_2  [UI / Story]
; ADDRESS:  1000:721b  (Length: 0x1d bytes)
; CALLERS:  FUN_1000_7164
; PURPOSE:  Renders page 2 of story prologue artwork and narrative.
; ------------------------------------------------------------------------------
ui_draw_story_page_2:
    PUSH CX                              ; 1000:721b
    PUSH SI                              ; 1000:721c
    PUSH AX                              ; 1000:721d
    PUSH BX                              ; 1000:721e
    CALL 0x1000:7255                     ; 1000:721f ; -> 1000:7255
    POP BX                               ; 1000:7222
    POP AX                               ; 1000:7223
    POP SI                               ; 1000:7224
    SUB AX,0x8                           ; 1000:7225
    ADD BX,0x6                           ; 1000:7228
    PUSH SI                              ; 1000:722b
    PUSH AX                              ; 1000:722c
    PUSH BX                              ; 1000:722d
    CALL 0x1000:7261                     ; 1000:722e ; -> 1000:7261
    POP BX                               ; 1000:7231
    POP AX                               ; 1000:7232
    POP SI                               ; 1000:7233
    POP CX                               ; 1000:7234
    LOOP 0x1000:721b                     ; 1000:7235 ; -> 1000:721b
    RET                                  ; 1000:7237

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_story_page_3  [UI / Story]
; ADDRESS:  1000:7238  (Length: 0x1d bytes)
; CALLERS:  FUN_1000_7164
; PURPOSE:  Renders page 3 of story prologue artwork and narrative.
; ------------------------------------------------------------------------------
ui_draw_story_page_3:
    PUSH CX                              ; 1000:7238
    PUSH SI                              ; 1000:7239
    PUSH AX                              ; 1000:723a
    PUSH BX                              ; 1000:723b
    CALL 0x1000:7255                     ; 1000:723c ; -> 1000:7255
    POP BX                               ; 1000:723f
    POP AX                               ; 1000:7240
    POP SI                               ; 1000:7241
    SUB AX,0xc                           ; 1000:7242
    ADD BX,0x8                           ; 1000:7245
    PUSH SI                              ; 1000:7248
    PUSH AX                              ; 1000:7249
    PUSH BX                              ; 1000:724a
    CALL 0x1000:7261                     ; 1000:724b ; -> 1000:7261
    POP BX                               ; 1000:724e
    POP AX                               ; 1000:724f
    POP SI                               ; 1000:7250
    POP CX                               ; 1000:7251
    LOOP 0x1000:7238                     ; 1000:7252 ; -> 1000:7238
    RET                                  ; 1000:7254

; ------------------------------------------------------------------------------
; FUNCTION: ui_wait_for_keypress  [UI / Input]
; ADDRESS:  1000:7255  (Length: 0xc bytes)
; CALLERS:  FUN_1000_7164, FUN_1000_7238, FUN_1000_71fe, FUN_1000_721b
; PURPOSE:  Waits for user keypress or timer timeout before advancing text screen.
; ------------------------------------------------------------------------------
ui_wait_for_keypress:
    MOV DI,word ptr CS:[0x773f]          ; 1000:7255 ; -> 1000:773f
    MOV SI,0x6000                        ; 1000:725a
    CALL 0x1000:7765                     ; 1000:725d ; -> 1000:7765
    RET                                  ; 1000:7260

; ------------------------------------------------------------------------------
; FUNCTION: ui_fade_in_story_page  [UI / Palette]
; ADDRESS:  1000:7261  (Length: 0x17 bytes)
; CALLERS:  FUN_1000_7164, FUN_1000_7238, FUN_1000_71fe, FUN_1000_721b
; PURPOSE:  Fades in palette for story illustration.
; ------------------------------------------------------------------------------
ui_fade_in_story_page:
    MOV DS,word ptr CS:[0x5e]            ; 1000:7261 ; -> 1000:005e
    CALL 0x1000:77a3                     ; 1000:7266 ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:7269 ; -> 1000:005c
    CALL 0x1000:774e                     ; 1000:726e ; -> 1000:774e
    MOV AX,0x1                           ; 1000:7271
    CALL 0x1000:7278                     ; 1000:7274 ; -> 1000:7278
    RET                                  ; 1000:7277

; ------------------------------------------------------------------------------
; FUNCTION: ui_wait_retrace_ticks  [Video / Timing]
; ADDRESS:  1000:7278  (Length: 0x1b bytes)
; CALLERS:  FUN_1000_75ad, FUN_1000_7298, FUN_1000_7164, FUN_1000_7261
; PURPOSE:  Delays execution for specified number of EGA vertical retraces.
; ------------------------------------------------------------------------------
ui_wait_retrace_ticks:
    PUSH AX                              ; 1000:7278
    MOV AX,0x1                           ; 1000:7279
    CALL 0x1000:027a                     ; 1000:727c ; -> 1000:027a
    CMP byte ptr CS:[0x61],0x0           ; 1000:727f ; -> 1000:0061
    JNZ 0x1000:728c                      ; 1000:7285 ; -> 1000:728c
    POP AX                               ; 1000:7287
    DEC AX                               ; 1000:7288
    JNZ 0x1000:7278                      ; 1000:7289 ; -> 1000:7278
    RET                                  ; 1000:728b
    CALL 0x1000:022b                     ; 1000:728c ; -> 1000:022b
    MOV AX,0x3                           ; 1000:728f
    INT3                                 ; 1000:7292

; ------------------------------------------------------------------------------
; FUNCTION: ui_title_screen  [UI / Title]
; ADDRESS:  1000:7298  (Length: 0x168 bytes)
; CALLERS:  FUN_1000_7164
; PURPOSE:  Displays Captain Comic II: Fractured Reality title screen logo, artwork, and copyright notices.
; ------------------------------------------------------------------------------
ui_title_screen:
    MOV AX,0xd                           ; 1000:7298
    INT 0x10                             ; 1000:729b
    CALL 0x1000:7741                     ; 1000:729d ; -> 1000:7741
    MOV BX,0x7106                        ; 1000:72a0
    CLI                                  ; 1000:72a3
    MOV word ptr CS:[0x278],BX           ; 1000:72a4 ; -> 1000:0278
    MOV word ptr CS:[BX + 0x2],0x0       ; 1000:72a9 ; -> 1000:7108
    MOV word ptr CS:[BX + 0x4],0x0       ; 1000:72af ; -> 1000:710a
    MOV word ptr CS:[BX + 0xa],0x0       ; 1000:72b5 ; -> 1000:7110
    MOV word ptr CS:[BX + 0xc],0x0       ; 1000:72bb ; -> 1000:7112
    MOV word ptr CS:[BX + 0x12],0x0      ; 1000:72c1 ; -> 1000:7118
    MOV word ptr CS:[BX + 0x14],0x0      ; 1000:72c7 ; -> 1000:711a
    STI                                  ; 1000:72cd
    CALL 0x1000:0238                     ; 1000:72ce ; -> 1000:0238
    MOV DX,0xabd1                        ; 1000:72d1
    CALL 0x1000:015d                     ; 1000:72d4 ; -> 1000:015d
    MOV DI,0x4000                        ; 1000:72d7
    MOV DX,0xac12                        ; 1000:72da
    CALL 0x1000:7990                     ; 1000:72dd ; -> 1000:7990
    MOV DI,0x6000                        ; 1000:72e0
    MOV DX,0xac1c                        ; 1000:72e3
    CALL 0x1000:7990                     ; 1000:72e6 ; -> 1000:7990
    MOV SI,0x4000                        ; 1000:72e9
    MOV DI,word ptr CS:[0x773f]          ; 1000:72ec ; -> 1000:773f
    CALL 0x1000:7765                     ; 1000:72f1 ; -> 1000:7765
    CALL 0x1000:774e                     ; 1000:72f4 ; -> 1000:774e
    MOV DX,0xac00                        ; 1000:72f7
    CALL 0x1000:2022                     ; 1000:72fa ; -> 1000:2022
    MOV AX,0x32                          ; 1000:72fd
    CALL 0x1000:7278                     ; 1000:7300 ; -> 1000:7278
    MOV AX,0x8                           ; 1000:7303
    PUSH AX                              ; 1000:7306
    CALL 0x1000:74e1                     ; 1000:7307 ; -> 1000:74e1
    CALL 0x1000:774e                     ; 1000:730a ; -> 1000:774e
    MOV AX,0x1                           ; 1000:730d
    CALL 0x1000:7278                     ; 1000:7310 ; -> 1000:7278
    POP AX                               ; 1000:7313
    ADD AX,0x8                           ; 1000:7314
    CMP AX,0x148                         ; 1000:7317
    JL 0x1000:7306                       ; 1000:731a ; -> 1000:7306
    MOV BX,0x7120                        ; 1000:731c
    CLI                                  ; 1000:731f
    MOV word ptr CS:[0x278],BX           ; 1000:7320 ; -> 1000:0278
    MOV word ptr CS:[BX + 0x2],0x0       ; 1000:7325 ; -> 1000:7122
    MOV word ptr CS:[BX + 0x4],0x0       ; 1000:732b ; -> 1000:7124
    MOV word ptr CS:[BX + 0xa],0x0       ; 1000:7331 ; -> 1000:712a
    MOV word ptr CS:[BX + 0xc],0x0       ; 1000:7337 ; -> 1000:712c
    MOV word ptr CS:[BX + 0x12],0x0      ; 1000:733d ; -> 1000:7132
    MOV word ptr CS:[BX + 0x14],0x0      ; 1000:7343 ; -> 1000:7134
    STI                                  ; 1000:7349
    CALL 0x1000:0238                     ; 1000:734a ; -> 1000:0238
    MOV DX,0xabd1                        ; 1000:734d
    CALL 0x1000:015d                     ; 1000:7350 ; -> 1000:015d
    MOV word ptr [0x6c6],0x0             ; 1000:7353
    MOV word ptr [0x6c8],0x81            ; 1000:7359
    MOV byte ptr [0x8ad],0x1             ; 1000:735f
    MOV word ptr [0xabf5],0x10           ; 1000:7364
    MOV SI,0x6000                        ; 1000:736a
    MOV DI,word ptr CS:[0x773f]          ; 1000:736d ; -> 1000:773f
    CALL 0x1000:7765                     ; 1000:7372 ; -> 1000:7765
    CALL 0x1000:7622                     ; 1000:7375 ; -> 1000:7622
    CALL 0x1000:774e                     ; 1000:7378 ; -> 1000:774e
    MOV AX,0x1                           ; 1000:737b
    CALL 0x1000:7278                     ; 1000:737e ; -> 1000:7278
    ADD word ptr [0x6c6],0x8             ; 1000:7381
    DEC word ptr [0xabf5]                ; 1000:7386
    JNZ 0x1000:736a                      ; 1000:738a ; -> 1000:736a
    XOR SI,SI                            ; 1000:738c
    CALL 0x1000:75ad                     ; 1000:738e ; -> 1000:75ad
    MOV byte ptr [0x8ad],0x2             ; 1000:7391
    MOV word ptr [0xabf5],0xa            ; 1000:7396
    MOV SI,0x6000                        ; 1000:739c
    MOV DI,word ptr CS:[0x773f]          ; 1000:739f ; -> 1000:773f
    CALL 0x1000:7765                     ; 1000:73a4 ; -> 1000:7765
    CALL 0x1000:7622                     ; 1000:73a7 ; -> 1000:7622
    CALL 0x1000:774e                     ; 1000:73aa ; -> 1000:774e
    MOV AX,0x1                           ; 1000:73ad
    CALL 0x1000:7278                     ; 1000:73b0 ; -> 1000:7278
    SUB word ptr [0x6c6],0x8             ; 1000:73b3
    DEC word ptr [0xabf5]                ; 1000:73b8
    JNZ 0x1000:739c                      ; 1000:73bc ; -> 1000:739c
    XOR SI,SI                            ; 1000:73be
    CALL 0x1000:75ad                     ; 1000:73c0 ; -> 1000:75ad
    MOV byte ptr [0x8ad],0x1             ; 1000:73c3
    MOV word ptr [0xabf5],0x2            ; 1000:73c8
    MOV SI,0x6000                        ; 1000:73ce
    MOV DI,word ptr CS:[0x773f]          ; 1000:73d1 ; -> 1000:773f
    CALL 0x1000:7765                     ; 1000:73d6 ; -> 1000:7765
    CALL 0x1000:7622                     ; 1000:73d9 ; -> 1000:7622
    CALL 0x1000:774e                     ; 1000:73dc ; -> 1000:774e
    MOV AX,0x1                           ; 1000:73df
    CALL 0x1000:7278                     ; 1000:73e2 ; -> 1000:7278
    ADD word ptr [0x6c6],0x8             ; 1000:73e5
    DEC word ptr [0xabf5]                ; 1000:73ea
    JNZ 0x1000:73ce                      ; 1000:73ee ; -> 1000:73ce
    XOR SI,SI                            ; 1000:73f0
    CALL 0x1000:75ad                     ; 1000:73f2 ; -> 1000:75ad
    MOV AX,0x1                           ; 1000:73f5
    LEA BX,[0xac3a]                      ; 1000:73f8
    MOV CX,0x9                           ; 1000:73fc
    INT3                                 ; 1000:73ff

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_title_stars  [UI / Title]
; ADDRESS:  1000:74e1  (Length: 0x49 bytes)
; CALLERS:  FUN_1000_7298
; PURPOSE:  Renders animated twinkling starfield background on title screen.
; ------------------------------------------------------------------------------
ui_draw_title_stars:
    PUSH DS                              ; 1000:74e1
    MOV BX,0xa000                        ; 1000:74e2
    MOV DS,BX                            ; 1000:74e5
    SHR AX,0x1                           ; 1000:74e7
    SHR AX,0x1                           ; 1000:74e9
    SHR AX,0x1                           ; 1000:74eb
    CMP AX,0x28                          ; 1000:74ed
    JZ 0x1000:750e                       ; 1000:74f0 ; -> 1000:750e
    MOV SI,0x4000                        ; 1000:74f2
    MOV DI,word ptr CS:[0x773f]          ; 1000:74f5 ; -> 1000:773f
    PUSH AX                              ; 1000:74fa
    ADD DI,AX                            ; 1000:74fb
    MOV DX,0x28                          ; 1000:74fd
    SUB DX,AX                            ; 1000:7500
    ADD SI,0x280                         ; 1000:7502
    ADD DI,0x280                         ; 1000:7506
    CALL 0x1000:752a                     ; 1000:750a ; -> 1000:752a
    POP AX                               ; 1000:750d
    MOV SI,0x6000                        ; 1000:750e
    MOV DI,word ptr CS:[0x773f]          ; 1000:7511 ; -> 1000:773f
    ADD SI,0x28                          ; 1000:7516
    SUB SI,AX                            ; 1000:7519
    MOV DX,AX                            ; 1000:751b
    ADD SI,0x280                         ; 1000:751d
    ADD DI,0x280                         ; 1000:7521
    CALL 0x1000:752a                     ; 1000:7525 ; -> 1000:752a
    POP DS                               ; 1000:7528
    RET                                  ; 1000:7529

; ------------------------------------------------------------------------------
; FUNCTION: ui_update_title_star  [UI / Title]
; ADDRESS:  1000:752a  (Length: 0x29 bytes)
; CALLERS:  FUN_1000_74e1
; PURPOSE:  Updates brightness and position of an individual star in the starfield.
; ------------------------------------------------------------------------------
ui_update_title_star:
    XOR CL,CL                            ; 1000:752a
    PUSH DX                              ; 1000:752c
    CALL 0x1000:7a68                     ; 1000:752d ; -> 1000:7a68
    POP DX                               ; 1000:7530
    CALL 0x1000:7553                     ; 1000:7531 ; -> 1000:7553
    MOV CL,0x1                           ; 1000:7534
    PUSH DX                              ; 1000:7536
    CALL 0x1000:7a68                     ; 1000:7537 ; -> 1000:7a68
    POP DX                               ; 1000:753a
    CALL 0x1000:7553                     ; 1000:753b ; -> 1000:7553
    MOV CL,0x2                           ; 1000:753e
    PUSH DX                              ; 1000:7540
    CALL 0x1000:7a68                     ; 1000:7541 ; -> 1000:7a68
    POP DX                               ; 1000:7544
    CALL 0x1000:7553                     ; 1000:7545 ; -> 1000:7553
    MOV CL,0x3                           ; 1000:7548
    PUSH DX                              ; 1000:754a
    CALL 0x1000:7a68                     ; 1000:754b ; -> 1000:7a68
    POP DX                               ; 1000:754e
    CALL 0x1000:7553                     ; 1000:754f ; -> 1000:7553
    RET                                  ; 1000:7552

; ------------------------------------------------------------------------------
; FUNCTION: math_lfsr_random  [Math / PRNG]
; ADDRESS:  1000:7553  (Length: 0x1a bytes)
; CALLERS:  FUN_1000_752a
; PURPOSE:  Generates pseudo-random number using Linear Feedback Shift Register.
; ------------------------------------------------------------------------------
math_lfsr_random:
    PUSH SI                              ; 1000:7553
    PUSH DI                              ; 1000:7554
    MOV CX,0xa0                          ; 1000:7555
    PUSH CX                              ; 1000:7558
    MOV CX,DX                            ; 1000:7559
    PUSH SI                              ; 1000:755b
    PUSH DI                              ; 1000:755c
    MOVSB.REP ES:DI,SI                   ; 1000:755d
    POP DI                               ; 1000:755f
    POP SI                               ; 1000:7560
    ADD SI,0x28                          ; 1000:7561
    ADD DI,0x28                          ; 1000:7564
    POP CX                               ; 1000:7567
    LOOP 0x1000:7558                     ; 1000:7568 ; -> 1000:7558
    POP DI                               ; 1000:756a
    POP SI                               ; 1000:756b
    RET                                  ; 1000:756c

; ------------------------------------------------------------------------------
; FUNCTION: ui_draw_title_logo  [UI / Title]
; ADDRESS:  1000:75ad  (Length: 0x75 bytes)
; CALLERS:  FUN_1000_7298
; PURPOSE:  Draws multicolor Captain Comic II title logo onto video buffer.
; ------------------------------------------------------------------------------
ui_draw_title_logo:
    MOV word ptr [0xabf5],0xa            ; 1000:75ad
    PUSH SI                              ; 1000:75b3
    MOV SI,0x6000                        ; 1000:75b4
    MOV DI,word ptr CS:[0x773f]          ; 1000:75b7 ; -> 1000:773f
    CALL 0x1000:7765                     ; 1000:75bc ; -> 1000:7765
    MOV SI,0x0                           ; 1000:75bf
    TEST word ptr [0xabf5],0x1           ; 1000:75c2
    JNZ 0x1000:75cd                      ; 1000:75c8 ; -> 1000:75cd
    MOV SI,0x146                         ; 1000:75ca
    MOV AX,[0x6c6]                       ; 1000:75cd
    MOV BX,word ptr [0x6c8]              ; 1000:75d0
    MOV DS,word ptr CS:[0x5e]            ; 1000:75d4 ; -> 1000:005e
    CALL 0x1000:77a3                     ; 1000:75d9 ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:75dc ; -> 1000:005c
    POP SI                               ; 1000:75e1
    OR SI,SI                             ; 1000:75e2
    JZ 0x1000:7600                       ; 1000:75e4 ; -> 1000:7600
    MOV AX,0xc0                          ; 1000:75e6
    MOV BX,0x50                          ; 1000:75e9
    MOV DS,word ptr CS:[0x5e]            ; 1000:75ec ; -> 1000:005e
    MOV DI,word ptr CS:[0x773f]          ; 1000:75f1 ; -> 1000:773f
    PUSH SI                              ; 1000:75f6
    CALL 0x1000:7927                     ; 1000:75f7 ; -> 1000:7927
    POP SI                               ; 1000:75fa
    MOV DS,word ptr CS:[0x5c]            ; 1000:75fb ; -> 1000:005c
    CALL 0x1000:774e                     ; 1000:7600 ; -> 1000:774e
    MOV AX,0x1                           ; 1000:7603
    CALL 0x1000:7278                     ; 1000:7606 ; -> 1000:7278
    CMP word ptr [0xabf5],0x9            ; 1000:7609 ; -> 34fe:abf5
    JNZ 0x1000:761b                      ; 1000:760e ; -> 1000:761b
    MOV AX,0x1                           ; 1000:7610
    LEA BX,[0xac92]                      ; 1000:7613
    MOV CX,0x8                           ; 1000:7617
    INT3                                 ; 1000:761a
    DEC word ptr [0xabf5]                ; 1000:761b ; -> 34fe:abf5
    JNZ 0x1000:75b3                      ; 1000:761f ; -> 1000:75b3
    RET                                  ; 1000:7621

; ------------------------------------------------------------------------------
; FUNCTION: ui_clear_title_logo  [UI / Title]
; ADDRESS:  1000:7622  (Length: 0x3e bytes)
; CALLERS:  FUN_1000_7298
; PURPOSE:  Erases title logo graphic area.
; ------------------------------------------------------------------------------
ui_clear_title_logo:
    MOV CH,byte ptr [0x8af]              ; 1000:7622
    INC CH                               ; 1000:7626
    CMP CH,0x4                           ; 1000:7628
    JL 0x1000:762f                       ; 1000:762b ; -> 1000:762f
    MOV CH,0x1                           ; 1000:762d
    MOV byte ptr [0x8af],CH              ; 1000:762f
    MOV SI,0x4882                        ; 1000:7633
    CMP byte ptr [0x8ad],0x1             ; 1000:7636
    JZ 0x1000:7640                       ; 1000:763b ; -> 1000:7640
    MOV SI,0x4860                        ; 1000:763d
    MOV AL,[0x8af]                       ; 1000:7640
    XOR AH,AH                            ; 1000:7643
    SHL AX,0x1                           ; 1000:7645
    ADD SI,AX                            ; 1000:7647
    MOV AX,[0x6c6]                       ; 1000:7649
    MOV BX,word ptr [0x6c8]              ; 1000:764c
    MOV DS,word ptr CS:[0x5e]            ; 1000:7650 ; -> 1000:005e
    MOV SI,word ptr [SI]                 ; 1000:7655
    CALL 0x1000:77a3                     ; 1000:7657 ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:765a ; -> 1000:005c
    RET                                  ; 1000:765f

; ------------------------------------------------------------------------------
; FUNCTION: vid_init_ega_regs  [Video / Hardware]
; ADDRESS:  1000:7741  (Length: 0xd bytes)
; CALLERS:  FUN_1000_7298, FUN_1000_0185, entry
; PURPOSE:  Initializes EGA hardware registers (Sequencer, Graphics Controller, CRTC) to 320x200 16-color mode.
; ------------------------------------------------------------------------------
vid_init_ega_regs:
    XOR AX,AX                            ; 1000:7741
    CALL 0x1000:7a89                     ; 1000:7743 ; -> 1000:7a89
    MOV word ptr CS:[0x773f],0x2000      ; 1000:7746 ; -> 1000:773f
    RET                                  ; 1000:774d

; ------------------------------------------------------------------------------
; FUNCTION: vid_wait_vblank  [Video / Hardware]
; ADDRESS:  1000:774e  (Length: 0x17 bytes)
; CALLERS:  FUN_1000_660b, FUN_1000_677a, FUN_1000_7057, FUN_1000_61c8, FUN_1000_7164, FUN_1000_3db6, FUN_1000_0185, entry, FUN_1000_75ad, FUN_1000_6df4, FUN_1000_02b4, FUN_1000_7298, FUN_1000_7261, FUN_1000_2341, FUN_1000_46e6
; PURPOSE:  Polls Port 0x3da bit 3 until vertical retrace blanking interval begins.
; ------------------------------------------------------------------------------
vid_wait_vblank:
    MOV AL,CS:[0x229]                    ; 1000:774e ; -> 1000:0229
    MOV CS:[0x22a],AL                    ; 1000:7752 ; -> 1000:022a
    MOV AX,CS:[0x773f]                   ; 1000:7756 ; -> 1000:773f
    CALL 0x1000:7a89                     ; 1000:775a ; -> 1000:7a89
    XOR word ptr CS:[0x773f],0x2000      ; 1000:775d ; -> 1000:773f
    RET                                  ; 1000:7764

; ------------------------------------------------------------------------------
; FUNCTION: vid_wait_not_vblank  [Video / Hardware]
; ADDRESS:  1000:7765  (Length: 0x3e bytes)
; CALLERS:  FUN_1000_75ad, FUN_1000_7255, FUN_1000_7298, FUN_1000_7164
; PURPOSE:  Polls Port 0x3da bit 3 until vertical retrace blanking interval ends.
; ------------------------------------------------------------------------------
vid_wait_not_vblank:
    PUSH DS                              ; 1000:7765
    MOV AX,0xa000                        ; 1000:7766
    MOV DS,AX                            ; 1000:7769
    MOV ES,AX                            ; 1000:776b
    XOR CL,CL                            ; 1000:776d
    CALL 0x1000:7a68                     ; 1000:776f ; -> 1000:7a68
    MOV CX,0xfa0                         ; 1000:7772
    PUSH SI                              ; 1000:7775
    PUSH DI                              ; 1000:7776
    MOVSW.REP ES:DI,SI                   ; 1000:7777
    POP DI                               ; 1000:7779
    POP SI                               ; 1000:777a
    MOV CL,0x1                           ; 1000:777b
    CALL 0x1000:7a68                     ; 1000:777d ; -> 1000:7a68
    MOV CX,0xfa0                         ; 1000:7780
    PUSH SI                              ; 1000:7783
    PUSH DI                              ; 1000:7784
    MOVSW.REP ES:DI,SI                   ; 1000:7785
    POP DI                               ; 1000:7787
    POP SI                               ; 1000:7788
    MOV CL,0x2                           ; 1000:7789
    CALL 0x1000:7a68                     ; 1000:778b ; -> 1000:7a68
    MOV CX,0xfa0                         ; 1000:778e
    PUSH SI                              ; 1000:7791
    PUSH DI                              ; 1000:7792
    MOVSW.REP ES:DI,SI                   ; 1000:7793
    POP DI                               ; 1000:7795
    POP SI                               ; 1000:7796
    MOV CL,0x3                           ; 1000:7797
    CALL 0x1000:7a68                     ; 1000:7799 ; -> 1000:7a68
    MOV CX,0xfa0                         ; 1000:779c
    MOVSW.REP ES:DI,SI                   ; 1000:779f
    POP DS                               ; 1000:77a1
    RET                                  ; 1000:77a2

; ------------------------------------------------------------------------------
; FUNCTION: vid_clear_page  [Video / Hardware]
; ADDRESS:  1000:77a3  (Length: 0x4e bytes)
; CALLERS:  FUN_1000_677a, FUN_1000_75ad, FUN_1000_7622, FUN_1000_7dbb, FUN_1000_7261, FUN_1000_46e6, FUN_1000_5d5f, FUN_1000_31eb
; PURPOSE:  Clears 32KB EGA video page with zero bytes across all 4 planes.
; ------------------------------------------------------------------------------
vid_clear_page:
    PUSH BP                              ; 1000:77a3
    PUSH AX                              ; 1000:77a4
    SHR AX,0x1                           ; 1000:77a5
    SHR AX,0x1                           ; 1000:77a7
    SHR AX,0x1                           ; 1000:77a9
    MOV DX,BX                            ; 1000:77ab
    SHL BX,0x1                           ; 1000:77ad
    SHL BX,0x1                           ; 1000:77af
    SHL BX,0x1                           ; 1000:77b1
    MOV CL,0x5                           ; 1000:77b3
    SHL DX,CL                            ; 1000:77b5
    ADD BX,DX                            ; 1000:77b7
    ADD BX,AX                            ; 1000:77b9
    MOV DI,word ptr CS:[0x773f]          ; 1000:77bb ; -> 1000:773f
    ADD DI,BX                            ; 1000:77c0
    LODSW SI                             ; 1000:77c2
    SHR AX,0x1                           ; 1000:77c3
    SHR AX,0x1                           ; 1000:77c5
    SHR AX,0x1                           ; 1000:77c7
    MOV DX,AX                            ; 1000:77c9
    LODSW SI                             ; 1000:77cb
    MOV BX,AX                            ; 1000:77cc
    LODSW SI                             ; 1000:77ce
    MOV BP,SI                            ; 1000:77cf
    ADD SI,AX                            ; 1000:77d1
    POP AX                               ; 1000:77d3
    TEST AX,0x4                          ; 1000:77d4
    JZ 0x1000:77dc                       ; 1000:77d7 ; -> 1000:77dc
    JMP 0x1000:7833                      ; 1000:77d9 ; -> 1000:7833
    MOV CL,0x0                           ; 1000:77dc
    CALL 0x1000:77f2                     ; 1000:77de ; -> 1000:77f2
    MOV CL,0x1                           ; 1000:77e1
    CALL 0x1000:77f2                     ; 1000:77e3 ; -> 1000:77f2
    MOV CL,0x2                           ; 1000:77e6
    CALL 0x1000:77f2                     ; 1000:77e8 ; -> 1000:77f2
    MOV CL,0x3                           ; 1000:77eb
    CALL 0x1000:77f2                     ; 1000:77ed ; -> 1000:77f2
    POP BP                               ; 1000:77f0
    RET                                  ; 1000:77f1

; ------------------------------------------------------------------------------
; FUNCTION: vid_fill_plane  [Video / Hardware]
; ADDRESS:  1000:77f2  (Length: 0x41 bytes)
; CALLERS:  FUN_1000_77a3
; PURPOSE:  Fills specific EGA bitplane with pattern byte.
; ------------------------------------------------------------------------------
vid_fill_plane:
    PUSH DX                              ; 1000:77f2
    CALL 0x1000:7a68                     ; 1000:77f3 ; -> 1000:7a68
    POP DX                               ; 1000:77f6
    PUSH DI                              ; 1000:77f7
    PUSH BX                              ; 1000:77f8
    MOV AX,BX                            ; 1000:77f9
    MOV BX,BP                            ; 1000:77fb
    PUSH AX                              ; 1000:77fd
    PUSH DI                              ; 1000:77fe
    MOV CX,DX                            ; 1000:77ff
    CMP CX,0x1                           ; 1000:7801
    JZ 0x1000:781e                       ; 1000:7804 ; -> 1000:781e
    MOV AX,word ptr [BX]                 ; 1000:7806
    ADD BX,0x2                           ; 1000:7808
    AND word ptr ES:[DI],AX              ; 1000:780b
    LODSW SI                             ; 1000:780e
    OR word ptr ES:[DI],AX               ; 1000:780f
    ADD DI,0x2                           ; 1000:7812
    SUB CX,0x2                           ; 1000:7815
    OR CX,CX                             ; 1000:7818
    JNZ 0x1000:7801                      ; 1000:781a ; -> 1000:7801
    JMP 0x1000:7828                      ; 1000:781c ; -> 1000:7828
    MOV AL,byte ptr [BX]                 ; 1000:781e
    INC BX                               ; 1000:7820
    AND byte ptr ES:[DI],AL              ; 1000:7821
    LODSB SI                             ; 1000:7824
    OR byte ptr ES:[DI],AL               ; 1000:7825
    POP DI                               ; 1000:7828
    ADD DI,0x28                          ; 1000:7829
    POP AX                               ; 1000:782c
    DEC AX                               ; 1000:782d
    JNZ 0x1000:77fd                      ; 1000:782e ; -> 1000:77fd
    POP BX                               ; 1000:7830
    POP DI                               ; 1000:7831
    RET                                  ; 1000:7832

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_read_plane  [Video / Hardware]
; ADDRESS:  1000:7833  (Length: 0x16 bytes)
; CALLERS:  FUN_1000_77a3
; PURPOSE:  Configures EGA Read Map Select register (Port 0x3ce index 4).
; ------------------------------------------------------------------------------
vid_set_read_plane:
    MOV CL,0x0                           ; 1000:7833
    CALL 0x1000:7849                     ; 1000:7835 ; -> 1000:7849
    MOV CL,0x1                           ; 1000:7838
    CALL 0x1000:7849                     ; 1000:783a ; -> 1000:7849
    MOV CL,0x2                           ; 1000:783d
    CALL 0x1000:7849                     ; 1000:783f ; -> 1000:7849
    MOV CL,0x3                           ; 1000:7842
    CALL 0x1000:7849                     ; 1000:7844 ; -> 1000:7849
    POP BP                               ; 1000:7847
    RET                                  ; 1000:7848

; ------------------------------------------------------------------------------
; FUNCTION: vid_copy_page  [Video / Hardware]
; ADDRESS:  1000:7849  (Length: 0x7d bytes)
; CALLERS:  FUN_1000_7833
; PURPOSE:  Fast VRAM-to-VRAM page copy using EGA internal latch registers.
; ------------------------------------------------------------------------------
vid_copy_page:
    PUSH DX                              ; 1000:7849
    CALL 0x1000:7a68                     ; 1000:784a ; -> 1000:7a68
    POP DX                               ; 1000:784d
    PUSH DI                              ; 1000:784e
    PUSH BX                              ; 1000:784f
    PUSH BP                              ; 1000:7850
    MOV AX,BX                            ; 1000:7851
    MOV BX,BP                            ; 1000:7853
    PUSH AX                              ; 1000:7855
    PUSH DI                              ; 1000:7856
    MOV CX,DX                            ; 1000:7857
    XOR BP,BP                            ; 1000:7859
    MOV AX,0xffff                        ; 1000:785b
    MOV AL,byte ptr [BX]                 ; 1000:785e
    INC BX                               ; 1000:7860
    SHL AX,0x1                           ; 1000:7861
    SHL AX,0x1                           ; 1000:7863
    SHL AX,0x1                           ; 1000:7865
    SHL AX,0x1                           ; 1000:7867
    AND byte ptr ES:[DI],AH              ; 1000:7869
    XCHG AX,BP                           ; 1000:786c
    LODSB SI                             ; 1000:786d
    SHL AX,0x1                           ; 1000:786e
    SHL AX,0x1                           ; 1000:7870
    SHL AX,0x1                           ; 1000:7872
    SHL AX,0x1                           ; 1000:7874
    OR byte ptr ES:[DI],AH               ; 1000:7876
    INC DI                               ; 1000:7879
    XCHG AX,BP                           ; 1000:787a
    CMP CX,0x1                           ; 1000:787b
    JZ 0x1000:78b0                       ; 1000:787e ; -> 1000:78b0
    SHL AX,0x1                           ; 1000:7880
    SHL AX,0x1                           ; 1000:7882
    SHL AX,0x1                           ; 1000:7884
    SHL AX,0x1                           ; 1000:7886
    MOV AL,byte ptr [BX]                 ; 1000:7888
    INC BX                               ; 1000:788a
    SHL AX,0x1                           ; 1000:788b
    SHL AX,0x1                           ; 1000:788d
    SHL AX,0x1                           ; 1000:788f
    SHL AX,0x1                           ; 1000:7891
    AND byte ptr ES:[DI],AH              ; 1000:7893
    XCHG AX,BP                           ; 1000:7896
    SHL AX,0x1                           ; 1000:7897
    SHL AX,0x1                           ; 1000:7899
    SHL AX,0x1                           ; 1000:789b
    SHL AX,0x1                           ; 1000:789d
    LODSB SI                             ; 1000:789f
    SHL AX,0x1                           ; 1000:78a0
    SHL AX,0x1                           ; 1000:78a2
    SHL AX,0x1                           ; 1000:78a4
    SHL AX,0x1                           ; 1000:78a6
    OR byte ptr ES:[DI],AH               ; 1000:78a8
    INC DI                               ; 1000:78ab
    XCHG AX,BP                           ; 1000:78ac
    DEC CX                               ; 1000:78ad
    JMP 0x1000:787b                      ; 1000:78ae ; -> 1000:787b
    OR AL,0xf                            ; 1000:78b0
    AND byte ptr ES:[DI],AL              ; 1000:78b2
    MOV AX,BP                            ; 1000:78b5
    OR byte ptr ES:[DI],AL               ; 1000:78b7
    POP DI                               ; 1000:78ba
    ADD DI,0x28                          ; 1000:78bb
    POP AX                               ; 1000:78be
    DEC AX                               ; 1000:78bf
    JNZ 0x1000:7855                      ; 1000:78c0 ; -> 1000:7855
    POP BP                               ; 1000:78c2
    POP BX                               ; 1000:78c3
    POP DI                               ; 1000:78c4
    RET                                  ; 1000:78c5

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_map_mask  [Video / Hardware]
; ADDRESS:  1000:78c6  (Length: 0x1b bytes)
; CALLERS:  FUN_1000_451c
; PURPOSE:  Sets EGA Sequencer Map Mask register (Port 0x3c4 index 2) to select plane write enables.
; ------------------------------------------------------------------------------
vid_set_map_mask:
    MOV DI,word ptr CS:[0x773f]          ; 1000:78c6 ; -> 1000:773f
    PUSH AX                              ; 1000:78cb
    PUSH BX                              ; 1000:78cc
    PUSH SI                              ; 1000:78cd
    CALL 0x1000:78e1                     ; 1000:78ce ; -> 1000:78e1
    MOV DI,word ptr CS:[0x773f]          ; 1000:78d1 ; -> 1000:773f
    XOR DI,0x2000                        ; 1000:78d6
    POP SI                               ; 1000:78da
    POP BX                               ; 1000:78db
    POP AX                               ; 1000:78dc
    CALL 0x1000:78e1                     ; 1000:78dd ; -> 1000:78e1
    RET                                  ; 1000:78e0

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_bit_mask  [Video / Hardware]
; ADDRESS:  1000:78e1  (Length: 0x2a bytes)
; CALLERS:  FUN_1000_78c6
; PURPOSE:  Sets EGA Graphics Controller Bit Mask register (Port 0x3ce index 8).
; ------------------------------------------------------------------------------
vid_set_bit_mask:
    PUSH BP                              ; 1000:78e1
    SHR AX,0x1                           ; 1000:78e2
    SHR AX,0x1                           ; 1000:78e4
    SHR AX,0x1                           ; 1000:78e6
    MOV DX,BX                            ; 1000:78e8
    SHL BX,0x1                           ; 1000:78ea
    SHL BX,0x1                           ; 1000:78ec
    SHL BX,0x1                           ; 1000:78ee
    MOV CL,0x5                           ; 1000:78f0
    SHL DX,CL                            ; 1000:78f2
    ADD BX,DX                            ; 1000:78f4
    ADD BX,AX                            ; 1000:78f6
    ADD DI,BX                            ; 1000:78f8
    LODSW SI                             ; 1000:78fa
    SHR AX,0x1                           ; 1000:78fb
    SHR AX,0x1                           ; 1000:78fd
    SHR AX,0x1                           ; 1000:78ff
    MOV DX,AX                            ; 1000:7901
    LODSW SI                             ; 1000:7903
    MOV BX,AX                            ; 1000:7904
    LODSW SI                             ; 1000:7906
    ADD SI,AX                            ; 1000:7907
    JMP 0x1000:794c                      ; 1000:7909 ; -> 1000:794c

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_write_mode_2  [Video / Hardware]
; ADDRESS:  1000:790c  (Length: 0x1b bytes)
; CALLERS:  FUN_1000_3b51, FUN_1000_3a5f, FUN_1000_3ace, FUN_1000_6668, FUN_1000_3a17, FUN_1000_3aa0, FUN_1000_3b12
; PURPOSE:  Configures EGA Graphics Controller to Write Mode 2 for color-per-pixel blits.
; ------------------------------------------------------------------------------
vid_set_write_mode_2:
    MOV DI,word ptr CS:[0x773f]          ; 1000:790c ; -> 1000:773f
    PUSH AX                              ; 1000:7911
    PUSH BX                              ; 1000:7912
    PUSH SI                              ; 1000:7913
    CALL 0x1000:7927                     ; 1000:7914 ; -> 1000:7927
    MOV DI,word ptr CS:[0x773f]          ; 1000:7917 ; -> 1000:773f
    XOR DI,0x2000                        ; 1000:791c
    POP SI                               ; 1000:7920
    POP BX                               ; 1000:7921
    POP AX                               ; 1000:7922
    CALL 0x1000:7927                     ; 1000:7923 ; -> 1000:7927
    RET                                  ; 1000:7926

; ------------------------------------------------------------------------------
; FUNCTION: vid_restore_write_mode  [Video / Hardware]
; ADDRESS:  1000:7927  (Length: 0x3b bytes)
; CALLERS:  FUN_1000_677a, FUN_1000_75ad, FUN_1000_790c
; PURPOSE:  Restores EGA Graphics Controller to Write Mode 0.
; ------------------------------------------------------------------------------
vid_restore_write_mode:
    PUSH BP                              ; 1000:7927
    SHR AX,0x1                           ; 1000:7928
    SHR AX,0x1                           ; 1000:792a
    SHR AX,0x1                           ; 1000:792c
    MOV DX,BX                            ; 1000:792e
    SHL BX,0x1                           ; 1000:7930
    SHL BX,0x1                           ; 1000:7932
    SHL BX,0x1                           ; 1000:7934
    MOV CL,0x5                           ; 1000:7936
    SHL DX,CL                            ; 1000:7938
    ADD BX,DX                            ; 1000:793a
    ADD BX,AX                            ; 1000:793c
    ADD DI,BX                            ; 1000:793e
    LODSW SI                             ; 1000:7940
    SHR AX,0x1                           ; 1000:7941
    SHR AX,0x1                           ; 1000:7943
    SHR AX,0x1                           ; 1000:7945
    MOV DX,AX                            ; 1000:7947
    LODSW SI                             ; 1000:7949
    MOV BX,AX                            ; 1000:794a
    MOV CL,0x0                           ; 1000:794c
    CALL 0x1000:7962                     ; 1000:794e ; -> 1000:7962
    MOV CL,0x1                           ; 1000:7951
    CALL 0x1000:7962                     ; 1000:7953 ; -> 1000:7962
    MOV CL,0x2                           ; 1000:7956
    CALL 0x1000:7962                     ; 1000:7958 ; -> 1000:7962
    MOV CL,0x3                           ; 1000:795b
    CALL 0x1000:7962                     ; 1000:795d ; -> 1000:7962
    POP BP                               ; 1000:7960
    RET                                  ; 1000:7961

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_color_compare  [Video / Hardware]
; ADDRESS:  1000:7962  (Length: 0x2c bytes)
; CALLERS:  FUN_1000_7927
; PURPOSE:  Configures EGA Color Compare register (Port 0x3ce index 2).
; ------------------------------------------------------------------------------
vid_set_color_compare:
    PUSH DX                              ; 1000:7962
    CALL 0x1000:7a68                     ; 1000:7963 ; -> 1000:7a68
    POP DX                               ; 1000:7966
    PUSH DI                              ; 1000:7967
    PUSH BX                              ; 1000:7968
    MOV AX,BX                            ; 1000:7969
    MOV BX,BP                            ; 1000:796b
    PUSH AX                              ; 1000:796d
    PUSH DI                              ; 1000:796e
    MOV CX,DX                            ; 1000:796f
    TEST CX,0x1                          ; 1000:7971
    JZ 0x1000:797b                       ; 1000:7975 ; -> 1000:797b
    MOVSB ES:DI,SI                       ; 1000:7977
    DEC CX                               ; 1000:7978
    JZ 0x1000:7983                       ; 1000:7979 ; -> 1000:7983
    SHR CX,0x1                           ; 1000:797b
    OR CX,CX                             ; 1000:797d
    JZ 0x1000:7983                       ; 1000:797f ; -> 1000:7983
    MOVSW.REP ES:DI,SI                   ; 1000:7981
    POP DI                               ; 1000:7983
    ADD DI,0x28                          ; 1000:7984
    POP AX                               ; 1000:7987
    DEC AX                               ; 1000:7988
    JNZ 0x1000:796d                      ; 1000:7989 ; -> 1000:796d
    POP BX                               ; 1000:798b
    POP DI                               ; 1000:798c
    RET                                  ; 1000:798d

; ------------------------------------------------------------------------------
; FUNCTION: vid_load_default_palette  [Video / Palette]
; ADDRESS:  1000:7990  (Length: 0x9 bytes)
; CALLERS:  FUN_1000_7298
; PURPOSE:  Loads standard outdoor 16-color EGA palette.
; ------------------------------------------------------------------------------
vid_load_default_palette:
    PUSH DS                              ; 1000:7990
    CALL 0x1000:79a2                     ; 1000:7991 ; -> 1000:79a2
    CALL 0x1000:79c7                     ; 1000:7994 ; -> 1000:79c7
    POP DS                               ; 1000:7997
    RET                                  ; 1000:7998

; ------------------------------------------------------------------------------
; FUNCTION: vid_load_alt_palette  [Video / Palette]
; ADDRESS:  1000:7999  (Length: 0x9 bytes)
; CALLERS:  FUN_1000_0185
; PURPOSE:  Loads alternate cavern/underground 16-color EGA palette.
; ------------------------------------------------------------------------------
vid_load_alt_palette:
    PUSH DS                              ; 1000:7999
    CALL 0x1000:79a2                     ; 1000:799a ; -> 1000:79a2
    CALL 0x1000:7a13                     ; 1000:799d ; -> 1000:7a13
    POP DS                               ; 1000:79a0
    RET                                  ; 1000:79a1

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_overscan_color  [Video / Palette]
; ADDRESS:  1000:79a2  (Length: 0x25 bytes)
; CALLERS:  FUN_1000_7999, FUN_1000_7990
; PURPOSE:  Sets screen border/overscan color via BIOS INT 10h AH=0Bh.
; ------------------------------------------------------------------------------
vid_set_overscan_color:
    MOV AX,0x3d00                        ; 1000:79a2
    INT 0x21                             ; 1000:79a5
    JNC 0x1000:79ac                      ; 1000:79a7 ; -> 1000:79ac
    JMP 0x1000:0202                      ; 1000:79a9 ; -> 1000:0202
    MOV BX,AX                            ; 1000:79ac
    MOV AX,0x1d5d                        ; 1000:79ae
    MOV DS,AX                            ; 1000:79b1
    MOV DX,0x600                         ; 1000:79b3
    MOV CX,0x7fff                        ; 1000:79b6
    MOV AX,0x3f00                        ; 1000:79b9
    INT 0x21                             ; 1000:79bc
    MOV AX,0x3e00                        ; 1000:79be
    INT 0x21                             ; 1000:79c1
    MOV SI,0x600                         ; 1000:79c3
    RET                                  ; 1000:79c6

; ------------------------------------------------------------------------------
; FUNCTION: vid_load_palette_buffer  [Video / Palette]
; ADDRESS:  1000:79c7  (Length: 0x26 bytes)
; CALLERS:  FUN_1000_7990, FUN_1000_02b4, FUN_1000_7164, FUN_1000_0185, entry
; PURPOSE:  Loads custom palette array into active palette staging buffer.
; ------------------------------------------------------------------------------
vid_load_palette_buffer:
    LODSW SI                             ; 1000:79c7
    MOV CS:[0x798e],AX                   ; 1000:79c8 ; -> 1000:798e
    MOV CL,0x0                           ; 1000:79cc
    CALL 0x1000:7a68                     ; 1000:79ce ; -> 1000:7a68
    CALL 0x1000:79ed                     ; 1000:79d1 ; -> 1000:79ed
    MOV CL,0x1                           ; 1000:79d4
    CALL 0x1000:7a68                     ; 1000:79d6 ; -> 1000:7a68
    CALL 0x1000:79ed                     ; 1000:79d9 ; -> 1000:79ed
    MOV CL,0x2                           ; 1000:79dc
    CALL 0x1000:7a68                     ; 1000:79de ; -> 1000:7a68
    CALL 0x1000:79ed                     ; 1000:79e1 ; -> 1000:79ed
    MOV CL,0x3                           ; 1000:79e4
    CALL 0x1000:7a68                     ; 1000:79e6 ; -> 1000:7a68
    CALL 0x1000:79ed                     ; 1000:79e9 ; -> 1000:79ed
    RET                                  ; 1000:79ec

; ------------------------------------------------------------------------------
; FUNCTION: vid_cycle_palette_step  [Video / Palette]
; ADDRESS:  1000:79ed  (Length: 0x26 bytes)
; CALLERS:  FUN_1000_79c7
; PURPOSE:  Rotates cycling palette colors by one step to animate waterfalls and energy barriers.
; ------------------------------------------------------------------------------
vid_cycle_palette_step:
    MOV BX,DI                            ; 1000:79ed
    LODSB SI                             ; 1000:79ef
    TEST AL,0x80                         ; 1000:79f0
    JNZ 0x1000:79fc                      ; 1000:79f2 ; -> 1000:79fc
    XOR AH,AH                            ; 1000:79f4
    MOV CX,AX                            ; 1000:79f6
    MOVSB.REP ES:DI,SI                   ; 1000:79f8
    JMP 0x1000:7a05                      ; 1000:79fa ; -> 1000:7a05
    XOR AH,AH                            ; 1000:79fc
    AND AL,0x7f                          ; 1000:79fe
    MOV CX,AX                            ; 1000:7a00
    LODSB SI                             ; 1000:7a02
    STOSB.REP ES:DI                      ; 1000:7a03
    MOV AX,DI                            ; 1000:7a05
    SUB AX,BX                            ; 1000:7a07
    CMP AX,word ptr CS:[0x798e]          ; 1000:7a09 ; -> 1000:798e
    JL 0x1000:79ef                       ; 1000:7a0e ; -> 1000:79ef
    MOV DI,BX                            ; 1000:7a10
    RET                                  ; 1000:7a12

; ------------------------------------------------------------------------------
; FUNCTION: vid_apply_palette  [Video / Palette]
; ADDRESS:  1000:7a13  (Length: 0x26 bytes)
; CALLERS:  FUN_1000_7999, FUN_1000_02b4, entry
; PURPOSE:  Writes palette staging buffer into EGA Attribute Controller palette registers.
; ------------------------------------------------------------------------------
vid_apply_palette:
    LODSW SI                             ; 1000:7a13
    MOV CS:[0x798e],AX                   ; 1000:7a14 ; -> 1000:798e
    MOV CL,0x0                           ; 1000:7a18
    CALL 0x1000:7a68                     ; 1000:7a1a ; -> 1000:7a68
    CALL 0x1000:7a39                     ; 1000:7a1d ; -> 1000:7a39
    MOV CL,0x1                           ; 1000:7a20
    CALL 0x1000:7a68                     ; 1000:7a22 ; -> 1000:7a68
    CALL 0x1000:7a39                     ; 1000:7a25 ; -> 1000:7a39
    MOV CL,0x2                           ; 1000:7a28
    CALL 0x1000:7a68                     ; 1000:7a2a ; -> 1000:7a68
    CALL 0x1000:7a39                     ; 1000:7a2d ; -> 1000:7a39
    MOV CL,0x3                           ; 1000:7a30
    CALL 0x1000:7a68                     ; 1000:7a32 ; -> 1000:7a68
    CALL 0x1000:7a39                     ; 1000:7a35 ; -> 1000:7a39
    RET                                  ; 1000:7a38

; ------------------------------------------------------------------------------
; FUNCTION: vid_dim_palette  [Video / Palette]
; ADDRESS:  1000:7a39  (Length: 0x2f bytes)
; CALLERS:  FUN_1000_7a13
; PURPOSE:  Dims palette colors toward black for screen fade-out.
; ------------------------------------------------------------------------------
vid_dim_palette:
    MOV BX,DI                            ; 1000:7a39
    LODSB SI                             ; 1000:7a3b
    TEST AL,0x80                         ; 1000:7a3c
    JNZ 0x1000:7a4d                      ; 1000:7a3e ; -> 1000:7a4d
    XOR AH,AH                            ; 1000:7a40
    MOV CX,AX                            ; 1000:7a42
    LODSB SI                             ; 1000:7a44
    OR byte ptr ES:[DI],AL               ; 1000:7a45
    INC DI                               ; 1000:7a48
    LOOP 0x1000:7a44                     ; 1000:7a49 ; -> 1000:7a44
    JMP 0x1000:7a5a                      ; 1000:7a4b ; -> 1000:7a5a
    XOR AH,AH                            ; 1000:7a4d
    AND AL,0x7f                          ; 1000:7a4f
    MOV CX,AX                            ; 1000:7a51
    LODSB SI                             ; 1000:7a53
    OR byte ptr ES:[DI],AL               ; 1000:7a54
    INC DI                               ; 1000:7a57
    LOOP 0x1000:7a54                     ; 1000:7a58 ; -> 1000:7a54
    MOV AX,DI                            ; 1000:7a5a
    SUB AX,BX                            ; 1000:7a5c
    CMP AX,word ptr CS:[0x798e]          ; 1000:7a5e ; -> 1000:798e
    JL 0x1000:7a3b                       ; 1000:7a63 ; -> 1000:7a3b
    MOV DI,BX                            ; 1000:7a65
    RET                                  ; 1000:7a67

; ------------------------------------------------------------------------------
; FUNCTION: vid_out_dx_ax  [Video / Hardware]
; ADDRESS:  1000:7a68  (Length: 0x21 bytes)
; CALLERS:  FUN_1000_7ccd, FUN_1000_7765, FUN_1000_7b03, FUN_1000_7849, FUN_1000_7b8b, FUN_1000_1f89, FUN_1000_752a, FUN_1000_7bef, FUN_1000_1f72, FUN_1000_79c7, FUN_1000_7d31, FUN_1000_77f2, FUN_1000_1dc0, FUN_1000_7962, FUN_1000_7a13
; PURPOSE:  Helper routine to write index and data byte simultaneously to EGA I/O port.
; ------------------------------------------------------------------------------
vid_out_dx_ax:
    MOV AH,0x1                           ; 1000:7a68
    SHL AH,CL                            ; 1000:7a6a
    MOV AL,0x2                           ; 1000:7a6c
    MOV DX,0x3c4                         ; 1000:7a6e
    OUT DX,AL                            ; 1000:7a71
    INC DX                               ; 1000:7a72
    XCHG AL,AH                           ; 1000:7a73
    OUT DX,AL                            ; 1000:7a75
    DEC DX                               ; 1000:7a76
    XCHG AL,AH                           ; 1000:7a77
    MOV AH,CL                            ; 1000:7a79
    MOV AL,0x4                           ; 1000:7a7b
    MOV DX,0x3ce                         ; 1000:7a7d
    OUT DX,AL                            ; 1000:7a80
    INC DX                               ; 1000:7a81
    XCHG AL,AH                           ; 1000:7a82
    OUT DX,AL                            ; 1000:7a84
    DEC DX                               ; 1000:7a85
    XCHG AL,AH                           ; 1000:7a86
    RET                                  ; 1000:7a88

; ------------------------------------------------------------------------------
; FUNCTION: vid_in_status  [Video / Hardware]
; ADDRESS:  1000:7a89  (Length: 0x22 bytes)
; CALLERS:  FUN_1000_774e, FUN_1000_7741
; PURPOSE:  Reads EGA Input Status Register 1 from Port 0x3da.
; ------------------------------------------------------------------------------
vid_in_status:
    MOV DX,0x3da                         ; 1000:7a89
    IN AL,DX                             ; 1000:7a8c
    TEST AL,0x8                          ; 1000:7a8d
    JNZ 0x1000:7a8c                      ; 1000:7a8f ; -> 1000:7a8c
    MOV CX,AX                            ; 1000:7a91
    MOV DX,0x3d4                         ; 1000:7a93
    MOV AL,0xc                           ; 1000:7a96
    MOV AH,CH                            ; 1000:7a98
    OUT DX,AL                            ; 1000:7a9a
    INC DX                               ; 1000:7a9b
    XCHG AL,AH                           ; 1000:7a9c
    OUT DX,AL                            ; 1000:7a9e
    DEC DX                               ; 1000:7a9f
    XCHG AL,AH                           ; 1000:7aa0
    MOV DX,0x3da                         ; 1000:7aa2
    IN AL,DX                             ; 1000:7aa5
    TEST AL,0x8                          ; 1000:7aa6
    JZ 0x1000:7aa5                       ; 1000:7aa8 ; -> 1000:7aa5
    RET                                  ; 1000:7aaa

; ------------------------------------------------------------------------------
; FUNCTION: vid_setup_blit_rect  [Video / Blitting]
; ADDRESS:  1000:7aab  (Length: 0x1d bytes)
; CALLERS:  FUN_1000_451c
; PURPOSE:  Calculates destination VRAM address and row strides for rectangular blit operation.
; ------------------------------------------------------------------------------
vid_setup_blit_rect:
    MOV DI,word ptr CS:[0x773f]          ; 1000:7aab ; -> 1000:773f
    PUSH AX                              ; 1000:7ab0
    PUSH BX                              ; 1000:7ab1
    PUSH CX                              ; 1000:7ab2
    PUSH DX                              ; 1000:7ab3
    CALL 0x1000:7ac8                     ; 1000:7ab4 ; -> 1000:7ac8
    MOV DI,word ptr CS:[0x773f]          ; 1000:7ab7 ; -> 1000:773f
    XOR DI,0x2000                        ; 1000:7abc
    POP DX                               ; 1000:7ac0
    POP CX                               ; 1000:7ac1
    POP BX                               ; 1000:7ac2
    POP AX                               ; 1000:7ac3
    CALL 0x1000:7ac8                     ; 1000:7ac4 ; -> 1000:7ac8
    RET                                  ; 1000:7ac7

; ------------------------------------------------------------------------------
; FUNCTION: vid_set_data_rotate  [Video / Hardware]
; ADDRESS:  1000:7ac8  (Length: 0x3b bytes)
; CALLERS:  FUN_1000_6f47, FUN_1000_4206, FUN_1000_677a, FUN_1000_7057, FUN_1000_61c8, FUN_1000_7aab
; PURPOSE:  Configures EGA Data Rotate / Function Select register (Port 0x3ce index 3).
; ------------------------------------------------------------------------------
vid_set_data_rotate:
    PUSH DX                              ; 1000:7ac8
    PUSH CX                              ; 1000:7ac9
    SHR AX,0x1                           ; 1000:7aca
    SHR AX,0x1                           ; 1000:7acc
    SHR AX,0x1                           ; 1000:7ace
    MOV DX,BX                            ; 1000:7ad0
    SHL BX,0x1                           ; 1000:7ad2
    SHL BX,0x1                           ; 1000:7ad4
    SHL BX,0x1                           ; 1000:7ad6
    MOV CL,0x5                           ; 1000:7ad8
    SHL DX,CL                            ; 1000:7ada
    ADD BX,DX                            ; 1000:7adc
    ADD BX,AX                            ; 1000:7ade
    ADD DI,BX                            ; 1000:7ae0
    POP AX                               ; 1000:7ae2
    SHR AX,0x1                           ; 1000:7ae3
    SHR AX,0x1                           ; 1000:7ae5
    SHR AX,0x1                           ; 1000:7ae7
    MOV DX,AX                            ; 1000:7ae9
    POP AX                               ; 1000:7aeb
    MOV BX,AX                            ; 1000:7aec
    MOV CL,0x0                           ; 1000:7aee
    CALL 0x1000:7b03                     ; 1000:7af0 ; -> 1000:7b03
    MOV CL,0x1                           ; 1000:7af3
    CALL 0x1000:7b03                     ; 1000:7af5 ; -> 1000:7b03
    MOV CL,0x2                           ; 1000:7af8
    CALL 0x1000:7b03                     ; 1000:7afa ; -> 1000:7b03
    MOV CL,0x3                           ; 1000:7afd
    CALL 0x1000:7b03                     ; 1000:7aff ; -> 1000:7b03
    RET                                  ; 1000:7b02

; ------------------------------------------------------------------------------
; FUNCTION: vid_reset_data_rotate  [Video / Hardware]
; ADDRESS:  1000:7b03  (Length: 0x2c bytes)
; CALLERS:  FUN_1000_7ac8
; PURPOSE:  Resets EGA Data Rotate register to default direct copy mode.
; ------------------------------------------------------------------------------
vid_reset_data_rotate:
    PUSH DX                              ; 1000:7b03
    CALL 0x1000:7a68                     ; 1000:7b04 ; -> 1000:7a68
    POP DX                               ; 1000:7b07
    PUSH DI                              ; 1000:7b08
    PUSH BX                              ; 1000:7b09
    MOV AX,BX                            ; 1000:7b0a
    PUSH AX                              ; 1000:7b0c
    XOR AX,AX                            ; 1000:7b0d
    PUSH DI                              ; 1000:7b0f
    MOV CX,DX                            ; 1000:7b10
    TEST CX,0x1                          ; 1000:7b12
    JZ 0x1000:7b1c                       ; 1000:7b16 ; -> 1000:7b1c
    STOSB ES:DI                          ; 1000:7b18
    DEC CX                               ; 1000:7b19
    JZ 0x1000:7b24                       ; 1000:7b1a ; -> 1000:7b24
    SHR CX,0x1                           ; 1000:7b1c
    OR CX,CX                             ; 1000:7b1e
    JZ 0x1000:7b24                       ; 1000:7b20 ; -> 1000:7b24
    STOSW.REP ES:DI                      ; 1000:7b22
    POP DI                               ; 1000:7b24
    ADD DI,0x28                          ; 1000:7b25
    POP AX                               ; 1000:7b28
    DEC AX                               ; 1000:7b29
    JNZ 0x1000:7b0c                      ; 1000:7b2a ; -> 1000:7b0c
    POP BX                               ; 1000:7b2c
    POP DI                               ; 1000:7b2d
    RET                                  ; 1000:7b2e

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_tile_aligned  [Video / Blitting]
; ADDRESS:  1000:7b31  (Length: 0x59 bytes)
; CALLERS:  FUN_1000_7dbb
; PURPOSE:  Fast 16x16 tile blitter for byte-aligned horizontal positions.
; ------------------------------------------------------------------------------
vid_blit_tile_aligned:
    PUSH BP                              ; 1000:7b31
    PUSH AX                              ; 1000:7b32
    PUSH CX                              ; 1000:7b33
    SHR AX,0x1                           ; 1000:7b34
    SHR AX,0x1                           ; 1000:7b36
    SHR AX,0x1                           ; 1000:7b38
    MOV DX,BX                            ; 1000:7b3a
    SHL BX,0x1                           ; 1000:7b3c
    SHL BX,0x1                           ; 1000:7b3e
    SHL BX,0x1                           ; 1000:7b40
    MOV CL,0x5                           ; 1000:7b42
    SHL DX,CL                            ; 1000:7b44
    ADD BX,DX                            ; 1000:7b46
    ADD BX,AX                            ; 1000:7b48
    MOV DI,word ptr CS:[0x773f]          ; 1000:7b4a ; -> 1000:773f
    ADD DI,BX                            ; 1000:7b4f
    LODSW SI                             ; 1000:7b51
    SHR AX,0x1                           ; 1000:7b52
    SHR AX,0x1                           ; 1000:7b54
    SHR AX,0x1                           ; 1000:7b56
    MOV DX,AX                            ; 1000:7b58
    LODSW SI                             ; 1000:7b5a
    MOV BX,AX                            ; 1000:7b5b
    LODSW SI                             ; 1000:7b5d
    MOV BP,SI                            ; 1000:7b5e
    ADD SI,AX                            ; 1000:7b60
    POP AX                               ; 1000:7b62
    MOV CX,BX                            ; 1000:7b63
    SUB CX,AX                            ; 1000:7b65
    MOV word ptr CS:[0x7b2f],CX          ; 1000:7b67 ; -> 1000:7b2f
    POP AX                               ; 1000:7b6c
    TEST AX,0x4                          ; 1000:7b6d
    JZ 0x1000:7b75                       ; 1000:7b70 ; -> 1000:7b75
    JMP 0x1000:7bd9                      ; 1000:7b72 ; -> 1000:7bd9
    MOV CL,0x0                           ; 1000:7b75
    CALL 0x1000:7b8b                     ; 1000:7b77 ; -> 1000:7b8b
    MOV CL,0x1                           ; 1000:7b7a
    CALL 0x1000:7b8b                     ; 1000:7b7c ; -> 1000:7b8b
    MOV CL,0x2                           ; 1000:7b7f
    CALL 0x1000:7b8b                     ; 1000:7b81 ; -> 1000:7b8b
    MOV CL,0x3                           ; 1000:7b84
    CALL 0x1000:7b8b                     ; 1000:7b86 ; -> 1000:7b8b
    POP BP                               ; 1000:7b89
    RET                                  ; 1000:7b8a

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_tile_shifted  [Video / Blitting]
; ADDRESS:  1000:7b8b  (Length: 0x4e bytes)
; CALLERS:  FUN_1000_7b31
; PURPOSE:  16x16 tile blitter with bit-shift rotation for smooth pixel-accurate horizontal scrolling.
; ------------------------------------------------------------------------------
vid_blit_tile_shifted:
    PUSH DX                              ; 1000:7b8b
    CALL 0x1000:7a68                     ; 1000:7b8c ; -> 1000:7a68
    POP DX                               ; 1000:7b8f
    PUSH DI                              ; 1000:7b90
    PUSH BX                              ; 1000:7b91
    MOV AX,BX                            ; 1000:7b92
    MOV BX,BP                            ; 1000:7b94
    PUSH AX                              ; 1000:7b96
    PUSH DI                              ; 1000:7b97
    MOV CX,DX                            ; 1000:7b98
    CMP AX,word ptr CS:[0x7b2f]          ; 1000:7b9a ; -> 1000:7b2f
    JLE 0x1000:7ba7                      ; 1000:7b9f ; -> 1000:7ba7
    ADD BX,CX                            ; 1000:7ba1
    ADD SI,CX                            ; 1000:7ba3
    JMP 0x1000:7bce                      ; 1000:7ba5 ; -> 1000:7bce
    CMP CX,0x1                           ; 1000:7ba7
    JZ 0x1000:7bc4                       ; 1000:7baa ; -> 1000:7bc4
    MOV AX,word ptr [BX]                 ; 1000:7bac
    ADD BX,0x2                           ; 1000:7bae
    AND word ptr ES:[DI],AX              ; 1000:7bb1
    LODSW SI                             ; 1000:7bb4
    OR word ptr ES:[DI],AX               ; 1000:7bb5
    ADD DI,0x2                           ; 1000:7bb8
    SUB CX,0x2                           ; 1000:7bbb
    OR CX,CX                             ; 1000:7bbe
    JNZ 0x1000:7ba7                      ; 1000:7bc0 ; -> 1000:7ba7
    JMP 0x1000:7bce                      ; 1000:7bc2 ; -> 1000:7bce
    MOV AL,byte ptr [BX]                 ; 1000:7bc4
    INC BX                               ; 1000:7bc6
    AND byte ptr ES:[DI],AL              ; 1000:7bc7
    LODSB SI                             ; 1000:7bca
    OR byte ptr ES:[DI],AL               ; 1000:7bcb
    POP DI                               ; 1000:7bce
    ADD DI,0x28                          ; 1000:7bcf
    POP AX                               ; 1000:7bd2
    DEC AX                               ; 1000:7bd3
    JNZ 0x1000:7b96                      ; 1000:7bd4 ; -> 1000:7b96
    POP BX                               ; 1000:7bd6
    POP DI                               ; 1000:7bd7
    RET                                  ; 1000:7bd8

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_glyph_aligned  [Video / Blitting]
; ADDRESS:  1000:7bd9  (Length: 0x16 bytes)
; CALLERS:  FUN_1000_7b31
; PURPOSE:  8x8 font glyph blitter aligned to byte boundaries.
; ------------------------------------------------------------------------------
vid_blit_glyph_aligned:
    MOV CL,0x0                           ; 1000:7bd9
    CALL 0x1000:7bef                     ; 1000:7bdb ; -> 1000:7bef
    MOV CL,0x1                           ; 1000:7bde
    CALL 0x1000:7bef                     ; 1000:7be0 ; -> 1000:7bef
    MOV CL,0x2                           ; 1000:7be3
    CALL 0x1000:7bef                     ; 1000:7be5 ; -> 1000:7bef
    MOV CL,0x3                           ; 1000:7be8
    CALL 0x1000:7bef                     ; 1000:7bea ; -> 1000:7bef
    POP BP                               ; 1000:7bed
    RET                                  ; 1000:7bee

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_glyph_shifted  [Video / Blitting]
; ADDRESS:  1000:7bef  (Length: 0x8a bytes)
; CALLERS:  FUN_1000_7bd9
; PURPOSE:  8x8 font glyph blitter with arbitrary pixel X shift.
; ------------------------------------------------------------------------------
vid_blit_glyph_shifted:
    PUSH DX                              ; 1000:7bef
    CALL 0x1000:7a68                     ; 1000:7bf0 ; -> 1000:7a68
    POP DX                               ; 1000:7bf3
    PUSH DI                              ; 1000:7bf4
    PUSH BX                              ; 1000:7bf5
    PUSH BP                              ; 1000:7bf6
    MOV AX,BX                            ; 1000:7bf7
    MOV BX,BP                            ; 1000:7bf9
    PUSH AX                              ; 1000:7bfb
    PUSH DI                              ; 1000:7bfc
    MOV CX,DX                            ; 1000:7bfd
    CMP AX,word ptr CS:[0x7b2f]          ; 1000:7bff ; -> 1000:7b2f
    JLE 0x1000:7c0c                      ; 1000:7c04 ; -> 1000:7c0c
    ADD BX,CX                            ; 1000:7c06
    ADD SI,CX                            ; 1000:7c08
    JMP 0x1000:7c6d                      ; 1000:7c0a ; -> 1000:7c6d
    XOR BP,BP                            ; 1000:7c0c
    MOV AX,0xffff                        ; 1000:7c0e
    MOV AL,byte ptr [BX]                 ; 1000:7c11
    INC BX                               ; 1000:7c13
    SHL AX,0x1                           ; 1000:7c14
    SHL AX,0x1                           ; 1000:7c16
    SHL AX,0x1                           ; 1000:7c18
    SHL AX,0x1                           ; 1000:7c1a
    AND byte ptr ES:[DI],AH              ; 1000:7c1c
    XCHG AX,BP                           ; 1000:7c1f
    LODSB SI                             ; 1000:7c20
    SHL AX,0x1                           ; 1000:7c21
    SHL AX,0x1                           ; 1000:7c23
    SHL AX,0x1                           ; 1000:7c25
    SHL AX,0x1                           ; 1000:7c27
    OR byte ptr ES:[DI],AH               ; 1000:7c29
    INC DI                               ; 1000:7c2c
    XCHG AX,BP                           ; 1000:7c2d
    CMP CX,0x1                           ; 1000:7c2e
    JZ 0x1000:7c63                       ; 1000:7c31 ; -> 1000:7c63
    SHL AX,0x1                           ; 1000:7c33
    SHL AX,0x1                           ; 1000:7c35
    SHL AX,0x1                           ; 1000:7c37
    SHL AX,0x1                           ; 1000:7c39
    MOV AL,byte ptr [BX]                 ; 1000:7c3b
    INC BX                               ; 1000:7c3d
    SHL AX,0x1                           ; 1000:7c3e
    SHL AX,0x1                           ; 1000:7c40
    SHL AX,0x1                           ; 1000:7c42
    SHL AX,0x1                           ; 1000:7c44
    AND byte ptr ES:[DI],AH              ; 1000:7c46
    XCHG AX,BP                           ; 1000:7c49
    SHL AX,0x1                           ; 1000:7c4a
    SHL AX,0x1                           ; 1000:7c4c
    SHL AX,0x1                           ; 1000:7c4e
    SHL AX,0x1                           ; 1000:7c50
    LODSB SI                             ; 1000:7c52
    SHL AX,0x1                           ; 1000:7c53
    SHL AX,0x1                           ; 1000:7c55
    SHL AX,0x1                           ; 1000:7c57
    SHL AX,0x1                           ; 1000:7c59
    OR byte ptr ES:[DI],AH               ; 1000:7c5b
    INC DI                               ; 1000:7c5e
    XCHG AX,BP                           ; 1000:7c5f
    DEC CX                               ; 1000:7c60
    JMP 0x1000:7c2e                      ; 1000:7c61 ; -> 1000:7c2e
    OR AL,0xf                            ; 1000:7c63
    AND byte ptr ES:[DI],AL              ; 1000:7c65
    MOV AX,BP                            ; 1000:7c68
    OR byte ptr ES:[DI],AL               ; 1000:7c6a
    POP DI                               ; 1000:7c6d
    ADD DI,0x28                          ; 1000:7c6e
    POP AX                               ; 1000:7c71
    DEC AX                               ; 1000:7c72
    JNZ 0x1000:7bfb                      ; 1000:7c73 ; -> 1000:7bfb
    POP BP                               ; 1000:7c75
    POP BX                               ; 1000:7c76
    POP DI                               ; 1000:7c77
    RET                                  ; 1000:7c78

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_sprite_masked_aligned  [Video / Blitting]
; ADDRESS:  1000:7c79  (Length: 0x53 bytes)
; CALLERS:  FUN_1000_7dbb
; PURPOSE:  Masked 16x16 sprite blitter aligned to byte boundaries.
; ------------------------------------------------------------------------------
vid_blit_sprite_masked_aligned:
    PUSH BP                              ; 1000:7c79
    PUSH AX                              ; 1000:7c7a
    MOV word ptr CS:[0x7b2f],CX          ; 1000:7c7b ; -> 1000:7b2f
    SHR AX,0x1                           ; 1000:7c80
    SHR AX,0x1                           ; 1000:7c82
    SHR AX,0x1                           ; 1000:7c84
    MOV DX,BX                            ; 1000:7c86
    SHL BX,0x1                           ; 1000:7c88
    SHL BX,0x1                           ; 1000:7c8a
    SHL BX,0x1                           ; 1000:7c8c
    MOV CL,0x5                           ; 1000:7c8e
    SHL DX,CL                            ; 1000:7c90
    ADD BX,DX                            ; 1000:7c92
    ADD BX,AX                            ; 1000:7c94
    MOV DI,word ptr CS:[0x773f]          ; 1000:7c96 ; -> 1000:773f
    ADD DI,BX                            ; 1000:7c9b
    LODSW SI                             ; 1000:7c9d
    SHR AX,0x1                           ; 1000:7c9e
    SHR AX,0x1                           ; 1000:7ca0
    SHR AX,0x1                           ; 1000:7ca2
    MOV DX,AX                            ; 1000:7ca4
    LODSW SI                             ; 1000:7ca6
    MOV BX,AX                            ; 1000:7ca7
    LODSW SI                             ; 1000:7ca9
    MOV BP,SI                            ; 1000:7caa
    ADD SI,AX                            ; 1000:7cac
    POP AX                               ; 1000:7cae
    TEST AX,0x4                          ; 1000:7caf
    JZ 0x1000:7cb7                       ; 1000:7cb2 ; -> 1000:7cb7
    JMP 0x1000:7d1b                      ; 1000:7cb4 ; -> 1000:7d1b
    MOV CL,0x0                           ; 1000:7cb7
    CALL 0x1000:7ccd                     ; 1000:7cb9 ; -> 1000:7ccd
    MOV CL,0x1                           ; 1000:7cbc
    CALL 0x1000:7ccd                     ; 1000:7cbe ; -> 1000:7ccd
    MOV CL,0x2                           ; 1000:7cc1
    CALL 0x1000:7ccd                     ; 1000:7cc3 ; -> 1000:7ccd
    MOV CL,0x3                           ; 1000:7cc6
    CALL 0x1000:7ccd                     ; 1000:7cc8 ; -> 1000:7ccd
    POP BP                               ; 1000:7ccb
    RET                                  ; 1000:7ccc

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_sprite_masked_shifted  [Video / Blitting]
; ADDRESS:  1000:7ccd  (Length: 0x4e bytes)
; CALLERS:  FUN_1000_7c79
; PURPOSE:  Masked 16x16 sprite blitter with arbitrary pixel X shift.
; ------------------------------------------------------------------------------
vid_blit_sprite_masked_shifted:
    PUSH DX                              ; 1000:7ccd
    CALL 0x1000:7a68                     ; 1000:7cce ; -> 1000:7a68
    POP DX                               ; 1000:7cd1
    PUSH DI                              ; 1000:7cd2
    PUSH BX                              ; 1000:7cd3
    MOV AX,BX                            ; 1000:7cd4
    MOV BX,BP                            ; 1000:7cd6
    PUSH AX                              ; 1000:7cd8
    PUSH DI                              ; 1000:7cd9
    MOV CX,DX                            ; 1000:7cda
    CMP AX,word ptr CS:[0x7b2f]          ; 1000:7cdc ; -> 1000:7b2f
    JG 0x1000:7ce9                       ; 1000:7ce1 ; -> 1000:7ce9
    ADD BX,CX                            ; 1000:7ce3
    ADD SI,CX                            ; 1000:7ce5
    JMP 0x1000:7d10                      ; 1000:7ce7 ; -> 1000:7d10
    CMP CX,0x1                           ; 1000:7ce9
    JZ 0x1000:7d06                       ; 1000:7cec ; -> 1000:7d06
    MOV AX,word ptr [BX]                 ; 1000:7cee
    ADD BX,0x2                           ; 1000:7cf0
    AND word ptr ES:[DI],AX              ; 1000:7cf3
    LODSW SI                             ; 1000:7cf6
    OR word ptr ES:[DI],AX               ; 1000:7cf7
    ADD DI,0x2                           ; 1000:7cfa
    SUB CX,0x2                           ; 1000:7cfd
    OR CX,CX                             ; 1000:7d00
    JNZ 0x1000:7ce9                      ; 1000:7d02 ; -> 1000:7ce9
    JMP 0x1000:7d10                      ; 1000:7d04 ; -> 1000:7d10
    MOV AL,byte ptr [BX]                 ; 1000:7d06
    INC BX                               ; 1000:7d08
    AND byte ptr ES:[DI],AL              ; 1000:7d09
    LODSB SI                             ; 1000:7d0c
    OR byte ptr ES:[DI],AL               ; 1000:7d0d
    POP DI                               ; 1000:7d10
    ADD DI,0x28                          ; 1000:7d11
    POP AX                               ; 1000:7d14
    DEC AX                               ; 1000:7d15
    JNZ 0x1000:7cd8                      ; 1000:7d16 ; -> 1000:7cd8
    POP BX                               ; 1000:7d18
    POP DI                               ; 1000:7d19
    RET                                  ; 1000:7d1a

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_small_sprite_aligned  [Video / Blitting]
; ADDRESS:  1000:7d1b  (Length: 0x16 bytes)
; CALLERS:  FUN_1000_7c79
; PURPOSE:  Small 8x8 projectile/bullet sprite blitter aligned to byte boundaries.
; ------------------------------------------------------------------------------
vid_blit_small_sprite_aligned:
    MOV CL,0x0                           ; 1000:7d1b
    CALL 0x1000:7d31                     ; 1000:7d1d ; -> 1000:7d31
    MOV CL,0x1                           ; 1000:7d20
    CALL 0x1000:7d31                     ; 1000:7d22 ; -> 1000:7d31
    MOV CL,0x2                           ; 1000:7d25
    CALL 0x1000:7d31                     ; 1000:7d27 ; -> 1000:7d31
    MOV CL,0x3                           ; 1000:7d2a
    CALL 0x1000:7d31                     ; 1000:7d2c ; -> 1000:7d31
    POP BP                               ; 1000:7d2f
    RET                                  ; 1000:7d30

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_small_sprite_shifted  [Video / Blitting]
; ADDRESS:  1000:7d31  (Length: 0x8a bytes)
; CALLERS:  FUN_1000_7d1b
; PURPOSE:  Small 8x8 projectile/bullet sprite blitter with pixel shift.
; ------------------------------------------------------------------------------
vid_blit_small_sprite_shifted:
    PUSH DX                              ; 1000:7d31
    CALL 0x1000:7a68                     ; 1000:7d32 ; -> 1000:7a68
    POP DX                               ; 1000:7d35
    PUSH DI                              ; 1000:7d36
    PUSH BX                              ; 1000:7d37
    PUSH BP                              ; 1000:7d38
    MOV AX,BX                            ; 1000:7d39
    MOV BX,BP                            ; 1000:7d3b
    PUSH AX                              ; 1000:7d3d
    PUSH DI                              ; 1000:7d3e
    MOV CX,DX                            ; 1000:7d3f
    CMP AX,word ptr CS:[0x7b2f]          ; 1000:7d41 ; -> 1000:7b2f
    JG 0x1000:7d4e                       ; 1000:7d46 ; -> 1000:7d4e
    ADD BX,CX                            ; 1000:7d48
    ADD SI,CX                            ; 1000:7d4a
    JMP 0x1000:7daf                      ; 1000:7d4c ; -> 1000:7daf
    XOR BP,BP                            ; 1000:7d4e
    MOV AX,0xffff                        ; 1000:7d50
    MOV AL,byte ptr [BX]                 ; 1000:7d53
    INC BX                               ; 1000:7d55
    SHL AX,0x1                           ; 1000:7d56
    SHL AX,0x1                           ; 1000:7d58
    SHL AX,0x1                           ; 1000:7d5a
    SHL AX,0x1                           ; 1000:7d5c
    AND byte ptr ES:[DI],AH              ; 1000:7d5e
    XCHG AX,BP                           ; 1000:7d61
    LODSB SI                             ; 1000:7d62
    SHL AX,0x1                           ; 1000:7d63
    SHL AX,0x1                           ; 1000:7d65
    SHL AX,0x1                           ; 1000:7d67
    SHL AX,0x1                           ; 1000:7d69
    OR byte ptr ES:[DI],AH               ; 1000:7d6b
    INC DI                               ; 1000:7d6e
    XCHG AX,BP                           ; 1000:7d6f
    CMP CX,0x1                           ; 1000:7d70
    JZ 0x1000:7da5                       ; 1000:7d73 ; -> 1000:7da5
    SHL AX,0x1                           ; 1000:7d75
    SHL AX,0x1                           ; 1000:7d77
    SHL AX,0x1                           ; 1000:7d79
    SHL AX,0x1                           ; 1000:7d7b
    MOV AL,byte ptr [BX]                 ; 1000:7d7d
    INC BX                               ; 1000:7d7f
    SHL AX,0x1                           ; 1000:7d80
    SHL AX,0x1                           ; 1000:7d82
    SHL AX,0x1                           ; 1000:7d84
    SHL AX,0x1                           ; 1000:7d86
    AND byte ptr ES:[DI],AH              ; 1000:7d88
    XCHG AX,BP                           ; 1000:7d8b
    SHL AX,0x1                           ; 1000:7d8c
    SHL AX,0x1                           ; 1000:7d8e
    SHL AX,0x1                           ; 1000:7d90
    SHL AX,0x1                           ; 1000:7d92
    LODSB SI                             ; 1000:7d94
    SHL AX,0x1                           ; 1000:7d95
    SHL AX,0x1                           ; 1000:7d97
    SHL AX,0x1                           ; 1000:7d99
    SHL AX,0x1                           ; 1000:7d9b
    OR byte ptr ES:[DI],AH               ; 1000:7d9d
    INC DI                               ; 1000:7da0
    XCHG AX,BP                           ; 1000:7da1
    DEC CX                               ; 1000:7da2
    JMP 0x1000:7d70                      ; 1000:7da3 ; -> 1000:7d70
    OR AL,0xf                            ; 1000:7da5
    AND byte ptr ES:[DI],AL              ; 1000:7da7
    MOV AX,BP                            ; 1000:7daa
    OR byte ptr ES:[DI],AL               ; 1000:7dac
    POP DI                               ; 1000:7daf
    ADD DI,0x28                          ; 1000:7db0
    POP AX                               ; 1000:7db3
    DEC AX                               ; 1000:7db4
    JNZ 0x1000:7d3d                      ; 1000:7db5 ; -> 1000:7d3d
    POP BP                               ; 1000:7db7
    POP BX                               ; 1000:7db8
    POP DI                               ; 1000:7db9
    RET                                  ; 1000:7dba

; ------------------------------------------------------------------------------
; FUNCTION: vid_blit_planar_block  [Video / Blitting]
; ADDRESS:  1000:7dbb  (Length: 0x8f bytes)
; CALLERS:  FUN_1000_3dfb, FUN_1000_409c, FUN_1000_437b, FUN_1000_378e, FUN_1000_2341, FUN_1000_5090
; PURPOSE:  Universal planar block blitter: blits arbitrary width and height rectangles across all 4 EGA planes.
; ------------------------------------------------------------------------------
vid_blit_planar_block:
    PUSH BP                              ; 1000:7dbb
    MOV BP,SP                            ; 1000:7dbc
    SUB SP,0x4                           ; 1000:7dbe
    MOV DS,word ptr CS:[0x5e]            ; 1000:7dc1 ; -> 1000:005e
    MOV CX,word ptr [SI]                 ; 1000:7dc6
    MOV word ptr [BP + -0x2],CX          ; 1000:7dc8 ; -> Stack[-0x4]
    MOV CX,word ptr [SI + 0x2]           ; 1000:7dcb
    MOV word ptr [BP + -0x4],CX          ; 1000:7dce ; -> Stack[-0x6]
    MOV DS,word ptr CS:[0x5c]            ; 1000:7dd1 ; -> 1000:005c
    SUB AX,word ptr [0x6c2]              ; 1000:7dd6 ; -> 34fe:06c2
    JL 0x1000:7e3f                       ; 1000:7dda ; -> 1000:7e3f
    MOV CX,AX                            ; 1000:7ddc
    ADD CX,word ptr [BP + -0x2]          ; 1000:7dde ; -> Stack[-0x4]
    CMP CX,0xd0                          ; 1000:7de1
    JG 0x1000:7e3f                       ; 1000:7de5 ; -> 1000:7e3f
    ADD AX,0x8                           ; 1000:7de7
    SUB BX,word ptr [0x6c4]              ; 1000:7dea ; -> 34fe:06c4
    JGE 0x1000:7e0b                      ; 1000:7dee ; -> 1000:7e0b
    MOV CX,BX                            ; 1000:7df0
    NEG CX                               ; 1000:7df2
    CMP CX,word ptr [BP + -0x4]          ; 1000:7df4 ; -> Stack[-0x6]
    JGE 0x1000:7e3f                      ; 1000:7df7 ; -> 1000:7e3f
    ADD BX,0x8                           ; 1000:7df9
    MOV DS,word ptr CS:[0x5e]            ; 1000:7dfc ; -> 1000:005e
    CALL 0x1000:7b31                     ; 1000:7e01 ; -> 1000:7b31
    MOV DS,word ptr CS:[0x5c]            ; 1000:7e04 ; -> 1000:005c
    JMP 0x1000:7e43                      ; 1000:7e09 ; -> 1000:7e43
    MOV CX,BX                            ; 1000:7e0b
    ADD CX,word ptr [BP + -0x4]          ; 1000:7e0d ; -> Stack[-0x6]
    SUB CX,0xa0                          ; 1000:7e10
    JLE 0x1000:7e2d                      ; 1000:7e14 ; -> 1000:7e2d
    CMP CX,word ptr [BP + -0x4]          ; 1000:7e16 ; -> Stack[-0x6]
    JGE 0x1000:7e3f                      ; 1000:7e19 ; -> 1000:7e3f
    ADD BX,0x8                           ; 1000:7e1b
    MOV DS,word ptr CS:[0x5e]            ; 1000:7e1e ; -> 1000:005e
    CALL 0x1000:7c79                     ; 1000:7e23 ; -> 1000:7c79
    MOV DS,word ptr CS:[0x5c]            ; 1000:7e26 ; -> 1000:005c
    JMP 0x1000:7e43                      ; 1000:7e2b ; -> 1000:7e43
    ADD BX,0x8                           ; 1000:7e2d
    MOV DS,word ptr CS:[0x5e]            ; 1000:7e30 ; -> 1000:005e
    CALL 0x1000:77a3                     ; 1000:7e35 ; -> 1000:77a3
    MOV DS,word ptr CS:[0x5c]            ; 1000:7e38 ; -> 1000:005c
    JMP 0x1000:7e43                      ; 1000:7e3d ; -> 1000:7e43
    XOR AX,AX                            ; 1000:7e3f
    JMP 0x1000:7e46                      ; 1000:7e41 ; -> 1000:7e46
    MOV AX,0x1                           ; 1000:7e43
    MOV SP,BP                            ; 1000:7e46
    POP BP                               ; 1000:7e48
    RET                                  ; 1000:7e49

