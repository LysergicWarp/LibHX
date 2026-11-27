#ifndef HX_HW_315_SMPC_H
#define HX_HW_315_SMPC_H

#include <hx/hw/map.h>

/** @file
 * The SMPC's parallel I/O registers: the lines of the two peripheral ports.
 *
 * A port has seven lines. PDR holds them, and DDR makes each an input or an output. IOSEL gives
 * a port to the SMPC, which then reads its peripheral for the SH-2, or to the SH-2, which then
 * drives and reads the lines itself. EXLE makes a port's TH the input of the PAD interrupt and
 * of VDP2's external latch.
 *
 * Every register is a byte at an odd address. The bytes between them are neither read nor
 * written.
 *
 * DDR1, DDR2, IOSEL and EXLE are written and never read, so a program keeps what it wrote.
 * IOSEL and EXLE hold a bit for each port, so a write sets both ports.
 *
 * From power on every line is an input, both ports are the SMPC's, and EXLE is off.
 *
 * The registers the SMPC's commands go through are not in this map. */

#define SMPC_PDR1 0x20100075
#define SMPC_PDR2 0x20100077
#define SMPC_DDR1 0x20100079
#define SMPC_DDR2 0x2010007B
#define SMPC_IOSEL 0x2010007D
#define SMPC_EXLE 0x2010007F

/* A port's lines. One that is an input reads as the line is. One that is an output is driven as
 * it is written, and reads as it was written */
#define SMPC_PDR1_TH 0x40
#define SMPC_PDR1_TR 0x20
#define SMPC_PDR1_TL 0x10
#define SMPC_PDR1_R 0x08
#define SMPC_PDR1_L 0x04
#define SMPC_PDR1_D 0x02
#define SMPC_PDR1_U 0x01
#define SMPC_PDR2_TH 0x40
#define SMPC_PDR2_TR 0x20
#define SMPC_PDR2_TL 0x10
#define SMPC_PDR2_R 0x08
#define SMPC_PDR2_L 0x04
#define SMPC_PDR2_D 0x02
#define SMPC_PDR2_U 0x01

/* A bit for each line, at the line's place in PDR: 1 for an output and 0 for an input */
#define SMPC_DDR1_DDR1 0x7F
#define SMPC_DDR2_DDR2 0x7F

/* 1 gives the port to the SH-2 and 0 to the SMPC */
#define SMPC_IOSEL_IOSEL2 0x02
#define SMPC_IOSEL_IOSEL1 0x01

/* A port whose bit is 1 has its TH as an input in DDR */
#define SMPC_EXLE_EXLE2 0x02
#define SMPC_EXLE_EXLE1 0x01

#ifndef __ASSEMBLER__

struct smpc_ports {
        uint8_t pdr1;
        uint8_t unused_76;
        uint8_t pdr2;
        uint8_t unused_78;
        uint8_t ddr1;
        uint8_t unused_7a;
        uint8_t ddr2;
        uint8_t unused_7c;
        uint8_t iosel;
        uint8_t unused_7e;
        uint8_t exle;
};

#define SMPC_PORTS ((volatile struct smpc_ports *)SMPC_PDR1)

HX_MAP_AT(struct smpc_ports, pdr1, SMPC_PDR1, SMPC_PDR1);
HX_MAP_AT(struct smpc_ports, pdr2, SMPC_PDR1, SMPC_PDR2);
HX_MAP_AT(struct smpc_ports, ddr1, SMPC_PDR1, SMPC_DDR1);
HX_MAP_AT(struct smpc_ports, ddr2, SMPC_PDR1, SMPC_DDR2);
HX_MAP_AT(struct smpc_ports, iosel, SMPC_PDR1, SMPC_IOSEL);
HX_MAP_AT(struct smpc_ports, exle, SMPC_PDR1, SMPC_EXLE);

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_315_SMPC_H */
