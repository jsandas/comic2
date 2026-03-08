typedef unsigned char   undefined;

typedef unsigned char    undefined1;
typedef unsigned int    undefined2;
typedef unsigned int    word;
typedef struct OLD_IMAGE_DOS_RELOC OLD_IMAGE_DOS_RELOC, *POLD_IMAGE_DOS_RELOC;

struct OLD_IMAGE_DOS_RELOC {
    word offset;
    word segment;
};

typedef struct OLD_IMAGE_DOS_HEADER OLD_IMAGE_DOS_HEADER, *POLD_IMAGE_DOS_HEADER;

struct OLD_IMAGE_DOS_HEADER {
    char e_magic[2]; // Magic number
    word e_cblp; // Bytes of last page
    word e_cp; // Pages in file
    word e_crlc; // Relocations
    word e_cparhdr; // Size of header in paragraphs
    word e_minalloc; // Minimum extra paragraphs needed
    word e_maxalloc; // Maximum extra paragraphs needed
    word e_ss; // Initial (relative) SS value
    word e_sp; // Initial SP value
    word e_csum; // Checksum
    word e_ip; // Initial IP value
    word e_cs; // Initial (relative) CS value
    word e_lfarlc; // File address of relocation table
    word e_ovno; // Overlay number
};



undefined2 DAT_1d5d_8ace;
char DAT_1000_0245;
uint DAT_1000_0062;
uint DAT_34fe_0000;
char DAT_1000_0061;
undefined1 DAT_1000_022a;
undefined1 DAT_1000_0229;
undefined2 DAT_1000_05c0;
undefined2 DAT_1000_05be;
undefined2 DAT_1000_0681;
undefined2 DAT_1000_067f;
undefined2 DAT_1000_067a;
undefined2 DAT_1000_0678;
undefined2 DAT_1000_08c5;
undefined2 DAT_1000_08c3;
undefined2 DAT_1000_0938;
undefined2 DAT_1000_0936;
undefined2 UNK_0000_000c;
undefined2 UNK_0000_000e;
undefined2 UNK_0000_0020;
undefined2 UNK_0000_0022;
undefined2 UNK_0000_0024;
undefined2 UNK_0000_0026;
undefined2 UNK_0000_0054;
undefined2 UNK_0000_0056;
undefined2 UNK_0000_008c;
undefined2 UNK_0000_008e;
int DAT_1000_0278;
undefined2 DAT_1000_93dd;
undefined DAT_1d5d_0602;
undefined1 DAT_1000_025a;
char DAT_1000_025a;
char DAT_1000_025d;
char DAT_1000_025c;
char DAT_1000_025b;
char DAT_1000_0258;
char DAT_1000_0259;
int DAT_1000_12d0;
char DAT_1000_0257;
int DAT_1000_0e5c;
int DAT_1000_0e64;
char DAT_1000_0250;
char DAT_1000_0255;
char DAT_1000_0256;
undefined2 DAT_1000_005c;
undefined1 UNK_0000_041a;
undefined1 UNK_0000_041c;
int DAT_1000_14d4;
undefined2 DAT_1000_005e;
int DAT_1000_773f;
undefined1 DAT_1000_0258;
undefined1 DAT_1000_0259;
undefined1 DAT_1000_025d;
undefined1 DAT_1000_0061;
int DAT_1000_127c;
int DAT_1000_1018;
undefined1 DAT_1000_0060;
undefined2 DAT_1000_0278;
undefined2 DAT_1000_6636;
undefined2 DAT_1000_6638;
undefined2 DAT_1000_663e;
undefined2 DAT_1000_6640;
undefined2 DAT_1000_6646;
undefined2 DAT_1000_6648;
undefined2 DAT_1000_7ed5;
undefined2 DAT_1000_8067;
undefined2 DAT_1000_7ef9;
undefined2 DAT_1000_7efb;
undefined2 DAT_1000_7108;
undefined2 DAT_1000_710a;
undefined2 DAT_1000_7110;
undefined2 DAT_1000_7112;
undefined2 DAT_1000_7118;
undefined2 DAT_1000_711a;
undefined2 DAT_1000_7122;
undefined2 DAT_1000_7124;
undefined2 DAT_1000_712a;
undefined2 DAT_1000_712c;
undefined2 DAT_1000_7132;
undefined2 DAT_1000_7134;
undefined2 DAT_1000_773f;
uint DAT_1000_773f;
undefined2 DAT_1000_798e;
int DAT_1000_798e;
int DAT_1000_7b2f;
undefined2 DAT_1000_7b2f;

// WARNING: Stack frame is not setup normally: Input value of stackpointer is not used
// WARNING: This function may have set the stack pointer

void entry(void)

