/*
 *  Created on: 5 de jul. de 2025
 *      Author: Osmar Bruno
 * Modificações neste fork: Arthur Xavier
 */

#include "../programa.h"

#define SW0_PORT 1
#define SW0_PIN 13
#define SW1_PORT 0
#define SW1_PIN 14
#define SW2_PORT 0
#define SW2_PIN 13

static unsigned char valor = -1;

// Definição dos pinos como entrada dos botões
void iniciaTeclado(void)
{
    Chip_GPIO_SetPinDIR(LPC_GPIO, SW0_PORT, SW0_PIN, 0); //SW1
	Chip_GPIO_SetPinDIR(LPC_GPIO, SW1_PORT, SW1_PIN, 0); //SW2
	Chip_GPIO_SetPinDIR(LPC_GPIO, SW2_PORT, SW2_PIN, 0); //SW3

}

// Debounce dos e verificação dos 3 botões
void kpdebounce() {

	const int portas[3] = {SW0_PORT, SW1_PORT, SW2_PORT};
	const int pinos[3] = {SW0_PIN, SW1_PIN, SW2_PIN};

	unsigned char temp = 0b0000;

	for(int i = 0; i < 3; i++){
		if(Chip_GPIO_GetPinState(LPC_GPIO,portas[i],pinos[i]) == 0){

			for(volatile int delay = 0; delay<10000; delay++);

			if(Chip_GPIO_GetPinState(LPC_GPIO,portas[i],pinos[i]) == 0){
				temp |= (1 << i);
			}
		}
	}

	valor = temp;
}

int checkButton() {
	return valor;
}
