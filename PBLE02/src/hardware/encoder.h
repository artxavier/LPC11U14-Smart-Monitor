/*
 * Modificações neste fork: Arthur Xavier
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "../programa.h"

void encoderInit(void);
char encoderCheckButton(void);
void encoderCheckRotation(void);
void debounce(void);
int encoderGetpos(void);
void encoderReset(void);

#endif /* ENCODER_H */