{
  code *pcVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined2 extraout_DX;
  undefined1 *puVar6;
  undefined2 *puVar7;
  undefined2 uVar8;
  
  bVar2 = in(0x61);
  out(0x61,bVar2 & 0xfc);
  DAT_1d5d_8ace = 0x73;
  install_interrupt_handlers();
  puVar6 = (undefined1 *)0x7d0;
  DAT_1d5d_8ace = 0x79;
  wait_for_timer();
  iVar5 = -1;
  do {
    in(extraout_DX);
    if (DAT_1000_0245 == '\x01') {
      DAT_1000_0062 = (uint)-iVar5 / 0x1c;
      if (DAT_1000_0062 < 0x501) {
        DAT_1000_0062 = DAT_1000_0062 + (0x500 - DAT_1000_0062 >> 2);
      }
      goto LAB_1000_00af;
    }
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  DAT_1000_0062 = 0x500;
LAB_1000_00af:
  pcVar1 = (code *)swi(0x10);
  uVar4 = (*pcVar1)();
  DAT_34fe_0000 = uVar4 & 0xff;
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x10);
  cVar3 = (*pcVar1)();
  if (cVar3 == '\r') {
    cVar3 = '\x10';
    pcVar1 = (code *)swi(0x10);
    (*pcVar1)();
    if ('\x02' < cVar3) {
      *(undefined2 *)(puVar6 + -2) = 0xee;
      set_graphics_page();
      *(undefined2 *)(puVar6 + -2) = 0x34fe;
      *(undefined2 *)(puVar6 + -4) = 0xff;
      load_graphics_block();
      *(undefined2 *)(puVar6 + -4) = 0x10a;
      ega_blit_plane();
      uVar8 = *(undefined2 *)(puVar6 + -2);
      *(undefined2 *)(puVar6 + -2) = 0x111;
      set_vga_palette();
      *(undefined2 *)(puVar6 + -2) = 0x114;
      toggle_graphics_page();
      *(undefined2 *)(puVar6 + -2) = 0x11a;
      set_vga_palette();
      DAT_1000_0061 = '\0';
      iVar5 = 0x19;
      do {
        *(undefined2 *)(puVar6 + -2) = 0x129;
        wait_for_timer();
        if (DAT_1000_0061 != '\0') goto LAB_1000_0136;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      puVar7 = (undefined2 *)(puVar6 + -2);
      puVar6 = puVar6 + -2;
      *puVar7 = 0x136;
      play_demo();
LAB_1000_0136:
      *(undefined2 *)(puVar6 + -2) = uVar8;
      *(undefined2 *)(puVar6 + -4) = 0x147;
      load_graphics_block();
      *(undefined2 *)(puVar6 + -2) = 0x14e;
      set_vga_palette();
      *(undefined2 *)(puVar6 + -2) = 0x151;
      toggle_graphics_page();
      *(undefined2 *)(puVar6 + -2) = 0x157;
      wait_for_timer();
      *(undefined2 *)(puVar6 + -2) = 0x15a;
      start_game();
      run_game();
      return;
    }
  }
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  exit_game();
  return;
}



undefined4 __cdecl16near set_vga_palette(void)

{
  undefined1 *puVar1;
  uint uVar2;
  byte bVar3;
  int iVar4;
  undefined1 *in_DX;
  int iVar5;
  undefined2 unaff_DS;
  
  do {
    bVar3 = in(0x3da);
  } while ((bVar3 & 8) == 0);
  iVar4 = 0x10;
  iVar5 = 0;
  do {
    puVar1 = in_DX;
    in_DX = in_DX + 1;
    out(0x3c0,(char)iVar5);
    uVar2 = (uint)iVar5 >> 8;
    out(0x3c0,*puVar1);
    iVar4 = iVar4 + -1;
    iVar5 = iVar5 + 1;
  } while (iVar4 != 0);
  out(0x3c0,0x20);
  return CONCAT22(0x3c0,CONCAT11((char)uVar2,0x20));
}



// Graphics mode initialization chunk (part of main game loop sub_35DE)
// Sets EGA 320x200 16-color mode (INT 10h, AX=0Dh)
void init_ega_graphics_mode(void)

{
  code *pcVar1;
  
  clear_sound_flags();
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  set_graphics_page();
  load_graphics_block();
  ega_palette_setup();
  set_vga_palette();
  toggle_graphics_page();
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Error handler - displays DOS error message and exits
// Part of error handling routine (chunk of sub_636F)
void error_handler_exit(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  wait_for_input_signal();
  exit_game();
  return;
}



void exit_game(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Clear sound/state flags (unmute or enable)
// Sets byte_229=0, byte_22A=0
void __cdecl16near clear_sound_flags(void)

{
  DAT_1000_022a = 0;
  DAT_1000_0229 = 0;
  return;
}



// Set sound/state flag (mute or disable)
// Sets byte_229=1, byte_22A=0
void __cdecl16near set_sound_flag(void)

{
  DAT_1000_022a = 0;
  DAT_1000_0229 = 1;
  return;
}



void __cdecl16near wait_for_timer(void)

{
  int in_AX;
  
  do {
    do {
    } while (DAT_1000_0245 != '\x01');
    DAT_1000_0245 = '\0';
    in_AX = in_AX + -1;
  } while (in_AX != 0);
  return;
}



void start_game(void)

{
  code *pcVar1;
  
  load_graphics_block();
  ega_blit_plane();
  set_vga_palette();
  toggle_graphics_page();
  set_vga_palette();
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// WARNING: Removing unreachable block (ram,0x000105a7)
// Wait for interrupt-driven input/signal
// Sets byte_61=0, then busy-waits until byte_61 becomes non-zero (set by interrupt handler)
void __cdecl16near wait_for_input_signal(void)

{
                    // WARNING: Do nothing block with infinite loop
  do {
  } while( true );
}



void __cdecl16near install_interrupt_handlers(void)

{
  DAT_1000_05c0 = uRam00000026;
  DAT_1000_05be = uRam00000024;
  uRam00000024 = 0x5c2;
  uRam00000026 = 0x1000;
  DAT_1000_0681 = uRam00000022;
  DAT_1000_067f = uRam00000020;
  uRam00000020 = 0x683;
  uRam00000022 = 0x1000;
  DAT_1000_067a = uRam0000008e;
  DAT_1000_0678 = uRam0000008c;
  uRam0000008c = 0x67c;
  uRam0000008e = 0x1000;
  DAT_1000_08c5 = uRam0000000e;
  DAT_1000_08c3 = uRam0000000c;
  uRam0000000c = 0x8c7;
  uRam0000000e = 0x1000;
  DAT_1000_0938 = uRam00000056;
  DAT_1000_0936 = uRam00000054;
  uRam00000054 = 0x93a;
  uRam00000056 = 0x1000;
  return;
}



// Viewport/camera clipping setup - calculates render boundaries
void setup_render_viewport(void)

{
  uint uVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x218 = *(undefined1 *)0x8ad;
  *(undefined1 *)0x219 = *(undefined1 *)0x89d;
  uVar1 = *(uint *)0x6c6;
  *(uint *)0x210 = uVar1;
  iVar2 = uVar1 - 0x60;
  if (uVar1 < 0x60) {
    iVar2 = 0;
  }
  if (*(int *)0x29c + -0xd0 < iVar2) {
    iVar2 = *(int *)0x29c + -0xd0;
  }
  *(int *)0x6c2 = iVar2;
  uVar1 = *(uint *)0x6c8;
  *(uint *)0x212 = uVar1;
  iVar2 = uVar1 - 0x40;
  if (uVar1 < 0x40) {
    iVar2 = 0;
  }
  if (*(int *)0x29e + -0xa0 < iVar2) {
    iVar2 = *(int *)0x29e + -0xa0;
  }
  *(int *)0x6c4 = iVar2;
  if (((*(int *)0x6fa == 2) && (*(int *)0x6fc == 1)) && (*(int *)0x6c6 == 0)) {
    *(undefined1 *)0x924 = 2;
    *(undefined2 *)0x928 = 0;
    *(undefined2 *)0x92a = 0x20;
    *(undefined1 *)0x925 = 2;
    *(undefined2 *)0x926 = 8;
  }
  if (((*(int *)0x6fa == 9) && (*(int *)0x6fc == 1)) && (*(int *)0x6c6 == 0)) {
    *(undefined1 *)0x92c = 2;
    *(undefined2 *)0x930 = 0;
    *(undefined2 *)0x932 = 0xc0;
    *(undefined1 *)0x92d = 0;
    *(undefined2 *)0x92e = 0;
  }
  display_handler_49ad();
  return;
}



// Load sprite/animation data from level tables
void load_sprite_animation_data(void)

{
  char *pcVar1;
  char cVar2;
  int *piVar3;
  char cVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  undefined2 unaff_DS;
  
  piVar3 = (int *)*(int *)(*(int *)0x6fa * 2 + 0x16a8);
  iVar5 = *piVar3;
  if ((*(int *)0x6fa == 5) && ('\x04' < *(char *)0x220)) {
    iVar5 = 0x16c6;
  }
  DAT_1000_0278 = iVar5;
  *(undefined2 *)(iVar5 + 2) = 0;
  *(undefined2 *)(iVar5 + 4) = 0;
  *(undefined2 *)(iVar5 + 10) = 0;
  *(undefined2 *)(iVar5 + 0xc) = 0;
  *(undefined2 *)(iVar5 + 0x12) = 0;
  *(undefined2 *)(iVar5 + 0x14) = 0;
  pcVar6 = (char *)piVar3[1];
  cVar4 = *pcVar6;
  *(char *)0x6ca = cVar4;
  cVar2 = pcVar6[1];
  *(char *)0x6cb = cVar2;
  pcVar6 = pcVar6 + 2;
  pcVar7 = (char *)0x6cc;
  cVar4 = (cVar2 - cVar4) + '\x01';
  do {
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar7 = *pcVar1;
    pcVar7 = pcVar7 + 1;
    cVar4 = cVar4 + -1;
  } while (cVar4 != '\0');
  pcVar6 = (char *)piVar3[2];
  cVar4 = *pcVar6;
  *(char *)0x6d6 = cVar4;
  cVar2 = pcVar6[1];
  *(char *)0x6d7 = cVar2;
  pcVar6 = pcVar6 + 2;
  pcVar7 = (char *)0x6d8;
  cVar4 = (cVar2 - cVar4) + '\x01';
  do {
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar7 = *pcVar1;
    pcVar7 = pcVar7 + 1;
    cVar4 = cVar4 + -1;
  } while (cVar4 != '\0');
  pcVar6 = (char *)piVar3[3];
  cVar4 = *pcVar6;
  *(char *)0x6e2 = cVar4;
  cVar2 = pcVar6[1];
  *(char *)0x6e3 = cVar2;
  pcVar6 = pcVar6 + 2;
  pcVar7 = (char *)0x6e4;
  cVar4 = (cVar2 - cVar4) + '\x01';
  do {
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar7 = *pcVar1;
    pcVar7 = pcVar7 + 1;
    cVar4 = cVar4 + -1;
  } while (cVar4 != '\0');
  *(undefined1 *)0x93a = 0;
  if (*(int *)0x6fa == 0xc) {
    *(undefined1 *)0x93a = 1;
  }
  return;
}



// Animation update handler
void update_animation_state(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined2 *puVar5;
  undefined1 *puVar6;
  int *piVar7;
  undefined2 unaff_DS;
  
  iVar3 = *(int *)(*(int *)0x6fa * 2 + 0x16a8);
  puVar5 = (undefined2 *)*(int *)(*(int *)(iVar3 + 8) + *(int *)0x6fc * 2);
  *(undefined2 *)0x6ee = *puVar5;
  *(undefined2 *)0x6f0 = puVar5 + 1;
  puVar5 = (undefined2 *)*(int *)(*(int *)(iVar3 + 10) + *(int *)0x6fc * 2);
  *(undefined2 *)0x6f2 = *puVar5;
  *(undefined2 *)0x6f4 = puVar5 + 1;
  puVar5 = (undefined2 *)*(int *)(*(int *)(iVar3 + 0xc) + *(int *)0x6fc * 2);
  *(undefined2 *)0x6f6 = *puVar5;
  *(undefined2 *)0x6f8 = puVar5 + 1;
  piVar4 = (int *)0xaf0;
  piVar7 = (int *)0x700;
  iVar2 = 0;
  *(undefined2 *)0x6fe = 0;
  iVar3 = *(int *)0x6fa;
  iVar1 = *(int *)0x6fc;
  do {
    if ((iVar3 == *piVar4) && (iVar1 == piVar4[1])) {
      *piVar7 = piVar4[2];
      piVar7[1] = piVar4[3];
      piVar7[3] = piVar4[4];
      piVar7[2] = iVar2;
      piVar7[2] = piVar7[2] | piVar4[5] & 0x8000U;
      *(int *)0x6fe = *(int *)0x6fe + 1;
      piVar7 = piVar7 + 4;
    }
    iVar2 = iVar2 + 1;
    piVar4 = piVar4 + 6;
  } while (iVar2 != *(int *)0x940);
  puVar5 = (undefined2 *)0x8ee;
  iVar3 = 6;
  do {
    *puVar5 = 0xffff;
    puVar5 = puVar5 + 4;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  *(undefined1 *)0x8b1 = 0;
  puVar6 = (undefined1 *)0x8b2;
  iVar3 = 6;
  do {
    *puVar6 = 0;
    puVar6 = puVar6 + 9;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  *(undefined1 *)0x924 = 0;
  if ((*(int *)0x6fa == 2) && (*(int *)0x6fc == 0)) {
    *(undefined1 *)0x924 = 1;
    *(undefined2 *)0x928 = 0xb0;
    *(undefined2 *)0x92a = 0xe0;
    *(undefined2 *)0x926 = 0;
  }
  *(undefined1 *)0x92c = 0;
  if ((*(int *)0x6fa == 9) && (*(int *)0x6fc == 0)) {
    *(undefined1 *)0x92c = 1;
    *(undefined2 *)0x930 = 0x558;
    *(undefined2 *)0x932 = 0x20;
  }
  *(undefined1 *)0x8a8 = 0;
  return;
}



// Load level/area sprite table
void load_area_sprite_table(void)

{
  int in_AX;
  int iVar1;
  int in_BX;
  int iVar2;
  int *piVar3;
  undefined2 unaff_DS;
  
  if (*(int *)0x6fa != in_AX) {
    process_input_controls();
    iVar1 = clear_sound_flags();
    if (in_BX != -1) {
      *(undefined2 *)0x6fc = 0xffff;
    }
    *(int *)0x6fa = iVar1;
    iVar2 = *(int *)(iVar1 * 2 + -0x6c14) + 2;
    render_sprite(iVar2,in_BX,iVar1);
    iVar2 = iVar2 + 2;
    func_1ffe();
    piVar3 = (int *)(iVar2 + 2);
    func_2022();
    if (*piVar3 != 0) {
      update_render();
    }
    load_sprite_animation_data();
    set_sound_flag();
  }
  if ((*(int *)0x6fc != in_BX) && (in_BX != -1)) {
    *(int *)0x6fc = in_BX;
    update_palette();
    func_1e86();
    update_animation_state();
  }
  return;
}



// Keyboard state check helper
void check_keyboard_state(void)

{
  int *in_BX;
  
  if ((*in_BX != -1) && (*in_BX != 5)) {
    return;
  }
  return;
}



// Input/control processing - returns input state
uint process_input_controls(void)

{
  uint in_AX;
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  byte bVar6;
  int iVar7;
  int *piVar8;
  undefined2 unaff_DS;
  bool bVar9;
  
  bVar9 = in_AX < 5;
  if (in_AX == 5) {
    check_keyboard_state();
    if ((((!bVar9) && (check_keyboard_state(), !bVar9)) && (check_keyboard_state(), !bVar9)) &&
       (((check_keyboard_state(), !bVar9 && (check_keyboard_state(), !bVar9)) && (check_keyboard_state(), !bVar9)))) {
      if ('\x04' < *(char *)0x220) {
        return 5;
      }
      DAT_1000_93dd = 0xffff;
      *(undefined1 *)0x220 = 5;
      return 5;
    }
    piVar8 = (int *)0x0;
    iVar1 = 0;
    iVar3 = 0x40;
    do {
      iVar1 = iVar1 + *piVar8;
      piVar8 = piVar8 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    piVar8 = (int *)0xf000;
    iVar3 = 0;
    iVar7 = 0;
    iVar4 = 0x40;
    do {
      iVar3 = iVar3 + *piVar8;
      iVar7 = iVar7 + piVar8[0x7c0];
      piVar8 = piVar8 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    uVar2 = *(int *)0x9ce6 + iVar1 + iVar3 + iVar7;
    uVar2 = uVar2 + (uVar2 >> 5) & 0x7ff;
    bVar6 = (byte)uVar2;
    iVar1 = CONCAT11((char)(uVar2 >> 8) + '\x12' + (0xcb < bVar6),bVar6 + 0x34);
    uVar2 = iVar1 * 0x1b;
    if ((uVar2 & 0x40) != 0) {
      uVar2 = iVar1 * -0x1b;
    }
    uVar5 = 0x93aa;
    if (uVar2 == *(uint *)0x9cf8) {
      uVar5 = 0x990;
    }
    *(undefined2 *)0x93a8 = uVar5;
  }
  return in_AX;
}



// Tile lookup and collision check
uint check_tile_collision(void)

{
  uint in_AX;
  uint in_BX;
  undefined2 unaff_DS;
  
  if (*(int *)0x29a <= (int)(in_BX >> 4)) {
    return 0x100;
  }
  return (uint)*(byte *)(*(int *)((in_BX >> 4) * 2 + 0x2a0) + (in_AX >> 4));
}



// Tile/map data lookup for collision detection
void lookup_tile_data(void)

{
  int iVar1;
  undefined2 unaff_DS;
  int local_4;
  
  local_4 = 0;
  iVar1 = *(int *)0x6c0 * 0x10 - *(int *)0x6c4;
  if ((iVar1 != 0 && *(int *)0x6c4 <= *(int *)0x6c0 * 0x10) && (local_4 = iVar1, 0xa0 < iVar1)) {
    local_4 = 0xa0;
  }
  calc_sprite_offset(*(uint *)0x6c2 >> 3,0xa0 - local_4);
  calc_sprite_offset();
  calc_sprite_offset();
  calc_sprite_offset();
  return;
}



// Sprite tile offset calculation
void calculate_sprite_offset(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined2 in_AX;
  int iVar4;
  int iVar5;
  int unaff_BP;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  func_7a68();
  iVar3 = *(int *)0x298;
  iVar5 = *(int *)(unaff_BP + -2);
  if (iVar5 != 0) {
    do {
      for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
        puVar2 = unaff_DI;
        unaff_DI = unaff_DI + 1;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        *puVar2 = *puVar1;
      }
      unaff_SI = unaff_SI + iVar3 + -0xd;
      unaff_DI = unaff_DI + 7;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    unaff_SI = (undefined2 *)*(undefined2 *)(unaff_BP + -6);
  }
  for (iVar5 = *(int *)(unaff_BP + -4); iVar5 != 0; iVar5 = iVar5 + -1) {
    for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
      puVar2 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      puVar1 = unaff_SI;
      unaff_SI = unaff_SI + 1;
      *puVar2 = *puVar1;
    }
    unaff_SI = unaff_SI + iVar3 + -0xd;
    unaff_DI = unaff_DI + 7;
  }
  return;
}



// Draw/blit sprite data
void draw_sprite(void)

{
  uint in_BX;
  undefined2 unaff_DS;
  
  if ((int)(in_BX >> 4) < *(int *)0x6c0) {
    update_anim_frame();
    update_anim_frame();
    update_anim_frame();
    update_anim_frame();
  }
  else {
    update_frame_alt();
    update_frame_alt();
    update_frame_alt();
    update_frame_alt();
  }
  return;
}



// Level animation/effect handler
void handle_level_effects(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined2 unaff_DS;
  int local_4;
  
  *(undefined2 *)0x6c0 = (int)(0x600 / (ulong)*(uint *)0x298);
  iVar2 = *(int *)0x298;
  piVar4 = (int *)0x6a0;
  iVar1 = 0;
  iVar3 = 0x10;
  do {
    *piVar4 = iVar1;
    piVar4 = piVar4 + 1;
    iVar1 = iVar1 + iVar2 * 2;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  piVar4 = (int *)0x4a0;
  iVar2 = 0x4000;
  iVar3 = 0;
  do {
    *piVar4 = iVar2;
    iVar2 = iVar2 + iVar1;
    piVar4 = piVar4 + 1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < *(int *)0x6c0);
  iVar2 = 0;
  do {
    *piVar4 = iVar2;
    iVar2 = iVar2 + iVar1;
    piVar4 = piVar4 + 1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < *(int *)0x29a);
  local_4 = 0;
  iVar2 = 0x9a8;
  do {
    iVar1 = *(int *)0x298;
    do {
      iVar2 = iVar2 + 1;
      if (local_4 < *(int *)0x6c0) {
        update_anim_frame(iVar1,iVar2);
        update_anim_frame();
        update_anim_frame();
        update_anim_frame();
      }
      else {
        update_frame_alt();
        update_frame_alt();
        update_frame_alt();
        update_frame_alt();
      }
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    local_4 = local_4 + 1;
  } while (local_4 < *(int *)0x29a);
  return;
}



// Animation frame update
void update_animation_frame(void)

{
  undefined2 *puVar1;
  int iVar2;
  int iVar3;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  iVar3 = 0x10;
  iVar2 = *(int *)0x298;
  do {
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    *unaff_DI = *puVar1;
    unaff_DI = unaff_DI + iVar2;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}



// Animation frame update (alt)
void update_frame_alt(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 in_AX;
  int iVar3;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  
  func_7a68();
  iVar3 = 0x10;
  iVar2 = *(int *)0x298;
  do {
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    *unaff_DI = *puVar1;
    unaff_DI = unaff_DI + iVar2;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}



// Sprite render helper
void render_sprite(void)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  byte *pbVar4;
  code *pcVar5;
  uint uVar6;
  byte *pbVar7;
  byte *pbVar8;
  byte *pbVar9;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  pcVar5 = (code *)swi(0x21);
  (*pcVar5)();
  if (!(bool)in_CF) {
    pcVar5 = (code *)swi(0x21);
    (*pcVar5)();
    pcVar5 = (code *)swi(0x21);
    (*pcVar5)();
    pcVar5 = (code *)swi(0x21);
    (*pcVar5)();
    pbVar9 = (byte *)0x13a8;
    pbVar7 = (byte *)0x600;
    while( true ) {
      while( true ) {
        pbVar8 = pbVar7 + 1;
        bVar2 = *pbVar7;
        if (-1 < (char)bVar2) break;
        pbVar7 = pbVar7 + 2;
        bVar3 = *pbVar8;
        for (uVar6 = (uint)(byte)-bVar2; uVar6 != 0; uVar6 = uVar6 - 1) {
          pbVar1 = pbVar9;
          pbVar9 = pbVar9 + 1;
          *pbVar1 = bVar3;
        }
      }
      if ((char)bVar2 < '\x01') break;
      for (uVar6 = (uint)bVar2; pbVar7 = pbVar8, uVar6 != 0; uVar6 = uVar6 - 1) {
        pbVar4 = pbVar9;
        pbVar9 = pbVar9 + 1;
        pbVar1 = pbVar8;
        pbVar8 = pbVar8 + 1;
        *pbVar4 = *pbVar1;
      }
    }
    return;
  }
  error_handler();
  return;
}



// Graphics blit helper
void blit_graphics(void)

{
  code *pcVar1;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



// Graphics copy/update helper
void update_graphics(void)

{
  code *pcVar1;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



// Graphics update handler
void update_render(void)

{
  code *pcVar1;
  int iVar2;
  byte *pbVar3;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  iVar2 = 0xc00;
  pbVar3 = (byte *)0x9ec0;
  do {
    *pbVar3 = *pbVar3 ^ 0x25;
    pbVar3 = pbVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

// Graphics/palette update
void update_palette(void)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  byte *pbVar4;
  int iVar5;
  int in_AX;
  int iVar6;
  uint uVar7;
  int iVar8;
  int in_BX;
  int *piVar9;
  byte *pbVar10;
  byte *pbVar11;
  byte *pbVar12;
  undefined2 unaff_DS;
  
  if (in_AX == _DAT_1d5d_0602) {
    iVar8 = in_BX * 6;
    *(undefined2 *)0x298 = *(undefined2 *)(iVar8 + 0x604);
    *(undefined2 *)0x29a = *(undefined2 *)(iVar8 + 0x606);
    pbVar12 = (byte *)0x9a8;
    pbVar10 = (byte *)(*(int *)(iVar8 + 0x608) + 0x600);
    while( true ) {
      while( true ) {
        pbVar11 = pbVar10 + 1;
        bVar2 = *pbVar10;
        if (-1 < (char)bVar2) break;
        pbVar10 = pbVar10 + 2;
        bVar3 = *pbVar11;
        for (uVar7 = (uint)(byte)-bVar2; uVar7 != 0; uVar7 = uVar7 - 1) {
          pbVar1 = pbVar12;
          pbVar12 = pbVar12 + 1;
          *pbVar1 = bVar3;
        }
      }
      if ((char)bVar2 < '\x01') break;
      for (uVar7 = (uint)bVar2; pbVar10 = pbVar11, uVar7 != 0; uVar7 = uVar7 - 1) {
        pbVar4 = pbVar12;
        pbVar12 = pbVar12 + 1;
        pbVar1 = pbVar11;
        pbVar11 = pbVar11 + 1;
        *pbVar4 = *pbVar1;
      }
    }
    piVar9 = (int *)0x2a0;
    iVar8 = *(int *)0x29a;
    iVar6 = 0x9a8;
    iVar5 = *(int *)0x298;
    do {
      *piVar9 = iVar6;
      iVar6 = iVar6 + iVar5;
      piVar9 = piVar9 + 1;
      iVar8 = iVar8 + -1;
    } while (iVar8 != 0);
    *(int *)0x29c = *(int *)0x298 << 4;
    *(int *)0x29e = *(int *)0x29a << 4;
    return;
  }
  error_handler();
  return;
}



// Game state handler - Level/area transition and sprite loading
// Part of main game loop (sub_35DE)
void handle_level_transition(void)

{
  code *pcVar1;
  byte bVar2;
  int unaff_SI;
  int *piVar3;
  undefined2 unaff_DS;
  uint in_stack_00000000;
  int iVar4;
  
  *(undefined2 *)0x892 = 0;
  iVar4 = *(int *)(unaff_SI + 0xc);
  if (*(char *)(unaff_SI + 6) == '\0') {
    func_3bc6(*(undefined2 *)(unaff_SI + 10),iVar4,*(undefined2 *)(unaff_SI + 0xe));
  }
  else {
    if (*(char *)(unaff_SI + 6) == -1) {
      pcVar1 = (code *)swi(3);
      (*pcVar1)();
      return;
    }
    play_animation_sequence();
    if (unaff_SI == -0x475c) {
      if (*(char *)0x221 != '\x01') {
        piVar3 = (int *)0xb8a4;
        goto LAB_1000_2198;
      }
      calculate_display_position();
    }
  }
  if (iVar4 == -1) {
    load_area_sprite_table();
    *(undefined2 *)0x6c6 = 0x28;
    *(undefined2 *)0x6c8 = 0x60;
    *(undefined1 *)0x8ad = 1;
    setup_render_viewport();
    player_death_animation();
    *(undefined1 *)0x897 = 0;
    *(undefined1 *)0x291 = 1;
    *(undefined1 *)0x89c = 0;
    game_tick_handler();
    return;
  }
  if (iVar4 == -2) {
    graphics_mode_reset();
    return;
  }
  load_area_sprite_table();
  piVar3 = (int *)*(undefined2 *)0x6f0;
  if (in_stack_00000000 != 0) {
    do {
      piVar3 = piVar3 + 9;
      bVar2 = (char)in_stack_00000000 - 1;
      in_stack_00000000 = (uint)bVar2;
    } while (bVar2 != 0);
  }
  *(int *)0x6c6 = ((uint)piVar3[2] >> 1) + *piVar3;
  *(int *)0x6c8 = piVar3[1];
  setup_render_viewport();
  if (piVar3 == (int *)0xb8a4) {
    calculate_display_position();
  }
LAB_1000_2198:
  if (((char)piVar3[3] != '\0') && ((char)piVar3[3] != -1)) {
    render_graphics();
  }
  DAT_1000_025a = 0;
  wait_for_timer();
  game_tick_handler();
  return;
}



// Reset player to starting position
// Part of main game loop (sub_35DE)
void reset_player_position(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x8ad = 1;
  screen_fade_effect();
  load_area_sprite_table();
  *(undefined2 *)0x6c6 = 0x28;
  *(undefined2 *)0x6c8 = 0x60;
  *(undefined1 *)0x8ad = 1;
  setup_render_viewport();
  player_death_animation();
  *(undefined1 *)0x897 = 0;
  *(undefined1 *)0x291 = 1;
  *(undefined1 *)0x89c = 0;
  game_tick_handler();
  return;
}



// Graphics mode transition handler
// Returns to EGA graphics initialization
void graphics_mode_reset(void)

{
  calculate_display_position();
  wait_for_timer();
  title_screen();
  init_ega_graphics();
  return;
}



// Sound/music trigger via INT 3
// Part of main game loop (sub_35DE)
void trigger_sound_interrupt(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Load level entry position from table
// Part of main game loop (sub_35DE)
// WARNING: Removing unreachable block (ram,0x0001216c)

void load_level_entry_position(void)

{
  int *piVar1;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x8ad = 1;
  *(undefined2 *)0x6c6 = 0x28;
  screen_fade_effect();
  load_area_sprite_table();
  piVar1 = (int *)*(int *)0x6f0;
  *(int *)0x6c6 = ((uint)piVar1[2] >> 1) + *piVar1;
  *(int *)0x6c8 = piVar1[1];
  setup_render_viewport();
  if (piVar1 == (int *)0xb8a4) {
    calculate_display_position();
  }
  if (((char)piVar1[3] != '\0') && ((char)piVar1[3] != -1)) {
    render_graphics();
  }
  DAT_1000_025a = 0;
  wait_for_timer();
  game_tick_handler();
  return;
}



// Major game state reset - clears flags, reinitializes player
// Part of main game loop (sub_35DE)
void reset_game_state(void)

{
  undefined2 unaff_DS;
  
  state_setup_3aa0();
  action_handler_3a92();
  *(undefined1 *)0x8ed = 0xc;
  if (*(int *)0x222 < 0x205) {
    *(undefined2 *)0x222 = 0x205;
  }
  movement_handler_3a5f();
  *(undefined2 *)0x6c6 = *(undefined2 *)0x210;
  *(undefined2 *)0x6c8 = *(undefined2 *)0x212;
  *(undefined1 *)0x8ad = *(undefined1 *)0x218;
  *(undefined1 *)0x89d = *(undefined1 *)0x219;
  *(undefined2 *)0x892 = 0;
  *(undefined2 *)0x894 = 0;
  *(undefined1 *)0x897 = 0;
  *(undefined1 *)0x8a3 = 0;
  *(undefined1 *)0x8a6 = 0;
  *(undefined1 *)0x8a8 = 0;
  *(undefined1 *)0x91e = 0;
  *(undefined1 *)0x921 = 0;
  *(undefined1 *)0x89a = 0;
  *(undefined1 *)0x89e = 0;
  *(undefined1 *)0x8b1 = 0;
  *(undefined1 *)0x8b0 = 0;
  *(undefined1 *)0x8ec = 0;
  *(undefined1 *)0x291 = 0;
  *(undefined1 *)0x89c = 0;
  *(undefined1 *)0x93b = 0;
  *(undefined1 *)0x89b = 1;
  *(undefined2 *)0x6fc = 0xffff;
  load_area_sprite_table();
  setup_render_viewport();
  *(undefined1 *)0x896 = 0;
  animation_data_handler();
  if (*(char *)0x220 == -1) {
    *(undefined1 *)0x220 = 0;
    player_death_animation();
  }
  game_tick_handler();
  return;
}



// Reset level and position data
// Part of main game loop (sub_35DE)
void reset_level_data(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x6fa = 0xffff;
  load_area_sprite_table();
  *(undefined2 *)0x210 = 0x288;
  *(undefined2 *)0x212 = 0x70;
  reset_game_state();
  return;
}



// Main game tick handler - updates video and game state
// Part of main game loop (sub_35DE)
void game_tick_handler(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  int *piVar5;
  code *pcVar6;
  undefined2 uVar7;
  int iVar8;
  byte bVar9;
  char cVar10;
  uint uVar11;
  int iVar12;
  int *piVar13;
  undefined2 unaff_DS;
  bool bVar14;
  
  do {
    uRam0000041c = uRam0000041a;
    *(char *)0x962b = *(char *)0x962b + '\x01';
    *(BADSPACEBASE **)0x8e8 = register0x00000010;
    cVar10 = *(char *)0x8af + '\x01';
    if (cVar10 == '\x04') {
      cVar10 = '\x01';
    }
    *(char *)0x8af = cVar10;
    *(undefined1 *)0x8ae = 0;
    if (*(char *)0x962a != '\0') {
      *(char *)0x962a = *(char *)0x962a + -1;
    }
    if ((*(char *)0x8ec == '\0') && (*(char *)0x921 == '\0')) {
      if ('\x01' < *(char *)0x924) {
        game_logic_3657();
        return;
      }
      if (*(char *)0x92c < '\x02') {
        if (*(char *)0x89d != '\0') {
          handle_airborne_movement();
          return;
        }
        if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
           ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
          game_loop_chunk_3610();
          DAT_1000_025a = '\0';
        }
        if (*(char *)0x897 != '\0') {
          update_player_physics();
          return;
        }
        if (*(char *)0x8a3 != '\0') {
          handle_player_animation();
          return;
        }
        if (*(char *)0x8a7 != '\0') {
          handle_attack_animation();
          return;
        }
        if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
           (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
          game_logic_385f();
          return;
        }
        iVar8 = check_tile_collision();
        if ((*(int *)0x294 < iVar8) && (iVar8 <= *(int *)0x296)) {
          handle_player_death();
          return;
        }
        if (*(char *)0x8a6 != '\0') {
          check_player_state();
          return;
        }
        if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
          *(undefined1 *)0x897 = 1;
          if (DAT_1000_025b == '\x01') {
            *(undefined1 *)0x897 = 1;
            *(undefined1 *)0x898 = 0;
            *(undefined2 *)0x894 = 5;
            *(undefined2 *)0x892 = 0;
            *(undefined1 *)0x89a = 2;
            pcVar6 = (code *)swi(3);
            (*pcVar6)();
            return;
          }
          if (*(char *)0x9628 == '\x01') {
            *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
          }
          pcVar6 = (code *)swi(3);
          (*pcVar6)();
          return;
        }
        if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
          if ((*(int *)0x6fa == 5) &&
             ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
              ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
            menu_handler();
            return;
          }
          uVar11 = *(uint *)0x6ee;
          piVar13 = (int *)*(undefined2 *)0x6f0;
          if (uVar11 != 0) {
            do {
              if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
                 (*(int *)0x6c8 == piVar13[1])) {
                piVar4 = (int *)piVar13[8];
                if (piVar4 != (int *)0x0) {
                  piVar5 = (int *)*piVar4;
                  if (piVar5 != (int *)0x0) {
                    if ((piVar4[1] != piVar5[4]) || (*piVar5 != -1)) {
                      game_loop_chunk_3610();
                      break;
                    }
                    *(undefined2 *)piVar13[8] = 0;
                  }
                }
                *(undefined1 *)0x962a = 5;
                handle_level_transition();
                return;
              }
              piVar13 = piVar13 + 9;
              bVar9 = (char)uVar11 - 1;
              uVar11 = (uint)bVar9;
            } while (bVar9 != 0);
          }
        }
        *(undefined2 *)0x892 = 0;
        if (DAT_1000_0258 == '\x01') {
          *(undefined2 *)0x892 = 0xfffb;
          graphics_handler();
        }
        if (DAT_1000_0259 == '\x01') {
          *(undefined2 *)0x892 = 5;
          anim_handler();
        }
        bVar14 = 0xffdf < *(uint *)0x6c8;
        func_2c9d();
        if (bVar14) {
          if (*(char *)0x89c == '\x01') {
            reset_player_position();
            return;
          }
        }
        else {
          *(undefined1 *)0x897 = 1;
          *(undefined1 *)0x898 = 0;
          *(undefined2 *)0x894 = 5;
          if (*(int *)0x892 < 0) {
            *(undefined2 *)0x892 = 0xffff;
          }
          else if (*(int *)0x892 != 0) {
            *(undefined2 *)0x892 = 1;
          }
        }
        if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
           (DAT_1000_12d0 != -1)) {
          iVar12 = *(int *)0x6c8 + 0x20;
          iVar8 = check_tile_collision();
          if (((0x1b < iVar8) && (iVar8 < 0x1f)) ||
             (((*(uint *)0x6c6 & 8) != 0 &&
              ((iVar8 = CONCAT11((char)((uint)iVar8 >> 8),*(undefined1 *)(iVar12 + 1)), 0x1b < iVar8
               && (iVar8 < 0x1f)))))) {
            *(undefined1 *)0x9628 = 1;
            uVar3 = *(undefined1 *)0x8ae;
            if (*(char *)0x9629 < '\0') {
              move_player_left();
              *(undefined1 *)0x8ae = uVar3;
            }
            else {
              move_player_right();
              *(undefined1 *)0x8ae = uVar3;
            }
          }
        }
        if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
           (*(int *)0x6c6 == 0x28)) {
          if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
            load_level_entry_position();
            return;
          }
        }
        else {
          *(undefined1 *)0x291 = 0;
        }
        if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
           (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
          render_chunk_46e6();
          DAT_1000_0061 = '\0';
        }
      }
      else {
        if (*(char *)0x896 != '\0') {
          *(undefined1 *)0x896 = 0;
          animation_data_handler();
        }
        *(int *)0x92e = *(int *)0x92e + 2;
        if (0xc < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xc;
        }
        *(int *)0x930 = *(int *)0x930 + 8;
        uVar11 = *(int *)0x932 + *(int *)0x92e;
        iVar12 = uVar11 + 0x20;
        iVar8 = check_tile_collision();
        bVar9 = *(byte *)(iVar12 - *(int *)0x298);
        if ((char)iVar8 == '\x16') {
LAB_1000_38ec:
          *(undefined1 *)0x8ec = 10;
          pcVar6 = (code *)swi(3);
          (*pcVar6)();
          return;
        }
        if (bVar9 == 0x20) {
LAB_1000_3915:
          uVar11 = uVar11 & 0xfff0;
          if (-1 < *(int *)0x92e) {
            *(undefined2 *)0x92e = 0xfff0;
          }
        }
        else if (iVar8 <= *(int *)0x292) {
          uVar11 = uVar11 & 0xfff0;
          *(undefined2 *)0x92e = 1;
          if (bVar9 != 0x1f) {
            if (bVar9 == 0x16) goto LAB_1000_38ec;
            *(undefined2 *)0x92e = 0;
            if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
          }
        }
        if ((*(int *)0x92e < 0) && ((int)(uint)bVar9 <= *(int *)0x292)) {
          uVar11 = *(uint *)0x932;
          *(undefined2 *)0x92e = 0;
        }
        *(uint *)0x932 = uVar11;
        *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
        *(undefined1 *)0x898 = *(undefined1 *)0x899;
        if (DAT_1000_025c == '\x01') {
          *(undefined1 *)0x898 = 0;
        }
        *(int *)0x6c6 = *(int *)0x930 + 8;
        *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
        if (*(char *)0x8ec == '\n') {
          *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
          *(int *)0x6c6 = *(int *)0x6c6 + -8;
        }
        if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
          main_game_loop();
        }
        if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
          *(int *)0x6c2 = *(int *)0x6c2 + 8;
        }
        func_2b8e();
      }
      if (*(char *)0x924 != '\0') {
        game_loop_chunk_36c0();
      }
      if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
        cVar10 = *(char *)0x899;
        if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
          cVar10 = cVar10 + '\x01';
        }
        *(char *)0x898 = cVar10;
      }
      if (DAT_1000_025d == '\x01') {
        if (*(char *)0x896 == '\0') {
          if (*(char *)0x8ea != '\0') {
            if (*(char *)0x21c == '\0') {
              game_loop_chunk_3610();
            }
            else {
              load_resource_5ceb();
            }
            sprite_handler_3b12();
          }
        }
        else {
          state_setup_3ace();
          if (*(char *)0x896 == '\x03') {
            if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
              state_transition();
            }
          }
          else if ((*(char *)0x896 == '\x01') &&
                  ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
            sprite_physics();
          }
        }
      }
      else {
        state_setup_3ace();
      }
      if (DAT_1000_0250 == DAT_1000_0061) {
        DAT_1000_0061 = '\0';
        game_logic_39ad();
      }
      if (DAT_1000_0255 == DAT_1000_0061) {
        DAT_1000_0061 = '\0';
        sprite_handler_327d();
      }
      else if (DAT_1000_0256 == DAT_1000_0061) {
        DAT_1000_0061 = '\0';
        sprite_handler_327d();
      }
    }
    if (*(char *)0x93b == '\0') {
      if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
    }
    else {
      *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
      if (((*(byte *)0x962b & 1) != 0) ||
         (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
        puVar1 = (uint *)0x6c4;
        uVar11 = *puVar1;
        *puVar1 = *puVar1 - 2;
        if (uVar11 < 2) {
          *(int *)0x6c4 = *(int *)0x6c4 + 2;
        }
      }
    }
    func_1d2c();
    sprite_render_437b();
    load_resource_5d5f();
    update_entities();
    uVar7 = DAT_1000_005c;
    if (*(char *)0x8ec == '\0') {
      if (*(char *)0x924 < '\x02') {
        if (*(char *)0x92c < '\x02') {
          func_7dbb();
          unaff_DS = uVar7;
        }
        else if (*(char *)0x92d == '\0') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else if (*(char *)0x925 == '\x02') {
        func_7dbb();
      }
      else {
        func_7dbb();
      }
    }
    else {
      pcVar2 = (char *)0x8ec;
      *pcVar2 = *pcVar2 + -1;
      uVar7 = DAT_1000_005c;
      if (*pcVar2 == '\0') {
LAB_1000_29e3:
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if ('\x01' < *(char *)0x8ec) {
        func_7dbb();
        unaff_DS = uVar7;
      }
    }
    if (*(char *)0x89e != '\0') {
      *(char *)0x89e = *(char *)0x89e + -1;
      func_7dbb();
    }
    if (*(char *)0x8b1 != '\0') {
      sprite_handler_31eb();
    }
    if (*(char *)0x8a8 != '\0') {
      pcVar2 = (char *)0x8a8;
      *pcVar2 = *pcVar2 + -1;
      if (*pcVar2 == '\0') {
        *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
      }
      else if ((*(byte *)0x962b & 1) != 0) {
        func_7dbb();
      }
    }
    if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
      pcVar2 = (char *)0x91e;
      *pcVar2 = *pcVar2 + -1;
      if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
        func_7dbb();
      }
    }
    if (*(char *)0x924 == '\x01') {
      game_loop_chunk_378e();
    }
    if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
      func_7dbb();
    }
    if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
      DAT_1000_0061 = '\0';
      *(undefined1 *)0x921 = 0;
    }
    if (*(char *)0x8ed != '\0') {
      *(char *)0x8ed = *(char *)0x8ed + -1;
      render_handler_3b51();
    }
    toggle_graphics_page();
    if (*(int *)0x29e < *(int *)0x6c8) goto LAB_1000_29e3;
    wait_for_timer();
  } while( true );
}



// Player physics update - handles movement, gravity, velocity
// Part of main game loop (sub_35DE)
void update_player_physics(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  int *piVar5;
  code *pcVar6;
  undefined2 uVar7;
  char cVar8;
  int iVar9;
  byte bVar10;
  int iVar11;
  uint uVar12;
  int *piVar13;
  undefined2 unaff_DS;
  bool bVar14;
  
  if (*(char *)0x898 != '\0') {
    pcVar2 = (char *)0x898;
    *pcVar2 = *pcVar2 + -1;
    if (*pcVar2 != '\0') {
      if (DAT_1000_025c == '\x01') {
        *(int *)0x894 = *(int *)0x894 + -7;
      }
      else {
        *(undefined1 *)0x898 = 0;
      }
    }
  }
  iVar11 = *(int *)0x894;
  iVar9 = iVar11 + *(int *)0x6c8;
  if (SCARRY2(iVar11,*(int *)0x6c8) != iVar9 < 0) {
    iVar9 = 0;
  }
  *(int *)0x6c8 = iVar9;
  *(int *)0x9620 = iVar11;
  iVar11 = iVar11 + 5;
  if (0x17 < iVar11) {
    iVar11 = 0x17;
  }
  *(int *)0x894 = iVar11;
  iVar11 = *(int *)0x892;
  if (DAT_1000_0258 == '\x01') {
    *(undefined1 *)0x8ad = 2;
    iVar11 = iVar11 + -1;
    if (iVar11 < -5) {
      iVar11 = -5;
    }
  }
  if (DAT_1000_0259 == '\x01') {
    *(undefined1 *)0x8ad = 1;
    iVar11 = iVar11 + 1;
    if (5 < iVar11) {
      iVar11 = 5;
    }
  }
  *(int *)0x892 = iVar11;
  if (iVar11 != 0) {
    if (iVar11 < 1) {
      *(int *)0x892 = *(int *)0x892 + 1;
      move_player_left();
    }
    else {
      *(int *)0x892 = *(int *)0x892 + -1;
      move_player_right();
    }
  }
  bVar14 = false;
  if (*(int *)0x894 < 1) {
    *(undefined1 *)0x8b0 = 2;
    func_2c9d();
    *(undefined1 *)0x8b0 = 0;
    if (!bVar14) goto LAB_1000_2ad2;
    *(undefined2 *)0x894 = 0;
    *(uint *)0x6c8 = *(uint *)0x6c8 & 0xfff0;
    *(int *)0x6c8 = *(int *)0x6c8 + 10;
LAB_1000_2b4c:
    bVar14 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar14) {
      *(undefined1 *)0x89b = 0;
    }
    else {
      *(undefined1 *)0x89b = 1;
    }
  }
  else {
LAB_1000_2ad2:
    if (*(int *)0x9620 < 1) goto LAB_1000_2b4c;
    bVar14 = *(char *)0x924 == '\0';
    if ((*(char *)0x924 == '\x01') && (game_loop_chunk_362a(), bVar14)) {
LAB_1000_2b73:
      *(int *)0x6c8 = *(int *)0x6c8 + 8;
LAB_1000_2b78:
      *(uint *)0x6c8 = *(uint *)0x6c8 & 0xfff0;
      *(undefined1 *)0x897 = 0;
      *(undefined2 *)0x894 = 0;
      goto LAB_1000_2b88;
    }
    if (*(char *)0x89a != '\0') {
      *(char *)0x89a = *(char *)0x89a + -1;
    }
    bVar14 = 0xffdf < *(uint *)0x6c8;
    *(undefined1 *)0x8b0 = 1;
    iVar11 = func_2c9d();
    *(undefined1 *)0x8b0 = 0;
    if (bVar14) {
      if (*(char *)0x89b != '\x01') goto LAB_1000_2b31;
      goto LAB_1000_2b78;
    }
    if ((iVar11 <= *(int *)0x296) && (*(int *)0x294 < iVar11)) {
      handle_player_death();
      return;
    }
    *(undefined1 *)0x89b = 1;
LAB_1000_2b31:
    bVar14 = 0xffd7 < *(uint *)0x6c8;
    *(undefined1 *)0x8b0 = 1;
    func_2c9d();
    *(undefined1 *)0x8b0 = 0;
    if (bVar14) goto LAB_1000_2b73;
  }
  *(undefined1 *)0x8ae = 2;
LAB_1000_2b88:
  func_2b8e();
LAB_1000_267f:
  do {
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar8 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar8 = cVar8 + '\x01';
      }
      *(char *)0x898 = cVar8;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar12 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar12 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar7 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar7;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar7 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar7;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar8 = *(char *)0x8af + '\x01';
      if (cVar8 == '\x04') {
        cVar8 = '\x01';
      }
      *(char *)0x8af = cVar8;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if ('\x01' < *(char *)0x92c) {
      if (*(char *)0x896 != '\0') {
        *(undefined1 *)0x896 = 0;
        animation_data_handler();
      }
      *(int *)0x92e = *(int *)0x92e + 2;
      if (0xc < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xc;
      }
      *(int *)0x930 = *(int *)0x930 + 8;
      uVar12 = *(int *)0x932 + *(int *)0x92e;
      iVar9 = uVar12 + 0x20;
      iVar11 = check_tile_collision();
      bVar10 = *(byte *)(iVar9 - *(int *)0x298);
      if ((char)iVar11 == '\x16') {
LAB_1000_38ec:
        *(undefined1 *)0x8ec = 10;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (bVar10 == 0x20) {
LAB_1000_3915:
        uVar12 = uVar12 & 0xfff0;
        if (-1 < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xfff0;
        }
      }
      else if (iVar11 <= *(int *)0x292) {
        uVar12 = uVar12 & 0xfff0;
        *(undefined2 *)0x92e = 1;
        if (bVar10 != 0x1f) {
          if (bVar10 == 0x16) goto LAB_1000_38ec;
          *(undefined2 *)0x92e = 0;
          if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
        }
      }
      if ((*(int *)0x92e < 0) && ((int)(uint)bVar10 <= *(int *)0x292)) {
        uVar12 = *(uint *)0x932;
        *(undefined2 *)0x92e = 0;
      }
      *(uint *)0x932 = uVar12;
      *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
      *(undefined1 *)0x898 = *(undefined1 *)0x899;
      if (DAT_1000_025c == '\x01') {
        *(undefined1 *)0x898 = 0;
      }
      *(int *)0x6c6 = *(int *)0x930 + 8;
      *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
      if (*(char *)0x8ec == '\n') {
        *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
        *(int *)0x6c6 = *(int *)0x6c6 + -8;
      }
      if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
        main_game_loop();
      }
      if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
      func_2b8e();
      goto LAB_1000_267f;
    }
    if (*(char *)0x89d != '\0') {
      handle_airborne_movement();
      return;
    }
    if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
       ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
      game_loop_chunk_3610();
      DAT_1000_025a = '\0';
    }
    if (*(char *)0x897 != '\0') {
      update_player_physics();
      return;
    }
    if (*(char *)0x8a3 != '\0') {
      handle_player_animation();
      return;
    }
    if (*(char *)0x8a7 != '\0') {
      handle_attack_animation();
      return;
    }
    if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
       (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
      game_logic_385f();
      return;
    }
    iVar11 = check_tile_collision();
    if ((*(int *)0x294 < iVar11) && (iVar11 <= *(int *)0x296)) {
      handle_player_death();
      return;
    }
    if (*(char *)0x8a6 != '\0') {
      check_player_state();
      return;
    }
    if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
      *(undefined1 *)0x897 = 1;
      if (DAT_1000_025b == '\x01') {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        *(undefined2 *)0x892 = 0;
        *(undefined1 *)0x89a = 2;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (*(char *)0x9628 == '\x01') {
        *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
      }
      pcVar6 = (code *)swi(3);
      (*pcVar6)();
      return;
    }
    if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
      if ((*(int *)0x6fa == 5) &&
         ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
          ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
        menu_handler();
        return;
      }
      uVar12 = *(uint *)0x6ee;
      piVar13 = (int *)*(undefined2 *)0x6f0;
      if (uVar12 != 0) {
        do {
          if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
             (*(int *)0x6c8 == piVar13[1])) {
            piVar4 = (int *)piVar13[8];
            if (piVar4 != (int *)0x0) {
              piVar5 = (int *)*piVar4;
              if (piVar5 != (int *)0x0) {
                if ((piVar4[1] != piVar5[4]) || (*piVar5 != -1)) {
                  game_loop_chunk_3610();
                  break;
                }
                *(undefined2 *)piVar13[8] = 0;
              }
            }
            *(undefined1 *)0x962a = 5;
            handle_level_transition();
            return;
          }
          piVar13 = piVar13 + 9;
          bVar10 = (char)uVar12 - 1;
          uVar12 = (uint)bVar10;
        } while (bVar10 != 0);
      }
    }
    *(undefined2 *)0x892 = 0;
    if (DAT_1000_0258 == '\x01') {
      *(undefined2 *)0x892 = 0xfffb;
      graphics_handler();
    }
    if (DAT_1000_0259 == '\x01') {
      *(undefined2 *)0x892 = 5;
      anim_handler();
    }
    bVar14 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar14) {
      if (*(char *)0x89c == '\x01') {
        reset_player_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x894 = 5;
      if (*(int *)0x892 < 0) {
        *(undefined2 *)0x892 = 0xffff;
      }
      else if (*(int *)0x892 != 0) {
        *(undefined2 *)0x892 = 1;
      }
    }
    if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
       (DAT_1000_12d0 != -1)) {
      iVar9 = *(int *)0x6c8 + 0x20;
      iVar11 = check_tile_collision();
      if (((0x1b < iVar11) && (iVar11 < 0x1f)) ||
         (((*(uint *)0x6c6 & 8) != 0 &&
          ((iVar11 = CONCAT11((char)((uint)iVar11 >> 8),*(undefined1 *)(iVar9 + 1)), 0x1b < iVar11
           && (iVar11 < 0x1f)))))) {
        *(undefined1 *)0x9628 = 1;
        uVar3 = *(undefined1 *)0x8ae;
        if (*(char *)0x9629 < '\0') {
          move_player_left();
          *(undefined1 *)0x8ae = uVar3;
        }
        else {
          move_player_right();
          *(undefined1 *)0x8ae = uVar3;
        }
      }
    }
    if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
       (*(int *)0x6c6 == 0x28)) {
      if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
        load_level_entry_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x291 = 0;
    }
    if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
       (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
      render_chunk_46e6();
      DAT_1000_0061 = '\0';
    }
  } while( true );
}



