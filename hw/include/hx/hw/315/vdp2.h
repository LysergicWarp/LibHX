#ifndef HX_HW_315_VDP2_H
#define HX_HW_315_VDP2_H

#include <hx/hw/map.h>

/** @file
 * VDP2's memory and its registers: 142 of 16 bits each, and two reserved words among them. The
 * addresses are the ones past the SH-2's cache.
 *
 * TVMD, EXTEN, VRSIZE and RAMCTL are written and can be read back, VRSIZE's version number read
 * alone. TVSTAT, HCNT and VCNT are read alone. No other is marked as readable.
 *
 * Two reads do something: one of EXTEN latches the counters while its EXLTEN is 0, and one of
 * TVSTAT clears its EXLTFG and EXSYFG.
 *
 * The bits are named for TVMD, TVSTAT, BGON, CHCTLA, CHCTLB, the pattern name control registers
 * and BKTAU alone. */

/* VRAM, 512 kbytes, and the colour RAM, 4 kbytes */
#define VDP2_VRAM 0x25E00000
#define VDP2_VRAM_SIZE 0x80000
#define VDP2_CRAM 0x25F00000
#define VDP2_CRAM_SIZE 0x1000

/* The screen: its mode, the external signals, its status, the VRAM size, the two counters, the
 * RAM control */
#define VDP2_TVMD 0x25F80000
#define VDP2_EXTEN 0x25F80002
#define VDP2_TVSTAT 0x25F80004
#define VDP2_VRSIZE 0x25F80006
#define VDP2_HCNT 0x25F80008
#define VDP2_VCNT 0x25F8000A
#define VDP2_RAMCTL 0x25F8000E

/* The VRAM cycle patterns of banks A0, A1, B0 and B1 */
#define VDP2_CYCA0L 0x25F80010
#define VDP2_CYCA0U 0x25F80012
#define VDP2_CYCA1L 0x25F80014
#define VDP2_CYCA1U 0x25F80016
#define VDP2_CYCB0L 0x25F80018
#define VDP2_CYCB0U 0x25F8001A
#define VDP2_CYCB1L 0x25F8001C
#define VDP2_CYCB1U 0x25F8001E

/* The screens shown, the mosaic, the special function codes, the character control, the bitmap
 * palette numbers */
#define VDP2_BGON 0x25F80020
#define VDP2_MZCTL 0x25F80022
#define VDP2_SFSEL 0x25F80024
#define VDP2_SFCODE 0x25F80026
#define VDP2_CHCTLA 0x25F80028
#define VDP2_CHCTLB 0x25F8002A
#define VDP2_BMPNA 0x25F8002C
#define VDP2_BMPNB 0x25F8002E

/* The pattern name control of NBG0 to NBG3 and of RBG0, the plane sizes, the map offsets */
#define VDP2_PNCN0 0x25F80030
#define VDP2_PNCN1 0x25F80032
#define VDP2_PNCN2 0x25F80034
#define VDP2_PNCN3 0x25F80036
#define VDP2_PNCR 0x25F80038
#define VDP2_PLSZ 0x25F8003A
#define VDP2_MPOFN 0x25F8003C
#define VDP2_MPOFR 0x25F8003E

/* The maps of NBG0 to NBG3, two planes a word */
#define VDP2_MPABN0 0x25F80040
#define VDP2_MPCDN0 0x25F80042
#define VDP2_MPABN1 0x25F80044
#define VDP2_MPCDN1 0x25F80046
#define VDP2_MPABN2 0x25F80048
#define VDP2_MPCDN2 0x25F8004A
#define VDP2_MPABN3 0x25F8004C
#define VDP2_MPCDN3 0x25F8004E

/* The maps of rotation parameters A and B, two planes a word */
#define VDP2_MPABRA 0x25F80050
#define VDP2_MPCDRA 0x25F80052
#define VDP2_MPEFRA 0x25F80054
#define VDP2_MPGHRA 0x25F80056
#define VDP2_MPIJRA 0x25F80058
#define VDP2_MPKLRA 0x25F8005A
#define VDP2_MPMNRA 0x25F8005C
#define VDP2_MPOPRA 0x25F8005E
#define VDP2_MPABRB 0x25F80060
#define VDP2_MPCDRB 0x25F80062
#define VDP2_MPEFRB 0x25F80064
#define VDP2_MPGHRB 0x25F80066
#define VDP2_MPIJRB 0x25F80068
#define VDP2_MPKLRB 0x25F8006A
#define VDP2_MPMNRB 0x25F8006C
#define VDP2_MPOPRB 0x25F8006E

