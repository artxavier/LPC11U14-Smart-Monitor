/*
 * Modificações neste fork: Arthur Xavier
 */

#include "e2prom.h"
#include "../I2C.h"

#define EEPROM_ADDR 0x50
#define EEPROM_WP_PORT 0
#define EEPROM_WP_PIN 3

// Função de definição da comunicação I2C entre E2PROM e Microcontrolador
// Definição do pino de Write Protect
void iniciaEEPROM(void) {

    iniciaI2C();

    Chip_IOCON_PinMuxSet(LPC_IOCON, EEPROM_WP_PORT, EEPROM_WP_PIN, IOCON_FUNC0 | IOCON_MODE_INACT);

    Chip_GPIO_SetPinDIR(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 1);

    Chip_GPIO_SetPinState(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 1);
}

// Função de escrever no E2PROM (16 bits por endereço)
void EEPROM_EscreverByte(uint16_t addr, uint8_t dado) {
    Chip_GPIO_SetPinState(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 0); // Desativa WP

    unsigned char buffer[3];
    buffer[0] = (addr >> 8) & 0xFF;
    buffer[1] = addr & 0xFF;
    buffer[2] = dado;

    I2C_Transmitir(EEPROM_ADDR, buffer, 3);

    delayMS(10);

    Chip_GPIO_SetPinState(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 1); // Ativa WP
}

// Função de ler informação do E2PROM
uint8_t EEPROM_LerByte(uint16_t addr) {
    unsigned char buffer[2];
    unsigned char dado_lido = 0;

    buffer[0] = (addr >> 8) & 0xFF;
    buffer[1] = addr & 0xFF;
    I2C_Transmitir(EEPROM_ADDR, buffer, 2);

    I2C_Receber(EEPROM_ADDR, &dado_lido, 1);

    return dado_lido;
}