// EGA graphics buffer swap
void toggle_graphics_buffer(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  
  iVar3 = *(int *)0x6c4;
  iVar1 = *(int *)0x6c8 - iVar3;
  if (iVar1 < 0x30) {
    if (iVar3 != 0) {
      if (iVar3 + iVar1 < 0x30) {
        *(undefined2 *)0x6c4 = 0;
      }
      else {
        *(int *)0x6c4 = iVar3 + iVar1 + -0x30;
      }
    }
  }
  else if ((0x50 < iVar1) && (iVar2 = *(int *)0x29e + -0xa0, iVar3 != iVar2)) {
    iVar3 = iVar3 + iVar1 + -0x50;
    if (iVar2 < iVar3) {
      *(int *)0x6c4 = iVar2;
    }
    else {
      *(int *)0x6c4 = iVar3;
    }
  }
  return;
}



// Graphics mode/page handler
void graphics_handler(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  if (*(char *)0x8ad == '\x02') {
    if (*(char *)0x9628 == '\x01') {
      *(undefined1 *)0x8ae = 1;
      return;
    }
    *(undefined1 *)0x9629 = 0xff;
    uVar1 = *(uint *)0x6c6;
    if (uVar1 == 0) {
      main_game_loop();
      func_2c32();
      return;
    }
    bVar2 = uVar1 < 8;
    func_2d61();
    if (bVar2) {
      *(undefined2 *)0x892 = 0;
    }
    else {
      *(int *)0x6c6 = uVar1 - 8;
      *(undefined1 *)0x8ae = 1;
      if (((int)((uVar1 - 8) - *(int *)0x6c2) < 0x50) && (*(int *)0x6c2 != 0)) {
        *(int *)0x6c2 = *(int *)0x6c2 + -8;
      }
    }
  }
  else {
    *(undefined1 *)0x8ad = 2;
  }
  return;
}



// Move player left with collision detection
void move_player_left(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  *(undefined1 *)0x9629 = 0xff;
  uVar1 = *(uint *)0x6c6;
  if (uVar1 == 0) {
    main_game_loop();
    func_2c32();
    return;
  }
  bVar2 = uVar1 < 8;
  func_2d61();
  if (bVar2) {
    *(undefined2 *)0x892 = 0;
  }
  else {
    *(int *)0x6c6 = uVar1 - 8;
    *(undefined1 *)0x8ae = 1;
    if (((int)((uVar1 - 8) - *(int *)0x6c2) < 0x50) && (*(int *)0x6c2 != 0)) {
      *(int *)0x6c2 = *(int *)0x6c2 + -8;
    }
  }
  return;
}



void __cdecl16near func_2c32(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x892 = 0;
  return;
}



// Animation state handler
void anim_handler(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  if (*(char *)0x8ad == '\x01') {
    if (*(char *)0x9628 == '\x01') {
      *(undefined1 *)0x8ae = 1;
      return;
    }
    *(undefined1 *)0x9629 = 1;
    if (*(int *)0x29c <= *(int *)0x6c6 + 0x10) {
      main_game_loop();
      func_2c32();
      return;
    }
    uVar1 = *(int *)0x6c6 + 8;
    bVar2 = 0xfff0 < uVar1;
    func_2d61();
    if (bVar2) {
      *(undefined2 *)0x892 = 0;
    }
    else {
      *(uint *)0x6c6 = uVar1;
      *(undefined1 *)0x8ae = 1;
      if ((0x70 < (int)(uVar1 - *(int *)0x6c2)) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
    }
  }
  else {
    *(undefined1 *)0x8ad = 1;
  }
  return;
}



// Move player right with collision detection
void move_player_right(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  *(undefined1 *)0x9629 = 1;
  if (*(int *)0x29c <= *(int *)0x6c6 + 0x10) {
    main_game_loop();
    func_2c32();
    return;
  }
  uVar1 = *(int *)0x6c6 + 8;
  bVar2 = 0xfff0 < uVar1;
  func_2d61();
  if (bVar2) {
    *(undefined2 *)0x892 = 0;
  }
  else {
    *(uint *)0x6c6 = uVar1;
    *(undefined1 *)0x8ae = 1;
    if ((0x70 < (int)(uVar1 - *(int *)0x6c2)) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
      *(int *)0x6c2 = *(int *)0x6c2 + 8;
    }
  }
  return;
}



// Sprite collision detection and rendering
void func_2c9d(void)

{
  uint in_AX;
  int iVar1;
  int in_BX;
  int unaff_DI;
  undefined2 unaff_DS;
  
  iVar1 = check_tile_collision();
  if (iVar1 == 0x100) {
    return;
  }
  if (iVar1 <= unaff_DI) {
    if (*(char *)0x8b0 < '\x01') goto LAB_1000_2cd1;
    iVar1 = collision_check_2d06();
  }
  if ('\x01' < *(char *)0x8b0) {
    iVar1 = collision_check_2d31();
  }
LAB_1000_2cd1:
  if ((in_AX & 0xc) != 0) {
    if (CONCAT11((char)((uint)iVar1 >> 8),*(undefined1 *)(in_BX + 1)) <= unaff_DI) {
      if (*(char *)0x8b0 < '\x01') {
        return;
      }
      collision_check_2d06();
    }
    if ('\x01' < *(char *)0x8b0) {
      collision_check_2d31();
    }
  }
  return;
}



undefined2 __cdecl16near collision_check_2d06(void)

{
  undefined2 in_AX;
  undefined2 unaff_DS;
  
  if ((*(char *)0x6d6 <= (char)in_AX) && ((char)in_AX <= *(char *)0x6d7)) {
    entity_collision();
  }
  return in_AX;
}



undefined2 __cdecl16near collision_check_2d31(void)

{
  undefined2 in_AX;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  if ((*(char *)0x6e2 <= (char)in_AX) && ((char)in_AX <= *(char *)0x6e3)) {
    *(undefined2 *)(unaff_BP + -2) = 1;
    entity_collision();
  }
  return in_AX;
}



// Collision detection - tile boundary check
void func_2d61(void)

{
  uint in_AX;
  int iVar1;
  undefined1 uVar2;
  int iVar3;
  undefined1 *puVar4;
  undefined2 unaff_DS;
  
  iVar3 = *(int *)0x6c8 + 0xd;
  iVar1 = check_tile_collision();
  if (iVar1 <= *(int *)0x292) {
    return;
  }
  puVar4 = (undefined1 *)(iVar3 + *(int *)0x298);
  uVar2 = (undefined1)((uint)iVar1 >> 8);
  if (*(int *)0x292 < CONCAT11(uVar2,*puVar4)) {
    if ((CONCAT11(uVar2,*puVar4) <= *(int *)0x294) &&
       (*(int *)0x294 < CONCAT11(uVar2,puVar4[(*(uint *)0x6c6 >> 4) - (in_AX >> 4)]))) {
      *(undefined1 *)0x89b = 0;
    }
  }
  else if (*(int *)0x292 < CONCAT11(uVar2,puVar4[(*(uint *)0x6c6 >> 4) - (in_AX >> 4)])) {
    return;
  }
  return;
}



// Player death/respawn handler - resets position and triggers sound
// Part of main game loop (sub_35DE)
void handle_player_death(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Horizontal movement handler for airborne player (jumping/falling)
// Part of main game loop (sub_35DE)
void handle_airborne_movement(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  int *piVar5;
  code *pcVar6;
  undefined2 uVar7;
  char cVar8;
  uint uVar9;
  byte bVar10;
  int iVar11;
  int iVar12;
  int *piVar13;
  undefined2 unaff_DS;
  bool bVar14;
  
  uVar9 = *(int *)0x894 + *(int *)0x6c8;
  if (DAT_1000_025a == '\x01') {
    uVar9 = uVar9 - 2;
  }
  if ((int)uVar9 < 0) {
    uVar9 = 0;
  }
  bVar14 = false;
  if (*(int *)0x894 != 0) {
    if (*(int *)0x894 < 0) {
      collision_helper_2fee();
      if (bVar14) {
        *(int *)0x6c8 = (uVar9 & 0xfff0) + 0x10;
        goto LAB_1000_2f31;
      }
    }
    else {
      bVar14 = 0xfff0 < uVar9;
      collision_helper_2fee();
      if (bVar14) {
        *(uint *)0x6c8 = uVar9 & 0xfff0;
        goto LAB_1000_2f31;
      }
    }
  }
  *(uint *)0x6c8 = uVar9;
LAB_1000_2f31:
  iVar11 = *(int *)0x894 + -5;
  if (iVar11 < -4) {
    iVar11 = -4;
  }
  *(int *)0x894 = iVar11;
  iVar11 = *(int *)0x892;
  if (DAT_1000_0258 == '\x01') {
    *(undefined1 *)0x8ad = 2;
    iVar11 = iVar11 + -1;
    if (iVar11 < -5) {
      iVar11 = -5;
    }
  }
  if (DAT_1000_0259 == '\x01') {
    *(undefined1 *)0x8ad = 1;
    iVar11 = iVar11 + 1;
    if (5 < iVar11) {
      iVar11 = 5;
    }
  }
  *(int *)0x892 = iVar11;
  if (iVar11 != 0) {
    if (iVar11 < 1) {
      *(int *)0x892 = *(int *)0x892 + 1;
      move_left_airborne();
    }
    else {
      *(int *)0x892 = *(int *)0x892 + -1;
      move_right_airborne();
    }
  }
  if (DAT_1000_025b == '\x01') {
    iVar11 = *(int *)0x894 + 7;
    if (4 < iVar11) {
      iVar11 = 4;
    }
    *(int *)0x894 = iVar11;
  }
  iVar11 = check_tile_collision();
  if (*(int *)0x296 < iVar11) {
    *(uint *)0x6c8 = *(uint *)0x6c8 & 0xfff0;
    *(int *)0x6c8 = *(int *)0x6c8 + 0xc;
    if (DAT_1000_025c == '\x01') {
      pcVar6 = (code *)swi(3);
      (*pcVar6)();
      return;
    }
  }
  bVar14 = 0xfff7 < *(uint *)0x6c8;
  collision_check_3019();
  if (!bVar14) {
    pcVar6 = (code *)swi(3);
    (*pcVar6)();
    return;
  }
  *(undefined1 *)0x8ae = 3;
  func_2b8e();
LAB_1000_267f:
  do {
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar8 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar8 = cVar8 + '\x01';
      }
      *(char *)0x898 = cVar8;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar9 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar9 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar7 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar7;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar7 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar7;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar8 = *(char *)0x8af + '\x01';
      if (cVar8 == '\x04') {
        cVar8 = '\x01';
      }
      *(char *)0x8af = cVar8;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if ('\x01' < *(char *)0x92c) {
      if (*(char *)0x896 != '\0') {
        *(undefined1 *)0x896 = 0;
        animation_data_handler();
      }
      *(int *)0x92e = *(int *)0x92e + 2;
      if (0xc < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xc;
      }
      *(int *)0x930 = *(int *)0x930 + 8;
      uVar9 = *(int *)0x932 + *(int *)0x92e;
      iVar12 = uVar9 + 0x20;
      iVar11 = check_tile_collision();
      bVar10 = *(byte *)(iVar12 - *(int *)0x298);
      if ((char)iVar11 == '\x16') {
LAB_1000_38ec:
        *(undefined1 *)0x8ec = 10;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (bVar10 == 0x20) {
LAB_1000_3915:
        uVar9 = uVar9 & 0xfff0;
        if (-1 < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xfff0;
        }
      }
      else if (iVar11 <= *(int *)0x292) {
        uVar9 = uVar9 & 0xfff0;
        *(undefined2 *)0x92e = 1;
        if (bVar10 != 0x1f) {
          if (bVar10 == 0x16) goto LAB_1000_38ec;
          *(undefined2 *)0x92e = 0;
          if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
        }
      }
      if ((*(int *)0x92e < 0) && ((int)(uint)bVar10 <= *(int *)0x292)) {
        uVar9 = *(uint *)0x932;
        *(undefined2 *)0x92e = 0;
      }
      *(uint *)0x932 = uVar9;
      *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
      *(undefined1 *)0x898 = *(undefined1 *)0x899;
      if (DAT_1000_025c == '\x01') {
        *(undefined1 *)0x898 = 0;
      }
      *(int *)0x6c6 = *(int *)0x930 + 8;
      *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
      if (*(char *)0x8ec == '\n') {
        *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
        *(int *)0x6c6 = *(int *)0x6c6 + -8;
      }
      if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
        main_game_loop();
      }
      if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
      func_2b8e();
      goto LAB_1000_267f;
    }
    if (*(char *)0x89d != '\0') {
      handle_airborne_movement();
      return;
    }
    if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
       ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
      game_loop_chunk_3610();
      DAT_1000_025a = '\0';
    }
    if (*(char *)0x897 != '\0') {
      update_player_physics();
      return;
    }
    if (*(char *)0x8a3 != '\0') {
      handle_player_animation();
      return;
    }
    if (*(char *)0x8a7 != '\0') {
      handle_attack_animation();
      return;
    }
    if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
       (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
      game_logic_385f();
      return;
    }
    iVar11 = check_tile_collision();
    if ((*(int *)0x294 < iVar11) && (iVar11 <= *(int *)0x296)) {
      handle_player_death();
      return;
    }
    if (*(char *)0x8a6 != '\0') {
      check_player_state();
      return;
    }
    if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
      *(undefined1 *)0x897 = 1;
      if (DAT_1000_025b == '\x01') {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        *(undefined2 *)0x892 = 0;
        *(undefined1 *)0x89a = 2;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (*(char *)0x9628 == '\x01') {
        *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
      }
      pcVar6 = (code *)swi(3);
      (*pcVar6)();
      return;
    }
    if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
      if ((*(int *)0x6fa == 5) &&
         ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
          ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
        menu_handler();
        return;
      }
      uVar9 = *(uint *)0x6ee;
      piVar13 = (int *)*(undefined2 *)0x6f0;
      if (uVar9 != 0) {
        do {
          if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
             (*(int *)0x6c8 == piVar13[1])) {
            piVar4 = (int *)piVar13[8];
            if (piVar4 != (int *)0x0) {
              piVar5 = (int *)*piVar4;
              if (piVar5 != (int *)0x0) {
                if ((piVar4[1] != piVar5[4]) || (*piVar5 != -1)) {
                  game_loop_chunk_3610();
                  break;
                }
                *(undefined2 *)piVar13[8] = 0;
              }
            }
            *(undefined1 *)0x962a = 5;
            handle_level_transition();
            return;
          }
          piVar13 = piVar13 + 9;
          bVar10 = (char)uVar9 - 1;
          uVar9 = (uint)bVar10;
        } while (bVar10 != 0);
      }
    }
    *(undefined2 *)0x892 = 0;
    if (DAT_1000_0258 == '\x01') {
      *(undefined2 *)0x892 = 0xfffb;
      graphics_handler();
    }
    if (DAT_1000_0259 == '\x01') {
      *(undefined2 *)0x892 = 5;
      anim_handler();
    }
    bVar14 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar14) {
      if (*(char *)0x89c == '\x01') {
        reset_player_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x894 = 5;
      if (*(int *)0x892 < 0) {
        *(undefined2 *)0x892 = 0xffff;
      }
      else if (*(int *)0x892 != 0) {
        *(undefined2 *)0x892 = 1;
      }
    }
    if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
       (DAT_1000_12d0 != -1)) {
      iVar12 = *(int *)0x6c8 + 0x20;
      iVar11 = check_tile_collision();
      if (((0x1b < iVar11) && (iVar11 < 0x1f)) ||
         (((*(uint *)0x6c6 & 8) != 0 &&
          ((iVar11 = CONCAT11((char)((uint)iVar11 >> 8),*(undefined1 *)(iVar12 + 1)), 0x1b < iVar11
           && (iVar11 < 0x1f)))))) {
        *(undefined1 *)0x9628 = 1;
        uVar3 = *(undefined1 *)0x8ae;
        if (*(char *)0x9629 < '\0') {
          move_player_left();
          *(undefined1 *)0x8ae = uVar3;
        }
        else {
          move_player_right();
          *(undefined1 *)0x8ae = uVar3;
        }
      }
    }
    if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
       (*(int *)0x6c6 == 0x28)) {
      if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
        load_level_entry_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x291 = 0;
    }
    if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
       (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
      render_chunk_46e6();
      DAT_1000_0061 = '\0';
    }
  } while( true );
}



undefined2 __cdecl16near collision_helper_2fee(void)

{
  uint in_AX;
  int iVar1;
  undefined1 uVar2;
  int in_BX;
  int unaff_DI;
  undefined2 unaff_DS;
  
  if (in_BX < *(int *)0x29e) {
    iVar1 = check_tile_collision();
    if (((unaff_DI < iVar1) &&
        (uVar2 = (undefined1)((uint)iVar1 >> 8),
        unaff_DI < CONCAT11(uVar2,*(undefined1 *)(in_BX + 1)))) &&
       (((in_AX & 0xc) == 0 || (unaff_DI < CONCAT11(uVar2,*(undefined1 *)(in_BX + 2)))))) {
      return 0;
    }
  }
  return 0;
}



void __cdecl16near collision_check_3019(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = check_tile_collision();
  if (iVar1 <= *(int *)0x296) {
    return;
  }
  return;
}



// Move player left while airborne (jumping/falling)
void move_left_airborne(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  uVar1 = *(uint *)0x6c6;
  if (uVar1 == 0) {
    main_game_loop();
    func_2c32();
    return;
  }
  bVar2 = uVar1 < 8;
  collision_2d_check();
  if (bVar2) {
    *(undefined2 *)0x892 = 0;
  }
  else {
    *(int *)0x6c6 = uVar1 - 8;
    if (((int)((uVar1 - 8) - *(int *)0x6c2) < 0x50) && (*(int *)0x6c2 != 0)) {
      *(int *)0x6c2 = *(int *)0x6c2 + -8;
    }
  }
  return;
}



void __cdecl16near move_left_cleanup(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x892 = 0;
  return;
}



// Move player right while airborne (jumping/falling)
void move_right_airborne(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  if (*(int *)0x29c <= *(int *)0x6c6 + 0x20) {
    main_game_loop();
    move_left_cleanup();
    return;
  }
  uVar1 = *(int *)0x6c6 + 8;
  bVar2 = 0xffe0 < uVar1;
  collision_2d_check();
  if (bVar2) {
    *(undefined2 *)0x892 = 0;
  }
  else {
    *(uint *)0x6c6 = uVar1;
    if ((0x60 < (int)(uVar1 - *(int *)0x6c2)) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
      *(int *)0x6c2 = *(int *)0x6c2 + 8;
    }
  }
  return;
}



void __cdecl16near collision_2d_check(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x294;
  iVar3 = *(int *)0x6c8;
  iVar2 = check_tile_collision();
  if ((iVar1 < iVar2) &&
     (((*(uint *)0x6c8 & 0xf) < 5 ||
      (iVar1 < CONCAT11((char)((uint)iVar2 >> 8),*(undefined1 *)(iVar3 + *(int *)0x298)))))) {
    return;
  }
  return;
}



void __cdecl16near sprite_physics(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x8ad == '\x01') {
    if (*(int *)0x29c + -0x18 < *(int *)0x6c6) {
      return;
    }
    *(undefined1 *)0x8a5 = 1;
  }
  else {
    if (*(int *)0x6c6 < 8) {
      return;
    }
    *(undefined1 *)0x8a5 = 2;
  }
  *(undefined1 *)0x8a3 = 1;
  *(undefined1 *)0x8a4 = 3;
  *(undefined1 *)0x8af = 1;
  *(undefined1 *)0x8ae = 4;
  *(undefined2 *)0x892 = 0;
  return;
}



// Player animation/state countdown handler
// Part of main game loop (sub_35DE)
void handle_player_animation(void)

{
  uint *puVar1;
  char *pcVar2;
  int *piVar3;
  int *piVar4;
  code *pcVar5;
  undefined2 uVar6;
  char cVar7;
  int iVar8;
  byte bVar9;
  uint uVar10;
  int iVar11;
  int *piVar12;
  undefined2 unaff_DS;
  bool bVar13;
  undefined1 uVar14;
  
  if ((*(char *)0x8a4 == '\x03') && (DAT_1000_025d == '\x01')) {
    *(undefined1 *)0x8af = 1;
    *(undefined1 *)0x8ae = 4;
  }
  else {
    pcVar2 = (char *)0x8a4;
    *pcVar2 = *pcVar2 + -1;
    if (*pcVar2 == '\0') {
      *(undefined1 *)0x8a3 = 0;
      *(undefined1 *)0x8ae = 0;
    }
    else {
      uVar10 = *(uint *)0x6c6;
      if ((uVar10 & 8) == 0) {
        iVar8 = uVar10 - 0x10;
        if (*(char *)0x8a5 == '\x01') {
          iVar8 = uVar10 + 0x10;
        }
        iVar11 = *(int *)0x6c8;
        if (*(char *)0x8a4 != '\x02') {
          iVar11 = iVar11 + 0x10;
        }
        cVar7 = check_tile_collision(iVar11,iVar8);
        if ((*(char *)0x6ca <= cVar7) && (cVar7 <= *(char *)0x6cb)) {
          entity_collision();
        }
      }
      *(undefined1 *)0x8ae = 4;
    }
  }
LAB_1000_267f:
  do {
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar7 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar7 = cVar7 + '\x01';
      }
      *(char *)0x898 = cVar7;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar10 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar10 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar6 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar6;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar6 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar6;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar5 = (code *)swi(3);
        (*pcVar5)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar7 = *(char *)0x8af + '\x01';
      if (cVar7 == '\x04') {
        cVar7 = '\x01';
      }
      *(char *)0x8af = cVar7;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if ('\x01' < *(char *)0x92c) {
      if (*(char *)0x896 != '\0') {
        *(undefined1 *)0x896 = 0;
        animation_data_handler();
      }
      *(int *)0x92e = *(int *)0x92e + 2;
      if (0xc < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xc;
      }
      *(int *)0x930 = *(int *)0x930 + 8;
      uVar10 = *(int *)0x932 + *(int *)0x92e;
      iVar11 = uVar10 + 0x20;
      iVar8 = check_tile_collision();
      bVar9 = *(byte *)(iVar11 - *(int *)0x298);
      if ((char)iVar8 == '\x16') {
LAB_1000_38ec:
        *(undefined1 *)0x8ec = 10;
        pcVar5 = (code *)swi(3);
        (*pcVar5)();
        return;
      }
      if (bVar9 == 0x20) {
LAB_1000_3915:
        uVar10 = uVar10 & 0xfff0;
        if (-1 < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xfff0;
        }
      }
      else if (iVar8 <= *(int *)0x292) {
        uVar10 = uVar10 & 0xfff0;
        *(undefined2 *)0x92e = 1;
        if (bVar9 != 0x1f) {
          if (bVar9 == 0x16) goto LAB_1000_38ec;
          *(undefined2 *)0x92e = 0;
          if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
        }
      }
      if ((*(int *)0x92e < 0) && ((int)(uint)bVar9 <= *(int *)0x292)) {
        uVar10 = *(uint *)0x932;
        *(undefined2 *)0x92e = 0;
      }
      *(uint *)0x932 = uVar10;
      *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
      *(undefined1 *)0x898 = *(undefined1 *)0x899;
      if (DAT_1000_025c == '\x01') {
        *(undefined1 *)0x898 = 0;
      }
      *(int *)0x6c6 = *(int *)0x930 + 8;
      *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
      if (*(char *)0x8ec == '\n') {
        *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
        *(int *)0x6c6 = *(int *)0x6c6 + -8;
      }
      if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
        main_game_loop();
      }
      if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
      func_2b8e();
      goto LAB_1000_267f;
    }
    if (*(char *)0x89d != '\0') {
      handle_airborne_movement();
      return;
    }
    if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
       ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
      game_loop_chunk_3610();
      DAT_1000_025a = '\0';
    }
    if (*(char *)0x897 != '\0') {
      update_player_physics();
      return;
    }
    if (*(char *)0x8a3 != '\0') {
      handle_player_animation();
      return;
    }
    if (*(char *)0x8a7 != '\0') {
      handle_attack_animation();
      return;
    }
    if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
       (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
      game_logic_385f();
      return;
    }
    iVar8 = check_tile_collision();
    if ((*(int *)0x294 < iVar8) && (iVar8 <= *(int *)0x296)) {
      handle_player_death();
      return;
    }
    if (*(char *)0x8a6 != '\0') {
      check_player_state();
      return;
    }
    if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
      *(undefined1 *)0x897 = 1;
      if (DAT_1000_025b == '\x01') {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        *(undefined2 *)0x892 = 0;
        *(undefined1 *)0x89a = 2;
        pcVar5 = (code *)swi(3);
        (*pcVar5)();
        return;
      }
      if (*(char *)0x9628 == '\x01') {
        *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
      }
      pcVar5 = (code *)swi(3);
      (*pcVar5)();
      return;
    }
    if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
      if ((*(int *)0x6fa == 5) &&
         ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
          ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
        menu_handler();
        return;
      }
      uVar10 = *(uint *)0x6ee;
      piVar12 = (int *)*(undefined2 *)0x6f0;
      if (uVar10 != 0) {
        do {
          if (((*piVar12 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar12 <= piVar12[2])) &&
             (*(int *)0x6c8 == piVar12[1])) {
            piVar3 = (int *)piVar12[8];
            if (piVar3 != (int *)0x0) {
              piVar4 = (int *)*piVar3;
              if (piVar4 != (int *)0x0) {
                if ((piVar3[1] != piVar4[4]) || (*piVar4 != -1)) {
                  game_loop_chunk_3610();
                  break;
                }
                *(undefined2 *)piVar12[8] = 0;
              }
            }
            *(undefined1 *)0x962a = 5;
            handle_level_transition();
            return;
          }
          piVar12 = piVar12 + 9;
          bVar9 = (char)uVar10 - 1;
          uVar10 = (uint)bVar9;
        } while (bVar9 != 0);
      }
    }
    *(undefined2 *)0x892 = 0;
    if (DAT_1000_0258 == '\x01') {
      *(undefined2 *)0x892 = 0xfffb;
      graphics_handler();
    }
    if (DAT_1000_0259 == '\x01') {
      *(undefined2 *)0x892 = 5;
      anim_handler();
    }
    bVar13 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar13) {
      if (*(char *)0x89c == '\x01') {
        reset_player_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x894 = 5;
      if (*(int *)0x892 < 0) {
        *(undefined2 *)0x892 = 0xffff;
      }
      else if (*(int *)0x892 != 0) {
        *(undefined2 *)0x892 = 1;
      }
    }
    if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
       (DAT_1000_12d0 != -1)) {
      iVar11 = *(int *)0x6c8 + 0x20;
      iVar8 = check_tile_collision();
      if (((0x1b < iVar8) && (iVar8 < 0x1f)) ||
         (((*(uint *)0x6c6 & 8) != 0 &&
          ((iVar8 = CONCAT11((char)((uint)iVar8 >> 8),*(undefined1 *)(iVar11 + 1)), 0x1b < iVar8 &&
           (iVar8 < 0x1f)))))) {
        *(undefined1 *)0x9628 = 1;
        uVar14 = *(undefined1 *)0x8ae;
        if (*(char *)0x9629 < '\0') {
          move_player_left();
          *(undefined1 *)0x8ae = uVar14;
        }
        else {
          move_player_right();
          *(undefined1 *)0x8ae = uVar14;
        }
      }
    }
    if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
       (*(int *)0x6c6 == 0x28)) {
      if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
        load_level_entry_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x291 = 0;
    }
    if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
       (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
      render_chunk_46e6();
      DAT_1000_0061 = '\0';
    }
  } while( true );
}



