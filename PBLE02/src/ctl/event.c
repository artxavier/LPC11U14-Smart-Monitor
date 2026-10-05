/*
 * Modificações neste fork: Arthur Xavier
 */

#include <string.h>
#include "../hardware/Teclado.h"
#include "../hardware/encoder.h"
#include "../hardware/Serial.h"
#include "../util.h"
#include "event.h"
#include "var.h"
#include "stateMachine.h"

static unsigned int key_ant;
static unsigned int keyenc_ant;
static int enc_pos_ant;
extern int modo_edicao; //variavel externa vinda de stateMachine.c

void eventInit(void) {
    iniciaTeclado();
    encoderInit();
    key_ant = 0;
    keyenc_ant = 0;
    enc_pos_ant = encoderGetpos();
}


// ----------------------------------
// PROCESSADOR DE COMANDOS DA SERIAL
// ----------------------------------
unsigned int processaSerial(void) {
    char comando_recebido[32];

    // Verifica se chegou um comando na serial
    if (serial_TemComandoPronto(comando_recebido)) {

        // 1. Converte para minúsculas
        char lower_cmd[32];
        int k = 0;
        while(comando_recebido[k] != '\0' && k < 31) {
            char c = comando_recebido[k];
            if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
            lower_cmd[k] = c;
            k++;
        }
        lower_cmd[k] = '\0';

        // 2. Comandos GET (Mostra os valores atuais de Alarme(H/L), Língua e Sensor)
        if (strncmp(lower_cmd, "get", 3) == 0) {
            char *cmd_body = lower_cmd + 3;

            if (strcmp(cmd_body, "alarmelow") == 0) {
                serial_enviaString("\r\n#alarmeLow=");
                serial_enviaInteiro(getAlarmLevel_L());
                serial_enviaString("#\r\n");
                setState(STATE_ALARME);
            }
            else if (strcmp(cmd_body, "alarmehigh") == 0) {
                serial_enviaString("\r\n#alarmeHigh=");
                serial_enviaInteiro(getAlarmLevel_H());
                serial_enviaString("#\r\n");
                setState(STATE_ALARME);
            }
            else if (strcmp(cmd_body, "language") == 0) {
                serial_enviaString("\r\n#Language=");
                char lang = getLanguage();
                if(lang == 0){
                    serial_enviaString("PORTUGUES");
                }
                else if(lang == 1){
                    serial_enviaString("ENGLISH");
                }
                serial_enviaString("#\r\n");
                setState(STATE_IDIOMA);
            }
            else if (strcmp(cmd_body, "sensor") == 0) {
                char str_val[8];
                floatParaString(getSensorLevel_V(), str_val);

                serial_enviaString("\r\n#sensor=");
                serial_enviaInt4Dig(getSensorLevel());
                serial_enviaString("/");
                serial_enviaString(str_val);
                serial_enviaString("V#\r\n");
                setState(STATE_TENSAO);
            }

            return EV_NOEVENT; // Finaliza o GET
        }

        // 3. Comandos SET (Alteram os valores de Alarme(H/L) e Língua)
        // Comandos: (setalarmehigh:---), (setalarmelow:---), (setlanguage:0/1).
        else if (strncmp(lower_cmd, "set", 3) == 0) {
            char *cmd_body = lower_cmd + 3;

            if (strncmp(cmd_body, "alarmelow:", 10) == 0) {
                int valor = 0;
                int i = 10;
                while (cmd_body[i] >= '0' && cmd_body[i] <= '9') {
                    valor = (valor * 10) + (cmd_body[i] - '0');
                    i++;
                }
                setAlarmLevel_L(valor);
                setState(STATE_ALARME);
                serial_enviaString("\r\n-> Alarme L (Min) Atualizado!\r\n");

                modo_edicao = 2; // Força o modo de edição 2 pra no stateMachine.c ele gravar na memória
                return EV_ENTER; // Simula o aperto de botão
            }
            else if (strncmp(cmd_body, "alarmehigh:", 11) == 0) {
                int valor = 0;
                int i = 11;
                while (cmd_body[i] >= '0' && cmd_body[i] <= '9') {
                    valor = (valor * 10) + (cmd_body[i] - '0');
                    i++;
                }
                setAlarmLevel_H(valor);
                setState(STATE_ALARME);
                serial_enviaString("\r\n-> Alarme H (Max) Atualizado!\r\n");

                modo_edicao = 2; // Força o modo de edição 2 pra no stateMachine.c ele gravar na memória
                return EV_ENTER; // Simula o aperto de botão
            }
            else if (strncmp(cmd_body, "language:", 9) == 0) {
                int valor = 0;
                int i = 9;
                while (cmd_body[i] >= '0' && cmd_body[i] <= '9') {
                    valor = (valor * 10) + (cmd_body[i] - '0');
                    i++;
                }
                setLanguage(valor);
                setState(STATE_IDIOMA);
                serial_enviaString("\r\n-> Lingua Atualizada!\r\n");

                modo_edicao = 1; // Força o modo de edição 1 pra no stateMachine.c ele gravar na memória
                return EV_ENTER; // Simula aperto de botão
            }
        }
    }

    // 4. RETORNO PARA CASO NADA OCORRA NA SERIAL
    return EV_NOEVENT;
}

// -------------------------
// LEITURA GERAL DE EVENTOS
// -------------------------
unsigned int eventRead(void) {
    int key = checkButton();
    int keyenc = encoderCheckButton();
    int enc_pos = encoderGetpos();
    int ev = EV_NOEVENT;

    // 1. VERIFICA COMANDO NA SERIAL
    unsigned int ev_serial = processaSerial();
    if (ev_serial != EV_NOEVENT) {
        return ev_serial; // Retorna o evento para tratar o comando da serial
    }

    // 2. EVENTOS DO GIRO DO ENCODER PARA AJUSTE DE CONFIGURAÇÃO DE ALARME E LÍNGUA
    if (enc_pos > enc_pos_ant) {
        ev = EV_ENC_CW;
    }
    else if (enc_pos < enc_pos_ant) {
        ev = EV_ENC_CCW;
    }
    enc_pos_ant = enc_pos;

    // 3. EVENTOS DOS BOTÕES FÍSICOS PARA NAVEGAÇÃO NO MENU
    if (ev == EV_NOEVENT && key != key_ant) {
        if (key & (1 << 0)) ev = EV_RIGHT;
        else if (key & (1 << 1)) ev = EV_LEFT;
        else if (key & (1 << 2)) ev = EV_HOME;
    }

    // 4. EVENTO DO CLIQUE (BOTÃO DO ENCODER)
    if (keyenc != keyenc_ant) {
        if (keyenc == 1) ev = EV_ENTER;
    }

    key_ant = key;
    keyenc_ant = keyenc;

    return ev;
}
