/*
 *  Created on: 5 de jul. de 2025
 *      Author: Osmar Bruno
 * Modificações neste fork: Arthur Xavier
 */

#include "../programa.h"

void iniciaLED(void);
void ligaLED(int);
void desligaLED(int);
void toggleLED(int);

void iniciaLED(void)
{

    LPC_SYSCTL->SYSAHBCLKCTRL |= (1 << 6);
    LPC_SYSCTL->SYSAHBCLKCTRL |= (1 << 16);

    LPC_IOCON->PIO0[12] = 0xD1;

    LPC_IOCON->PIO0[11] = 0xD1;


    LPC_GPIO->DIR[0] |= (1 << 11);
    LPC_GPIO->DIR[0] |= (1 << 12);


    LPC_GPIO->SET[0] = (1 << 11);
    LPC_GPIO->SET[0] = (1 << 12);
}

void ligaLED(int led)
{
    switch(led) {
        case 0:
            Chip_GPIO_SetPinState(LPC_GPIO, 1, 24, 1);
            break;
        case 1:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 1);
            break;
        case 2:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 1);
            break;
        case 3:
            Chip_GPIO_SetPinState(LPC_GPIO, 1, 28, 1);
            break;
        default:
            break;
    }
}

void desligaLED(int led)
{
    switch(led) {
        case 0:
            Chip_GPIO_SetPinState(LPC_GPIO, 1, 24, 0);
            break;
        case 1:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 0);
            break;
        case 2:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 0);
            break;
        case 3:
            Chip_GPIO_SetPinState(LPC_GPIO, 1, 28, 0);
            break;
        default:
            break;
    }
}

void defineLED(int led, int state)
{
	switch(led) {
		case 0:
			Chip_GPIO_SetPinState(LPC_GPIO, 1, 24, state);
			break;
		case 1:
			Chip_GPIO_SetPinState(LPC_GPIO, 0, 6, state);
			break;
		case 2:
			Chip_GPIO_SetPinState(LPC_GPIO, 0, 7, state);
			break;
		case 3:
			Chip_GPIO_SetPinState(LPC_GPIO, 1, 28, state);
			break;
		default:
			break;
	}
}

void toggleLED(int led)
{
    switch(led) {
        case 0:
            Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 11);
            break;
        case 1:
            Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 12);
            break;
        case 2:
            Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 7);
            break;
        case 3:
            Chip_GPIO_SetPinToggle(LPC_GPIO, 1, 28);
            break;
        default:
            break;
    }
}
