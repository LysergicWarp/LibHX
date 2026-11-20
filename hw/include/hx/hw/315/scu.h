#ifndef HX_HW_315_SCU_H
#define HX_HW_315_SCU_H

#include <hx/hw/map.h>

/* The SCU: DMA on three levels, its DSP, two timers, the interrupt mask and status, the A-Bus.
 *
 * Every register is 32 bits. The addresses are the ones past the SH-2's cache: through the cache
 * a read fills a line of four registers, some of which are not to be read.
 *
 * What lies between the registers is neither read nor written, 25FE00AC least of all.
 *
 * Written and never read, so a program keeps what it wrote: the DMA counts, add values, enables
 * and modes, DSTP, PPD, PDA, the three timer registers, IMS, ASR0, ASR1 and AREF, and PPAF's PR,
 * EP, ES and LE. Read alone: DSTA, VER, and PPAF's T0, S, Z, C, V and E. */

/* DMA level 0: the address read from, the address written to, the count of bytes, what each
 * address goes up by, the enable and the start, the mode. Levels 1 and 2 are the same.
 *
 * No register of a level is written while its DMA runs */
#define SCU_D0R 0x25FE0000
#define SCU_D0W 0x25FE0004
#define SCU_D0C 0x25FE0008
#define SCU_D0AD 0x25FE000C
#define SCU_D0EN 0x25FE0010
#define SCU_D0MD 0x25FE0014

#define SCU_D1R 0x25FE0020
#define SCU_D1W 0x25FE0024
#define SCU_D1C 0x25FE0028
#define SCU_D1AD 0x25FE002C
#define SCU_D1EN 0x25FE0030
#define SCU_D1MD 0x25FE0034

#define SCU_D2R 0x25FE0040
#define SCU_D2W 0x25FE0044
#define SCU_D2C 0x25FE0048
#define SCU_D2AD 0x25FE004C
#define SCU_D2EN 0x25FE0050
#define SCU_D2MD 0x25FE0054

/* The DMA force-stop and the DMA status. The SCU's final specification deletes both: the
 * force-stop is not written, and the status is no longer said to hold */
#define SCU_DSTP 0x25FE0060
#define SCU_DSTA 0x25FE007C

/* The DSP: its control port, its program RAM's data, its data RAM's address and data */
#define SCU_PPAF 0x25FE0080
#define SCU_PPD 0x25FE0084
#define SCU_PDA 0x25FE0088
#define SCU_PDD 0x25FE008C

/* Timer 0's compare, timer 1's set data and mode */
#define SCU_T0C 0x25FE0090
#define SCU_T1S 0x25FE0094
#define SCU_T1MD 0x25FE0098

/* The interrupt mask and the interrupt status. A status bit written 0 is reset and one written 1
 * stays as it is. The SCU's final specification prohibits the write */
#define SCU_IMS 0x25FE00A0
#define SCU_IST 0x25FE00A4

/* The A-Bus: its interrupt acknowledge, its two set registers, its refresh */
#define SCU_AIACK 0x25FE00A8
#define SCU_ASR0 0x25FE00B0
#define SCU_ASR1 0x25FE00B4
#define SCU_AREF 0x25FE00B8

/* The SDRAM select and the SCU's version */
#define SCU_RSEL 0x25FE00C4
#define SCU_VER 0x25FE00C8

/* The count is 20 bits on level 0 and 12 on levels 1 and 2 */
#define SCU_D0R_D0R 0x07FFFFFF
#define SCU_D0W_D0W 0x07FFFFFF
#define SCU_D0C_D0C 0x000FFFFF
#define SCU_D1R_D1R 0x07FFFFFF
#define SCU_D1W_D1W 0x07FFFFFF
#define SCU_D1C_D1C 0x00000FFF
#define SCU_D2R_D2R 0x07FFFFFF
#define SCU_D2W_D2W 0x07FFFFFF
#define SCU_D2C_D2C 0x00000FFF

/* The read address goes up by 4 bytes or stays. The write address stays for 0, and for 1 to 7
 * goes up by 2, 4, 8, 16, 32, 64 and 128 bytes. The SCU's final specification narrows both by
 * the bus the address is on */
