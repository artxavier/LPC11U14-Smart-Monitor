/*
 * Modificações neste fork: Arthur Xavier
 */

#include "output.h"
#include "../hardware/LCD.h"
#include "../hardware/LED.h"
#include "timer.h"
#include "stateMachine.h"
#include "var.h"

#define NUM_IDIOMAS 3

// Línguas do menu (Português, Inglês e Francês)
static char * msgs[STATE_FIM][NUM_IDIOMAS] = {
    {"Tensao Sensor  ", "Sensor Voltage ", "Tension Capteur "},
    {"Conf. Alarmes  ", "Alarm Config   ", "Reg. de L'alarme"},
    {"Alterar Idioma ", "Change Language", "Changer Langue  "},
    {"**** ALARME ****", "**** ALARM **** ", "**** ALARME ****"}
};

void outputInit(void) {
    iniciaLCD();
    iniciaLED();
    desligaLED(1);
    desligaLED(2);
}

// Printa a tela de acordo com o estado da máquina de estados
void outputPrint(int numTela, int idioma, int editando) {

    LCD_comando(0x80);
    LCD_string(msgs[numTela][idioma]);

    // Asterisco ou não caso em modo de edição de tela
    if (editando) {
        LCD_comando(0x8F);
        LCD_string("*");
    }
    else if(!editando){
        LCD_comando(0x8F);
        LCD_string(" ");
    }

    LCD_comando(0xC0);

    // Tela principal
    if (numTela == STATE_TENSAO) {
        char str_val[8];
        floatParaString(getSensorLevel_V(), str_val);
        LCD_string("ADC:"); LCD_int(getSensorLevel());
        LCD_string(" V:");  LCD_string(str_val); LCD_string("  ");
    }
    // Tela de alarme
    else if (numTela == STATE_ALARME) {
        if (editando == 1) LCD_string("*L:");
        else               LCD_string("L:");
        LCD_int(getAlarmLevel_L());

        if (editando == 2) LCD_string(" *H:");
        else               LCD_string("  H:");
        LCD_int(getAlarmLevel_H());

        LCD_string("  "); // Limpa sujeira residual
    }

    // Tela de idioma
    else if (numTela == STATE_IDIOMA) {
        if (getLanguage() == 0) LCD_string("Portugues       ");
        if (getLanguage() == 1) LCD_string("English         ");
        if (getLanguage() == 2) LCD_string("Francais        ");
    }

    // Tela alarme disparado
    else if (numTela == STATE_DISPARA) {
        LCD_string("CRITICO: ");
        LCD_int(getSensorLevel());
        LCD_string("!   ");
        ligaLED(1);
        ligaLED(2);
        delayMS(100);
        desligaLED(1);
        desligaLED(2);

    }
}
