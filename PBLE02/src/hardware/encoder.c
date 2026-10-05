/*
 * Modificações neste fork: Arthur Xavier
 */

#include "encoder.h"

// --- MAPEAMENTO DE PINOS ---
#define ENC_PORT_A  0
#define ENC_PIN_A   2   // SWA -> PIO0_2

#define ENC_PORT_B  1
#define ENC_PIN_B   26   // SWB -> PIO1_26

#define ENC_PORT_S1 1
#define ENC_PIN_S1  27   // S1  -> PIO2_8
// ---------------------------

static unsigned char valor = 0;
static unsigned char antigo;
static unsigned char EAnterior;

static int pos = 0;

// Init de cada pino e estado inicial
void encoderInit(void) {
    // Configura os pinos como entrada (DIR = 0)
    Chip_GPIO_SetPinDIR(LPC_GPIO, ENC_PORT_A,  ENC_PIN_A,  0);
    Chip_GPIO_SetPinDIR(LPC_GPIO, ENC_PORT_B,  ENC_PIN_B,  0);
    Chip_GPIO_SetPinDIR(LPC_GPIO, ENC_PORT_S1, ENC_PIN_S1, 0);

    // Lê o estado inicial do pino A para a primeira comparação
    EAnterior = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_A, ENC_PIN_A);
    encoderReset();
}

// Debounce e verificação do botão do encoder
void debounce(void) {
    antigo = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_S1, ENC_PIN_S1);

    if (antigo != Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_S1, ENC_PIN_S1)) {
        antigo = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_S1, ENC_PIN_S1);

        // Loop de atraso (o volatile impede o compilador de remover o loop)
        for (volatile int i = 0; i < 100; i++) {
            if (antigo != Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_S1, ENC_PIN_S1)) {
                i = 0;
            }
        }
    }
    valor = antigo;
}

char encoderCheckButton(void) {
    debounce();
    return valor;
}

// Verificação de giro do encoder e retorno de posição atual
void encoderCheckRotation(void) {
    unsigned char Atual;
    unsigned char stateA = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_A, ENC_PIN_A);
    unsigned char stateB = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_B, ENC_PIN_B);

    // Concatena as leituras em um valor de 2 bits
    Atual = (stateA << 1) | stateB;

    if (Atual != EAnterior) {
        // Lógica de incremento (Sentido Horário)
        if ((Atual == 0 && EAnterior == 1) || (Atual == 2 && EAnterior == 0) ||
            (Atual == 3 && EAnterior == 2) || (Atual == 1 && EAnterior == 3)) {
            pos++;
        }

        // Lógica de decremento (Sentido Anti-horário)
        if ((Atual == 1 && EAnterior == 0) || (Atual == 3 && EAnterior == 1) ||
            (Atual == 2 && EAnterior == 3) || (Atual == 0 && EAnterior == 2)) {
            pos--;
        }

        EAnterior = Atual;
    }
}

int encoderGetpos(void) {
    return pos;
}

void encoderReset(void) {
    pos = 0;
}
