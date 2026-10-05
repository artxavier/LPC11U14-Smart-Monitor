/*
 *  Created on: 5 de jul. de 2025
 *      Author: Osmar Bruno
 * Modificações neste fork: Arthur Xavier
 */

#ifndef TECLADO_H
#define TECLADO_H
	#include "../programa.h"
    void iniciaTeclado(void);
    int lerBotaoPin(int, int);
    int lerBotao(const char *botao);
    void botaoLigaLED(void); //Deve-se utilizar um loop para funcionar corretamente
    void kpdebounce();
    int checkButton();
#endif