void __cdecl16near entity_collision(void)

{
  byte bVar1;
  code *pcVar2;
  char in_AL;
  int in_CX;
  int in_DX;
  undefined2 in_BX;
  int iVar3;
  char *pcVar4;
  int iVar5;
  undefined2 unaff_DS;
  
  if (*(char *)0x8b1 != '\x06') {
    iVar3 = 0x8b2;
    iVar5 = 6;
    while ((*(int *)(iVar3 + 5) != in_CX || (*(int *)(iVar3 + 7) != in_DX))) {
      iVar3 = iVar3 + 9;
      iVar5 = iVar5 + -1;
      if (iVar5 == 0) {
        for (pcVar4 = (char *)0x8b2; *pcVar4 != '\0'; pcVar4 = pcVar4 + 9) {
        }
        *(char *)0x8b1 = *(char *)0x8b1 + '\x01';
        *pcVar4 = '\x01';
        pcVar4[1] = in_AL;
        *(undefined2 *)(pcVar4 + 2) = in_BX;
        pcVar4[4] = '\0';
        *(int *)(pcVar4 + 5) = in_CX;
        *(int *)(pcVar4 + 7) = in_DX;
        bVar1 = pcVar4[1];
        func_1e09();
        if (*(int *)0x292 < (int)(uint)bVar1) {
          render_chunk_4579();
        }
        pcVar2 = (code *)swi(3);
        (*pcVar2)();
        return;
      }
    }
  }
  return;
}



void __cdecl16near sprite_handler_31eb(void)

{
  char *pcVar1;
  undefined2 unaff_DS;
  
  pcVar1 = (char *)0x8b2;
  *(undefined2 *)0x9620 = 0;
  do {
    if (*pcVar1 != '\0') {
      *(int *)0x9620 = *(int *)0x9620 + 1;
      if ((((*(int *)0x6c2 <= *(int *)(pcVar1 + 5)) && (*(int *)(pcVar1 + 5) - *(int *)0x6c2 < 0xc1)
           ) && (*(int *)0x6c4 <= *(int *)(pcVar1 + 7))) &&
         (*(int *)(pcVar1 + 7) - *(int *)0x6c4 < 0x91)) {
        ega_palette_77a3();
        unaff_DS = DAT_1000_005c;
      }
      pcVar1[4] = pcVar1[4] + '\x01';
      if (pcVar1[4] == '\x03') {
        if (*pcVar1 == '\x01') {
          **(char **)(pcVar1 + 2) = pcVar1[1];
        }
        *pcVar1 = '\0';
        (pcVar1 + 5)[0] = -1;
        (pcVar1 + 5)[1] = -1;
        *(char *)0x8b1 = *(char *)0x8b1 + -1;
        *(int *)0x9620 = *(int *)0x9620 + -1;
      }
    }
    pcVar1 = pcVar1 + 9;
  } while ((char)*(undefined2 *)0x9620 != *(char *)0x8b1);
  return;
}



void __cdecl16near sprite_handler_327d(void)

{
  char cVar1;
  char in_AL;
  char cVar2;
  undefined2 unaff_DS;
  bool bVar3;
  
  bVar3 = *(char *)0x89d == '\x01';
  if ((!bVar3) && (bVar3 = *(char *)0x924 == '\x02', !bVar3)) {
    bVar3 = *(char *)0x92c == '\x02';
  }
  if (!bVar3) {
    cVar2 = *(char *)0x896;
    do {
      while( true ) {
        cVar2 = cVar2 + in_AL;
        if ('\x03' < cVar2) {
          return;
        }
        if (cVar2 < '\0') {
          return;
        }
        if (cVar2 == '\0') goto LAB_1000_32cb;
        if (cVar2 != '\x03') break;
        cVar1 = *(char *)0x21f;
joined_r0x000132c0:
        if (cVar1 == '\x01') goto LAB_1000_32cb;
      }
      if (cVar2 == '\x02') {
        cVar1 = *(char *)0x21e;
        goto joined_r0x000132c0;
      }
    } while (*(char *)0x21d != '\x01');
LAB_1000_32cb:
    *(undefined1 *)0x8a3 = 0;
    *(char *)0x896 = cVar2;
    animation_data_handler();
    if (*(char *)0x896 == '\x02') {
      if ((*(int *)0x222 != 0) && (*(int *)0x6fa != 0xe)) {
        *(undefined1 *)0x897 = 0;
        *(undefined1 *)0x8a6 = 1;
      }
    }
    else if (*(char *)0x8a6 == '\x01') {
      *(undefined1 *)0x8a6 = 0;
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
    }
  }
  return;
}



// Game state check - handles player state transitions
// Part of main game loop (sub_35DE)
void check_player_state(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  int *piVar5;
  code *pcVar6;
  undefined2 uVar7;
  char cVar8;
  int iVar9;
  byte bVar10;
  int iVar11;
  uint uVar12;
  int *piVar13;
  undefined2 unaff_DS;
  bool bVar14;
  
  if ((*(int *)0x6fa == 0xe) || (*(int *)0x222 == 0)) {
    *(undefined1 *)0x8a6 = 0;
    *(undefined1 *)0x897 = 1;
    *(undefined1 *)0x898 = 0;
    update_player_physics();
    return;
  }
  if (((*(byte *)0x962b & 3) == 0) && (DAT_1000_14d4 != -1)) {
    set_palette_48aa();
  }
  if (((DAT_1000_025d == '\x01') || (DAT_1000_025a == '\x01')) &&
     (*(int *)0x894 = *(int *)0x894 + -2, *(int *)0x894 < -0xc)) {
    *(undefined2 *)0x894 = 0xfff4;
  }
  iVar11 = *(int *)0x894;
  iVar9 = iVar11 + *(int *)0x6c8;
  if (SCARRY2(iVar11,*(int *)0x6c8) != iVar9 < 0) {
    iVar9 = 0;
    iVar11 = 0;
  }
  *(int *)0x6c8 = iVar9;
  *(int *)0x9620 = iVar11;
  iVar11 = iVar11 + 1;
  if (8 < iVar11) {
    iVar11 = 8;
  }
  *(int *)0x894 = iVar11;
  iVar11 = *(int *)0x892;
  if (DAT_1000_0258 == '\x01') {
    *(undefined1 *)0x8ad = 2;
    iVar11 = iVar11 + -1;
    if (iVar11 < -5) {
      iVar11 = -5;
    }
  }
  if (DAT_1000_0259 == '\x01') {
    *(undefined1 *)0x8ad = 1;
    iVar11 = iVar11 + 1;
    if (5 < iVar11) {
      iVar11 = 5;
    }
  }
  *(int *)0x892 = iVar11;
  if (iVar11 != 0) {
    if (iVar11 < 1) {
      *(int *)0x892 = *(int *)0x892 + 1;
      move_player_left();
    }
    else {
      *(int *)0x892 = *(int *)0x892 + -1;
      move_player_right();
    }
  }
  bVar14 = false;
  if (*(int *)0x894 < 1) {
    *(undefined1 *)0x8b0 = 2;
    func_2c9d();
    *(undefined1 *)0x8b0 = 0;
    if (bVar14) {
      *(undefined2 *)0x894 = 0;
      *(uint *)0x6c8 = *(uint *)0x6c8 & 0xfff0;
      *(int *)0x6c8 = *(int *)0x6c8 + 10;
      goto LAB_1000_2b88;
    }
  }
  if (0 < *(int *)0x9620) {
    bVar14 = 0xffdf < *(uint *)0x6c8;
    iVar11 = func_2c9d();
    if (!bVar14) {
      if ((iVar11 <= *(int *)0x296) && (*(int *)0x294 < iVar11)) {
        handle_player_death();
        return;
      }
      if (*(int *)0x6c8 + 0x28 < *(int *)0x29e) goto LAB_1000_2b88;
      *(int *)0x6c8 = *(int *)0x6c8 + 8;
    }
    *(uint *)0x6c8 = *(uint *)0x6c8 & 0xfff0;
    *(undefined2 *)0x894 = 0;
  }
LAB_1000_2b88:
  *(undefined1 *)0x8ae = 5;
  func_2b8e();
LAB_1000_267f:
  do {
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar8 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar8 = cVar8 + '\x01';
      }
      *(char *)0x898 = cVar8;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar12 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar12 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar7 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar7;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar7 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar7;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar8 = *(char *)0x8af + '\x01';
      if (cVar8 == '\x04') {
        cVar8 = '\x01';
      }
      *(char *)0x8af = cVar8;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if ('\x01' < *(char *)0x92c) {
      if (*(char *)0x896 != '\0') {
        *(undefined1 *)0x896 = 0;
        animation_data_handler();
      }
      *(int *)0x92e = *(int *)0x92e + 2;
      if (0xc < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xc;
      }
      *(int *)0x930 = *(int *)0x930 + 8;
      uVar12 = *(int *)0x932 + *(int *)0x92e;
      iVar9 = uVar12 + 0x20;
      iVar11 = check_tile_collision();
      bVar10 = *(byte *)(iVar9 - *(int *)0x298);
      if ((char)iVar11 == '\x16') {
LAB_1000_38ec:
        *(undefined1 *)0x8ec = 10;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (bVar10 == 0x20) {
LAB_1000_3915:
        uVar12 = uVar12 & 0xfff0;
        if (-1 < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xfff0;
        }
      }
      else if (iVar11 <= *(int *)0x292) {
        uVar12 = uVar12 & 0xfff0;
        *(undefined2 *)0x92e = 1;
        if (bVar10 != 0x1f) {
          if (bVar10 == 0x16) goto LAB_1000_38ec;
          *(undefined2 *)0x92e = 0;
          if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
        }
      }
      if ((*(int *)0x92e < 0) && ((int)(uint)bVar10 <= *(int *)0x292)) {
        uVar12 = *(uint *)0x932;
        *(undefined2 *)0x92e = 0;
      }
      *(uint *)0x932 = uVar12;
      *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
      *(undefined1 *)0x898 = *(undefined1 *)0x899;
      if (DAT_1000_025c == '\x01') {
        *(undefined1 *)0x898 = 0;
      }
      *(int *)0x6c6 = *(int *)0x930 + 8;
      *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
      if (*(char *)0x8ec == '\n') {
        *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
        *(int *)0x6c6 = *(int *)0x6c6 + -8;
      }
      if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
        main_game_loop();
      }
      if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
      func_2b8e();
      goto LAB_1000_267f;
    }
    if (*(char *)0x89d != '\0') {
      handle_airborne_movement();
      return;
    }
    if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
       ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
      game_loop_chunk_3610();
      DAT_1000_025a = '\0';
    }
    if (*(char *)0x897 != '\0') {
      update_player_physics();
      return;
    }
    if (*(char *)0x8a3 != '\0') {
      handle_player_animation();
      return;
    }
    if (*(char *)0x8a7 != '\0') {
      handle_attack_animation();
      return;
    }
    if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
       (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
      game_logic_385f();
      return;
    }
    iVar11 = check_tile_collision();
    if ((*(int *)0x294 < iVar11) && (iVar11 <= *(int *)0x296)) {
      handle_player_death();
      return;
    }
    if (*(char *)0x8a6 != '\0') {
      check_player_state();
      return;
    }
    if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
      *(undefined1 *)0x897 = 1;
      if (DAT_1000_025b == '\x01') {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        *(undefined2 *)0x892 = 0;
        *(undefined1 *)0x89a = 2;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (*(char *)0x9628 == '\x01') {
        *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
      }
      pcVar6 = (code *)swi(3);
      (*pcVar6)();
      return;
    }
    if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
      if ((*(int *)0x6fa == 5) &&
         ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
          ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
        menu_handler();
        return;
      }
      uVar12 = *(uint *)0x6ee;
      piVar13 = (int *)*(undefined2 *)0x6f0;
      if (uVar12 != 0) {
        do {
          if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
             (*(int *)0x6c8 == piVar13[1])) {
            piVar4 = (int *)piVar13[8];
            if (piVar4 != (int *)0x0) {
              piVar5 = (int *)*piVar4;
              if (piVar5 != (int *)0x0) {
                if ((piVar4[1] != piVar5[4]) || (*piVar5 != -1)) {
                  game_loop_chunk_3610();
                  break;
                }
                *(undefined2 *)piVar13[8] = 0;
              }
            }
            *(undefined1 *)0x962a = 5;
            handle_level_transition();
            return;
          }
          piVar13 = piVar13 + 9;
          bVar10 = (char)uVar12 - 1;
          uVar12 = (uint)bVar10;
        } while (bVar10 != 0);
      }
    }
    *(undefined2 *)0x892 = 0;
    if (DAT_1000_0258 == '\x01') {
      *(undefined2 *)0x892 = 0xfffb;
      graphics_handler();
    }
    if (DAT_1000_0259 == '\x01') {
      *(undefined2 *)0x892 = 5;
      anim_handler();
    }
    bVar14 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar14) {
      if (*(char *)0x89c == '\x01') {
        reset_player_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x894 = 5;
      if (*(int *)0x892 < 0) {
        *(undefined2 *)0x892 = 0xffff;
      }
      else if (*(int *)0x892 != 0) {
        *(undefined2 *)0x892 = 1;
      }
    }
    if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
       (DAT_1000_12d0 != -1)) {
      iVar9 = *(int *)0x6c8 + 0x20;
      iVar11 = check_tile_collision();
      if (((0x1b < iVar11) && (iVar11 < 0x1f)) ||
         (((*(uint *)0x6c6 & 8) != 0 &&
          ((iVar11 = CONCAT11((char)((uint)iVar11 >> 8),*(undefined1 *)(iVar9 + 1)), 0x1b < iVar11
           && (iVar11 < 0x1f)))))) {
        *(undefined1 *)0x9628 = 1;
        uVar3 = *(undefined1 *)0x8ae;
        if (*(char *)0x9629 < '\0') {
          move_player_left();
          *(undefined1 *)0x8ae = uVar3;
        }
        else {
          move_player_right();
          *(undefined1 *)0x8ae = uVar3;
        }
      }
    }
    if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
       (*(int *)0x6c6 == 0x28)) {
      if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
        load_level_entry_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x291 = 0;
    }
    if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
       (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
      render_chunk_46e6();
      DAT_1000_0061 = '\0';
    }
  } while( true );
}



void __cdecl16near state_transition(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x8ad == '\x01') {
    if (*(int *)0x29c + -0x18 < *(int *)0x6c6) {
      return;
    }
    *(undefined1 *)0x8a5 = 1;
  }
  else {
    if (*(int *)0x6c6 < 8) {
      return;
    }
    *(undefined1 *)0x8a5 = 2;
  }
  *(undefined1 *)0x8a7 = 1;
  *(undefined1 *)0x8a4 = 3;
  *(undefined1 *)0x8af = 1;
  *(undefined1 *)0x8ae = 6;
  *(undefined2 *)0x892 = 0;
  return;
}



// Action/attack animation handler
// Part of main game loop (sub_35DE)
void handle_attack_animation(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  code *pcVar5;
  undefined2 uVar6;
  char cVar7;
  int iVar8;
  int iVar9;
  byte bVar10;
  uint uVar11;
  int iVar12;
  int *piVar13;
  int *piVar14;
  char *pcVar15;
  undefined2 unaff_DS;
  bool bVar16;
  
  pcVar2 = (char *)0x8a4;
  *pcVar2 = *pcVar2 + -1;
  if (*pcVar2 == '\0') {
    *(undefined1 *)0x8a7 = 0;
    *(undefined1 *)0x8ae = 0;
  }
  else {
    if ((*(char *)0x8a4 == '\x01') && (iVar9 = *(int *)0x6fe, iVar9 != 0)) {
      iVar12 = *(int *)0x6c6 + -0x10;
      if (*(char *)0x8a5 == '\x01') {
        iVar12 = *(int *)0x6c6 + 0x10;
      }
      piVar13 = (int *)0x700;
LAB_1000_34eb:
      if ((((piVar13[2] & 0x8000U) != 0) || (iVar12 != *piVar13)) ||
         (*(int *)0x6c8 + 0x10 != piVar13[1])) goto LAB_1000_34fd;
      if ((piVar13[3] != -0x5012) || (*(char *)0x220 < '\x05')) {
        piVar14 = (int *)0x946;
        iVar9 = *(int *)0x944;
        do {
          if (piVar13[3] == *piVar14) {
            iVar9 = piVar14[1];
            piVar13[3] = iVar9;
            iVar8 = (piVar13[2] & 0x7fffU) * 0xc;
            iVar12 = piVar14[2];
            *(int *)(iVar8 + 0xaf8) = iVar9;
            *(int *)(iVar8 + 0xafa) = iVar12;
            if (*(char *)0x8b1 != '\x06') {
              for (pcVar15 = (char *)0x8b2; *pcVar15 != '\0'; pcVar15 = pcVar15 + 9) {
              }
              *(char *)0x8b1 = *(char *)0x8b1 + '\x01';
              *pcVar15 = '\x02';
              pcVar15[4] = '\0';
              *(undefined2 *)(pcVar15 + 5) = *(undefined2 *)(iVar8 + 0xaf4);
              *(undefined2 *)(pcVar15 + 7) = *(undefined2 *)(iVar8 + 0xaf6);
            }
            pcVar5 = (code *)swi(3);
            (*pcVar5)();
            return;
          }
          piVar14 = piVar14 + 3;
          iVar9 = iVar9 + -1;
        } while (iVar9 != 0);
        goto LAB_1000_3589;
      }
      *(undefined2 *)((piVar13[2] & 0x7fffU) * 0xc + 0xaf0) = 0xfffe;
      *piVar13 = -1;
      display_handler_401b();
      *(undefined1 *)0x8a7 = 0;
      *(undefined1 *)0x8ae = 0;
      *(undefined1 *)0x93b = 0x14;
      *(char *)0x226 = *(char *)0x226 + '\x01';
      if (*(char *)0x226 == '\x06') {
        display_handler_409c();
        return;
      }
      goto LAB_1000_267f;
    }
LAB_1000_3589:
    *(undefined1 *)0x8ae = 6;
  }
LAB_1000_267f:
  do {
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar7 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar7 = cVar7 + '\x01';
      }
      *(char *)0x898 = cVar7;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar11 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar11 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar6 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar6;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar6 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar6;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar5 = (code *)swi(3);
        (*pcVar5)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar7 = *(char *)0x8af + '\x01';
      if (cVar7 == '\x04') {
        cVar7 = '\x01';
      }
      *(char *)0x8af = cVar7;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if ('\x01' < *(char *)0x92c) {
      if (*(char *)0x896 != '\0') {
        *(undefined1 *)0x896 = 0;
        animation_data_handler();
      }
      *(int *)0x92e = *(int *)0x92e + 2;
      if (0xc < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xc;
      }
      *(int *)0x930 = *(int *)0x930 + 8;
      uVar11 = *(int *)0x932 + *(int *)0x92e;
      iVar12 = uVar11 + 0x20;
      iVar9 = check_tile_collision();
      bVar10 = *(byte *)(iVar12 - *(int *)0x298);
      if ((char)iVar9 == '\x16') {
LAB_1000_38ec:
        *(undefined1 *)0x8ec = 10;
        pcVar5 = (code *)swi(3);
        (*pcVar5)();
        return;
      }
      if (bVar10 == 0x20) {
LAB_1000_3915:
        uVar11 = uVar11 & 0xfff0;
        if (-1 < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xfff0;
        }
      }
      else if (iVar9 <= *(int *)0x292) {
        uVar11 = uVar11 & 0xfff0;
        *(undefined2 *)0x92e = 1;
        if (bVar10 != 0x1f) {
          if (bVar10 == 0x16) goto LAB_1000_38ec;
          *(undefined2 *)0x92e = 0;
          if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
        }
      }
      if ((*(int *)0x92e < 0) && ((int)(uint)bVar10 <= *(int *)0x292)) {
        uVar11 = *(uint *)0x932;
        *(undefined2 *)0x92e = 0;
      }
      *(uint *)0x932 = uVar11;
      *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
      *(undefined1 *)0x898 = *(undefined1 *)0x899;
      if (DAT_1000_025c == '\x01') {
        *(undefined1 *)0x898 = 0;
      }
      *(int *)0x6c6 = *(int *)0x930 + 8;
      *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
      if (*(char *)0x8ec == '\n') {
        *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
        *(int *)0x6c6 = *(int *)0x6c6 + -8;
      }
      if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
        main_game_loop();
      }
      if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
      func_2b8e();
      goto LAB_1000_267f;
    }
    if (*(char *)0x89d != '\0') {
      handle_airborne_movement();
      return;
    }
    if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
       ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
      game_loop_chunk_3610();
      DAT_1000_025a = '\0';
    }
    if (*(char *)0x897 != '\0') {
      update_player_physics();
      return;
    }
    if (*(char *)0x8a3 != '\0') {
      handle_player_animation();
      return;
    }
    if (*(char *)0x8a7 != '\0') {
      handle_attack_animation();
      return;
    }
    if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
       (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
      game_logic_385f();
      return;
    }
    iVar9 = check_tile_collision();
    if ((*(int *)0x294 < iVar9) && (iVar9 <= *(int *)0x296)) {
      handle_player_death();
      return;
    }
    if (*(char *)0x8a6 != '\0') {
      check_player_state();
      return;
    }
    if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
      *(undefined1 *)0x897 = 1;
      if (DAT_1000_025b == '\x01') {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        *(undefined2 *)0x892 = 0;
        *(undefined1 *)0x89a = 2;
        pcVar5 = (code *)swi(3);
        (*pcVar5)();
        return;
      }
      if (*(char *)0x9628 == '\x01') {
        *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
      }
      pcVar5 = (code *)swi(3);
      (*pcVar5)();
      return;
    }
    if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
      if ((*(int *)0x6fa == 5) &&
         ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
          ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
        menu_handler();
        return;
      }
      uVar11 = *(uint *)0x6ee;
      piVar13 = (int *)*(undefined2 *)0x6f0;
      if (uVar11 != 0) {
        do {
          if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
             (*(int *)0x6c8 == piVar13[1])) {
            piVar14 = (int *)piVar13[8];
            if (piVar14 != (int *)0x0) {
              piVar4 = (int *)*piVar14;
              if (piVar4 != (int *)0x0) {
                if ((piVar14[1] != piVar4[4]) || (*piVar4 != -1)) {
                  game_loop_chunk_3610();
                  break;
                }
                *(undefined2 *)piVar13[8] = 0;
              }
            }
            *(undefined1 *)0x962a = 5;
            handle_level_transition();
            return;
          }
          piVar13 = piVar13 + 9;
          bVar10 = (char)uVar11 - 1;
          uVar11 = (uint)bVar10;
        } while (bVar10 != 0);
      }
    }
    *(undefined2 *)0x892 = 0;
    if (DAT_1000_0258 == '\x01') {
      *(undefined2 *)0x892 = 0xfffb;
      graphics_handler();
    }
    if (DAT_1000_0259 == '\x01') {
      *(undefined2 *)0x892 = 5;
      anim_handler();
    }
    bVar16 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar16) {
      if (*(char *)0x89c == '\x01') {
        reset_player_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x894 = 5;
      if (*(int *)0x892 < 0) {
        *(undefined2 *)0x892 = 0xffff;
      }
      else if (*(int *)0x892 != 0) {
        *(undefined2 *)0x892 = 1;
      }
    }
    if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
       (DAT_1000_12d0 != -1)) {
      iVar12 = *(int *)0x6c8 + 0x20;
      iVar9 = check_tile_collision();
      if (((0x1b < iVar9) && (iVar9 < 0x1f)) ||
         (((*(uint *)0x6c6 & 8) != 0 &&
          ((iVar9 = CONCAT11((char)((uint)iVar9 >> 8),*(undefined1 *)(iVar12 + 1)), 0x1b < iVar9 &&
           (iVar9 < 0x1f)))))) {
        *(undefined1 *)0x9628 = 1;
        uVar3 = *(undefined1 *)0x8ae;
        if (*(char *)0x9629 < '\0') {
          move_player_left();
          *(undefined1 *)0x8ae = uVar3;
        }
        else {
          move_player_right();
          *(undefined1 *)0x8ae = uVar3;
        }
      }
    }
    if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
       (*(int *)0x6c6 == 0x28)) {
      if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
        load_level_entry_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x291 = 0;
    }
    if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
       (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
      render_chunk_46e6();
      DAT_1000_0061 = '\0';
    }
  } while( true );
LAB_1000_34fd:
  piVar13 = piVar13 + 4;
  iVar9 = iVar9 + -1;
  if (iVar9 == 0) goto LAB_1000_3589;
  goto LAB_1000_34eb;
}



void __cdecl16near main_game_loop(void)

{
  byte bVar1;
  int *piVar3;
  undefined2 unaff_DS;
  uint uVar2;
  
  uVar2 = *(uint *)0x6f2;
  piVar3 = (int *)*(undefined2 *)0x6f4;
  if (uVar2 != 0) {
    do {
      if (((*(int *)0x6c6 == *piVar3) && (piVar3[1] <= *(int *)0x6c8)) &&
         (*(int *)0x6c8 <= piVar3[2])) {
        trigger_sound_interrupt();
        return;
      }
      piVar3 = piVar3 + 6;
      bVar1 = (char)uVar2 - 1;
      uVar2 = (uint)bVar1;
    } while (bVar1 != 0);
  }
  return;
}



void __cdecl16near game_loop_chunk_3610(void)

{
  code *pcVar1;
  undefined2 in_AX;
  undefined2 unaff_DS;
  
  if (*(char *)0x91e == '\0') {
    *(undefined2 *)0x91f = in_AX;
    *(undefined1 *)0x91e = 8;
    pcVar1 = (code *)swi(3);
    (*pcVar1)();
    return;
  }
  return;
}



void __cdecl16near game_loop_chunk_362a(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  if ((*(int *)0x928 <= *(int *)0x6c6) && (*(int *)0x6c6 - *(int *)0x928 < 0x11)) {
    iVar1 = *(int *)0x92a - *(int *)0x6c8;
    if (*(int *)0x92a < *(int *)0x6c8) {
      iVar1 = -iVar1;
    }
    if (iVar1 < 0xd) {
      *(undefined1 *)0x924 = 2;
      *(undefined2 *)0x926 = 8;
      return;
    }
  }
  return;
}



// Part of main game loop (sub_35DE)
void game_logic_3657(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  int *piVar5;
  code *pcVar6;
  undefined2 uVar7;
  char cVar8;
  int iVar9;
  byte bVar10;
  uint uVar11;
  int iVar12;
  int *piVar13;
  undefined2 unaff_DS;
  bool bVar14;
  
  if (*(char *)0x896 != '\0') {
    *(undefined1 *)0x896 = 0;
    animation_data_handler();
  }
  if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
    *(undefined1 *)0x897 = 1;
    *(undefined2 *)0x892 = 5;
    *(undefined1 *)0x924 = 1;
    update_player_physics();
    return;
  }
  if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
    main_game_loop();
  }
  if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
    *(int *)0x6c2 = *(int *)0x6c2 + 8;
  }
  func_2b8e();