#define SCU_D0AD_D0RA 0x00000100
#define SCU_D0AD_D0WA 0x00000007
#define SCU_D1AD_D1RA 0x00000100
#define SCU_D1AD_D1WA 0x00000007
#define SCU_D2AD_D2RA 0x00000100
#define SCU_D2AD_D2WA 0x00000007

#define SCU_D0EN_D0EN 0x00000100
#define SCU_D0EN_D0GO 0x00000001
#define SCU_D1EN_D1EN 0x00000100
#define SCU_D1EN_D1GO 0x00000001
#define SCU_D2EN_D2EN 0x00000100
#define SCU_D2EN_D2GO 0x00000001

/* Indirect mode, the read address kept updated, the write address kept updated, and what starts
 * the level once it is enabled: 0 V-Blank-IN, 1 V-Blank-OUT, 2 H-Blank-IN, 3 timer 0, 4 timer 1,
 * 5 the sound request, 6 the sprite draw end, 7 the start bit */
#define SCU_D0MD_D0MOD 0x01000000
#define SCU_D0MD_D0RUP 0x00010000
#define SCU_D0MD_D0WUP 0x00000100
#define SCU_D0MD_D0FT 0x00000007
#define SCU_D1MD_D1MOD 0x01000000
#define SCU_D1MD_D1RUP 0x00010000
#define SCU_D1MD_D1WUP 0x00000100
#define SCU_D1MD_D1FT 0x00000007
#define SCU_D2MD_D2MOD 0x01000000
#define SCU_D2MD_D2RUP 0x00010000
#define SCU_D2MD_D2WUP 0x00000100
#define SCU_D2MD_D2FT 0x00000007

#define SCU_DSTP_DSTOP 0x00000001

/* As the manual has the deleted status: a DMA on the DSP's bus, on the B-Bus, on the A-Bus.
 * Level 1 and level 0 interrupted. For each level and for the DSP, its DMA waiting and its DMA
 * running */
#define SCU_DSTA_DACSD 0x00400000
#define SCU_DSTA_DACSB 0x00200000
#define SCU_DSTA_DACSA 0x00100000
#define SCU_DSTA_D1BK 0x00020000
#define SCU_DSTA_D0BK 0x00010000
#define SCU_DSTA_D2WT 0x00002000
#define SCU_DSTA_D2MV 0x00001000
#define SCU_DSTA_D1WT 0x00000200
#define SCU_DSTA_D1MV 0x00000100
#define SCU_DSTA_D0WT 0x00000020
#define SCU_DSTA_D0MV 0x00000010
#define SCU_DSTA_DDWT 0x00000002
#define SCU_DSTA_DDMV 0x00000001

/* A read of the port clears V and E, and can lose the DSP's end interrupt. The program has E at
 * 0 ahead of starting the DSP, or the end interrupt does not come */
#define SCU_PPAF_PR 0x04000000
#define SCU_PPAF_EP 0x02000000
#define SCU_PPAF_T0 0x00800000
#define SCU_PPAF_S 0x00400000
#define SCU_PPAF_Z 0x00200000
#define SCU_PPAF_C 0x00100000
#define SCU_PPAF_V 0x00080000
#define SCU_PPAF_E 0x00040000
#define SCU_PPAF_ES 0x00020000
#define SCU_PPAF_EX 0x00010000
#define SCU_PPAF_LE 0x00008000
#define SCU_PPAF_P 0x000000FF

#define SCU_PDA_RA 0x000000FF

/* A compare value past the frame's last line gives no interrupt. A set data of 0 counts as 512,
 * and one past a line's count no longer gives an interrupt on every line */
#define SCU_T0C_T0C 0x000003FF
#define SCU_T1S_T1S 0x000001FF

/* Timer 1 on every line, or on the lines timer 0 gives alone. The timers' enable */
#define SCU_T1MD_T1MD 0x00000100
#define SCU_T1MD_TENB 0x00000001

/* A bit set masks its interrupt: V-Blank-IN, V-Blank-OUT, H-Blank-IN, timer 0, timer 1, the DSP's
 * end, the sound request, the SMPC, the PAD, the end of DMA level 2, of level 1, of level 0, the
 * illegal DMA, the sprite draw end, and the A-Bus */
