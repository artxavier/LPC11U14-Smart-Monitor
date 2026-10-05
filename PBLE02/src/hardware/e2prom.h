/*
 * EEPROM.h
 * Biblioteca para EEPROM I2C
 * Modificações neste fork: Arthur Xavier
 */

#ifndef E2PROM_H
#define E2PROM_H

#include "../programa.h"


void iniciaEEPROM(void);
void EEPROM_EscreverByte(uint16_t endereco_memoria, uint8_t dado);
uint8_t EEPROM_LerByte(uint16_t endereco_memoria);

#endif /* E2PROM_H */