LAB_1000_267f:
  do {
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar8 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar8 = cVar8 + '\x01';
      }
      *(char *)0x898 = cVar8;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar11 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar11 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar7 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar7;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar7 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar7;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar8 = *(char *)0x8af + '\x01';
      if (cVar8 == '\x04') {
        cVar8 = '\x01';
      }
      *(char *)0x8af = cVar8;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if ('\x01' < *(char *)0x92c) {
      if (*(char *)0x896 != '\0') {
        *(undefined1 *)0x896 = 0;
        animation_data_handler();
      }
      *(int *)0x92e = *(int *)0x92e + 2;
      if (0xc < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xc;
      }
      *(int *)0x930 = *(int *)0x930 + 8;
      uVar11 = *(int *)0x932 + *(int *)0x92e;
      iVar12 = uVar11 + 0x20;
      iVar9 = check_tile_collision();
      bVar10 = *(byte *)(iVar12 - *(int *)0x298);
      if ((char)iVar9 == '\x16') {
LAB_1000_38ec:
        *(undefined1 *)0x8ec = 10;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (bVar10 == 0x20) {
LAB_1000_3915:
        uVar11 = uVar11 & 0xfff0;
        if (-1 < *(int *)0x92e) {
          *(undefined2 *)0x92e = 0xfff0;
        }
      }
      else if (iVar9 <= *(int *)0x292) {
        uVar11 = uVar11 & 0xfff0;
        *(undefined2 *)0x92e = 1;
        if (bVar10 != 0x1f) {
          if (bVar10 == 0x16) goto LAB_1000_38ec;
          *(undefined2 *)0x92e = 0;
          if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
        }
      }
      if ((*(int *)0x92e < 0) && ((int)(uint)bVar10 <= *(int *)0x292)) {
        uVar11 = *(uint *)0x932;
        *(undefined2 *)0x92e = 0;
      }
      *(uint *)0x932 = uVar11;
      *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
      *(undefined1 *)0x898 = *(undefined1 *)0x899;
      if (DAT_1000_025c == '\x01') {
        *(undefined1 *)0x898 = 0;
      }
      *(int *)0x6c6 = *(int *)0x930 + 8;
      *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
      if (*(char *)0x8ec == '\n') {
        *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
        *(int *)0x6c6 = *(int *)0x6c6 + -8;
      }
      if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
        main_game_loop();
      }
      if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
        *(int *)0x6c2 = *(int *)0x6c2 + 8;
      }
      func_2b8e();
      goto LAB_1000_267f;
    }
    if (*(char *)0x89d != '\0') {
      handle_airborne_movement();
      return;
    }
    if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
       ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
      game_loop_chunk_3610();
      DAT_1000_025a = '\0';
    }
    if (*(char *)0x897 != '\0') {
      update_player_physics();
      return;
    }
    if (*(char *)0x8a3 != '\0') {
      handle_player_animation();
      return;
    }
    if (*(char *)0x8a7 != '\0') {
      handle_attack_animation();
      return;
    }
    if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
       (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
      game_logic_385f();
      return;
    }
    iVar9 = check_tile_collision();
    if ((*(int *)0x294 < iVar9) && (iVar9 <= *(int *)0x296)) {
      handle_player_death();
      return;
    }
    if (*(char *)0x8a6 != '\0') {
      check_player_state();
      return;
    }
    if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
      *(undefined1 *)0x897 = 1;
      if (DAT_1000_025b == '\x01') {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        *(undefined2 *)0x892 = 0;
        *(undefined1 *)0x89a = 2;
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if (*(char *)0x9628 == '\x01') {
        *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
      }
      pcVar6 = (code *)swi(3);
      (*pcVar6)();
      return;
    }
    if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
      if ((*(int *)0x6fa == 5) &&
         ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
          ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
        menu_handler();
        return;
      }
      uVar11 = *(uint *)0x6ee;
      piVar13 = (int *)*(undefined2 *)0x6f0;
      if (uVar11 != 0) {
        do {
          if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
             (*(int *)0x6c8 == piVar13[1])) {
            piVar4 = (int *)piVar13[8];
            if (piVar4 != (int *)0x0) {
              piVar5 = (int *)*piVar4;
              if (piVar5 != (int *)0x0) {
                if ((piVar4[1] != piVar5[4]) || (*piVar5 != -1)) {
                  game_loop_chunk_3610();
                  break;
                }
                *(undefined2 *)piVar13[8] = 0;
              }
            }
            *(undefined1 *)0x962a = 5;
            handle_level_transition();
            return;
          }
          piVar13 = piVar13 + 9;
          bVar10 = (char)uVar11 - 1;
          uVar11 = (uint)bVar10;
        } while (bVar10 != 0);
      }
    }
    *(undefined2 *)0x892 = 0;
    if (DAT_1000_0258 == '\x01') {
      *(undefined2 *)0x892 = 0xfffb;
      graphics_handler();
    }
    if (DAT_1000_0259 == '\x01') {
      *(undefined2 *)0x892 = 5;
      anim_handler();
    }
    bVar14 = 0xffdf < *(uint *)0x6c8;
    func_2c9d();
    if (bVar14) {
      if (*(char *)0x89c == '\x01') {
        reset_player_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x894 = 5;
      if (*(int *)0x892 < 0) {
        *(undefined2 *)0x892 = 0xffff;
      }
      else if (*(int *)0x892 != 0) {
        *(undefined2 *)0x892 = 1;
      }
    }
    if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
       (DAT_1000_12d0 != -1)) {
      iVar12 = *(int *)0x6c8 + 0x20;
      iVar9 = check_tile_collision();
      if (((0x1b < iVar9) && (iVar9 < 0x1f)) ||
         (((*(uint *)0x6c6 & 8) != 0 &&
          ((iVar9 = CONCAT11((char)((uint)iVar9 >> 8),*(undefined1 *)(iVar12 + 1)), 0x1b < iVar9 &&
           (iVar9 < 0x1f)))))) {
        *(undefined1 *)0x9628 = 1;
        uVar3 = *(undefined1 *)0x8ae;
        if (*(char *)0x9629 < '\0') {
          move_player_left();
          *(undefined1 *)0x8ae = uVar3;
        }
        else {
          move_player_right();
          *(undefined1 *)0x8ae = uVar3;
        }
      }
    }
    if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
       (*(int *)0x6c6 == 0x28)) {
      if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
        load_level_entry_position();
        return;
      }
    }
    else {
      *(undefined1 *)0x291 = 0;
    }
    if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
       (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
      render_chunk_46e6();
      DAT_1000_0061 = '\0';
    }
  } while( true );
}



void __cdecl16near game_loop_chunk_36c0(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  iVar2 = *(int *)0x926;
  iVar1 = *(int *)0x928;
  *(int *)0x928 = iVar2 + iVar1;
  iVar2 = check_tile_collision(*(undefined2 *)0x92a,iVar2 + iVar1);
  if (iVar2 != 0x1b) {
    iVar2 = check_tile_collision();
    if (iVar2 == 0x1a) {
      *(undefined1 *)0x925 = 1;
      goto LAB_1000_3731;
    }
    if (iVar2 != 0x1b) {
      if ((iVar2 < 0x1a) && (*(char *)0x925 = *(char *)0x925 + '\x01', *(char *)0x925 != '\x04')) {
        *(undefined1 *)0x925 = 2;
      }
      else if ('\x01' < *(char *)0x925) {
        *(char *)0x925 = *(char *)0x925 + -1;
      }
      goto LAB_1000_3731;
    }
  }
  *(undefined1 *)0x925 = 3;
LAB_1000_3731:
  iVar2 = *(int *)0x926;
  if (*(char *)0x925 == '\x01') {
    *(int *)0x92a = *(int *)0x92a + iVar2;
  }
  if (*(char *)0x925 == '\x03') {
    *(int *)0x92a = *(int *)0x92a - iVar2;
  }
  if ('\x01' < *(char *)0x924) {
    *(undefined1 *)0x8ad = 1;
    *(int *)0x6c6 = *(int *)0x928 + 8;
    *(undefined2 *)0x6c8 = *(undefined2 *)0x92a;
    iVar2 = check_tile_collision();
    if (iVar2 <= *(int *)0x292) {
      *(undefined1 *)0x924 = 1;
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x892 = 0xfffb;
      *(undefined1 *)0x8ae = 2;
    }
  }
  return;
}



void __cdecl16near game_loop_chunk_378e(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  if (*(char *)0x925 == '\x02') {
    iVar1 = func_7dbb();
    if (iVar1 != 0) {
      return;
    }
  }
  else {
    iVar1 = func_7dbb();
    if (iVar1 != 0) {
      return;
    }
  }
  if (*(int *)0x6c6 < *(int *)0x928) {
    if (*(int *)0x6fc == 1) {
      *(undefined2 *)0x928 = 0;
      *(undefined2 *)0x92a = 0x20;
      *(undefined2 *)0x926 = 8;
      *(undefined1 *)0x925 = 2;
    }
    else {
      if (*(int *)0x6fc != 0) {
        return;
      }
      *(undefined2 *)0x928 = 0xb0;
      *(undefined2 *)0x92a = 0xe0;
      *(undefined2 *)0x926 = 0;
    }
    iVar1 = *(int *)0x6c6;
    if (iVar1 < 0xac0) {
LAB_1000_381c:
      if (*(int *)0x928 < iVar1 + -0x140) {
        *(undefined2 *)0x926 = 8;
        *(int *)0x928 = *(int *)0x928 + 0x10;
        *(int *)0x92a = *(int *)0x92a + -0x10;
        do {
          iVar2 = check_tile_collision();
          if (iVar2 < 0x1c) goto LAB_1000_381c;
          *(int *)0x92a = *(int *)0x92a + 0x10;
        } while (*(int *)0x92a < *(int *)0x29e);
        *(undefined2 *)0x92a = 0;
        goto LAB_1000_381c;
      }
    }
  }
  return;
}



// Part of main game loop (sub_35DE)
void game_logic_385f(void)