#define SCU_IMS_IMS0 0x00000001
#define SCU_IMS_IMS1 0x00000002
#define SCU_IMS_IMS2 0x00000004
#define SCU_IMS_IMS3 0x00000008
#define SCU_IMS_IMS4 0x00000010
#define SCU_IMS_IMS5 0x00000020
#define SCU_IMS_IMS6 0x00000040
#define SCU_IMS_IMS7 0x00000080
#define SCU_IMS_IMS8 0x00000100
#define SCU_IMS_IMS9 0x00000200
#define SCU_IMS_IMS10 0x00000400
#define SCU_IMS_IMS11 0x00000800
#define SCU_IMS_IMS12 0x00001000
#define SCU_IMS_IMS13 0x00002000
#define SCU_IMS_IMS15 0x00008000

/* The same interrupts as the mask's, bit for bit up to 13. The top half is the 16 external
 * ones. IST12 is not set by a DMA in indirect mode */
#define SCU_IST_IST0 0x00000001
#define SCU_IST_IST1 0x00000002
#define SCU_IST_IST2 0x00000004
#define SCU_IST_IST3 0x00000008
#define SCU_IST_IST4 0x00000010
#define SCU_IST_IST5 0x00000020
#define SCU_IST_IST6 0x00000040
#define SCU_IST_IST7 0x00000080
#define SCU_IST_IST8 0x00000100
#define SCU_IST_IST9 0x00000200
#define SCU_IST_IST10 0x00000400
#define SCU_IST_IST11 0x00000800
#define SCU_IST_IST12 0x00001000
#define SCU_IST_IST13 0x00002000
#define SCU_IST_IST31_16 0xFFFF0000

#define SCU_AIACK_AIACK 0x00000001

/* CS0 in the top half and CS1 in the bottom: the read ahead, which stays 0, the pre-charge after
 * a write and after a read, the external wait, the waits of a burst cycle and of a normal one,
 * the burst length, the bus size.
 *
 * Written only while the A-Bus is not being accessed */
#define SCU_ASR0_A0PRD 0x80000000
#define SCU_ASR0_A0WPC 0x40000000
#define SCU_ASR0_A0RPC 0x20000000
#define SCU_ASR0_A0EWT 0x10000000
#define SCU_ASR0_A0BW 0x0F000000
#define SCU_ASR0_A0NW 0x00F00000
#define SCU_ASR0_A0LN 0x000C0000
#define SCU_ASR0_A0SZ 0x00010000
#define SCU_ASR0_A1PRD 0x00008000
#define SCU_ASR0_A1WPC 0x00004000
#define SCU_ASR0_A1RPC 0x00002000
#define SCU_ASR0_A1EWT 0x00001000
#define SCU_ASR0_A1BW 0x00000F00
#define SCU_ASR0_A1NW 0x000000F0
#define SCU_ASR0_A1LN 0x0000000C
#define SCU_ASR0_A1SZ 0x00000001

/* The same for CS2 in the top half, which has no waits to set, and the dummy space in the
 * bottom */
#define SCU_ASR1_A2PRD 0x80000000
#define SCU_ASR1_A2WPC 0x40000000
#define SCU_ASR1_A2RPC 0x20000000
#define SCU_ASR1_A2EWT 0x10000000
#define SCU_ASR1_A2LN 0x000C0000
#define SCU_ASR1_A2SZ 0x00010000
#define SCU_ASR1_A3PRD 0x00008000
#define SCU_ASR1_A3WPC 0x00004000
#define SCU_ASR1_A3RPC 0x00002000
#define SCU_ASR1_A3EWT 0x00001000
#define SCU_ASR1_A3BW 0x00000F00
#define SCU_ASR1_A3NW 0x000000F0
#define SCU_ASR1_A3LN 0x0000000C
#define SCU_ASR1_A3SZ 0x00000001

/* The refresh's enable, on from power on and left so, and its waits */
#define SCU_AREF_ARFEN 0x00000010
#define SCU_AREF_ARWT 0x0000000F

/* 1 for the two 4 Mbit SDRAMs, as the boot ROM sets it */
#define SCU_RSEL_RSEL 0x00000001

#define SCU_VER_VER 0x0000000F

#ifndef __ASSEMBLER__