/* The scroll and the coordinate increment of NBG0 and of NBG1: horizontal then vertical, the
 * integer part then the fraction */
#define VDP2_SCXIN0 0x25F80070
#define VDP2_SCXDN0 0x25F80072
#define VDP2_SCYIN0 0x25F80074
#define VDP2_SCYDN0 0x25F80076
#define VDP2_ZMXIN0 0x25F80078
#define VDP2_ZMXDN0 0x25F8007A
#define VDP2_ZMYIN0 0x25F8007C
#define VDP2_ZMYDN0 0x25F8007E
#define VDP2_SCXIN1 0x25F80080
#define VDP2_SCXDN1 0x25F80082
#define VDP2_SCYIN1 0x25F80084
#define VDP2_SCYDN1 0x25F80086
#define VDP2_ZMXIN1 0x25F80088
#define VDP2_ZMXDN1 0x25F8008A
#define VDP2_ZMYIN1 0x25F8008C
#define VDP2_ZMYDN1 0x25F8008E

/* The scroll of NBG2 and of NBG3, the reduction enable, the line and vertical cell scroll
 * control, the vertical cell scroll table's address */
#define VDP2_SCXN2 0x25F80090
#define VDP2_SCYN2 0x25F80092
#define VDP2_SCXN3 0x25F80094
#define VDP2_SCYN3 0x25F80096
#define VDP2_ZMCTL 0x25F80098
#define VDP2_SCRCTL 0x25F8009A
#define VDP2_VCSTAU 0x25F8009C
#define VDP2_VCSTAL 0x25F8009E

/* The addresses of the line scroll tables of NBG0 and NBG1, of the line colour screen's table
 * and of the back screen's */
#define VDP2_LSTA0U 0x25F800A0
#define VDP2_LSTA0L 0x25F800A2
#define VDP2_LSTA1U 0x25F800A4
#define VDP2_LSTA1L 0x25F800A6
#define VDP2_LCTAU 0x25F800A8
#define VDP2_LCTAL 0x25F800AA
#define VDP2_BKTAU 0x25F800AC
#define VDP2_BKTAL 0x25F800AE

/* The rotation parameters: their mode and read control, the coefficient table's control and
 * address offset, the screen over pattern names, the parameter table's address */
#define VDP2_RPMD 0x25F800B0
#define VDP2_RPRCTL 0x25F800B2
#define VDP2_KTCTL 0x25F800B4
#define VDP2_KTAOF 0x25F800B6
#define VDP2_OVPNRA 0x25F800B8
#define VDP2_OVPNRB 0x25F800BA
#define VDP2_RPTAU 0x25F800BC
#define VDP2_RPTAL 0x25F800BE

/* The start and end points of windows 0 and 1 */
#define VDP2_WPSX0 0x25F800C0
#define VDP2_WPSY0 0x25F800C2
#define VDP2_WPEX0 0x25F800C4
#define VDP2_WPEY0 0x25F800C6
#define VDP2_WPSX1 0x25F800C8
#define VDP2_WPSY1 0x25F800CA
#define VDP2_WPEX1 0x25F800CC
#define VDP2_WPEY1 0x25F800CE

/* The window control, and the addresses of the line window tables of windows 0 and 1 */
#define VDP2_WCTLA 0x25F800D0
#define VDP2_WCTLB 0x25F800D2
#define VDP2_WCTLC 0x25F800D4
#define VDP2_WCTLD 0x25F800D6
#define VDP2_LWTA0U 0x25F800D8
#define VDP2_LWTA0L 0x25F800DA
#define VDP2_LWTA1U 0x25F800DC
#define VDP2_LWTA1L 0x25F800DE

/* The sprite and shadow control, the colour RAM address offsets, the line colour screen enable,
 * the special priority mode, the colour calculation control, the special colour calculation
 * mode */
#define VDP2_SPCTL 0x25F800E0
#define VDP2_SDCTL 0x25F800E2
#define VDP2_CRAOFA 0x25F800E4
#define VDP2_CRAOFB 0x25F800E6
#define VDP2_LNCLEN 0x25F800E8
#define VDP2_SFPRMD 0x25F800EA
#define VDP2_CCCTL 0x25F800EC
#define VDP2_SFCCMD 0x25F800EE

