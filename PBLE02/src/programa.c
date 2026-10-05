/*
 * main.c
 * Projeto: Sistema de Monitoramento e Alarme com Encoder
 * Modificações neste fork: Arthur Xavier
 */

#include "programa.h"      // O seu cabeçalho principal LPCOpen
#include "ctl/timer.h"         // Hardware: Timer32 para os milissegundos e Encoder
#include "hardware/e2prom.h"

#include "ctl/var.h"
#include "ctl/event.h"
#include "ctl/output.h"
#include "ctl/stateMachine.h"

const uint32_t OscRateIn = 12000000;
volatile uint32_t millis_counter = 0;

int main(void) {
    // 1. Configuração base do Microcontrolador (Padrão NXP LPCOpen)
    SystemCoreClockUpdate();

    varInit();

    iniciaEEPROM();

    iniciaTimer();

    iniciaSerial();
    Chip_UART_SetBaud(LPC_USART, 9600);

    outputInit();
    eventInit();
    smInit();
    iniciaADC();

    uint8_t L_HighByte = EEPROM_LerByte(0x0010);
    delayMS(5);
    uint8_t L_LowByte  = EEPROM_LerByte(0x0011);
    delayMS(5);
    uint8_t H_HighByte = EEPROM_LerByte(0x0012);
    delayMS(5);
    uint8_t H_LowByte  = EEPROM_LerByte(0x0013);
    delayMS(5);
    char Lang = EEPROM_LerByte(0x0014);
    delayMS(5);

   if (L_HighByte != 255 && L_LowByte != 255) {
	   setAlarmLevel_L((L_HighByte << 8) | L_LowByte);
       setAlarmLevel_H((H_HighByte << 8) | H_LowByte);
   }
   if (Lang != 255){
	   setLanguage(Lang);
   }

    for(;;){

        int valor = leSensor();

        setSensorLevel(valor);

        kpdebounce();
        debounce();

        smLoop();

        delayMS(50);
    }

    return 0;
}