struct scu {
        uint32_t d0r;
        uint32_t d0w;
        uint32_t d0c;
        uint32_t d0ad;
        uint32_t d0en;
        uint32_t d0md;
        uint32_t unused_18[2];
        uint32_t d1r;
        uint32_t d1w;
        uint32_t d1c;
        uint32_t d1ad;
        uint32_t d1en;
        uint32_t d1md;
        uint32_t unused_38[2];
        uint32_t d2r;
        uint32_t d2w;
        uint32_t d2c;
        uint32_t d2ad;
        uint32_t d2en;
        uint32_t d2md;
        uint32_t unused_58[2];
        uint32_t dstp;
        uint32_t unused_64[6];
        uint32_t dsta;
        uint32_t ppaf;
        uint32_t ppd;
        uint32_t pda;
        uint32_t pdd;
        uint32_t t0c;
        uint32_t t1s;
        uint32_t t1md;
        uint32_t unused_9c;
        uint32_t ims;
        uint32_t ist;
        uint32_t aiack;
        uint32_t unused_ac;
        uint32_t asr0;
        uint32_t asr1;
        uint32_t aref;
        uint32_t unused_bc[2];
        uint32_t rsel;
        uint32_t ver;
};

#define SCU ((volatile struct scu *)SCU_D0R)

HX_MAP_AT(struct scu, d0r, SCU_D0R, SCU_D0R);
HX_MAP_AT(struct scu, d0w, SCU_D0R, SCU_D0W);
HX_MAP_AT(struct scu, d0c, SCU_D0R, SCU_D0C);
HX_MAP_AT(struct scu, d0ad, SCU_D0R, SCU_D0AD);
HX_MAP_AT(struct scu, d0en, SCU_D0R, SCU_D0EN);
HX_MAP_AT(struct scu, d0md, SCU_D0R, SCU_D0MD);
HX_MAP_AT(struct scu, d1r, SCU_D0R, SCU_D1R);
HX_MAP_AT(struct scu, d1w, SCU_D0R, SCU_D1W);
HX_MAP_AT(struct scu, d1c, SCU_D0R, SCU_D1C);
HX_MAP_AT(struct scu, d1ad, SCU_D0R, SCU_D1AD);
HX_MAP_AT(struct scu, d1en, SCU_D0R, SCU_D1EN);
HX_MAP_AT(struct scu, d1md, SCU_D0R, SCU_D1MD);
HX_MAP_AT(struct scu, d2r, SCU_D0R, SCU_D2R);
HX_MAP_AT(struct scu, d2w, SCU_D0R, SCU_D2W);
HX_MAP_AT(struct scu, d2c, SCU_D0R, SCU_D2C);
HX_MAP_AT(struct scu, d2ad, SCU_D0R, SCU_D2AD);
HX_MAP_AT(struct scu, d2en, SCU_D0R, SCU_D2EN);
HX_MAP_AT(struct scu, d2md, SCU_D0R, SCU_D2MD);
HX_MAP_AT(struct scu, dstp, SCU_D0R, SCU_DSTP);
HX_MAP_AT(struct scu, dsta, SCU_D0R, SCU_DSTA);
HX_MAP_AT(struct scu, ppaf, SCU_D0R, SCU_PPAF);
HX_MAP_AT(struct scu, ppd, SCU_D0R, SCU_PPD);
HX_MAP_AT(struct scu, pda, SCU_D0R, SCU_PDA);
HX_MAP_AT(struct scu, pdd, SCU_D0R, SCU_PDD);
HX_MAP_AT(struct scu, t0c, SCU_D0R, SCU_T0C);
HX_MAP_AT(struct scu, t1s, SCU_D0R, SCU_T1S);
HX_MAP_AT(struct scu, t1md, SCU_D0R, SCU_T1MD);
HX_MAP_AT(struct scu, ims, SCU_D0R, SCU_IMS);
HX_MAP_AT(struct scu, ist, SCU_D0R, SCU_IST);
HX_MAP_AT(struct scu, aiack, SCU_D0R, SCU_AIACK);
HX_MAP_AT(struct scu, asr0, SCU_D0R, SCU_ASR0);
HX_MAP_AT(struct scu, asr1, SCU_D0R, SCU_ASR1);
HX_MAP_AT(struct scu, aref, SCU_D0R, SCU_AREF);
HX_MAP_AT(struct scu, rsel, SCU_D0R, SCU_RSEL);
HX_MAP_AT(struct scu, ver, SCU_D0R, SCU_VER);

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_315_SCU_H */