/* The priority numbers of the sprites, of NBG0 to NBG3 and of RBG0 */
#define VDP2_PRISA 0x25F800F0
#define VDP2_PRISB 0x25F800F2
#define VDP2_PRISC 0x25F800F4
#define VDP2_PRISD 0x25F800F6
#define VDP2_PRINA 0x25F800F8
#define VDP2_PRINB 0x25F800FA
#define VDP2_PRIR 0x25F800FC

/* The colour calculation ratios of the sprites, of NBG0 to NBG3, of RBG0, and of the line colour
 * screen and the back screen */
#define VDP2_CCRSA 0x25F80100
#define VDP2_CCRSB 0x25F80102
#define VDP2_CCRSC 0x25F80104
#define VDP2_CCRSD 0x25F80106
#define VDP2_CCRNA 0x25F80108
#define VDP2_CCRNB 0x25F8010A
#define VDP2_CCRR 0x25F8010C
#define VDP2_CCRLB 0x25F8010E

/* The colour offset's enable and select, and colour offsets A and B: red, green, blue */
#define VDP2_CLOFEN 0x25F80110
#define VDP2_CLOFSL 0x25F80112
#define VDP2_COAR 0x25F80114
#define VDP2_COAG 0x25F80116
#define VDP2_COAB 0x25F80118
#define VDP2_COBR 0x25F8011A
#define VDP2_COBG 0x25F8011C
#define VDP2_COBB 0x25F8011E

/* The display on, the border in the back screen's colour where 0 is black, then three fields:
 * the interlace mode, the lines, the width */
#define VDP2_TVMD_DISP 0x8000
#define VDP2_TVMD_BDCLMD 0x0100
#define VDP2_TVMD_LSMD 0x00C0
#define VDP2_TVMD_VRESO 0x0030
#define VDP2_TVMD_HRESO 0x0007

/* The counters latched by the external signal, the external sync taken, the vertical blank, the
 * horizontal blank, the odd field, a PAL console */
#define VDP2_TVSTAT_EXLTFG 0x0200
#define VDP2_TVSTAT_EXSYFG 0x0100
#define VDP2_TVSTAT_VBLANK 0x0008
#define VDP2_TVSTAT_HBLANK 0x0004
#define VDP2_TVSTAT_ODD 0x0002
#define VDP2_TVSTAT_PAL 0x0001

/* A screen shown: NBG0 to NBG3, RBG0, RBG1. Then a screen whose dots of colour 0 show too, where
 * 0 leaves them transparent: NBG0 to NBG3, RBG0 */
#define VDP2_BGON_N0ON 0x0001
#define VDP2_BGON_N1ON 0x0002
#define VDP2_BGON_N2ON 0x0004
#define VDP2_BGON_N3ON 0x0008
#define VDP2_BGON_R0ON 0x0010
#define VDP2_BGON_R1ON 0x0020
#define VDP2_BGON_N0TPON 0x0100
#define VDP2_BGON_N1TPON 0x0200
#define VDP2_BGON_N2TPON 0x0400
#define VDP2_BGON_N3TPON 0x0800
#define VDP2_BGON_R0TPON 0x1000

/* Of NBG0, then of NBG1: characters of four cells where 0 is one, the bitmap format where 0 is
 * the cell format, the bitmap's size, the colours */
#define VDP2_CHCTLA_N0CHSZ 0x0001
#define VDP2_CHCTLA_N0BMEN 0x0002
#define VDP2_CHCTLA_N0BMSZ 0x000C
#define VDP2_CHCTLA_N0CHCN 0x0070
#define VDP2_CHCTLA_N1CHSZ 0x0100
#define VDP2_CHCTLA_N1BMEN 0x0200
#define VDP2_CHCTLA_N1BMSZ 0x0C00
#define VDP2_CHCTLA_N1CHCN 0x3000

/* Of NBG2, of NBG3, then of RBG0: the same fields, NBG2 and NBG3 without a bitmap format */
#define VDP2_CHCTLB_N2CHSZ 0x0001
#define VDP2_CHCTLB_N2CHCN 0x0002
#define VDP2_CHCTLB_N3CHSZ 0x0010
#define VDP2_CHCTLB_N3CHCN 0x0020
#define VDP2_CHCTLB_R0CHSZ 0x0100
#define VDP2_CHCTLB_R0BMEN 0x0200
#define VDP2_CHCTLB_R0BMSZ 0x0400
#define VDP2_CHCTLB_R0CHCN 0x7000

