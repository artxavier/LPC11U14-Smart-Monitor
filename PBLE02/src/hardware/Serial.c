/*
 * Created on: 5 de jul. de 2025
 * Author: Osmar Bruno
 * Modificações neste fork: Arthur Xavier
 */

#include "../programa.h"
#include <string.h>

#define UART_SRB_SIZE 128
#define UART_RRB_SIZE 32

RINGBUFF_T txring, rxring;
uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

static void Init_UART_PinMux(void)
{
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, IOCON_FUNC1 | IOCON_MODE_INACT);    /* PIO0_18 used for RXD */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, IOCON_FUNC1 | IOCON_MODE_INACT);    /* PIO0_19 used for TXD */
#elif (defined(BOARD_NXP_XPRESSO_11C24) || defined(BOARD_MCORE48_1125))
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));/* RXD */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));/* TXD */
#else
#error "No Pin muxing defined for UART operation"
#endif
}

void UART_IRQHandler(void)
{
	Chip_UART_IRQRBHandler(LPC_USART, &rxring, &txring);
}

void iniciaSerial(void)
{
	Init_UART_PinMux();

    Chip_UART_Init(LPC_USART);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0);
	Chip_Clock_SetUARTClockDiv(1);

    Chip_UART_SetBaud(LPC_USART, 9600);
    Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
    Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(LPC_USART);

    RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
    RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

    Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));
    NVIC_SetPriority(UART0_IRQn, 1);
    NVIC_EnableIRQ(UART0_IRQn);
}

void desligaSerial(void)
{
	NVIC_DisableIRQ(UART0_IRQn);
    Chip_UART_DeInit(LPC_USART);
}

uint8_t dadoRecebido(void)
{
    uint8_t key = 0;
	Chip_UART_Read(LPC_USART, &key, 1);
	return key;
}

void enviaDado(uint8_t dado)
{
	uint8_t key = dado;
	Chip_UART_SendRB(LPC_USART, &txring,  &key, 1);
}

void serialLigaLED()
{
	static uint8_t key;
	key = dadoRecebido();

	if(key > 0)
	{
		switch(key) {
			case '0': toggleLED(0); break;
			case '1': toggleLED(1); break;
			case '2': toggleLED(2); break;
			case '3': toggleLED(3); break;
			default: break;
		}
		enviaDado(key);
	}
}

void serial_enviaString(const char *str)
{
    while (*str) {
        enviaDado(*str++);
    }
}

void serial_enviaInteiro(int val)
{
    char buffer[11];
    int i = 0;

    if (val == 0) {
        enviaDado('0');
        return;
    }
    if (val < 0) {
        enviaDado('-');
        val = -val;
    }
    while (val > 0) {
        buffer[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        enviaDado(buffer[--i]);
    }
}

void serial_enviaInt2Dig(int val)
{
    if (val < 0) val = 0;
    enviaDado((val / 10) % 10 + '0');
    enviaDado((val % 10) + '0');
}

void serial_enviaInt4Dig(int val)
{
    if (val < 0) val = 0;
    enviaDado((val / 1000) % 10 + '0');
    enviaDado((val / 100)  % 10 + '0');
    enviaDado((val / 10)   % 10 + '0');
    enviaDado(val % 10 + '0');
}

// ---------------------------------------------------------------
// Função de montagem e verificação se o comando na serial acabou
// ---------------------------------------------------------------
int serial_TemComandoPronto(char* bufferDestino) {
    static char command_buffer[32];
    static int buffer_index = 0;
    static int comando_ativo = 0;
    uint8_t byte_lido;

    while (Chip_UART_ReadRB(LPC_USART, &rxring, &byte_lido, 1) > 0) {

        enviaDado(byte_lido); // Eco no terminal

        if (byte_lido == '(') {
            buffer_index = 0;
            comando_ativo = 1;
            continue;
        }

        if (comando_ativo) {
            if (byte_lido == ')') {
                command_buffer[buffer_index] = '\0';
                comando_ativo = 0;
                strcpy(bufferDestino, command_buffer);
                return 1; // Avisa que tem comando pronto!
            } else {
                if (buffer_index < (sizeof(command_buffer) - 1)) {
                    command_buffer[buffer_index++] = byte_lido;
                } else {
                    comando_ativo = 0; // Previne Buffer Overflow
                }
            }
        }
    }
    return 0;
}
