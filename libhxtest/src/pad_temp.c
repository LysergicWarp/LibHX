#include <hx/test.h>

#include <hx/hw/315/smpc.h>

/* The lines the SH-2 drives, and the four it reads a part of the pad's buttons on */
#define TH SMPC_PDR1_TH
#define TR SMPC_PDR1_TR
#define DATA (SMPC_PDR1_R | SMPC_PDR1_L | SMPC_PDR1_D | SMPC_PDR1_U)

/* The pad asks for 2 microseconds between a change of TH or TR and the read. A read of the port
 * is a bus cycle of 2 clocks at the least, and a clock is 34.9 ns at the fastest, so 32 reads
 * take 2.2 microseconds or more. The last of them is kept */
#define SETTLE_READS 32

/* With TH and TR both high a Saturn pad gives L, then 1, 0 and 0, which no other peripheral
 * and no empty port does */
#define ID 0x7
#define ID_PAD 0x4

/* The data lines once TH and TR are as given */
static uint32_t
lines(volatile uint8_t *pdr, uint8_t out)
{
        uint8_t in = 0;

        *pdr = out;
        for (uint32_t read = 0; read < SETTLE_READS; read++) {
                in = *pdr;
        }
        return in & DATA;
}

uint32_t
hx_test_pad_temp(uint32_t port)
{
        volatile uint8_t * const pdr = port ? &SMPC_PORTS->pdr2 : &SMPC_PORTS->pdr1;
        volatile uint8_t * const ddr = port ? &SMPC_PORTS->ddr2 : &SMPC_PORTS->ddr1;

        /* The data, then the directions, then the mode. TH and TR are the outputs, both high,
         * which is the pad at rest. TH as an output asks for EXLE off */
        *pdr = TH | TR;
        *ddr = TH | TR;
        SMPC_PORTS->iosel = SMPC_IOSEL_IOSEL1 | SMPC_IOSEL_IOSEL2;
        SMPC_PORTS->exle = 0;

        /* L and the pad's ID. Right, Left, Down and Up. Start, A, C and B. R, X, Y and Z */
        const uint32_t first = lines(pdr, TH | TR);
        const uint32_t second = lines(pdr, TR);
        const uint32_t third = lines(pdr, TH);
        const uint32_t fourth = lines(pdr, 0);

        *pdr = TH | TR;

        if ((first & ID) != ID_PAD) {
                return 0;
        }

        /* A line is low for a button that is held */
        const uint32_t held = ~((second << 12) | (third << 8) | (fourth << 4) | first);

        return (held & HX_TEST_PAD_BUTTONS_TEMP) | HX_TEST_PAD_THERE_TEMP;
}
