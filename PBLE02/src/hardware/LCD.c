/*
 * LCD.c
 * Modificações neste fork: Arthur Xavier
 */

#include "LCD.h"

// Definições dos pinos de controle do LCD
#define PORTA_ENABLE 1
#define PINO_ENABLE 28

#define PORTA_RS 0
#define PINO_RS 21

#define PORTA_RW 1
#define PINO_RW 23

// Definições dos pinos de dados do LCD
#define PORTA_DB4 1
#define PINO_DB4 31

#define PORTA_DB5 1
#define PINO_DB5 24

#define PORTA_DB6 0
#define PINO_DB6 6

#define PORTA_DB7 0
#define PINO_DB7 7

// Função para colocar o pino em nível alto
void bit_set(int port, int pin)
{
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, port, pin);
}

// Função para colocar o pino em nível baixo
void bit_clr(int port, int pin)
{
    Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
}

// Função para enviar os dados para o display
void gravaDadosNoDisplay(char v_cDado)
{
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB7, PINO_DB7, (v_cDado >> 7) & 0x01);
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB6, PINO_DB6, (v_cDado >> 6) & 0x01);
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB5, PINO_DB5, (v_cDado >> 5) & 0x01);
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB4, PINO_DB4, (v_cDado >> 4) & 0x01);
    delayMS(1);

    bit_set(PORTA_ENABLE, PINO_ENABLE);
    delayMS(1);
    bit_clr(PORTA_ENABLE, PINO_ENABLE);

    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB7, PINO_DB7, (v_cDado >> 3) & 0x01);
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB6, PINO_DB6, (v_cDado >> 2) & 0x01);
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB5, PINO_DB5, (v_cDado >> 1) & 0x01);
    Chip_GPIO_SetPinState(LPC_GPIO, PORTA_DB4, PINO_DB4, (v_cDado >> 0) & 0x01);
    delayMS(1);

    bit_set(PORTA_ENABLE, PINO_ENABLE);
    delayMS(1);
    bit_clr(PORTA_ENABLE, PINO_ENABLE);
}

// Função para enviar um comando ao LCD
void LCD_comando(char v_cComando)
{
    bit_clr(PORTA_RS, PINO_RS);
    gravaDadosNoDisplay(v_cComando);
}

// Função para iniciar o LCD
void iniciaLCD()
{

    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_ENABLE, PINO_ENABLE, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_RS, PINO_RS, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_RW, PINO_RW, IOCON_FUNC0 | IOCON_MODE_INACT);

    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_DB4, PINO_DB4, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_DB5, PINO_DB5, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_DB6, PINO_DB6, IOCON_FUNC0 | IOCON_MODE_INACT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_DB7, PINO_DB7, IOCON_FUNC0 | IOCON_MODE_INACT);

    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_ENABLE, PINO_ENABLE, 1);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_RS, PINO_RS, 1);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_RW, PINO_RW, 1);

    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_DB4, PINO_DB4, 1);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_DB5, PINO_DB5, 1);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_DB6, PINO_DB6, 1);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_DB7, PINO_DB7, 1);


    bit_clr(PORTA_RW, PINO_RW);
    bit_clr(PORTA_ENABLE, PINO_ENABLE);
    bit_clr(PORTA_RS, PINO_RS);

    delayMS(100);

    LCD_comando(0x33);
    delayMS(5);
    LCD_comando(0x32);
    delayMS(5);

    LCD_comando(0x28);
    delayMS(5);

    LCD_comando(0x0C);
    delayMS(5);

    LCD_comando(0x01);
    delayMS(20);

    LCD_comando(0x06);
    delayMS(5);
}

// Função para enviar um caractere ao LCD
void LCD_caractere(char v_cCaractere)
{
    bit_set(PORTA_RS, PINO_RS);  // Indica que é dado
    gravaDadosNoDisplay(v_cCaractere);
}

// Função para escrever uma string no LCD
void LCD_string(const char *str)
{
    while (*str) {
        LCD_caractere(*str);
        str++;
    }
}

void LCD_int(int val) {
    if (val < 0) {
        val = val * (-1);
        LCD_caractere('-');
    }
    LCD_caractere((val / 10000) % 10 + 48);
    LCD_caractere((val / 1000) % 10 + 48);
    LCD_caractere((val / 100) % 10 + 48);
    LCD_caractere((val / 10) % 10 + 48);
    LCD_caractere((val / 1) % 10 + 48);
}

void LCD_int2Dig(int val) {
    if (val < 0) {
        val = val * (-1);
        LCD_caractere('-');
    }

    LCD_caractere((val / 10) % 10 + 48);
    LCD_caractere((val / 1) % 10 + 48);
}
