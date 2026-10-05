/* * File:   var.h
 * Modificações neste fork: Arthur Xavier
 */

#ifndef VAR_H
#define VAR_H

void varInit(void);

// Máquina de Estados e Idioma
char getState(void);
void setState(char newState);
char getLanguage(void);
void setLanguage(char newLanguage);
// Sensor ADC e Alarmes (High e Low)
int getSensorLevel(void);
void setSensorLevel(int newLevel);
float getSensorLevel_V(void); // Retorna a tensão já convertida em Volts

int getAlarmLevel_L(void);
void setAlarmLevel_L(int newLevel);

int getAlarmLevel_H(void);
void setAlarmLevel_H(int newLevel);

#endif /* VAR_H */
