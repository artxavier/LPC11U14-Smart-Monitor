/*
 * Modificações neste fork: Arthur Xavier
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

enum {
    STATE_TENSAO,
    STATE_ALARME,       // Tela ÚNICA para os dois alarmes!
    STATE_IDIOMA,
    STATE_DISPARA,
    STATE_FIM
};

extern int modo_edicao;

void smInit(void);
void smLoop(void);

#endif /* STATEMACHINE_H */
