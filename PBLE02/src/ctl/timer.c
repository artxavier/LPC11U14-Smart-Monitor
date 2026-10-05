/*
 * Modificações neste fork: Arthur Xavier
 */

#include "timer.h"
#include "../hardware/encoder.h"


void iniciaTimer(void)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CT32B0);


	LPC_TIMER32_0->PR = 39;
	LPC_TIMER32_0->MR[0] = 999;


	LPC_TIMER32_0->MCR = (1 << 0) | (1 << 1);

	NVIC_EnableIRQ(TIMER_32_0_IRQn);

	LPC_TIMER32_0->TCR = 1;
}

void TIMER32_0_IRQHandler(void) {

    if (LPC_TIMER32_0->IR & (1 << 0)) {

        LPC_TIMER32_0->IR = (1 << 0);

        millis_counter++;

        encoderCheckRotation();
    }
}