{
  uint *puVar1;
  char *pcVar2;
  undefined1 uVar3;
  int *piVar4;
  int *piVar5;
  code *pcVar6;
  undefined2 uVar7;
  char cVar8;
  int iVar9;
  byte bVar10;
  uint uVar11;
  int iVar12;
  int *piVar13;
  undefined2 unaff_DS;
  bool bVar14;
  
  *(undefined2 *)0x92e = 0;
  *(undefined1 *)0x8a6 = 0;
  *(undefined1 *)0x92c = 2;
  do {
    if (*(char *)0x896 != '\0') {
      *(undefined1 *)0x896 = 0;
      animation_data_handler();
    }
    *(int *)0x92e = *(int *)0x92e + 2;
    if (0xc < *(int *)0x92e) {
      *(undefined2 *)0x92e = 0xc;
    }
    *(int *)0x930 = *(int *)0x930 + 8;
    uVar11 = *(int *)0x932 + *(int *)0x92e;
    iVar12 = uVar11 + 0x20;
    iVar9 = check_tile_collision();
    bVar10 = *(byte *)(iVar12 - *(int *)0x298);
    if ((char)iVar9 == '\x16') {
LAB_1000_38ec:
      *(undefined1 *)0x8ec = 10;
      pcVar6 = (code *)swi(3);
      (*pcVar6)();
      return;
    }
    if (bVar10 == 0x20) {
LAB_1000_3915:
      uVar11 = uVar11 & 0xfff0;
      if (-1 < *(int *)0x92e) {
        *(undefined2 *)0x92e = 0xfff0;
      }
    }
    else if (iVar9 <= *(int *)0x292) {
      uVar11 = uVar11 & 0xfff0;
      *(undefined2 *)0x92e = 1;
      if (bVar10 != 0x1f) {
        if (bVar10 == 0x16) goto LAB_1000_38ec;
        *(undefined2 *)0x92e = 0;
        if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) goto LAB_1000_3915;
      }
    }
    if ((*(int *)0x92e < 0) && ((int)(uint)bVar10 <= *(int *)0x292)) {
      uVar11 = *(uint *)0x932;
      *(undefined2 *)0x92e = 0;
    }
    *(uint *)0x932 = uVar11;
    *(undefined1 *)0x92d = (char)*(undefined2 *)0x92e;
    *(undefined1 *)0x898 = *(undefined1 *)0x899;
    if (DAT_1000_025c == '\x01') {
      *(undefined1 *)0x898 = 0;
    }
    *(int *)0x6c6 = *(int *)0x930 + 8;
    *(undefined2 *)0x6c8 = *(undefined2 *)0x932;
    if (*(char *)0x8ec == '\n') {
      *(int *)0x6c8 = *(int *)0x6c8 + 0x10;
      *(int *)0x6c6 = *(int *)0x6c6 + -8;
    }
    if (*(int *)0x6c6 + 0x10 == *(int *)0x29c) {
      main_game_loop();
    }
    if ((0x70 < *(int *)0x6c6 - *(int *)0x6c2) && (*(int *)0x6c2 + 0xd0 != *(int *)0x29c)) {
      *(int *)0x6c2 = *(int *)0x6c2 + 8;
    }
    func_2b8e();
LAB_1000_267f:
    if (*(char *)0x924 != '\0') {
      game_loop_chunk_36c0();
    }
    if ((*(char *)0x897 != '\x01') && (DAT_1000_025c != '\x01')) {
      cVar8 = *(char *)0x899;
      if ((DAT_1000_0e5c == -1) && (DAT_1000_0e64 == -0x4f6c)) {
        cVar8 = cVar8 + '\x01';
      }
      *(char *)0x898 = cVar8;
    }
    if (DAT_1000_025d == '\x01') {
      if (*(char *)0x896 == '\0') {
        if (*(char *)0x8ea != '\0') {
          if (*(char *)0x21c == '\0') {
            game_loop_chunk_3610();
          }
          else {
            load_resource_5ceb();
          }
          sprite_handler_3b12();
        }
      }
      else {
        state_setup_3ace();
        if (*(char *)0x896 == '\x03') {
          if (*(char *)0x897 == '\0' && *(char *)0x89d == '\0') {
            state_transition();
          }
        }
        else if ((*(char *)0x896 == '\x01') &&
                ((*(char *)0x897 == '\0' && *(char *)0x89d == '\0') && *(char *)0x8a3 == '\0')) {
          sprite_physics();
        }
      }
    }
    else {
      state_setup_3ace();
    }
    if (DAT_1000_0250 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      game_logic_39ad();
    }
    if (DAT_1000_0255 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    else if (DAT_1000_0256 == DAT_1000_0061) {
      DAT_1000_0061 = '\0';
      sprite_handler_327d();
    }
    do {
      if (*(char *)0x93b == '\0') {
        if (*(char *)0x93a == '\x01') goto LAB_1000_2770;
      }
      else {
        *(char *)0x93b = *(char *)0x93b + -1;
LAB_1000_2770:
        if (((*(byte *)0x962b & 1) != 0) ||
           (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
          puVar1 = (uint *)0x6c4;
          uVar11 = *puVar1;
          *puVar1 = *puVar1 - 2;
          if (uVar11 < 2) {
            *(int *)0x6c4 = *(int *)0x6c4 + 2;
          }
        }
      }
      func_1d2c();
      sprite_render_437b();
      load_resource_5d5f();
      update_entities();
      uVar7 = DAT_1000_005c;
      if (*(char *)0x8ec == '\0') {
        if (*(char *)0x924 < '\x02') {
          if (*(char *)0x92c < '\x02') {
            func_7dbb();
            unaff_DS = uVar7;
          }
          else if (*(char *)0x92d == '\0') {
            func_7dbb();
          }
          else {
            func_7dbb();
          }
        }
        else if (*(char *)0x925 == '\x02') {
          func_7dbb();
        }
        else {
          func_7dbb();
        }
      }
      else {
        pcVar2 = (char *)0x8ec;
        *pcVar2 = *pcVar2 + -1;
        uVar7 = DAT_1000_005c;
        if (*pcVar2 == '\0') goto LAB_1000_29e3;
        if ('\x01' < *(char *)0x8ec) {
          func_7dbb();
          unaff_DS = uVar7;
        }
      }
      if (*(char *)0x89e != '\0') {
        *(char *)0x89e = *(char *)0x89e + -1;
        func_7dbb();
      }
      if (*(char *)0x8b1 != '\0') {
        sprite_handler_31eb();
      }
      if (*(char *)0x8a8 != '\0') {
        pcVar2 = (char *)0x8a8;
        *pcVar2 = *pcVar2 + -1;
        if (*pcVar2 == '\0') {
          *(undefined2 *)*(undefined2 *)0x8ab = *(undefined2 *)0x8a9;
        }
        else if ((*(byte *)0x962b & 1) != 0) {
          func_7dbb();
        }
      }
      if ((*(char *)0x91e != '\0') && (*(char *)0x921 != '\x01')) {
        pcVar2 = (char *)0x91e;
        *pcVar2 = *pcVar2 + -1;
        if ((*pcVar2 != '\0') && ((*(byte *)0x962b & 1) != 0)) {
          func_7dbb();
        }
      }
      if (*(char *)0x924 == '\x01') {
        game_loop_chunk_378e();
      }
      if (('\0' < *(char *)0x92c) && ((*(char *)0x92c == '\x01' || (*(char *)0x8ec != '\0')))) {
        func_7dbb();
      }
      if ((*(char *)0x921 != '\0') && (render_tiles(), DAT_1000_0061 == '\x1c')) {
        DAT_1000_0061 = '\0';
        *(undefined1 *)0x921 = 0;
      }
      if (*(char *)0x8ed != '\0') {
        *(char *)0x8ed = *(char *)0x8ed + -1;
        render_handler_3b51();
      }
      toggle_graphics_page();
      if (*(int *)0x29e < *(int *)0x6c8) {
LAB_1000_29e3:
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      wait_for_timer();
      uRam0000041c = uRam0000041a;
      *(char *)0x962b = *(char *)0x962b + '\x01';
      *(BADSPACEBASE **)0x8e8 = register0x00000010;
      cVar8 = *(char *)0x8af + '\x01';
      if (cVar8 == '\x04') {
        cVar8 = '\x01';
      }
      *(char *)0x8af = cVar8;
      *(undefined1 *)0x8ae = 0;
      if (*(char *)0x962a != '\0') {
        *(char *)0x962a = *(char *)0x962a + -1;
      }
    } while ((*(char *)0x8ec != '\0') || (*(char *)0x921 != '\0'));
    if ('\x01' < *(char *)0x924) {
      game_logic_3657();
      return;
    }
    if (*(char *)0x92c < '\x02') {
      if (*(char *)0x89d != '\0') {
        handle_airborne_movement();
        return;
      }
      if (((*(char *)0x896 == '\x02') && ((*(int *)0x222 == 0 || (*(int *)0x6fa == 0xe)))) &&
         ((DAT_1000_025a == '\x01' || (DAT_1000_025d == '\x01')))) {
        game_loop_chunk_3610();
        DAT_1000_025a = '\0';
      }
      if (*(char *)0x897 != '\0') {
        update_player_physics();
        return;
      }
      if (*(char *)0x8a3 != '\0') {
        handle_player_animation();
        return;
      }
      if (*(char *)0x8a7 != '\0') {
        handle_attack_animation();
        return;
      }
      if (((*(char *)0x92c == '\x01') && (*(int *)0x930 <= *(int *)0x6c6)) &&
         (*(int *)0x6c6 - *(int *)0x930 < 0x19)) {
        game_logic_385f();
        return;
      }
      iVar9 = check_tile_collision();
      if ((*(int *)0x294 < iVar9) && (iVar9 <= *(int *)0x296)) {
        handle_player_death();
        return;
      }
      if (*(char *)0x8a6 != '\0') {
        check_player_state();
        return;
      }
      if ((DAT_1000_025c == '\x01') && (*(char *)0x898 != '\0')) {
        *(undefined1 *)0x897 = 1;
        if (DAT_1000_025b == '\x01') {
          *(undefined1 *)0x897 = 1;
          *(undefined1 *)0x898 = 0;
          *(undefined2 *)0x894 = 5;
          *(undefined2 *)0x892 = 0;
          *(undefined1 *)0x89a = 2;
          pcVar6 = (code *)swi(3);
          (*pcVar6)();
          return;
        }
        if (*(char *)0x9628 == '\x01') {
          *(char *)0x898 = (*(byte *)0x898 >> 1) + 1;
        }
        pcVar6 = (code *)swi(3);
        (*pcVar6)();
        return;
      }
      if ((*(char *)0x962a == '\0') && (DAT_1000_025a == '\x01')) {
        if ((*(int *)0x6fa == 5) &&
           ((((*(int *)0x6fc == 3 && (*(int *)0x6c8 == 0x70)) && (0x26f < *(int *)0x6c6)) &&
            ((*(int *)0x6c6 < 0x2a1 && (*(char *)0x220 < '\x05')))))) {
          menu_handler();
          return;
        }
        uVar11 = *(uint *)0x6ee;
        piVar13 = (int *)*(undefined2 *)0x6f0;
        if (uVar11 != 0) {
          do {
            if (((*piVar13 <= *(int *)0x6c6) && (*(int *)0x6c6 - *piVar13 <= piVar13[2])) &&
               (*(int *)0x6c8 == piVar13[1])) {
              piVar4 = (int *)piVar13[8];
              if (piVar4 != (int *)0x0) {
                piVar5 = (int *)*piVar4;
                if (piVar5 != (int *)0x0) {
                  if ((piVar4[1] != piVar5[4]) || (*piVar5 != -1)) {
                    game_loop_chunk_3610();
                    break;
                  }
                  *(undefined2 *)piVar13[8] = 0;
                }
              }
              *(undefined1 *)0x962a = 5;
              handle_level_transition();
              return;
            }
            piVar13 = piVar13 + 9;
            bVar10 = (char)uVar11 - 1;
            uVar11 = (uint)bVar10;
          } while (bVar10 != 0);
        }
      }
      *(undefined2 *)0x892 = 0;
      if (DAT_1000_0258 == '\x01') {
        *(undefined2 *)0x892 = 0xfffb;
        graphics_handler();
      }
      if (DAT_1000_0259 == '\x01') {
        *(undefined2 *)0x892 = 5;
        anim_handler();
      }
      bVar14 = 0xffdf < *(uint *)0x6c8;
      func_2c9d();
      if (bVar14) {
        if (*(char *)0x89c == '\x01') {
          reset_player_position();
          return;
        }
      }
      else {
        *(undefined1 *)0x897 = 1;
        *(undefined1 *)0x898 = 0;
        *(undefined2 *)0x894 = 5;
        if (*(int *)0x892 < 0) {
          *(undefined2 *)0x892 = 0xffff;
        }
        else if (*(int *)0x892 != 0) {
          *(undefined2 *)0x892 = 1;
        }
      }
      if (((*(char *)0x897 == '\0') && (*(undefined1 *)0x9628 = 0, *(int *)0x6fa == 9)) &&
         (DAT_1000_12d0 != -1)) {
        iVar12 = *(int *)0x6c8 + 0x20;
        iVar9 = check_tile_collision();
        if (((0x1b < iVar9) && (iVar9 < 0x1f)) ||
           (((*(uint *)0x6c6 & 8) != 0 &&
            ((iVar9 = CONCAT11((char)((uint)iVar9 >> 8),*(undefined1 *)(iVar12 + 1)), 0x1b < iVar9
             && (iVar9 < 0x1f)))))) {
          *(undefined1 *)0x9628 = 1;
          uVar3 = *(undefined1 *)0x8ae;
          if (*(char *)0x9629 < '\0') {
            move_player_left();
            *(undefined1 *)0x8ae = uVar3;
          }
          else {
            move_player_right();
            *(undefined1 *)0x8ae = uVar3;
          }
        }
      }
      if ((((*(int *)0x6fa == 5) && (*(int *)0x6fc == 3)) && (*(int *)0x6c8 == 0x60)) &&
         (*(int *)0x6c6 == 0x28)) {
        if ((*(char *)0x291 != '\x01') && (*(char *)0x220 < '\x05')) {
          load_level_entry_position();
          return;
        }
      }
      else {
        *(undefined1 *)0x291 = 0;
      }
      if (((*(char *)0x8a8 == '\0') && (DAT_1000_0257 == DAT_1000_0061)) &&
         (DAT_1000_0061 = '\0', *(char *)0x897 == '\0')) {
        render_chunk_46e6();
        DAT_1000_0061 = '\0';
      }
      goto LAB_1000_267f;
    }
  } while( true );
}



// Part of main game loop (sub_35DE)
void game_logic_39ad(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



void __cdecl16near animation_data_handler(void)

{
  ega_palette_790c();
  ega_palette_790c();
  return;
}



void __cdecl16near movement_handler_3a5f(void)

{
  ega_palette_790c();
  ega_palette_790c();
  return;
}



void __cdecl16near action_handler_3a92(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x8eb = 0;
  iVar1 = 0x100;
  uVar3 = 0x43;
  iVar2 = 6;
  uVar4 = *(undefined2 *)0x98d8;
  do {
    ega_palette_790c(uVar4,iVar2,uVar3,iVar1);
    iVar1 = iVar1 + 8;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



void __cdecl16near state_setup_3aa0(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x8ea = 0;
  iVar1 = 0x100;
  uVar3 = 0x2a;
  iVar2 = 6;
  uVar4 = *(undefined2 *)0x98d8;
  do {
    ega_palette_790c(uVar4,iVar2,uVar3,iVar1);
    iVar1 = iVar1 + 8;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



void __cdecl16near state_setup_3ace(void)

{
  byte bVar1;
  undefined2 unaff_DS;
  
  bVar1 = *(byte *)0x8ea;
  if ((bVar1 != 0x17) &&
     (*(char *)0x8ea = *(char *)0x8ea + '\x01', (uint)(bVar1 >> 1) != bVar1 - 1 >> 1)) {
    ega_palette_790c();
  }
  return;
}



void __cdecl16near sprite_handler_3b12(void)

{
  char cVar1;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)0x8ea;
  if ((cVar1 != '\0') && (*(char *)0x8ea = *(char *)0x8ea + -1, (cVar1 - 1U & 1) == 0)) {
    ega_palette_790c();
  }
  return;
}



void __cdecl16near render_handler_3b51(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x8eb != '\f') {
    *(char *)0x8eb = *(char *)0x8eb + '\x01';
    ega_palette_790c();
  }
  return;
}



// Display sprite/graphics with sound trigger
void display_sprite(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Sprite/graphics display function
void render_graphics(void)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int *unaff_SI;
  undefined2 unaff_DS;
  
  iVar2 = (*unaff_SI - *(int *)0x6c2) + 8;
  iVar3 = (unaff_SI[1] - *(int *)0x6c4) + 8;
  uVar4 = (uint)*(byte *)(unaff_SI + 3);
  uVar5 = (uint)*(byte *)((int)unaff_SI + 7);
  uVar6 = (uint)*(byte *)(unaff_SI + 4);
  uVar7 = (uint)*(byte *)((int)unaff_SI + 9);
  func_1d2c();
  update_entities();
  draw_line_db6(uVar7,uVar6,uVar5,uVar4,iVar3,iVar2);
  wait_for_timer();
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Complex animation sequence handler
void play_animation_sequence(void)

{
  code *pcVar1;
  int *unaff_SI;
  undefined2 unaff_DS;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)(*(undefined1 *)((int)unaff_SI + 9),(char)unaff_SI[4],*(undefined1 *)((int)unaff_SI + 7),
            (char)unaff_SI[3],(unaff_SI[1] - *(int *)0x6c4) + 8,(*unaff_SI - *(int *)0x6c2) + 8);
  return;
}



void __cdecl16near draw_line_db6(void)

{
  sprite_render_437b();
  toggle_graphics_page();
  wait_for_timer();
  return;
}



void __cdecl16near draw_callback(void)

{
  func_7dbb();
  return;
}



// Player death/respawn animation
void player_death_animation(void)

{
  code *pcVar1;
  undefined2 unaff_DS;
  undefined2 local_4;
  
  *(undefined1 *)0x8ec = 10;
  local_4 = 10;
  do {
    func_1d2c();
    update_entities();
    draw_line_db6();
    local_4 = local_4 + -1;
  } while (local_4 != 0);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Fade out or screen transition effect
void screen_fade_effect(void)

{
  code *pcVar1;
  undefined2 unaff_DS;
  undefined2 local_4;
  
  *(undefined1 *)0x8ec = 10;
  local_4 = 10;
  do {
    func_1d2c();
    update_entities();
    draw_callback();
    draw_line_db6();
    local_4 = local_4 + -1;
  } while (local_4 != 0);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Position/coordinate calculation and display handler
void calculate_display_position(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Graphics/sprite display handler
void display_handler_401b(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



void __cdecl16near graphics_mode_4073(void)

{
  uint *puVar1;
  uint uVar2;
  uint in_AX;
  undefined2 unaff_DS;
  
  if (((in_AX & 1) != 0) ||
     (*(int *)0x6c4 = *(int *)0x6c4 + 2, *(int *)0x29e < *(int *)0x6c4 + 0xa0)) {
    puVar1 = (uint *)0x6c4;
    uVar2 = *puVar1;
    *puVar1 = *puVar1 - 2;
    if (uVar2 < 2) {
      *(int *)0x6c4 = *(int *)0x6c4 + 2;
    }
  }
  func_1d2c();
  update_entities();
  return;
}



// Graphics/sprite handler
void display_handler_409c(void)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int local_4;
  
  uVar3 = 0;
  uVar2 = 0;
  do {
    graphics_mode_4073(uVar2);
    draw_callback();
    if ((uVar3 & 1) == 0) {
      uVar2 = uVar2 ^ 4;
    }
    iVar5 = 0;
    do {
      uVar2 = uVar2 ^ 4;
      func_7dbb();
      iVar5 = iVar5 + 1;
    } while (iVar5 < 9);
    iVar5 = -0x68e8;
    iVar6 = 0;
    do {
      uVar2 = uVar2 ^ 4;
      func_7dbb();
      iVar5 = iVar5 + 4;
      iVar6 = iVar6 + 1;
    } while (iVar6 < 9);
    draw_line_db6();
    uVar3 = uVar3 + 1;
  } while (uVar3 != 0x32);
  local_4 = 0x50;
  iVar4 = 0;
  do {
    graphics_mode_4073(uVar2,iVar4,iVar6,iVar5);
    draw_callback();
    func_7dbb();
    draw_line_db6();
    local_4 = local_4 + -8;
    iVar4 = iVar4 + 1;
  } while (-8 < local_4);
  do {
    graphics_mode_4073(uVar2,iVar4);
    draw_callback();
    func_7dbb();
    uVar1 = DAT_1000_005c;
    draw_line_db6();
    iVar4 = iVar4 + 1;
    *(int *)0x6c8 = *(int *)0x6c8 + -8;
  } while (-0x20 < *(int *)0x6c8);
  *(undefined1 *)0x220 = 6;
  *(undefined2 *)0x210 = 0xa0;
  *(undefined2 *)0x212 = 0;
  *(undefined2 *)0x6fa = 0xffff;
  *(undefined2 *)0x6fc = 7;
  load_area_sprite_table();
  reset_game_state();
  return;
}



void __cdecl16near render_tiles(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  int iVar4;
  byte *unaff_SI;
  undefined2 unaff_DS;
  
  uVar1 = (uint)*unaff_SI;
  uVar2 = (uint)unaff_SI[1];
  if (uVar1 * 8 < 0xd1) {
    iVar3 = (uVar1 * -8 + 0xd0 >> 1 & 0xfff8) + 8;
  }
  else {
    iVar3 = 0;
  }
  if (uVar2 * 8 < 0xa1) {
    iVar4 = (uVar2 * -8 + 0xa0 >> 1 & 0xfff8) + 8;
  }
  else {
    iVar4 = 0;
  }
  ega_blit_7ac8(iVar4,iVar3,uVar2,uVar1);
  func_4339();
  iVar3 = uVar2 - 2;
  func_42e8();
  do {
    func_42e8();
  } while (extraout_DX != 1);
  func_42e8();
  do {
    func_42e8();
    func_4339();
    do {
      func_42e8();
    } while (extraout_DX_00 != 1);
    func_4339();
    func_42e8();
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  func_42e8();
  do {
    func_42e8();
  } while (extraout_DX_01 != 1);
  func_42e8();
  return;
}



undefined2 __cdecl16near func_42d4(void)

{
  char *pcVar1;
  undefined2 in_AX;
  char *unaff_SI;
  undefined2 unaff_DS;
  
  while (pcVar1 = unaff_SI, unaff_SI = unaff_SI + 1, *pcVar1 != '\0') {
    func_42e8();
  }
  return in_AX;
}



undefined4 __cdecl16near func_42e8(void)

{
  undefined2 uVar1;
  uint in_AX;
  int in_CX;
  int iVar2;
  undefined2 in_DX;
  uint in_BX;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined2 unaff_ES;
  
  uVar1 = DAT_1000_005e;
  puVar4 = (undefined1 *)(DAT_1000_773f + (in_BX >> 3) + in_CX * 0x28);
  puVar3 = (undefined1 *)((in_AX & 0xff) + 0xa6d4);
  iVar2 = 8;
  do {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 0x3b;
    puVar4 = puVar4 + 0x28;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return CONCAT22(in_DX,in_AX);
}



undefined1 __cdecl16near func_4339(void)

{
  undefined1 in_AL;
  
  out(0x3c4,2);
  out(0x3c5,in_AL);
  return 2;
}



void __cdecl16near sprite_render_437b(void)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  undefined2 unaff_DS;
  
  iVar7 = *(int *)0x6fe;
  if (iVar7 == 0) {
    return;
  }
  iVar1 = *(int *)0x6c2;
  iVar2 = *(int *)0x6c4;
  piVar6 = (int *)0x700;
  do {
    if (((((piVar6[2] & 0x8000U) == 0) && (iVar1 <= *piVar6)) && (*piVar6 <= iVar1 + 0xc0)) &&
       ((iVar2 + -0xf <= piVar6[1] && (piVar6[1] <= iVar2 + 0x9f)))) {
      iVar4 = 0xf;
      iVar5 = 0x1f;
      if (*(char *)0x89d == '\x01') {
        iVar5 = 0xf;
        iVar4 = 0x1f;
      }
      if (((*(int *)0x6c6 <= *piVar6 + 0xf) && (*piVar6 - iVar4 <= *(int *)0x6c6)) &&
         ((*(int *)0x6c8 <= piVar6[1] + 0xf &&
          ((piVar6[1] - iVar5 <= *(int *)0x6c8 && (*(char *)0x8ec == '\0')))))) {
        pcVar3 = (code *)swi(3);
        (*pcVar3)();
        return;
      }
      func_7dbb();
    }
    piVar6 = piVar6 + 4;
    iVar7 = iVar7 + -1;
    if (iVar7 == 0) {
      return;
    }
  } while( true );
}



void __cdecl16near render_level(void)

{
  int iVar1;
  int in_AX;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)(in_AX * 2 + 0x239);
  if (iVar1 == -1) {
    ega_blit_7aab();
    return;
  }
  ega_palette_78c6(iVar1 * 0xc + 0xaf0);
  return;
}



void __cdecl16near render_chunk_4579(void)

{
  int in_AX;
  int in_BX;
  int *piVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  iVar2 = *(int *)0x6fe;
  if (iVar2 == 0) {
    return;
  }
  piVar1 = (int *)0x700;
  do {
    if ((in_AX == *piVar1) && (in_BX == piVar1[1])) {
      piVar1[2] = piVar1[2] & 0x7fff;
    }
    piVar1 = piVar1 + 4;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



// Graphics resource loader
void load_graphics_resource(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// WARNING: Removing unreachable block (ram,0x0001476f)
// WARNING: Removing unreachable block (ram,0x00014776)
// WARNING: Removing unreachable block (ram,0x0001475c)
// WARNING: Removing unreachable block (ram,0x00014763)
// WARNING: Removing unreachable block (ram,0x00014783)
// WARNING: Removing unreachable block (ram,0x0001478d)
// WARNING: Removing unreachable block (ram,0x0001479a)
// WARNING: Removing unreachable block (ram,0x000147a2)
// WARNING: Removing unreachable block (ram,0x000147a9)
// WARNING: Removing unreachable block (ram,0x000147b0)
// WARNING: Removing unreachable block (ram,0x000147b8)
// WARNING: Removing unreachable block (ram,0x000147bf)
// WARNING: Removing unreachable block (ram,0x000147dd)
// WARNING: Removing unreachable block (ram,0x00014806)
// WARNING: Removing unreachable block (ram,0x0001480b)
// WARNING: Removing unreachable block (ram,0x00014811)
// WARNING: Removing unreachable block (ram,0x00014818)
// WARNING: Removing unreachable block (ram,0x00014819)
// WARNING: Removing unreachable block (ram,0x0001481d)

void __cdecl16near render_chunk_46e6(void)

{
  func_1d2c();
  sprite_render_437b();
  toggle_graphics_page();
  func_1d2c();
  sprite_render_437b();
  ega_palette_77a3();
  toggle_graphics_page();
  DAT_1000_0258 = 0;
  DAT_1000_0259 = 0;
  DAT_1000_025d = 0;
  DAT_1000_0061 = 0;
  do {
  } while (DAT_1000_0257 != '\0');
  render_level();
  wait_for_timer();
  return;
}



// Graphics handler
void graphics_handler_4870(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



void __cdecl16near set_palette(void)

{
  return;
}



void __cdecl16near set_palette_48aa(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined2 *)0x222;
  set_palette();
  *(undefined2 *)0x222 = uVar1;
  movement_handler_3a5f();
  return;
}



void __cdecl16near mode_setup_48d4(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x6fe;
  if (iVar1 != 0) {
    iVar2 = 0x700;
    do {
      if (0x7fff < *(uint *)(iVar2 + 2)) {
        *(int *)(iVar2 + 2) = -*(int *)(iVar2 + 2);
      }
      iVar2 = iVar2 + 8;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return;
}



void __cdecl16near display_handler_49ad(void)

{
  char cVar1;
  int iVar2;
  undefined2 *puVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_DS;
  int local_4;
  
  *(undefined1 *)0x9a22 = 0;
  *(undefined1 *)0x9a24 = 0;
  cVar1 = *(char *)(*(int *)0x6fa + 0x229);
  *(char *)0x9a23 = cVar1;
  if (cVar1 != '\x01') {
    mode_setup_48d4();
  }
  puVar3 = (undefined2 *)*(undefined2 *)0x6f8;
  for (iVar2 = *(int *)0x6f6; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)(puVar3 + 3) = 0;
    puVar3[4] = *puVar3;
    puVar3[5] = puVar3[1];
    puVar3 = puVar3 + 6;
  }
  local_4 = 0;
  iVar4 = 0x48ed;
  iVar5 = *(int *)0x6f8;
  iVar2 = *(int *)0x6f6;
  do {
    if (iVar2 == 0) {
      do {
        *(undefined2 *)(iVar4 + 6) = 0;
        iVar4 = iVar4 + 0x20;
        local_4 = local_4 + 1;
      } while (local_4 != 6);
      return;
    }
    if ((((*(int *)0x6c2 <= *(int *)(iVar5 + 8)) &&
         (*(int *)(iVar5 + 8) + *(int *)*(int *)(iVar5 + 4) <= *(int *)0x6c2 + 0xd0)) &&
        (*(int *)(iVar5 + 10) < *(int *)0x6c4 + 0xa0)) &&
       (*(int *)0x6c4 < *(int *)(iVar5 + 10) + ((int *)*(int *)(iVar5 + 4))[1])) {
      display_handler_4a70();
      iVar4 = iVar4 + 0x20;
      local_4 = local_4 + 1;
      if (local_4 == 6) {
        return;
      }
    }
    iVar5 = iVar5 + 0xc;
    iVar2 = iVar2 + -1;
  } while( true );
}



void __cdecl16near display_handler_4a70(void)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 *in_BX;
  undefined2 *unaff_SI;
  undefined2 *puVar4;
  int unaff_DI;
  undefined2 unaff_DS;
  
  iVar3 = 8;
  puVar4 = unaff_SI;
  do {
    puVar1 = in_BX;
    in_BX = in_BX + 1;
    *puVar4 = *puVar1;
    puVar4 = puVar4 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  unaff_SI[8] = *(undefined2 *)(unaff_DI + 8);
  unaff_SI[9] = *(undefined2 *)(unaff_DI + 10);
  unaff_SI[10] = 0;
  unaff_SI[0xb] = 0;
  unaff_SI[0xe] = 0;
  unaff_SI[0xd] = 0;
  unaff_SI[0xf] = unaff_DI;
  uVar2 = *(undefined2 *)0x9a20;
  *(uint *)0x9a20 = *(uint *)0x9a20 ^ 3;
  unaff_SI[0xc] = uVar2;
  *(undefined1 *)(unaff_DI + 6) = 1;
  if ((char)((uint)unaff_SI[2] >> 8) == '\x01') {
    if (*(char *)0x9a23 == '\x01') {
      if (*(char *)0x9a22 == '\0') {
        *(undefined1 *)0x9a22 = (char)unaff_SI[2];
      }
    }
    else {
      unaff_SI[8] = 0xffff;
    }
  }
  *(char *)0x9a24 = *(char *)0x9a24 + '\x01';
  return;
}



// Entity/sprite update loop
void update_entities(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_DS;
  int local_4;
  
  iVar3 = 0x48ed;
  iVar4 = *(int *)0x6f8;
  local_4 = *(int *)0x6f6;
  iVar1 = 6;
  do {
    iVar2 = iVar1;
    if (*(int *)(iVar3 + 6) != 0) {
      entity_update_chunk_4f15();
      return;
    }
LAB_1000_4b59:
    for (; local_4 != 0; local_4 = local_4 + -1) {
      if ((*(uint *)(iVar4 + 6) & 0x700) == 0) {
        if (*(int *)(iVar4 + 6) != 1) goto LAB_1000_4b7e;
      }
      else {
        if (*(char *)(iVar4 + 7) != '\x01') {
          *(char *)(iVar4 + 7) = *(char *)(iVar4 + 7) + -1;
        }
LAB_1000_4b7e:
        if ((((*(int *)(iVar4 + 8) < *(int *)0x6c2) ||
             (*(int *)0x6c2 + 0xd0 < *(int *)(iVar4 + 8) + *(int *)*(int *)(iVar4 + 4))) ||
            (*(int *)0x6c4 + 0xa0 <= *(int *)(iVar4 + 10))) ||
           (*(int *)(iVar4 + 10) + ((int *)*(int *)(iVar4 + 4))[1] <= *(int *)0x6c4)) {
          *(undefined1 *)(iVar4 + 6) = 0;
        }
        else if ((*(uint *)(iVar4 + 6) & 0x700) == 0) {
          if (*(char *)(iVar4 + 6) == '\0') {
LAB_1000_4be1:
            if (iVar3 != 0) {
              display_handler_4a70();
              entity_update_chunk_4f15();
              return;
            }
            *(undefined1 *)(iVar4 + 6) = 2;
          }
        }
        else if (((*(char *)(iVar4 + 7) == '\x01') && (*(char *)0x9a23 == '\x01')) &&
                (*(char *)0x9a24 < '\x05')) {
          *(undefined2 *)(iVar4 + 6) = 0x600;
          goto LAB_1000_4be1;
        }
      }
      iVar4 = iVar4 + 0xc;
    }
    if (iVar3 == 0) {
      return;
    }
    iVar3 = iVar3 + 0x20;
    iVar1 = iVar2 + -1;
  } while (iVar2 + -1 != 0);
  iVar3 = 0;
  goto LAB_1000_4b59;
}



void __cdecl16near entity_update_4b46(void)

{
  int iVar1;
  int unaff_BP;
  int unaff_SI;
  int iVar2;
  int unaff_DI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int in_stack_00000000;
  
  do {
    if (unaff_SI == 0) {
      return;
    }
    iVar2 = unaff_SI + 0x20;
    iVar1 = in_stack_00000000 + -1;
    if (iVar1 == 0) {
      iVar2 = 0;
      iVar1 = in_stack_00000000;
    }
    else if (*(int *)(unaff_SI + 0x26) != 0) {
      entity_update_chunk_4f15();
      return;
    }
    while (unaff_SI = iVar2, in_stack_00000000 = iVar1, *(int *)(unaff_BP + -2) != 0) {
      if ((*(uint *)(unaff_DI + 6) & 0x700) == 0) {
        if (*(int *)(unaff_DI + 6) != 1) goto LAB_1000_4b7e;
      }
      else {
        if (*(char *)(unaff_DI + 7) != '\x01') {
          *(char *)(unaff_DI + 7) = *(char *)(unaff_DI + 7) + -1;
        }
LAB_1000_4b7e:
        if ((((*(int *)(unaff_DI + 8) < *(int *)0x6c2) ||
             (*(int *)0x6c2 + 0xd0 < *(int *)(unaff_DI + 8) + *(int *)*(int *)(unaff_DI + 4))) ||
            (*(int *)0x6c4 + 0xa0 <= *(int *)(unaff_DI + 10))) ||
           (*(int *)(unaff_DI + 10) + ((int *)*(int *)(unaff_DI + 4))[1] <= *(int *)0x6c4)) {
          *(undefined1 *)(unaff_DI + 6) = 0;
        }
        else if ((*(uint *)(unaff_DI + 6) & 0x700) == 0) {
          if (*(char *)(unaff_DI + 6) == '\0') {
LAB_1000_4be1:
            if (iVar2 != 0) {
              display_handler_4a70();
              *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + -1;
              entity_update_chunk_4f15();
              return;
            }
            *(undefined1 *)(unaff_DI + 6) = 2;
          }
        }
        else if (((*(char *)(unaff_DI + 7) == '\x01') && (*(char *)0x9a23 == '\x01')) &&
                (*(char *)0x9a24 < '\x05')) {
          *(undefined2 *)(unaff_DI + 6) = 0x600;
          goto LAB_1000_4be1;
        }
      }
      unaff_DI = unaff_DI + 0xc;
      *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + -1;
    }
  } while( true );
}



void __cdecl16near entity_update_4c06(void)

{
  undefined2 *puVar1;
  int unaff_SI;
  
  *(undefined2 *)(unaff_SI + 6) = 0;
  puVar1 = (undefined2 *)*(int *)(unaff_SI + 0x1e);
  *(undefined1 *)(puVar1 + 3) = 2;
  puVar1[4] = *puVar1;
  puVar1[5] = puVar1[1];
  return;
}



// Entity update handler - part of update_entities
// Part of main game loop (sub_35DE) - function chunk
void entity_update_chunk_4c25(void)

{
  undefined2 *puVar1;
  code *pcVar2;
  int *unaff_SI;
  char *pcVar3;
  undefined2 unaff_DS;
  
  while (unaff_SI[3] = 0, *(char *)0x8b1 != '\x06') {
    for (pcVar3 = (char *)0x8b2; *pcVar3 != '\0'; pcVar3 = pcVar3 + 9) {
    }
    *(char *)0x8b1 = *(char *)0x8b1 + '\x01';
    *pcVar3 = '\x02';
    pcVar3[4] = '\0';
    *(int *)(pcVar3 + 5) = (*unaff_SI + -0x10 >> 1) + unaff_SI[8];
    *(int *)(pcVar3 + 7) = unaff_SI[9];
    if (unaff_SI[1] < 0x11) break;
    unaff_SI[1] = unaff_SI[1] + -0x10;
    unaff_SI[9] = unaff_SI[9] + unaff_SI[1];
    if (0x10 < unaff_SI[1]) {
      unaff_SI[1] = 0x10;
    }
  }
  puVar1 = (undefined2 *)unaff_SI[0xf];
  *(undefined1 *)(puVar1 + 3) = 2;
  puVar1[4] = *puVar1;
  puVar1[5] = puVar1[1];
  pcVar2 = (code *)swi(3);
  (*pcVar2)();
  return;
}



// Entity update handler - part of update_entities
// Part of main game loop (sub_35DE) - function chunk
void entity_update_chunk_4cc1(void)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  int *unaff_DI;
  char *pcVar4;
  undefined2 unaff_DS;
  
  if (*(char *)0x8b1 != '\x06') {
    for (pcVar4 = (char *)0x8b2; *pcVar4 != '\0'; pcVar4 = pcVar4 + 9) {
    }
    *(char *)0x8b1 = *(char *)0x8b1 + '\x01';
    *pcVar4 = '\x02';
    pcVar4[4] = '\x02';
    *(int *)(pcVar4 + 5) = *unaff_DI + -4;
    *(int *)(pcVar4 + 7) = unaff_DI[1] + -4;
  }
  *unaff_DI = -1;
  if (*(char *)0x9a22 == '\x01') {
    *(undefined1 *)(*(int *)0x6fa + 0x229) = 0;
    *(undefined1 *)0x9a23 = 0;
    iVar2 = 6;
    iVar3 = 0x48ed;
    do {
      *(undefined2 *)(iVar3 + 6) = 0;
      iVar3 = iVar3 + 0x20;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    load_graphics_resource();
    mode_setup_48d4();
    graphics_update_handler();
    entity_update_chunk_4c25();
    return;
  }
  *(char *)0x9a22 = *(char *)0x9a22 + -1;
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



// Entity update handler - part of update_entities
// Part of main game loop (sub_35DE) - function chunk
void entity_update_chunk_4d41(void)

{
  int iVar1;
  code *pcVar2;
  int *unaff_SI;
  char *pcVar3;
  undefined2 unaff_DS;
  
  while ((char)unaff_SI[2] != '\0') {
    if (*(char *)0x8ec != '\0') goto LAB_1000_4d62;
    if ((*(char *)((int)unaff_SI + 5) == '\x01') || (unaff_SI[3] = 0, *(char *)0x8b1 == '\x06')) {
LAB_1000_4dd2:
      pcVar2 = (code *)swi(3);
      (*pcVar2)();
      return;
    }
    for (pcVar3 = (char *)0x8b2; *pcVar3 != '\0'; pcVar3 = pcVar3 + 9) {
    }
    *(char *)0x8b1 = *(char *)0x8b1 + '\x01';
    *pcVar3 = '\x03';
    pcVar3[4] = '\0';
    *(int *)(pcVar3 + 5) = (*unaff_SI + -0x10 >> 1) + unaff_SI[8];
    *(int *)(pcVar3 + 7) = unaff_SI[9];
    if (unaff_SI[1] < 0x11) goto LAB_1000_4dd2;
    unaff_SI[1] = unaff_SI[1] + -0x10;
    unaff_SI[9] = unaff_SI[9] + unaff_SI[1];
    if (0x10 < unaff_SI[1]) {
      unaff_SI[1] = 0x10;
    }
  }
  if (*(char *)((int)unaff_SI + 5) == -0x80) {
    render_loop_6806();
  }
  else if ((*(char *)0x89d == '\0' && *(char *)0x8a7 == '\0') && *(char *)0x8a3 == '\0') {
    if (*(char *)((int)unaff_SI + 5) == '\x7f') {
      if (*(char *)0x8a6 != '\0' || *(char *)0x897 != '\0') {
        *(int *)0x892 = unaff_SI[10];
        *(int *)0x894 = *(int *)0x894 + -8;
        entity_update_chunk_5090();
        return;
      }
      *(undefined1 *)0x897 = 1;
      *(undefined1 *)0x898 = 0;
      *(int *)0x892 = unaff_SI[10];
      *(undefined2 *)0x894 = 0xfff8;
      entity_update_chunk_5090();
      return;
    }
    if (*(char *)((int)unaff_SI + 5) == '~') {
      *(byte *)0x897 = *(byte *)0x8a6 ^ 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x892 = 0xfff8;
      if (unaff_SI[0xc] != 2) {
        *(undefined2 *)0x892 = 8;
      }
      *(undefined2 *)0x894 = 0xfff4;
      entity_update_chunk_5090();
      return;
    }
    if (*(char *)((int)unaff_SI + 5) == '}') {
      if (6 < unaff_SI[9] - *(int *)0x6c8) {
        *(byte *)0x897 = *(byte *)0x8a6 ^ 1;
        *(undefined1 *)0x898 = 0;
        *(int *)0x894 = unaff_SI[0xb];
        *(undefined2 *)0x892 = 0;
        *(int *)0x6c6 = unaff_SI[8];
      }
      entity_update_chunk_5090();
      return;
    }
    if ((*(char *)((int)unaff_SI + 5) != '|') && (*(char *)((int)unaff_SI + 5) == '{')) {
      *(byte *)0x897 = *(byte *)0x8a6 ^ 1;
      *(undefined1 *)0x898 = 0;
      *(undefined2 *)0x892 = 0xfff8;
      iVar1 = unaff_SI[0xc];
      unaff_SI[0xc] = 1;
      if (iVar1 != 2) {
        unaff_SI[0xc] = 2;
        *(undefined2 *)0x892 = 8;
      }
      *(undefined2 *)0x894 = 0xfffd;
      entity_update_chunk_5090();
      return;
    }
    entity_update_chunk_5090();
    return;
  }
LAB_1000_4d62:
  entity_update_chunk_5090();
  return;
}



// Entity update handler - part of update_entities
// Part of main game loop (sub_35DE) - function chunk
void entity_update_chunk_4f15(void)

{
  int iVar1;
  int iVar2;
  char cVar3;
  uint uVar4;
  int unaff_BP;
  int *unaff_SI;
  int unaff_DI;
  int *piVar5;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int in_stack_00000000;
  
  if (*(char *)0x921 == '\0') {
    (*(code *)*(undefined2 *)((unaff_SI[3] + -1) * 2 + 0x4ae3))();
  }
  if (unaff_SI[3] == 0) {
    *(char *)0x9a24 = *(char *)0x9a24 + -1;
    goto LAB_1000_4b3b;
  }
  if ((((*(int *)0x6c2 <= unaff_SI[8]) && (unaff_SI[8] + *unaff_SI <= *(int *)0x6c2 + 0xd0)) &&
      (unaff_SI[9] < *(int *)0x6c4 + 0xa0)) && (*(int *)0x6c4 < unaff_SI[9] + unaff_SI[1])) {
    if (*(byte *)0x21c != 0) {
      uVar4 = (uint)*(byte *)0x21c;
      piVar5 = (int *)0x8ee;
      do {
        if ((((*piVar5 != -1) &&
             (iVar1 = *piVar5 + 0xc, iVar2 = iVar1 - unaff_SI[8], iVar2 != 0 && unaff_SI[8] <= iVar1
             )) && ((iVar2 + -0x10 < *unaff_SI &&
                    ((iVar1 = (piVar5[1] + 8) - unaff_SI[9],
                     iVar1 != 0 && unaff_SI[9] <= piVar5[1] + 8 && (iVar1 + -8 < unaff_SI[1]))))))
           && ((unaff_SI[2] & 0x8000U) == 0)) {
          cVar3 = (char)((uint)unaff_SI[2] >> 8);
          if (cVar3 == '\x01') {
            entity_update_chunk_4cc1();
            return;
          }
          if ((cVar3 != '|') && (cVar3 != '{')) {
            *piVar5 = -1;
            entity_update_chunk_4c25();
            return;
          }
        }
        piVar5 = piVar5 + 4;
        uVar4 = uVar4 - 1;
      } while (uVar4 != 0);
    }
    if (*(char *)0x89d == '\x01') {
      iVar1 = (*(int *)0x6c6 + 0x1e) - unaff_SI[8];
      if ((((iVar1 != 0 && unaff_SI[8] <= *(int *)0x6c6 + 0x1e) && (iVar1 + -0x1c < *unaff_SI)) &&
          (iVar1 = (*(int *)0x6c8 + 0xf) - unaff_SI[9],
          iVar1 != 0 && unaff_SI[9] <= *(int *)0x6c8 + 0xf)) && (iVar1 + -0xe < unaff_SI[1])) {
        entity_update_chunk_4d41();
        return;
      }
    }
    else {
      iVar1 = (*(int *)0x6c6 + 0xf) - unaff_SI[8];
      if (((iVar1 != 0 && unaff_SI[8] <= *(int *)0x6c6 + 0xf) && (iVar1 + -0xe < *unaff_SI)) &&
         ((iVar1 = (*(int *)0x6c8 + 0x20) - unaff_SI[9],
          iVar1 != 0 && unaff_SI[9] <= *(int *)0x6c8 + 0x20 && (iVar1 + -0x1e < unaff_SI[1])))) {
        entity_update_chunk_4d41();
        return;
      }
    }
    iVar1 = unaff_SI[0xd] + 1;
    if (unaff_SI[10] == 0 && unaff_SI[0xb] == 0) {
      if (unaff_SI[4] <= iVar1) {
        iVar1 = 0;
      }
    }
    else if (unaff_SI[5] <= iVar1) {
      iVar1 = 0;
    }
    unaff_SI[0xd] = iVar1;
    func_7dbb();
    entity_update_4b46();
    return;
  }
  iVar1 = unaff_SI[3];
  unaff_SI[3] = 0;
  iVar2 = unaff_SI[0xf];
  if ((*(uint *)(iVar2 + 6) & 0xff00) == 0) {
    if (((iVar1 != 0x13) && (iVar1 != 0x14)) && (iVar1 != 0x18)) {
      if ((iVar1 == 0x1b) || (iVar1 == 0x1c)) {
        entity_update_4c06();
        *(undefined1 *)(iVar2 + 6) = 1;
        goto LAB_1000_4fc7;
      }
      if (iVar1 != 0x15) {
        *(int *)(iVar2 + 8) = unaff_SI[8];
        *(int *)(iVar2 + 10) = unaff_SI[9];
        goto LAB_1000_4fc2;
      }
    }
    entity_update_4c06();
  }
  else {
LAB_1000_4fc2:
    *(undefined1 *)(iVar2 + 6) = 0;
  }
LAB_1000_4fc7:
  *(char *)0x9a24 = *(char *)0x9a24 + -1;
LAB_1000_4b3b:
  do {
    iVar1 = in_stack_00000000;
    if (unaff_SI[3] != 0) {
      entity_update_chunk_4f15();
      return;
    }
LAB_1000_4b59:
    while (*(int *)(unaff_BP + -2) != 0) {
      if ((*(uint *)(unaff_DI + 6) & 0x700) == 0) {
        if (*(int *)(unaff_DI + 6) != 1) goto LAB_1000_4b7e;
      }
      else {
        if (*(char *)(unaff_DI + 7) != '\x01') {
          *(char *)(unaff_DI + 7) = *(char *)(unaff_DI + 7) + -1;
        }
LAB_1000_4b7e:
        if ((((*(int *)(unaff_DI + 8) < *(int *)0x6c2) ||
             (*(int *)0x6c2 + 0xd0 < *(int *)(unaff_DI + 8) + *(int *)*(int *)(unaff_DI + 4))) ||
            (*(int *)0x6c4 + 0xa0 <= *(int *)(unaff_DI + 10))) ||
           (*(int *)(unaff_DI + 10) + ((int *)*(int *)(unaff_DI + 4))[1] <= *(int *)0x6c4)) {
          *(undefined1 *)(unaff_DI + 6) = 0;
        }
        else if ((*(uint *)(unaff_DI + 6) & 0x700) == 0) {
          if (*(char *)(unaff_DI + 6) == '\0') {
LAB_1000_4be1:
            if (unaff_SI != (int *)0x0) {
              display_handler_4a70();
              *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + -1;
              entity_update_chunk_4f15();
              return;
            }
            *(undefined1 *)(unaff_DI + 6) = 2;
          }
        }
        else if (((*(char *)(unaff_DI + 7) == '\x01') && (*(char *)0x9a23 == '\x01')) &&
                (*(char *)0x9a24 < '\x05')) {
          *(undefined2 *)(unaff_DI + 6) = 0x600;
          goto LAB_1000_4be1;
        }
      }
      unaff_DI = unaff_DI + 0xc;
      *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + -1;
    }
    if (unaff_SI == (int *)0x0) {
      return;
    }
    unaff_SI = unaff_SI + 0x10;
    in_stack_00000000 = iVar1 + -1;
  } while (iVar1 + -1 != 0);
  unaff_SI = (int *)0x0;
  goto LAB_1000_4b59;
}



// Entity update handler - part of update_entities
// Part of main game loop (sub_35DE) - function chunk
void entity_update_chunk_5090(void)

{
  int iVar1;
  int unaff_SI;
  
  iVar1 = *(int *)(unaff_SI + 0x1a) + 1;
  if (*(int *)(unaff_SI + 0x14) == 0 && *(int *)(unaff_SI + 0x16) == 0) {
    if (*(int *)(unaff_SI + 8) <= iVar1) {
      iVar1 = 0;
    }
  }
  else if (*(int *)(unaff_SI + 10) <= iVar1) {
    iVar1 = 0;
  }
  *(int *)(unaff_SI + 0x1a) = iVar1;
  func_7dbb();
  entity_update_4b46();
  return;
}



void __cdecl16near load_resource_5ceb(void)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined2 unaff_DS;
  
  uVar3 = (uint)*(byte *)0x21c;
  piVar4 = (int *)0x8ee;
  do {
    if (*piVar4 == -1) {
      *piVar4 = *(int *)0x6c6 + 4;
      iVar2 = -0x10;
      if ((*(char *)0x8ad != '\x02') && (iVar2 = 0x10, *(char *)0x89d == '\x01')) {
        *piVar4 = *piVar4 + 0x10;
      }
      piVar4[2] = iVar2;
      iVar2 = *(int *)0x6c8 + 4;
      if (*(char *)0x89d != '\x01') {
        iVar2 = *(int *)0x6c8 + 8;
      }
      piVar4[1] = iVar2;
      piVar4[3] = *(int *)0x9a9c;
      *(uint *)0x9a9c = *(uint *)0x9a9c ^ 4;
      piVar4[3] = piVar4[3] + -0xc;
      if (DAT_1000_127c != -1) {
        piVar4[3] = -0xc;
      }
      pcVar1 = (code *)swi(3);
      (*pcVar1)();
      return;
    }
    piVar4 = piVar4 + 4;
    uVar3 = uVar3 - 1;
  } while (uVar3 != 0);
  return;
}



void __cdecl16near load_resource_5d5f(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  undefined2 unaff_DS;
  
  if (*(byte *)0x21c == 0) {
    return;
  }
  uVar2 = (uint)*(byte *)0x21c;
  piVar4 = (int *)0x8ee;
  do {
    if (*piVar4 != -1) {
      *piVar4 = *piVar4 + piVar4[2];
      if (DAT_1000_1018 == -1) {
        iVar1 = piVar4[3];
        if (iVar1 < 0) {
          piVar4[3] = piVar4[3] + 0xc;
        }
        else {
          piVar4[1] = piVar4[1] + *(int *)(iVar1 * 2 + -0x6574);
          iVar1 = iVar1 + 1;
          if (iVar1 == 8) {
            iVar1 = 0;
          }
          piVar4[3] = iVar1;
        }
      }
      iVar1 = check_tile_collision();
      if (*(int *)0x292 < iVar1) {
        if (((*(int *)0x6c2 <= *piVar4) && (*piVar4 - *(int *)0x6c2 < 0xc9)) &&
           ((*(int *)0x6c4 <= piVar4[1] && (piVar4[1] - *(int *)0x6c4 < 0x99)))) {
          ega_palette_77a3();
          unaff_DS = DAT_1000_005c;
          goto LAB_1000_5df3;
        }
      }
      else {
        iVar1 = *piVar4 - *(int *)0x6c2;
        if ((((*(int *)0x6c2 <= *piVar4) && (iVar1 < 0xc9)) &&
            (iVar3 = piVar4[1] - *(int *)0x6c4, *(int *)0x6c4 <= piVar4[1])) && (iVar3 < 0x99)) {
          collision_check_2d06(piVar4,iVar3,iVar1);
          ega_palette_77a3();
          unaff_DS = DAT_1000_005c;
        }
      }
      *piVar4 = -1;
    }
LAB_1000_5df3:
    piVar4 = piVar4 + 4;
    uVar2 = uVar2 - 1;
    if (uVar2 == 0) {
      return;
    }
  } while( true );
}



void run_game(void)

{
  code *pcVar1;
  int iVar2;
  undefined1 in_CL;
  undefined1 extraout_DH;
  undefined2 unaff_DS;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *(undefined2 *)0xaba9 = CONCAT11(extraout_DH,in_CL);
  load_resource_6545();
  load_resource_633c();
  *(undefined2 *)0x9c8c = 0xffff;
  load_resource_62a4();
  iVar2 = func_6df4();
  if (iVar2 != 0) {
    load_resource_631a();
  }
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



void __cdecl16near load_resource_61c8(void)

{
  code *pcVar1;
  char cVar2;
  int iVar3;
  undefined2 unaff_DS;
  int iVar4;
  undefined1 *puVar5;
  undefined2 uVar6;
  
  uRam0000041c = uRam0000041a;
  DAT_1000_0060 = 1;
  uVar6 = 0x48;
  iVar3 = *(int *)0x9d3d * 0x10 + 0x40;
  puVar5 = (undefined1 *)*(undefined2 *)(*(int *)0x9d3d * 2 + *(int *)0x9d3b);
  iVar4 = 0;
  while( true ) {
    do {
      puVar5[iVar4] = 0;
      ega_blit_7ac8(iVar4,puVar5,iVar3,uVar6);
      func_4339();
      func_42d4();
      func_42e8();
      toggle_graphics_page();
      pcVar1 = (code *)swi(0x16);
      cVar2 = (*pcVar1)();
    } while (cVar2 == '\0');
    if (cVar2 == '\x1b') break;
    if (cVar2 == '\b') {
      if (iVar4 != 0) {
        iVar4 = iVar4 + -1;
      }
    }
    else {
      if (cVar2 == '\r') {
        DAT_1000_0060 = 0;
        return;
      }
      if ('`' < cVar2) {
        cVar2 = cVar2 + -0x20;
      }
      if (((cVar2 == ' ') || (('/' < cVar2 && (cVar2 < '[')))) && (iVar4 != 0xf)) {
        puVar5[iVar4] = cVar2;
        iVar4 = iVar4 + 1;
      }
    }
  }
  *puVar5 = 0;
  DAT_1000_0060 = 0;
  return;
}



void __cdecl16near load_resource_62a4(void)

{
  code *pcVar1;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    *(undefined2 *)0x9c94 = 0;
    *(undefined1 *)0x9c96 = 0;
    *(undefined1 *)0x9ca6 = 0;
    *(undefined1 *)0x9cb6 = 0;
    *(undefined1 *)0x9cc6 = 0;
    *(undefined1 *)0x9cd6 = 0;
    *(undefined2 *)0x9ce8 = 0;
    *(undefined2 *)0x9cea = 0;
    *(undefined2 *)0x9cec = 0;
    *(undefined2 *)0x9cf8 = 0;
    *(undefined2 *)0x9cee = 0;
    *(undefined2 *)0x9cf0 = 0;
    *(undefined2 *)0x9cf2 = 0;
    *(undefined2 *)0x9cf4 = 0;
    *(undefined2 *)0x9cf6 = 0;
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



void __cdecl16near load_resource_631a(void)

{
  code *pcVar1;
  bool bVar2;
  
  bVar2 = false;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (bVar2) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



void __cdecl16near load_resource_633c(void)

{
  code *pcVar1;
  bool bVar2;
  
  bVar2 = false;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (bVar2) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



void __cdecl16near load_resource_6545(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  uint uVar5;
  undefined2 unaff_DS;
  
  *(undefined2 *)0x9ce6 = *(undefined2 *)0xaba9;
  piVar3 = (int *)0x0;
  iVar2 = 0;
  iVar1 = 0x40;
  do {
    iVar2 = iVar2 + *piVar3;
    piVar3 = piVar3 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  *(int *)0xabab = iVar2;
  piVar3 = (int *)0xf000;
  iVar2 = 0;
  iVar1 = 0x40;
  do {
    iVar2 = iVar2 + *piVar3;
    piVar3 = piVar3 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  *(int *)0xabad = iVar2;
  piVar3 = (int *)0xff80;
  iVar2 = 0;
  iVar1 = 0x40;
  do {
    iVar2 = iVar2 + *piVar3;
    piVar3 = piVar3 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  *(int *)0xabaf = iVar2;
  uVar4 = *(int *)0xaba9 + *(int *)0xabab + *(int *)0xabad + *(int *)0xabaf;
  uVar4 = uVar4 + (uVar4 >> 5);
  uVar5 = uVar4 & 0x7ff;
  *(uint *)0xabb1 = uVar4 & 0xf;
  *(uint *)0xabb3 = uVar5 >> 4;
  iVar1 = uVar5 + 0x1234;
  uVar4 = iVar1 * 0x1b;
  if ((uVar4 & 0x40) != 0) {
    uVar4 = iVar1 * -0x1b;
  }
  *(uint *)0xabbb = uVar4;
  return;
}



void __cdecl16near title_screen(void)

{
  menu_loop_660b();
  return;
}



void __cdecl16near menu_loop_660b(void)

{
  code *pcVar1;
  undefined2 unaff_SI;
  undefined2 unaff_DI;
  undefined1 in_CF;
  
  do {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    if (!(bool)in_CF) {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      return;
    }
    render_tiles(unaff_DI,unaff_SI);
    toggle_graphics_page();
    wait_for_input_signal();
    in_CF = DAT_1000_0061 == '\0';
  } while (DAT_1000_0061 != '\x01');
  exit_game();
  return;
}



// Main menu or level selection handler
void menu_handler(void)

{
  code *pcVar1;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  ega_palette_790c();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  DAT_1000_0278 = 0x6634;
  DAT_1000_6636 = 0;
  DAT_1000_6638 = 0;
  DAT_1000_663e = 0;
  DAT_1000_6640 = 0;
  DAT_1000_6646 = 0;
  DAT_1000_6648 = 0;
LAB_1000_66e0:
  do {
    render_loop_677a();
    do {
      while (wait_for_input_signal(), DAT_1000_025a != '\x01') {
        if (DAT_1000_025b == '\x01') {
          if (*(int *)0x21a < 3) {
            *(int *)0x21a = *(int *)0x21a + 3;
            goto LAB_1000_66e0;
          }
        }
        else if (DAT_1000_0258 == '\x01') {
          if ((*(int *)0x21a != 0) && (*(int *)0x21a != 3)) {
            *(int *)0x21a = *(int *)0x21a + -1;
            goto LAB_1000_66e0;
          }
        }
        else if (DAT_1000_0259 == '\x01') {
          if ((*(int *)0x21a != 2) && (*(int *)0x21a != 5)) {
            *(int *)0x21a = *(int *)0x21a + 1;
            goto LAB_1000_66e0;
          }
        }
        else if (((DAT_1000_025c != '\0' || DAT_1000_025d != '\0') || (DAT_1000_0061 == '\x01')) ||
                (DAT_1000_0061 == '\x1c')) {
          DAT_1000_0061 = 0;
          reset_level_data();
          return;
        }
      }
    } while (*(int *)0x21a < 3);
    *(int *)0x21a = *(int *)0x21a + -3;
  } while( true );
}



void __cdecl16near render_loop_677a(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 6;
  iVar2 = -0x6170;
  do {
    ega_palette_77a3(iVar1,iVar2);
    iVar2 = iVar2 + 4;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  ega_blit_7ac8();
  ega_palette_77a3();
  func_4339();
  func_42d4();
  ega_palette_7927();
  toggle_graphics_page();
  return;
}



void __cdecl16near render_loop_6806(void)

{
  code *pcVar1;
  int iVar2;
  int unaff_SI;
  undefined2 unaff_DS;
  
  if ((*(char *)0x897 != '\0' || *(char *)0x921 != '\0') || *(char *)0x8a6 != '\0') {
    return;
  }
  iVar2 = *(int *)(unaff_SI + 0x1e);
  if (iVar2 == -0x768a) {
    if (*(char *)0x220 == '\0') {
      if (*(int *)(unaff_SI + 0x1c) == 1) {
        *(undefined1 *)0x220 = 1;
        iVar2 = 9;
      }
      else {
        if (1 < *(int *)(unaff_SI + 0x1c)) {
          return;
        }
        *(undefined2 *)(unaff_SI + 0x1c) = 1;
        iVar2 = 0;
      }
    }
    else {
      if (*(int *)(unaff_SI + 0x1c) != 0) {
        return;
      }
      *(undefined2 *)(unaff_SI + 0x1c) = 1;
      if ('\x01' < *(char *)0x220) {
        return;
      }
      iVar2 = 1;
    }
  }
  else if (((iVar2 == -0x7542) || (iVar2 == -0x7536)) || (iVar2 == -0x752a)) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    if (*(char *)0x220 < '\x02') {
      iVar2 = 2;
    }
    else if (*(char *)0x21d == '\0') {
      graphics_handler_4870();
      iVar2 = 6;
    }
    else {
      iVar2 = 7;
    }
  }
  else if (iVar2 == -0x73b0) {
    if (*(char *)0x220 < '\x02') {
      if (*(int *)(unaff_SI + 0x1c) == 0) {
        *(undefined2 *)(unaff_SI + 0x1c) = 1;
        iVar2 = 3;
      }
      else if (*(int *)(unaff_SI + 0x1c) == 1) {
        *(undefined2 *)(unaff_SI + 0x1c) = 2;
        iVar2 = 4;
      }
      else {
        *(undefined1 *)0x220 = 2;
        game_loop_chunk_3610();
        iVar2 = 5;
      }
    }
    else {
      if (*(int *)(unaff_SI + 0x1c) != 0) {
        return;
      }
      *(undefined2 *)(unaff_SI + 0x1c) = 1;
      game_loop_chunk_3610();
      iVar2 = 8;
    }
  }
  else if (iVar2 == -0x6de5) {
    if (*(char *)0x220 < '\x03') {
      if (*(int *)(unaff_SI + 0x1c) == 0) {
        *(undefined2 *)(unaff_SI + 0x1c) = 1;
        iVar2 = 0;
      }
      else if (*(int *)(unaff_SI + 0x1c) == 1) {
        *(undefined2 *)(unaff_SI + 0x1c) = 2;
        iVar2 = 1;
      }
      else if (*(int *)(unaff_SI + 0x1c) == 2) {
        *(undefined2 *)(unaff_SI + 0x1c) = 3;
        iVar2 = 2;
      }
      else if (*(int *)(unaff_SI + 0x1c) == 3) {
        *(undefined2 *)(unaff_SI + 0x1c) = 4;
        iVar2 = 3;
      }
      else {
        *(undefined1 *)0x220 = 3;
        DAT_1000_7ed5 = 4;
        DAT_1000_8067 = 4;
        DAT_1000_7ef9 = 4;
        DAT_1000_7efb = 1;
        iVar2 = 4;
      }
    }
    else {
      if (*(int *)(unaff_SI + 0x1c) != 0) {
        return;
      }
      *(undefined2 *)(unaff_SI + 0x1c) = 1;
      iVar2 = 5;
    }
  }
  else if (iVar2 == -0x6cf6) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 0;
  }
  else if (iVar2 == -0x67f3) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    if (*(char *)0x220 < '\x04') {
      *(undefined1 *)0x220 = 4;
      iVar2 = 0;
    }
    else if (*(char *)0x220 < '\x05') {
      iVar2 = 1;
    }
    else {
      iVar2 = 2;
    }
  }
  else if (iVar2 == -0x64be) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 0;
  }
  else if (iVar2 == -0x6480) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 1;
  }
  else if (iVar2 == -0x6468) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 2;
  }
  else if (iVar2 == -0x6474) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 3;
  }
  else if (iVar2 == -0x645a) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 4;
  }
  else if (iVar2 == -0x4c81) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 5;
  }
  else if (iVar2 == -0x4c73) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 6;
  }
  else if (iVar2 == -0x4c67) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 7;
  }
  else if (iVar2 == -0x4c5b) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 8;
  }
  else if (iVar2 == -0x4672) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 9;
  }
  else if (iVar2 == -0x38a4) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 0;
  }
  else if (iVar2 == -0x3716) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 1;
  }
  else if (iVar2 == -0x355a) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 2;
  }
  else if (iVar2 == -0x2dd9) {
    if (1 < *(int *)(unaff_SI + 0x1c)) {
      return;
    }
    if (*(int *)(unaff_SI + 0x1c) < 1) {
      *(undefined2 *)(unaff_SI + 0x1c) = 1;
      iVar2 = 3;
    }
    else {
      *(undefined2 *)(unaff_SI + 0x1c) = 2;
      iVar2 = 4;
    }
  }
  else if (iVar2 == -0x2d23) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 5;
  }
  else if (iVar2 == -0x2d17) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 6;
  }
  else if (iVar2 == -0x2c6d) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 7;
  }
  else if (iVar2 == -0x2c5f) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 8;
  }
  else if (iVar2 == -0x2c53) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 9;
  }
  else if (iVar2 == -0x2a63) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 10;
  }
  else if (iVar2 == -0x2a3d) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 0xb;
  }
  else if (iVar2 == -0x7af9) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 1;
  }
  else if (iVar2 == -0x7aed) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 2;
  }
  else if (iVar2 == -0x7ae1) {
    if (*(int *)(unaff_SI + 0x1c) != 0) {
      return;
    }
    *(undefined2 *)(unaff_SI + 0x1c) = 1;
    iVar2 = 3;
  }
  else if (iVar2 == -0x7ac9) {
    if (1 < *(int *)(unaff_SI + 0x1c)) {
      return;
    }
    if (*(int *)(unaff_SI + 0x1c) < 1) {
      *(undefined2 *)(unaff_SI + 0x1c) = 1;
      iVar2 = 4;
    }
    else {
      *(undefined2 *)(unaff_SI + 0x1c) = 2;
      iVar2 = 5;
    }
  }
  else {
    if (iVar2 != -0x6ce6) {
      return;
    }
    if (1 < *(int *)(unaff_SI + 0x1c)) {
      return;
    }
    if (*(int *)(unaff_SI + 0x1c) < 1) {
      *(undefined2 *)(unaff_SI + 0x1c) = 1;
      iVar2 = 6;
    }
    else {
      *(undefined2 *)(unaff_SI + 0x1c) = 2;
      iVar2 = 7;
    }
  }
  *(int *)0x922 = *(int *)(iVar2 * 2 + -0x6140) + -0x6140;
  *(undefined1 *)0x921 = 1;
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}



