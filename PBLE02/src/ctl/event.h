/*
 * Modificações neste fork: Arthur Xavier
 */

#ifndef EVENT_H
#define EVENT_H

enum {
    EV_HOME,
    EV_LEFT,
    EV_RIGHT,
    EV_ENTER,
    EV_ENC_CW,
    EV_ENC_CCW,
    EV_NOEVENT
};

void eventInit(void);
unsigned int eventRead(void);

#endif /* EVENT_H */
