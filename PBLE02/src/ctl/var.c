/*
 * Modificações neste fork: Arthur Xavier
 */

#include "var.h"

static char state;
static char language;

static int sensorLevel;
static int alarmLevel_L;
static int alarmLevel_H;

void varInit(void) {
    state = 0;
    language = 0;

    sensorLevel = 0;
    alarmLevel_L = 100; // Limite mínimo padrão
    alarmLevel_H = 900; // Limite máximo padrão
}

// Funções de retorno e definição de estado
char getState(void) { return state; }
void setState(char newState) { state = newState; }

// Funções de retorno e definição de língua
char getLanguage(void){
	return language;
}
void setLanguage(char newLanguage){
	if((newLanguage == 0)||(newLanguage == 1)||(newLanguage == 2)){
		language = newLanguage;
	}
}

// Funções de retorno e definição do valor do ADC
int getSensorLevel(void) {
	return sensorLevel;
}
void setSensorLevel(int newLevel) {
	sensorLevel = newLevel;
}

// Converte o valor do ADC (0 a 1023) para Volts (0 a 3.3V)
float getSensorLevel_V(void) { return (sensorLevel * 3.3f) / 1023.0f; }

// Funções de definição e retorno do High e Low do alarme
int getAlarmLevel_L(void) { return alarmLevel_L; }
void setAlarmLevel_L(int newLevel) {
	if(newLevel < 0){
		alarmLevel_L = 0;
	}
	else if (newLevel > 1023){
		alarmLevel_L = 1023;
	}
	else{
		alarmLevel_L = newLevel;
	}
}

int getAlarmLevel_H(void) { return alarmLevel_H; }
void setAlarmLevel_H(int newLevel) {
	if(newLevel < 0){
		alarmLevel_H = 0;
	}
	else if (newLevel > 1023){
		alarmLevel_H = 1023;
	}
	else{
		alarmLevel_H = newLevel;
	}
}
