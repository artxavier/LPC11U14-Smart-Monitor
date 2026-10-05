/*
 * I2C.c
 * Modificações neste fork: Arthur Xavier
 */

#include "programa.h"

#define I2C_TIMEOUT 100000 // Limite de tentativas para evitar loop infinito

void iniciaI2C(void) {
    LPC_SYSCTL->SYSAHBCLKCTRL |= 1<<16;

    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4, IOCON_FUNC1 | IOCON_MODE_INACT); // PIO0_4: SCL
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5, IOCON_FUNC1 | IOCON_MODE_INACT); // PIO0_5: SDA

    LPC_SYSCTL->SYSAHBCLKCTRL |= 1<<5;
    LPC_SYSCTL->PRESETCTRL |= 1<<1;

    LPC_I2C->SCLH = 240; // Clock I2C 100kHz
    LPC_I2C->SCLL = 240;

    LPC_I2C->CONSET |= (1 << 6); // Habilita periférico como mestre
}

void I2C_Transmitir(unsigned char endereco, unsigned char *valor, unsigned char qtd){
    unsigned char c;
    volatile uint32_t timeout;

    LPC_I2C->CONSET = (1 << 5); // START

    timeout = I2C_TIMEOUT;
    while (LPC_I2C->STAT != 0x08 && --timeout);
    if (!timeout) goto I2C_FALHA;

    LPC_I2C->DAT = endereco << 1; // Endereço + Modo Escrita
    LPC_I2C->CONCLR = (5 << 3);

    timeout = I2C_TIMEOUT;
    while (LPC_I2C->STAT != 0x18 && --timeout); // Aguarda ACK
    if (!timeout) goto I2C_FALHA;

    for (c = 0; c < qtd; c++) {
        LPC_I2C->DAT = valor[c];
        LPC_I2C->CONCLR = (1 << 3);

        timeout = I2C_TIMEOUT;
        while (LPC_I2C->STAT != 0x28 && --timeout);
        if (!timeout) goto I2C_FALHA;
    }

I2C_FALHA:
    LPC_I2C->CONSET = (1 << 4); // STOP (Libera o barramento)
    LPC_I2C->CONCLR = (1 << 3);
}

void I2C_Receber(unsigned char endereco, unsigned char *valor, unsigned char qtd){
    unsigned char c;
    volatile uint32_t timeout;

    LPC_I2C->CONSET = (1 << 5); // START

    timeout = I2C_TIMEOUT;
    while (LPC_I2C->STAT != 0x08 && --timeout);
    if (!timeout) goto I2C_FALHA_REC;

    LPC_I2C->DAT = (endereco << 1) + 1; // Endereço + Modo Leitura
    LPC_I2C->CONCLR = (5 << 3);

    timeout = I2C_TIMEOUT;
    while (LPC_I2C->STAT != 0x40 && --timeout);
    if (!timeout) goto I2C_FALHA_REC;

    for (c = 0; c < qtd; c++) {
        LPC_I2C->CONCLR = 0x2C;
        if (c != (qtd - 1)) {
            LPC_I2C->CONSET = 0x4; // Gera ACK
        }

        timeout = I2C_TIMEOUT;
        while ((LPC_I2C->STAT != 0x50) && (LPC_I2C->STAT != 0x58) && --timeout);
        if (!timeout) goto I2C_FALHA_REC;

        valor[c] = LPC_I2C->DAT;
    }

I2C_FALHA_REC:
    LPC_I2C->CONSET = (1 << 4); // STOP
    LPC_I2C->CONCLR = (1 << 3);
}