void __cdecl16near logic_handler_6da4(void)

{
  bool bVar1;
  uint in_AX;
  char cVar2;
  char cVar3;
  int in_DX;
  undefined2 *in_BX;
  undefined2 unaff_DS;
  
  while( true ) {
    cVar3 = (char)((uint)*in_BX >> 8);
    cVar2 = (char)*in_BX + (char)in_AX;
    if ('\t' < cVar2) {
      cVar2 = cVar2 + -10;
      cVar3 = cVar3 + '\x01';
    }
    cVar3 = cVar3 + (char)(in_AX >> 8);
    bVar1 = '\t' < cVar3;
    if (bVar1) {
      cVar3 = cVar3 + -10;
    }
    in_AX = (uint)bVar1;
    *in_BX = CONCAT11(cVar3,cVar2);
    movement_handler_3a5f(in_DX);
    if (in_BX == (undefined2 *)0x934) break;
    in_BX = in_BX + -1;
    in_DX = in_DX + -0x10;
    if (in_AX == 0) {
      return;
    }
  }
  return;
}



// Graphics/rendering update handler
void graphics_update_handler(void)

{
  logic_handler_6da4();
  return;
}



long __cdecl16near func_6df4(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  char cVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint in_DX;
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  int extraout_DX_02;
  uint uVar11;
  char *pcVar12;
  undefined2 unaff_DS;
  long lVar13;
  
  if (((*(int *)0xabab == *(int *)0x9ce8) && (*(int *)0xabad == *(int *)0x9cea)) &&
     (*(int *)0xabaf == *(int *)0x9cec)) {
    return (ulong)in_DX << 0x10;
  }
  do {
    logic_handler_6f47();
    toggle_graphics_page();
    logic_handler_6f47();
    logic_handler_7057();
    uVar1 = *(undefined2 *)0xabb1;
    uVar4 = *(undefined2 *)0xabb3;
    toggle_graphics_page();
    uVar6 = *(uint *)0xabb1;
    uVar9 = *(uint *)0xabb1 & 0xf;
    uVar11 = 0;
    do {
      uVar11 = (uVar11 | uVar9) + uVar6;
      uVar6 = uVar6 ^ 0x25;
      uVar9 = uVar9 - 1;
    } while (uVar9 != 0);
    pcVar12 = (char *)0xabb5;
    iVar7 = 0;
    iVar10 = 0;
    while( true ) {
      cVar3 = *pcVar12;
      if (cVar3 == '\0') break;
      uVar11 = (uint)(byte)(cVar3 - 0x30);
      iVar10 = iVar7 * 2;
      iVar7 = iVar7 * 10 + uVar11;
      pcVar12 = pcVar12 + 1;
    }
    *(int *)0x9cf8 = iVar7;
    *(undefined2 *)0xabb1 = CONCAT11((char)(uVar11 >> 8),cVar3);
    *(int *)0xabb3 = iVar10;
    toggle_graphics_page();
    uVar6 = *(uint *)0xabb1 & 0xf;
    do {
      uVar6 = uVar6 - 1;
    } while (uVar6 != 0);
    *(undefined2 *)0xabb1 = 0x25;
    uVar6 = *(uint *)0xabb1;
    uVar9 = *(uint *)0xabb3;
    uVar11 = *(uint *)0xabbb;
    uVar8 = uVar6 ^ *(uint *)0x9cf8 ^ uVar9;
    *(uint *)0xabb1 = uVar9;
    *(uint *)0xabb3 = uVar9 ^ uVar8;
    toggle_graphics_page();
    *(int *)0xabb3 = (uVar9 + (uVar6 ^ uVar11 ^ uVar9 ^ uVar8)) - *(int *)0xabb1;
    uVar6 = *(uint *)0xaba9;
    uVar2 = *(undefined2 *)0xabab;
    uVar9 = *(uint *)0xabad;
    uVar5 = *(undefined2 *)0xabaf;
    *(uint *)0x9ce6 = uVar6;
    *(undefined2 *)0x9ce8 = uVar2;
    *(uint *)0x9cea = uVar9;
    *(undefined2 *)0x9cec = uVar5;
    *(int *)0xabb1 = (uVar9 ^ uVar6) + *(int *)0xabb3;
    toggle_graphics_page();
    uVar6 = *(uint *)0xabb1 & 0xf;
    do {
      uVar6 = uVar6 - 1;
    } while (uVar6 != 0);
    iVar10 = *(int *)0xabb3;
    *(undefined2 *)0xabb1 = uVar1;
    *(undefined2 *)0xabb3 = uVar4;
  } while (iVar10 != 0);
  if (*(int *)0xabab != *(int *)0x9ce8) {
    ega_blit_7ac8();
    func_4339();
    func_42e8();
    do {
      func_42e8();
    } while (extraout_DX != 1);
    func_42e8();
    func_42e8();
    do {
      func_42e8();
    } while (extraout_DX_00 != 1);
    func_42e8();
    do {
      func_42e8();
      func_42e8();
    } while (extraout_DX_01 != 1);
    func_4339();
    func_42d4();
    func_42d4();
    func_42d4();
    func_42d4();
    func_42d4();
    func_42d4();
    func_42d4();
    func_42e8();
    func_42e8();
    for (iVar10 = *(int *)0xabb3; 99 < iVar10; iVar10 = iVar10 + -100) {
    }
    func_42e8();
    for (iVar10 = extraout_DX_02; 9 < iVar10; iVar10 = iVar10 + -10) {
    }
    func_42e8();
    func_42e8();
    lVar13 = func_42e8();
    return lVar13;
  }
  return 0x250001;
}



void __cdecl16near logic_handler_6f47(void)

{
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  int iVar1;
  int extraout_DX_02;
  undefined2 unaff_DS;
  
  ega_blit_7ac8();
  func_4339();
  func_42e8();
  do {
    func_42e8();
  } while (extraout_DX != 1);
  func_42e8();
  func_42e8();
  do {
    func_42e8();
  } while (extraout_DX_00 != 1);
  func_42e8();
  do {
    func_42e8();
    func_42e8();
  } while (extraout_DX_01 != 1);
  func_4339();
  func_42d4();
  func_42d4();
  func_42d4();
  func_42d4();
  func_42d4();
  func_42d4();
  func_42d4();
  func_42e8();
  func_42e8();
  for (iVar1 = *(int *)0xabb3; 99 < iVar1; iVar1 = iVar1 + -100) {
  }
  func_42e8();
  for (iVar1 = extraout_DX_02; 9 < iVar1; iVar1 = iVar1 + -10) {
  }
  func_42e8();
  func_42e8();
  func_42e8();
  return;
}



void __cdecl16near logic_handler_7057(void)

{
  char cVar1;
  undefined2 unaff_DS;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  uVar5 = 0xd8;
  uVar4 = 0x58;
  iVar3 = -0x544b;
  iVar2 = 0;
LAB_1000_7072:
  do {
    *(undefined1 *)(iVar3 + iVar2) = 0;
    ega_blit_7ac8(iVar2,iVar3,uVar4,uVar5);
    func_4339();
    func_42d4();
    func_42e8();
    toggle_graphics_page();
    do {
      wait_for_input_signal();
      if (DAT_1000_0061 == '\x01') {
        exit_game();
        return;
      }
      if (DAT_1000_0061 == '\x0e') {
        if (iVar2 != 0) {
          iVar2 = iVar2 + -1;
        }
        goto LAB_1000_7072;
      }
      if (DAT_1000_0061 == '\x1c') {
        ega_blit_7ac8();
        return;
      }
      if (DAT_1000_0061 == '\v') {
        cVar1 = '0';
        goto LAB_1000_70d0;
      }
    } while ('\v' < DAT_1000_0061);
    cVar1 = DAT_1000_0061 + '/';
LAB_1000_70d0:
    if (iVar2 != 5) {
      *(char *)(iVar3 + iVar2) = cVar1;
      iVar2 = iVar2 + 1;
    }
  } while( true );
}



