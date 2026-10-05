/*
 * Modificações neste fork: Arthur Xavier
 */

#include "stateMachine.h"
#include "var.h"
#include "event.h"
#include "output.h"
#include "timer.h"

int modo_edicao = 0;
signed char lang;
unsigned short int resto;

void smInit(void) {
	setState(STATE_TENSAO);
	lang = getLanguage();
}

void smLoop(void) {
	unsigned int evento = eventRead();
	int sensor = getSensorLevel();

	// ------------------------------------
	// 1. DISPARO DO ALARME (Trava o menu)
	// ------------------------------------
	if (sensor > getAlarmLevel_H() || sensor < getAlarmLevel_L()) {
		setState(STATE_DISPARA);
		outputPrint(STATE_DISPARA, getLanguage(), 0);
		return;
	} else if (getState() == STATE_DISPARA) {
		setState(STATE_TENSAO);
	}

	// -----------------------------------------
	// 2. MODO DE EDIÇÃO (Alarme(H/L) e Língua)
	// -----------------------------------------
	if (evento == EV_ENTER) {
		if (getState() == STATE_ALARME) {
			if (modo_edicao == 0) {
				modo_edicao = 1;      // 1º Clique: Edita Low
			} else if (modo_edicao == 1) {
				modo_edicao = 2;      // 2º Clique: Edita High
			} else {
				modo_edicao = 0;      // 3º Clique: Grava no E2PROM e Sai

				EEPROM_EscreverByte(0x0010, (getAlarmLevel_L() >> 8) & 0xFF);
				delayMS(10);
				EEPROM_EscreverByte(0x0011, getAlarmLevel_L() & 0xFF);
				delayMS(10);

				EEPROM_EscreverByte(0x0012, (getAlarmLevel_H() >> 8) & 0xFF);
				delayMS(10);
				EEPROM_EscreverByte(0x0013, getAlarmLevel_H() & 0xFF);
				delayMS(10);
			}
		} else if (getState() == STATE_IDIOMA) {
			modo_edicao = !modo_edicao; // Liga/Desliga edição de idioma

			// Grava idioma na E2PROM
			if (modo_edicao == 0) {
				EEPROM_EscreverByte(0x0014, getLanguage());
			}
		} else {
			// Fora do modo de edição
			modo_edicao = 0;
		}
	}

	// ------------------------------
	// 3. MODO DE NAVEGAÇÃO DE TELAS
	// ------------------------------
	if (modo_edicao == 0) {
		if (evento == EV_HOME) { // Botão que volta pra HOME
			setState(STATE_TENSAO);
		} else if (evento == EV_RIGHT) { // Botão que passa pra próxima tela
			int prox = getState() + 1;
			if (prox >= STATE_DISPARA)
				prox = STATE_TENSAO;
			setState(prox);
		} else if (evento == EV_LEFT) { // Botão que passa pra tela anterior
			int ant = getState() - 1;
			if (ant < 0)
				ant = STATE_IDIOMA;
			setState(ant);
		}
	}
	// ---------------------------------
	// 4. MODO EDIÇÃO (Giro de Encoder)
	// ---------------------------------
	else {
		if (evento == EV_HOME) {
			modo_edicao = 0;
			setState(STATE_TENSAO);
		}

		if (getState() == STATE_ALARME) {

			if (modo_edicao == 1) { // Editando o Low
				if (evento == EV_ENC_CW) {
					if ((getAlarmLevel_L() % 5) != 0) {
						resto = getAlarmLevel_L() % 5;
						setAlarmLevel_L(getAlarmLevel_L() + (5 - resto));
					} else {
						setAlarmLevel_L(getAlarmLevel_L() + 5);
					}
				}

				if (evento == EV_ENC_CCW) {
					if ((getAlarmLevel_L() % 5) != 0) {
						resto = getAlarmLevel_L() % 5;
						setAlarmLevel_L(getAlarmLevel_L() - resto);
					} else {
						setAlarmLevel_L(getAlarmLevel_L() - 5);
					}
				}
			}

			else if (modo_edicao == 2) { // Editando o High
				if (evento == EV_ENC_CW) {
					if ((getAlarmLevel_H() % 5) != 0) {
						resto = getAlarmLevel_H() % 5;
						setAlarmLevel_H(getAlarmLevel_H() + (5 - resto));
					} else {
						setAlarmLevel_H(getAlarmLevel_H() + 5);
					}
				}

				if (evento == EV_ENC_CCW) {
					if ((getAlarmLevel_H() % 5) != 0) {
						resto = getAlarmLevel_H() % 5;
						setAlarmLevel_H(getAlarmLevel_H() - resto);
					} else {
						setAlarmLevel_H(getAlarmLevel_H() - 5);
					}
				}
			}
		} else if (getState() == STATE_IDIOMA) { // Editando o Idioma
			if (evento == EV_ENC_CW) {
				lang++;
				if (lang > 2)
					lang = 0;
			} else if (evento == EV_ENC_CCW) {
				lang--;
				if (lang == -1)
					lang = 2;

			}
			setLanguage(lang);
		}
	}

	// Atualiza o Display
	outputPrint(getState(), getLanguage(), modo_edicao);
}