/* PNCN0 to PNCN3 and PNCR alike: pattern names of one word where 0 is two, a character number
 * of 12 bits in such a word where 0 is 10 bits and the two flips, then what a word lacks: the
 * special priority bit, the special colour calculation bit, the top 3 bits of the palette
 * number, 5 bits of the character number */
#define VDP2_PNCN_PNB 0x8000
#define VDP2_PNCN_CNSM 0x4000
#define VDP2_PNCN_SPR 0x0200
#define VDP2_PNCN_SCC 0x0100
#define VDP2_PNCN_SPLT 0x00E0
#define VDP2_PNCN_SCN 0x001F

/* A colour for each line where 0 is one for the screen, and the top 3 bits of the back screen
 * table's address, which counts words of VRAM */
#define VDP2_BKTAU_BKCLMD 0x8000
#define VDP2_BKTAU_BKTA 0x0007

#ifndef __ASSEMBLER__

struct vdp2 {
        uint16_t tvmd;
        uint16_t exten;
        uint16_t tvstat;
        uint16_t vrsize;
        uint16_t hcnt;
        uint16_t vcnt;
        uint16_t reserve_0c;
        uint16_t ramctl;
        uint16_t cyca0l;
        uint16_t cyca0u;
        uint16_t cyca1l;
        uint16_t cyca1u;
        uint16_t cycb0l;
        uint16_t cycb0u;
        uint16_t cycb1l;
        uint16_t cycb1u;
        uint16_t bgon;
        uint16_t mzctl;
        uint16_t sfsel;
        uint16_t sfcode;
        uint16_t chctla;
        uint16_t chctlb;
        uint16_t bmpna;
        uint16_t bmpnb;
        uint16_t pncn0;
        uint16_t pncn1;
        uint16_t pncn2;
        uint16_t pncn3;
        uint16_t pncr;
        uint16_t plsz;
        uint16_t mpofn;
        uint16_t mpofr;
        uint16_t mpabn0;
        uint16_t mpcdn0;
        uint16_t mpabn1;
        uint16_t mpcdn1;
        uint16_t mpabn2;
        uint16_t mpcdn2;
        uint16_t mpabn3;
        uint16_t mpcdn3;
        uint16_t mpabra;
        uint16_t mpcdra;
        uint16_t mpefra;
        uint16_t mpghra;
        uint16_t mpijra;
        uint16_t mpklra;
        uint16_t mpmnra;
        uint16_t mpopra;
        uint16_t mpabrb;
        uint16_t mpcdrb;
        uint16_t mpefrb;
        uint16_t mpghrb;
        uint16_t mpijrb;
        uint16_t mpklrb;
        uint16_t mpmnrb;
        uint16_t mpoprb;
        uint16_t scxin0;
        uint16_t scxdn0;
        uint16_t scyin0;
        uint16_t scydn0;
        uint16_t zmxin0;
        uint16_t zmxdn0;
        uint16_t zmyin0;
        uint16_t zmydn0;
        uint16_t scxin1;
        uint16_t scxdn1;
        uint16_t scyin1;
        uint16_t scydn1;
        uint16_t zmxin1;
        uint16_t zmxdn1;
        uint16_t zmyin1;
        uint16_t zmydn1;
        uint16_t scxn2;
        uint16_t scyn2;
        uint16_t scxn3;
        uint16_t scyn3;
        uint16_t zmctl;
        uint16_t scrctl;
        uint16_t vcstau;
        uint16_t vcstal;
        uint16_t lsta0u;
        uint16_t lsta0l;
        uint16_t lsta1u;
        uint16_t lsta1l;
        uint16_t lctau;
        uint16_t lctal;
        uint16_t bktau;
        uint16_t bktal;
        uint16_t rpmd;
        uint16_t rprctl;
        uint16_t ktctl;
        uint16_t ktaof;
        uint16_t ovpnra;
        uint16_t ovpnrb;
        uint16_t rptau;
        uint16_t rptal;
        uint16_t wpsx0;
        uint16_t wpsy0;
        uint16_t wpex0;
        uint16_t wpey0;
        uint16_t wpsx1;
        uint16_t wpsy1;
        uint16_t wpex1;
        uint16_t wpey1;
        uint16_t wctla;
        uint16_t wctlb;
        uint16_t wctlc;
        uint16_t wctld;
        uint16_t lwta0u;
        uint16_t lwta0l;
        uint16_t lwta1u;
        uint16_t lwta1l;
        uint16_t spctl;
        uint16_t sdctl;
        uint16_t craofa;
        uint16_t craofb;
        uint16_t lnclen;
        uint16_t sfprmd;
        uint16_t ccctl;
        uint16_t sfccmd;
        uint16_t prisa;
        uint16_t prisb;
        uint16_t prisc;
        uint16_t prisd;
        uint16_t prina;
        uint16_t prinb;
        uint16_t prir;
        uint16_t reserve_fe;
        uint16_t ccrsa;
        uint16_t ccrsb;
        uint16_t ccrsc;
        uint16_t ccrsd;
        uint16_t ccrna;
        uint16_t ccrnb;
        uint16_t ccrr;
        uint16_t ccrlb;
        uint16_t clofen;
        uint16_t clofsl;
        uint16_t coar;
        uint16_t coag;
        uint16_t coab;
        uint16_t cobr;
        uint16_t cobg;
        uint16_t cobb;
};