void play_demo(void)

{
  undefined2 unaff_DS;
  
  *(BADSPACEBASE **)0xabf3 = register0x00000010;
  DAT_1000_0061 = 0;
  load_graphics_block();
  ega_palette_init();
  toggle_graphics_page();
  set_vga_palette();
  func_2022();
  ega_graphics_7278();
  ega_graphics_7255();
  ega_graphics_7261();
  ega_graphics_7278();
  ega_graphics_7255();
  toggle_graphics_page();
  ega_graphics_7278();
  ega_graphics_71fe();
  ega_graphics_71fe();
  ega_graphics_721b();
  ega_graphics_721b();
  ega_graphics_7238();
  ega_graphics_7238();
  game_state_handler_7298();
  return;
}



void __cdecl16near ega_graphics_71fe(void)

{
  int in_AX;
  int in_CX;
  int in_BX;
  undefined2 unaff_SI;
  
  do {
    ega_graphics_7255(in_BX,in_AX,unaff_SI);
    in_AX = in_AX + -4;
    in_BX = in_BX + 3;
    ega_graphics_7261();
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  return;
}



void __cdecl16near ega_graphics_721b(void)

{
  int in_AX;
  int in_CX;
  int in_BX;
  undefined2 unaff_SI;
  
  do {
    ega_graphics_7255(in_BX,in_AX,unaff_SI);
    in_AX = in_AX + -8;
    in_BX = in_BX + 6;
    ega_graphics_7261();
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  return;
}



void __cdecl16near ega_graphics_7238(void)

{
  int in_AX;
  int in_CX;
  int in_BX;
  undefined2 unaff_SI;
  
  do {
    ega_graphics_7255(in_BX,in_AX,unaff_SI);
    in_AX = in_AX + -0xc;
    in_BX = in_BX + 8;
    ega_graphics_7261();
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  return;
}



void __cdecl16near ega_graphics_7255(void)

{
  ega_palette_init();
  return;
}



void __cdecl16near ega_graphics_7261(void)

{
  ega_palette_77a3();
  toggle_graphics_page();
  ega_graphics_7278();
  return;
}



void __cdecl16near ega_graphics_7278(void)

{
  code *pcVar1;
  int in_AX;
  
  do {
    wait_for_timer();
    if (DAT_1000_0061 != '\0') {
      clear_sound_flags();
      pcVar1 = (code *)swi(3);
      (*pcVar1)();
      return;
    }
    in_AX = in_AX + -1;
  } while (in_AX != 0);
  return;
}



// Game state handler - part of main loop
// Part of main game loop (sub_35DE) - function chunk
void game_state_handler_7298(void)

{
  int *piVar1;
  code *pcVar2;
  int iVar3;
  undefined2 unaff_DS;
  
  pcVar2 = (code *)swi(0x10);
  (*pcVar2)();
  set_graphics_page();
  DAT_1000_0278 = 0x7106;
  DAT_1000_7108 = 0;
  DAT_1000_710a = 0;
  DAT_1000_7110 = 0;
  DAT_1000_7112 = 0;
  DAT_1000_7118 = 0;
  DAT_1000_711a = 0;
  set_sound_flag();
  set_vga_palette();
  ega_palette_7990();
  ega_palette_7990();
  ega_palette_init();
  toggle_graphics_page();
  func_2022();
  ega_graphics_7278();
  iVar3 = 8;
  do {
    ega_graphics_74e1();
    toggle_graphics_page();
    ega_graphics_7278();
    iVar3 = iVar3 + 8;
  } while (iVar3 < 0x148);
  DAT_1000_0278 = 0x7120;
  DAT_1000_7122 = 0;
  DAT_1000_7124 = 0;
  DAT_1000_712a = 0;
  DAT_1000_712c = 0;
  DAT_1000_7132 = 0;
  DAT_1000_7134 = 0;
  set_sound_flag();
  set_vga_palette();
  *(undefined2 *)0x6c6 = 0;
  *(undefined2 *)0x6c8 = 0x81;
  *(undefined1 *)0x8ad = 1;
  *(undefined2 *)0xabf5 = 0x10;
  do {
    ega_palette_init();
    ega_graphics_7622();
    toggle_graphics_page();
    ega_graphics_7278();
    *(int *)0x6c6 = *(int *)0x6c6 + 8;
    piVar1 = (int *)0xabf5;
    *piVar1 = *piVar1 + -1;
  } while (*piVar1 != 0);
  ega_graphics_75ad();
  *(undefined1 *)0x8ad = 2;
  *(undefined2 *)0xabf5 = 10;
  do {
    ega_palette_init();
    ega_graphics_7622();
    toggle_graphics_page();
    ega_graphics_7278();
    *(int *)0x6c6 = *(int *)0x6c6 + -8;
    piVar1 = (int *)0xabf5;
    *piVar1 = *piVar1 + -1;
  } while (*piVar1 != 0);
  ega_graphics_75ad();
  *(undefined1 *)0x8ad = 1;
  *(undefined2 *)0xabf5 = 2;
  do {
    ega_palette_init();
    ega_graphics_7622();
    toggle_graphics_page();
    ega_graphics_7278();
    *(int *)0x6c6 = *(int *)0x6c6 + 8;
    piVar1 = (int *)0xabf5;
    *piVar1 = *piVar1 + -1;
  } while (*piVar1 != 0);
  ega_graphics_75ad();
  pcVar2 = (code *)swi(3);
  (*pcVar2)();
  return;
}



void __cdecl16near ega_graphics_74e1(void)

{
  uint in_AX;
  
  if (in_AX >> 3 != 0x28) {
    ega_graphics_752a(in_AX >> 3);
  }
  ega_graphics_752a();
  return;
}



void __cdecl16near ega_graphics_752a(void)

{
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  undefined2 extraout_DX_01;
  
  func_7a68();
  ega_graphics_7553();
  func_7a68(extraout_DX);
  ega_graphics_7553();
  func_7a68(extraout_DX_00);
  ega_graphics_7553();
  func_7a68(extraout_DX_01);
  ega_graphics_7553();
  return;
}



void __cdecl16near ega_graphics_7553(void)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  int in_DX;
  undefined1 *unaff_SI;
  undefined1 *puVar5;
  undefined1 *unaff_DI;
  undefined1 *puVar6;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  iVar3 = 0xa0;
  iVar4 = in_DX;
  puVar6 = unaff_DI;
  puVar5 = unaff_SI;
  do {
    for (; iVar4 != 0; iVar4 = iVar4 + -1) {
      puVar2 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      puVar1 = unaff_SI;
      unaff_SI = unaff_SI + 1;
      *puVar2 = *puVar1;
    }
    unaff_SI = puVar5 + 0x28;
    unaff_DI = puVar6 + 0x28;
    iVar3 = iVar3 + -1;
    iVar4 = in_DX;
    puVar6 = unaff_DI;
    puVar5 = unaff_SI;
  } while (iVar3 != 0);
  return;
}



void __cdecl16near ega_graphics_75ad(void)

{
  int *piVar1;
  code *pcVar2;
  undefined2 uVar3;
  int unaff_SI;
  undefined2 unaff_DS;
  
  *(undefined2 *)0xabf5 = 10;
  while( true ) {
    ega_palette_init();
    ega_palette_77a3();
    if (unaff_SI != 0) {
      ega_palette_7927();
    }
    uVar3 = DAT_1000_005c;
    toggle_graphics_page();
    ega_graphics_7278();
    if (*(int *)0xabf5 == 9) break;
    piVar1 = (int *)0xabf5;
    *piVar1 = *piVar1 + -1;
    if (*piVar1 == 0) {
      return;
    }
  }
  pcVar2 = (code *)swi(3);
  (*pcVar2)();
  return;
}



void __cdecl16near ega_graphics_7622(void)

{
  char cVar1;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)0x8af + '\x01';
  if ('\x03' < cVar1) {
    cVar1 = '\x01';
  }
  *(char *)0x8af = cVar1;
  ega_palette_77a3();
  return;
}



void __cdecl16near set_graphics_page(void)

{
  ega_blit_7a89();
  DAT_1000_773f = 0x2000;
  return;
}



void __cdecl16near toggle_graphics_page(void)

{
  DAT_1000_022a = DAT_1000_0229;
  ega_blit_7a89();
  DAT_1000_773f = DAT_1000_773f ^ 0x2000;
  return;
}



void __cdecl16near ega_palette_init(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined2 *unaff_SI;
  undefined2 *puVar4;
  undefined2 *unaff_DI;
  undefined2 *puVar5;
  
  func_7a68();
  puVar4 = unaff_SI;
  puVar5 = unaff_DI;
  for (iVar3 = 4000; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  func_7a68();
  puVar4 = unaff_SI;
  puVar5 = unaff_DI;
  for (iVar3 = 4000; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  func_7a68();
  puVar4 = unaff_SI;
  puVar5 = unaff_DI;
  for (iVar3 = 4000; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  func_7a68();
  for (iVar3 = 4000; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = unaff_DI;
    unaff_DI = unaff_DI + 1;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    *puVar2 = *puVar1;
  }
  return;
}



void __cdecl16near ega_palette_77a3(void)

{
  uint in_AX;
  
  if ((in_AX & 4) != 0) {
    ega_palette_7833();
    return;
  }
  ega_palette_77f2();
  ega_palette_77f2();
  ega_palette_77f2();
  ega_palette_77f2();
  return;
}



void __cdecl16near ega_palette_77f2(void)

{
  uint *puVar1;
  int iVar2;
  int in_DX;
  int in_BX;
  uint *unaff_BP;
  uint *unaff_SI;
  uint *unaff_DI;
  uint *puVar3;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  iVar2 = in_DX;
  puVar3 = unaff_DI;
LAB_1000_7801:
  do {
    if (iVar2 == 1) {
      puVar1 = unaff_BP;
      unaff_BP = (uint *)((int)unaff_BP + 1);
      *(byte *)unaff_DI = (byte)*unaff_DI & (byte)*puVar1;
      puVar1 = unaff_SI;
      unaff_SI = (uint *)((int)unaff_SI + 1);
      *(byte *)unaff_DI = (byte)*unaff_DI | (byte)*puVar1;
    }
    else {
      puVar1 = unaff_BP;
      unaff_BP = unaff_BP + 1;
      *unaff_DI = *unaff_DI & *puVar1;
      puVar1 = unaff_SI;
      unaff_SI = unaff_SI + 1;
      *unaff_DI = *unaff_DI | *puVar1;
      unaff_DI = unaff_DI + 1;
      iVar2 = iVar2 + -2;
      if (iVar2 != 0) goto LAB_1000_7801;
    }
    unaff_DI = puVar3 + 0x14;
    in_BX = in_BX + -1;
    iVar2 = in_DX;
    puVar3 = unaff_DI;
    if (in_BX == 0) {
      return;
    }
  } while( true );
}



void __cdecl16near ega_palette_7833(void)

{
  ega_palette_7849();
  ega_palette_7849();
  ega_palette_7849();
  ega_palette_7849();
  return;
}



void __cdecl16near ega_palette_7849(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int in_DX;
  int in_BX;
  undefined1 *unaff_BP;
  byte *unaff_SI;
  byte *unaff_DI;
  byte *pbVar4;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  do {
    iVar1 = CONCAT11(0xff,*unaff_BP) << 4;
    *unaff_DI = *unaff_DI & (byte)((uint)iVar1 >> 8);
    iVar2 = (uint)*unaff_SI << 4;
    *unaff_DI = *unaff_DI | (byte)((uint)iVar2 >> 8);
    iVar3 = in_DX;
    pbVar4 = unaff_DI;
    while( true ) {
      unaff_SI = unaff_SI + 1;
      pbVar4 = pbVar4 + 1;
      unaff_BP = unaff_BP + 1;
      if (iVar3 == 1) break;
      iVar1 = CONCAT11((char)((uint)(iVar1 << 4) >> 8),*unaff_BP) << 4;
      *pbVar4 = *pbVar4 & (byte)((uint)iVar1 >> 8);
      iVar2 = CONCAT11((char)((uint)(iVar2 << 4) >> 8),*unaff_SI) << 4;
      *pbVar4 = *pbVar4 | (byte)((uint)iVar2 >> 8);
      iVar3 = iVar3 + -1;
    }
    *pbVar4 = *pbVar4 & ((byte)iVar1 | 0xf);
    *pbVar4 = *pbVar4 | (byte)iVar2;
    unaff_DI = unaff_DI + 0x28;
    in_BX = in_BX + -1;
  } while (in_BX != 0);
  return;
}



void __cdecl16near ega_palette_78c6(void)

{
  ega_palette_78e1();
  ega_palette_78e1();
  return;
}



void __cdecl16near ega_palette_78e1(void)

{
  ega_palette_7962();
  ega_palette_7962();
  ega_palette_7962();
  ega_palette_7962();
  return;
}



void __cdecl16near ega_palette_790c(void)

{
  ega_palette_7927();
  ega_palette_7927();
  return;
}



void __cdecl16near ega_palette_7927(void)

{
  ega_palette_7962();
  ega_palette_7962();
  ega_palette_7962();
  ega_palette_7962();
  return;
}



void __cdecl16near ega_palette_7962(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  uint uVar3;
  uint in_DX;
  int in_BX;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 *puVar4;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  do {
    uVar3 = in_DX;
    puVar4 = unaff_DI;
    if ((in_DX & 1) == 0) {
LAB_1000_797b:
      uVar3 = uVar3 >> 1;
      if (uVar3 != 0) {
        for (; uVar3 != 0; uVar3 = uVar3 - 1) {
          puVar2 = puVar4;
          puVar4 = puVar4 + 1;
          puVar1 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *puVar2 = *puVar1;
        }
      }
    }
    else {
      puVar4 = (undefined2 *)((int)unaff_DI + 1);
      puVar1 = unaff_SI;
      unaff_SI = (undefined2 *)((int)unaff_SI + 1);
      *(undefined1 *)unaff_DI = *(undefined1 *)puVar1;
      uVar3 = in_DX - 1;
      if (uVar3 != 0) goto LAB_1000_797b;
    }
    unaff_DI = unaff_DI + 0x14;
    in_BX = in_BX + -1;
    if (in_BX == 0) {
      return;
    }
  } while( true );
}



void __cdecl16near ega_palette_7990(void)

{
  ega_palette_79a2();
  load_graphics_block();
  return;
}



void __cdecl16near ega_palette_setup(void)

{
  ega_palette_79a2();
  ega_blit_plane();
  return;
}



void __cdecl16near ega_palette_79a2(void)

{
  code *pcVar1;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)in_CF) {
    error_handler();
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



void __cdecl16near load_graphics_block(void)

{
  undefined2 *unaff_SI;
  undefined2 unaff_DS;
  
  DAT_1000_798e = *unaff_SI;
  func_7a68();
  ega_palette_79ed();
  func_7a68();
  ega_palette_79ed();
  func_7a68();
  ega_palette_79ed();
  func_7a68();
  ega_palette_79ed();
  return;
}



void __cdecl16near ega_palette_79ed(void)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  byte *pbVar4;
  uint uVar5;
  byte *unaff_SI;
  byte *pbVar6;
  byte *unaff_DI;
  byte *pbVar7;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  pbVar7 = unaff_DI;
  do {
    pbVar6 = unaff_SI + 1;
    bVar2 = *unaff_SI;
    if ((bVar2 & 0x80) == 0) {
      for (uVar5 = (uint)bVar2; uVar5 != 0; uVar5 = uVar5 - 1) {
        pbVar4 = pbVar7;
        pbVar7 = pbVar7 + 1;
        pbVar1 = pbVar6;
        pbVar6 = pbVar6 + 1;
        *pbVar4 = *pbVar1;
      }
    }
    else {
      pbVar1 = pbVar6;
      pbVar6 = unaff_SI + 2;
      bVar3 = *pbVar1;
      for (uVar5 = (uint)(bVar2 & 0x7f); uVar5 != 0; uVar5 = uVar5 - 1) {
        pbVar1 = pbVar7;
        pbVar7 = pbVar7 + 1;
        *pbVar1 = bVar3;
      }
    }
    unaff_SI = pbVar6;
  } while ((int)pbVar7 - (int)unaff_DI < DAT_1000_798e);
  return;
}



void __cdecl16near ega_blit_plane(void)

{
  undefined2 *unaff_SI;
  undefined2 unaff_DS;
  
  DAT_1000_798e = *unaff_SI;
  func_7a68();
  ega_blit_7a39();
  func_7a68();
  ega_blit_7a39();
  func_7a68();
  ega_blit_7a39();
  func_7a68();
  ega_blit_7a39();
  return;
}



void __cdecl16near ega_blit_7a39(void)

{
  byte *pbVar1;
  byte bVar2;
  uint uVar3;
  byte *unaff_SI;
  byte *pbVar4;
  byte *unaff_DI;
  byte *pbVar5;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  pbVar5 = unaff_DI;
  do {
    pbVar4 = unaff_SI + 1;
    bVar2 = *unaff_SI;
    if ((bVar2 & 0x80) == 0) {
      uVar3 = (uint)bVar2;
      do {
        pbVar1 = pbVar4;
        pbVar4 = pbVar4 + 1;
        *pbVar5 = *pbVar5 | *pbVar1;
        pbVar5 = pbVar5 + 1;
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
    }
    else {
      uVar3 = (uint)(bVar2 & 0x7f);
      pbVar1 = pbVar4;
      pbVar4 = unaff_SI + 2;
      bVar2 = *pbVar1;
      do {
        *pbVar5 = *pbVar5 | bVar2;
        pbVar5 = pbVar5 + 1;
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
    }
    unaff_SI = pbVar4;
  } while ((int)pbVar5 - (int)unaff_DI < DAT_1000_798e);
  return;
}



undefined1 __cdecl16near func_7a68(void)

{
  byte in_CL;
  
  out(0x3c4,2);
  out(0x3c5,'\x01' << (in_CL & 0x1f));
  out(0x3ce,4);
  out(0x3cf,in_CL);
  return 4;
}



void __cdecl16near ega_blit_7a89(void)

{
  byte bVar1;
  undefined2 in_AX;
  undefined1 uVar2;
  
  do {
    uVar2 = (undefined1)((uint)in_AX >> 8);
    bVar1 = in(0x3da);
    in_AX = CONCAT11(uVar2,bVar1);
  } while ((bVar1 & 8) != 0);
  out(0x3d4,0xc);
  out(0x3d5,uVar2);
  do {
    bVar1 = in(0x3da);
  } while ((bVar1 & 8) == 0);
  return;
}



void __cdecl16near ega_blit_7aab(void)

{
  ega_blit_7ac8();
  ega_blit_7ac8();
  return;
}



void __cdecl16near ega_blit_7ac8(void)

{
  ega_blit_7b03();
  ega_blit_7b03();
  ega_blit_7b03();
  ega_blit_7b03();
  return;
}



void __cdecl16near ega_blit_7b03(void)

{
  undefined2 *puVar1;
  uint uVar2;
  uint in_DX;
  int in_BX;
  undefined2 *unaff_DI;
  undefined2 *puVar3;
  undefined2 unaff_ES;
  
  func_7a68();
  do {
    uVar2 = in_DX;
    puVar3 = unaff_DI;
    if ((in_DX & 1) == 0) {
LAB_1000_7b1c:
      uVar2 = uVar2 >> 1;
      if (uVar2 != 0) {
        for (; uVar2 != 0; uVar2 = uVar2 - 1) {
          puVar1 = puVar3;
          puVar3 = puVar3 + 1;
          *puVar1 = 0;
        }
      }
    }
    else {
      puVar3 = (undefined2 *)((int)unaff_DI + 1);
      *(undefined1 *)unaff_DI = 0;
      uVar2 = in_DX - 1;
      if (uVar2 != 0) goto LAB_1000_7b1c;
    }
    unaff_DI = unaff_DI + 0x14;
    in_BX = in_BX + -1;
    if (in_BX == 0) {
      return;
    }
  } while( true );
}



void __cdecl16near ega_blit_7b31(void)

{
  uint in_AX;
  int in_CX;
  int unaff_SI;
  undefined2 unaff_DS;
  
  DAT_1000_7b2f = *(int *)(unaff_SI + 2) - in_CX;
  if ((in_AX & 4) != 0) {
    ega_blit_7bd9();
    return;
  }
  ega_blit_7b8b();
  ega_blit_7b8b();
  ega_blit_7b8b();
  ega_blit_7b8b();
  return;
}



void __cdecl16near ega_blit_7b8b(void)

{
  uint *puVar1;
  int iVar2;
  int in_DX;
  int in_BX;
  uint *unaff_BP;
  uint *unaff_SI;
  uint *unaff_DI;
  uint *puVar3;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  do {
    iVar2 = in_DX;
    puVar3 = unaff_DI;
    if (DAT_1000_7b2f < in_BX) {
      unaff_BP = (uint *)((int)unaff_BP + in_DX);
      unaff_SI = (uint *)((int)unaff_SI + in_DX);
    }
    else {
      do {
        if (iVar2 == 1) {
          puVar1 = unaff_BP;
          unaff_BP = (uint *)((int)unaff_BP + 1);
          *(byte *)puVar3 = (byte)*puVar3 & (byte)*puVar1;
          puVar1 = unaff_SI;
          unaff_SI = (uint *)((int)unaff_SI + 1);
          *(byte *)puVar3 = (byte)*puVar3 | (byte)*puVar1;
          break;
        }
        puVar1 = unaff_BP;
        unaff_BP = unaff_BP + 1;
        *puVar3 = *puVar3 & *puVar1;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        *puVar3 = *puVar3 | *puVar1;
        puVar3 = puVar3 + 1;
        iVar2 = iVar2 + -2;
      } while (iVar2 != 0);
    }
    unaff_DI = unaff_DI + 0x14;
    in_BX = in_BX + -1;
    if (in_BX == 0) {
      return;
    }
  } while( true );
}



void __cdecl16near ega_blit_7bd9(void)

{
  ega_blit_7bef();
  ega_blit_7bef();
  ega_blit_7bef();
  ega_blit_7bef();
  return;
}



void __cdecl16near ega_blit_7bef(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int in_DX;
  int in_BX;
  undefined1 *unaff_BP;
  byte *unaff_SI;
  byte *unaff_DI;
  byte *pbVar4;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  do {
    if (DAT_1000_7b2f < in_BX) {
      unaff_BP = unaff_BP + in_DX;
      unaff_SI = unaff_SI + in_DX;
    }
    else {
      iVar1 = CONCAT11(0xff,*unaff_BP) << 4;
      *unaff_DI = *unaff_DI & (byte)((uint)iVar1 >> 8);
      iVar2 = (uint)*unaff_SI << 4;
      *unaff_DI = *unaff_DI | (byte)((uint)iVar2 >> 8);
      iVar3 = in_DX;
      pbVar4 = unaff_DI;
      while( true ) {
        unaff_SI = unaff_SI + 1;
        pbVar4 = pbVar4 + 1;
        unaff_BP = unaff_BP + 1;
        if (iVar3 == 1) break;
        iVar1 = CONCAT11((char)((uint)(iVar1 << 4) >> 8),*unaff_BP) << 4;
        *pbVar4 = *pbVar4 & (byte)((uint)iVar1 >> 8);
        iVar2 = CONCAT11((char)((uint)(iVar2 << 4) >> 8),*unaff_SI) << 4;
        *pbVar4 = *pbVar4 | (byte)((uint)iVar2 >> 8);
        iVar3 = iVar3 + -1;
      }
      *pbVar4 = *pbVar4 & ((byte)iVar1 | 0xf);
      *pbVar4 = *pbVar4 | (byte)iVar2;
    }
    unaff_DI = unaff_DI + 0x28;
    in_BX = in_BX + -1;
  } while (in_BX != 0);
  return;
}



void __cdecl16near ega_blit_7c79(void)

{
  uint in_AX;
  undefined2 in_CX;
  
  DAT_1000_7b2f = in_CX;
  if ((in_AX & 4) != 0) {
    ega_blit_7d1b();
    return;
  }
  ega_blit_7ccd();
  ega_blit_7ccd();
  ega_blit_7ccd();
  ega_blit_7ccd();
  return;
}



void __cdecl16near ega_blit_7ccd(void)

{
  uint *puVar1;
  int iVar2;
  int in_DX;
  int in_BX;
  uint *unaff_BP;
  uint *unaff_SI;
  uint *unaff_DI;
  uint *puVar3;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  do {
    iVar2 = in_DX;
    puVar3 = unaff_DI;
    if (DAT_1000_7b2f < in_BX) {
      do {
        if (iVar2 == 1) {
          puVar1 = unaff_BP;
          unaff_BP = (uint *)((int)unaff_BP + 1);
          *(byte *)puVar3 = (byte)*puVar3 & (byte)*puVar1;
          puVar1 = unaff_SI;
          unaff_SI = (uint *)((int)unaff_SI + 1);
          *(byte *)puVar3 = (byte)*puVar3 | (byte)*puVar1;
          break;
        }
        puVar1 = unaff_BP;
        unaff_BP = unaff_BP + 1;
        *puVar3 = *puVar3 & *puVar1;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        *puVar3 = *puVar3 | *puVar1;
        puVar3 = puVar3 + 1;
        iVar2 = iVar2 + -2;
      } while (iVar2 != 0);
    }
    else {
      unaff_BP = (uint *)((int)unaff_BP + in_DX);
      unaff_SI = (uint *)((int)unaff_SI + in_DX);
    }
    unaff_DI = unaff_DI + 0x14;
    in_BX = in_BX + -1;
    if (in_BX == 0) {
      return;
    }
  } while( true );
}



void __cdecl16near ega_blit_7d1b(void)

{
  ega_blit_7d31();
  ega_blit_7d31();
  ega_blit_7d31();
  ega_blit_7d31();
  return;
}



void __cdecl16near ega_blit_7d31(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int in_DX;
  int in_BX;
  undefined1 *unaff_BP;
  byte *unaff_SI;
  byte *unaff_DI;
  byte *pbVar4;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  func_7a68();
  do {
    if (DAT_1000_7b2f < in_BX) {
      iVar1 = CONCAT11(0xff,*unaff_BP) << 4;
      *unaff_DI = *unaff_DI & (byte)((uint)iVar1 >> 8);
      iVar2 = (uint)*unaff_SI << 4;
      *unaff_DI = *unaff_DI | (byte)((uint)iVar2 >> 8);
      iVar3 = in_DX;
      pbVar4 = unaff_DI;
      while( true ) {
        unaff_SI = unaff_SI + 1;
        pbVar4 = pbVar4 + 1;
        unaff_BP = unaff_BP + 1;
        if (iVar3 == 1) break;
        iVar1 = CONCAT11((char)((uint)(iVar1 << 4) >> 8),*unaff_BP) << 4;
        *pbVar4 = *pbVar4 & (byte)((uint)iVar1 >> 8);
        iVar2 = CONCAT11((char)((uint)(iVar2 << 4) >> 8),*unaff_SI) << 4;
        *pbVar4 = *pbVar4 | (byte)((uint)iVar2 >> 8);
        iVar3 = iVar3 + -1;
      }
      *pbVar4 = *pbVar4 & ((byte)iVar1 | 0xf);
      *pbVar4 = *pbVar4 | (byte)iVar2;
    }
    else {
      unaff_BP = unaff_BP + in_DX;
      unaff_SI = unaff_SI + in_DX;
    }
    unaff_DI = unaff_DI + 0x28;
    in_BX = in_BX + -1;
  } while (in_BX != 0);
  return;
}



undefined2 __cdecl16near func_7dbb(void)

{
  int iVar1;
  int in_AX;
  undefined2 uVar2;
  int in_BX;
  int iVar3;
  int *unaff_SI;
  
  iVar1 = unaff_SI[1];
  if ((in_AX < *(int *)0x6c2) || (0xd0 < (in_AX - *(int *)0x6c2) + *unaff_SI)) {
LAB_1000_7e3f:
    uVar2 = 0;
  }
  else {
    iVar3 = in_BX - *(int *)0x6c4;
    if (in_BX < *(int *)0x6c4) {
      if (iVar1 <= -iVar3) goto LAB_1000_7e3f;
      ega_blit_7b31();
    }
    else {
      iVar3 = iVar3 + iVar1;
      if (iVar3 < 0xa1) {
        ega_palette_77a3();
      }
      else {
        if (iVar1 <= iVar3 + -0xa0) goto LAB_1000_7e3f;
        ega_blit_7c79();
      }
    }
    uVar2 = 1;
  }
  return uVar2;
}