#define VDP2 ((volatile struct vdp2 *)VDP2_TVMD)

HX_MAP_AT(struct vdp2, tvmd, VDP2_TVMD, VDP2_TVMD);
HX_MAP_AT(struct vdp2, exten, VDP2_TVMD, VDP2_EXTEN);
HX_MAP_AT(struct vdp2, tvstat, VDP2_TVMD, VDP2_TVSTAT);
HX_MAP_AT(struct vdp2, vrsize, VDP2_TVMD, VDP2_VRSIZE);
HX_MAP_AT(struct vdp2, hcnt, VDP2_TVMD, VDP2_HCNT);
HX_MAP_AT(struct vdp2, vcnt, VDP2_TVMD, VDP2_VCNT);
HX_MAP_AT(struct vdp2, ramctl, VDP2_TVMD, VDP2_RAMCTL);
HX_MAP_AT(struct vdp2, cyca0l, VDP2_TVMD, VDP2_CYCA0L);
HX_MAP_AT(struct vdp2, cyca0u, VDP2_TVMD, VDP2_CYCA0U);
HX_MAP_AT(struct vdp2, cyca1l, VDP2_TVMD, VDP2_CYCA1L);
HX_MAP_AT(struct vdp2, cyca1u, VDP2_TVMD, VDP2_CYCA1U);
HX_MAP_AT(struct vdp2, cycb0l, VDP2_TVMD, VDP2_CYCB0L);
HX_MAP_AT(struct vdp2, cycb0u, VDP2_TVMD, VDP2_CYCB0U);
HX_MAP_AT(struct vdp2, cycb1l, VDP2_TVMD, VDP2_CYCB1L);
HX_MAP_AT(struct vdp2, cycb1u, VDP2_TVMD, VDP2_CYCB1U);
HX_MAP_AT(struct vdp2, bgon, VDP2_TVMD, VDP2_BGON);
HX_MAP_AT(struct vdp2, mzctl, VDP2_TVMD, VDP2_MZCTL);
HX_MAP_AT(struct vdp2, sfsel, VDP2_TVMD, VDP2_SFSEL);
HX_MAP_AT(struct vdp2, sfcode, VDP2_TVMD, VDP2_SFCODE);
HX_MAP_AT(struct vdp2, chctla, VDP2_TVMD, VDP2_CHCTLA);
HX_MAP_AT(struct vdp2, chctlb, VDP2_TVMD, VDP2_CHCTLB);
HX_MAP_AT(struct vdp2, bmpna, VDP2_TVMD, VDP2_BMPNA);
HX_MAP_AT(struct vdp2, bmpnb, VDP2_TVMD, VDP2_BMPNB);
HX_MAP_AT(struct vdp2, pncn0, VDP2_TVMD, VDP2_PNCN0);
HX_MAP_AT(struct vdp2, pncn1, VDP2_TVMD, VDP2_PNCN1);
HX_MAP_AT(struct vdp2, pncn2, VDP2_TVMD, VDP2_PNCN2);
HX_MAP_AT(struct vdp2, pncn3, VDP2_TVMD, VDP2_PNCN3);
HX_MAP_AT(struct vdp2, pncr, VDP2_TVMD, VDP2_PNCR);
HX_MAP_AT(struct vdp2, plsz, VDP2_TVMD, VDP2_PLSZ);
HX_MAP_AT(struct vdp2, mpofn, VDP2_TVMD, VDP2_MPOFN);
HX_MAP_AT(struct vdp2, mpofr, VDP2_TVMD, VDP2_MPOFR);
HX_MAP_AT(struct vdp2, mpabn0, VDP2_TVMD, VDP2_MPABN0);
HX_MAP_AT(struct vdp2, mpcdn0, VDP2_TVMD, VDP2_MPCDN0);
HX_MAP_AT(struct vdp2, mpabn1, VDP2_TVMD, VDP2_MPABN1);
HX_MAP_AT(struct vdp2, mpcdn1, VDP2_TVMD, VDP2_MPCDN1);
HX_MAP_AT(struct vdp2, mpabn2, VDP2_TVMD, VDP2_MPABN2);
HX_MAP_AT(struct vdp2, mpcdn2, VDP2_TVMD, VDP2_MPCDN2);
HX_MAP_AT(struct vdp2, mpabn3, VDP2_TVMD, VDP2_MPABN3);
HX_MAP_AT(struct vdp2, mpcdn3, VDP2_TVMD, VDP2_MPCDN3);
HX_MAP_AT(struct vdp2, mpabra, VDP2_TVMD, VDP2_MPABRA);
HX_MAP_AT(struct vdp2, mpcdra, VDP2_TVMD, VDP2_MPCDRA);
HX_MAP_AT(struct vdp2, mpefra, VDP2_TVMD, VDP2_MPEFRA);
HX_MAP_AT(struct vdp2, mpghra, VDP2_TVMD, VDP2_MPGHRA);
HX_MAP_AT(struct vdp2, mpijra, VDP2_TVMD, VDP2_MPIJRA);
HX_MAP_AT(struct vdp2, mpklra, VDP2_TVMD, VDP2_MPKLRA);
HX_MAP_AT(struct vdp2, mpmnra, VDP2_TVMD, VDP2_MPMNRA);
HX_MAP_AT(struct vdp2, mpopra, VDP2_TVMD, VDP2_MPOPRA);
HX_MAP_AT(struct vdp2, mpabrb, VDP2_TVMD, VDP2_MPABRB);
HX_MAP_AT(struct vdp2, mpcdrb, VDP2_TVMD, VDP2_MPCDRB);
HX_MAP_AT(struct vdp2, mpefrb, VDP2_TVMD, VDP2_MPEFRB);
HX_MAP_AT(struct vdp2, mpghrb, VDP2_TVMD, VDP2_MPGHRB);
HX_MAP_AT(struct vdp2, mpijrb, VDP2_TVMD, VDP2_MPIJRB);
HX_MAP_AT(struct vdp2, mpklrb, VDP2_TVMD, VDP2_MPKLRB);
HX_MAP_AT(struct vdp2, mpmnrb, VDP2_TVMD, VDP2_MPMNRB);
HX_MAP_AT(struct vdp2, mpoprb, VDP2_TVMD, VDP2_MPOPRB);
HX_MAP_AT(struct vdp2, scxin0, VDP2_TVMD, VDP2_SCXIN0);
HX_MAP_AT(struct vdp2, scxdn0, VDP2_TVMD, VDP2_SCXDN0);
HX_MAP_AT(struct vdp2, scyin0, VDP2_TVMD, VDP2_SCYIN0);
HX_MAP_AT(struct vdp2, scydn0, VDP2_TVMD, VDP2_SCYDN0);
HX_MAP_AT(struct vdp2, zmxin0, VDP2_TVMD, VDP2_ZMXIN0);
HX_MAP_AT(struct vdp2, zmxdn0, VDP2_TVMD, VDP2_ZMXDN0);
HX_MAP_AT(struct vdp2, zmyin0, VDP2_TVMD, VDP2_ZMYIN0);
HX_MAP_AT(struct vdp2, zmydn0, VDP2_TVMD, VDP2_ZMYDN0);
HX_MAP_AT(struct vdp2, scxin1, VDP2_TVMD, VDP2_SCXIN1);
HX_MAP_AT(struct vdp2, scxdn1, VDP2_TVMD, VDP2_SCXDN1);
HX_MAP_AT(struct vdp2, scyin1, VDP2_TVMD, VDP2_SCYIN1);
HX_MAP_AT(struct vdp2, scydn1, VDP2_TVMD, VDP2_SCYDN1);
HX_MAP_AT(struct vdp2, zmxin1, VDP2_TVMD, VDP2_ZMXIN1);
HX_MAP_AT(struct vdp2, zmxdn1, VDP2_TVMD, VDP2_ZMXDN1);
HX_MAP_AT(struct vdp2, zmyin1, VDP2_TVMD, VDP2_ZMYIN1);
HX_MAP_AT(struct vdp2, zmydn1, VDP2_TVMD, VDP2_ZMYDN1);
HX_MAP_AT(struct vdp2, scxn2, VDP2_TVMD, VDP2_SCXN2);
HX_MAP_AT(struct vdp2, scyn2, VDP2_TVMD, VDP2_SCYN2);
HX_MAP_AT(struct vdp2, scxn3, VDP2_TVMD, VDP2_SCXN3);
HX_MAP_AT(struct vdp2, scyn3, VDP2_TVMD, VDP2_SCYN3);
HX_MAP_AT(struct vdp2, zmctl, VDP2_TVMD, VDP2_ZMCTL);
HX_MAP_AT(struct vdp2, scrctl, VDP2_TVMD, VDP2_SCRCTL);
HX_MAP_AT(struct vdp2, vcstau, VDP2_TVMD, VDP2_VCSTAU);
HX_MAP_AT(struct vdp2, vcstal, VDP2_TVMD, VDP2_VCSTAL);
HX_MAP_AT(struct vdp2, lsta0u, VDP2_TVMD, VDP2_LSTA0U);
HX_MAP_AT(struct vdp2, lsta0l, VDP2_TVMD, VDP2_LSTA0L);
HX_MAP_AT(struct vdp2, lsta1u, VDP2_TVMD, VDP2_LSTA1U);
HX_MAP_AT(struct vdp2, lsta1l, VDP2_TVMD, VDP2_LSTA1L);
HX_MAP_AT(struct vdp2, lctau, VDP2_TVMD, VDP2_LCTAU);
HX_MAP_AT(struct vdp2, lctal, VDP2_TVMD, VDP2_LCTAL);
HX_MAP_AT(struct vdp2, bktau, VDP2_TVMD, VDP2_BKTAU);
HX_MAP_AT(struct vdp2, bktal, VDP2_TVMD, VDP2_BKTAL);
HX_MAP_AT(struct vdp2, rpmd, VDP2_TVMD, VDP2_RPMD);
HX_MAP_AT(struct vdp2, rprctl, VDP2_TVMD, VDP2_RPRCTL);
HX_MAP_AT(struct vdp2, ktctl, VDP2_TVMD, VDP2_KTCTL);
HX_MAP_AT(struct vdp2, ktaof, VDP2_TVMD, VDP2_KTAOF);
HX_MAP_AT(struct vdp2, ovpnra, VDP2_TVMD, VDP2_OVPNRA);
HX_MAP_AT(struct vdp2, ovpnrb, VDP2_TVMD, VDP2_OVPNRB);
HX_MAP_AT(struct vdp2, rptau, VDP2_TVMD, VDP2_RPTAU);
HX_MAP_AT(struct vdp2, rptal, VDP2_TVMD, VDP2_RPTAL);
HX_MAP_AT(struct vdp2, wpsx0, VDP2_TVMD, VDP2_WPSX0);
HX_MAP_AT(struct vdp2, wpsy0, VDP2_TVMD, VDP2_WPSY0);
HX_MAP_AT(struct vdp2, wpex0, VDP2_TVMD, VDP2_WPEX0);
HX_MAP_AT(struct vdp2, wpey0, VDP2_TVMD, VDP2_WPEY0);
HX_MAP_AT(struct vdp2, wpsx1, VDP2_TVMD, VDP2_WPSX1);
HX_MAP_AT(struct vdp2, wpsy1, VDP2_TVMD, VDP2_WPSY1);
HX_MAP_AT(struct vdp2, wpex1, VDP2_TVMD, VDP2_WPEX1);
HX_MAP_AT(struct vdp2, wpey1, VDP2_TVMD, VDP2_WPEY1);
HX_MAP_AT(struct vdp2, wctla, VDP2_TVMD, VDP2_WCTLA);
HX_MAP_AT(struct vdp2, wctlb, VDP2_TVMD, VDP2_WCTLB);
HX_MAP_AT(struct vdp2, wctlc, VDP2_TVMD, VDP2_WCTLC);
HX_MAP_AT(struct vdp2, wctld, VDP2_TVMD, VDP2_WCTLD);
HX_MAP_AT(struct vdp2, lwta0u, VDP2_TVMD, VDP2_LWTA0U);
HX_MAP_AT(struct vdp2, lwta0l, VDP2_TVMD, VDP2_LWTA0L);
HX_MAP_AT(struct vdp2, lwta1u, VDP2_TVMD, VDP2_LWTA1U);
HX_MAP_AT(struct vdp2, lwta1l, VDP2_TVMD, VDP2_LWTA1L);
HX_MAP_AT(struct vdp2, spctl, VDP2_TVMD, VDP2_SPCTL);
HX_MAP_AT(struct vdp2, sdctl, VDP2_TVMD, VDP2_SDCTL);
HX_MAP_AT(struct vdp2, craofa, VDP2_TVMD, VDP2_CRAOFA);
HX_MAP_AT(struct vdp2, craofb, VDP2_TVMD, VDP2_CRAOFB);
HX_MAP_AT(struct vdp2, lnclen, VDP2_TVMD, VDP2_LNCLEN);
HX_MAP_AT(struct vdp2, sfprmd, VDP2_TVMD, VDP2_SFPRMD);
HX_MAP_AT(struct vdp2, ccctl, VDP2_TVMD, VDP2_CCCTL);
HX_MAP_AT(struct vdp2, sfccmd, VDP2_TVMD, VDP2_SFCCMD);
HX_MAP_AT(struct vdp2, prisa, VDP2_TVMD, VDP2_PRISA);
HX_MAP_AT(struct vdp2, prisb, VDP2_TVMD, VDP2_PRISB);
HX_MAP_AT(struct vdp2, prisc, VDP2_TVMD, VDP2_PRISC);
HX_MAP_AT(struct vdp2, prisd, VDP2_TVMD, VDP2_PRISD);
HX_MAP_AT(struct vdp2, prina, VDP2_TVMD, VDP2_PRINA);
HX_MAP_AT(struct vdp2, prinb, VDP2_TVMD, VDP2_PRINB);
HX_MAP_AT(struct vdp2, prir, VDP2_TVMD, VDP2_PRIR);
HX_MAP_AT(struct vdp2, ccrsa, VDP2_TVMD, VDP2_CCRSA);
HX_MAP_AT(struct vdp2, ccrsb, VDP2_TVMD, VDP2_CCRSB);
HX_MAP_AT(struct vdp2, ccrsc, VDP2_TVMD, VDP2_CCRSC);
HX_MAP_AT(struct vdp2, ccrsd, VDP2_TVMD, VDP2_CCRSD);
HX_MAP_AT(struct vdp2, ccrna, VDP2_TVMD, VDP2_CCRNA);
HX_MAP_AT(struct vdp2, ccrnb, VDP2_TVMD, VDP2_CCRNB);
HX_MAP_AT(struct vdp2, ccrr, VDP2_TVMD, VDP2_CCRR);
HX_MAP_AT(struct vdp2, ccrlb, VDP2_TVMD, VDP2_CCRLB);
HX_MAP_AT(struct vdp2, clofen, VDP2_TVMD, VDP2_CLOFEN);
HX_MAP_AT(struct vdp2, clofsl, VDP2_TVMD, VDP2_CLOFSL);
HX_MAP_AT(struct vdp2, coar, VDP2_TVMD, VDP2_COAR);
HX_MAP_AT(struct vdp2, coag, VDP2_TVMD, VDP2_COAG);
HX_MAP_AT(struct vdp2, coab, VDP2_TVMD, VDP2_COAB);
HX_MAP_AT(struct vdp2, cobr, VDP2_TVMD, VDP2_COBR);
HX_MAP_AT(struct vdp2, cobg, VDP2_TVMD, VDP2_COBG);
HX_MAP_AT(struct vdp2, cobb, VDP2_TVMD, VDP2_COBB);

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_315_VDP2_H */
