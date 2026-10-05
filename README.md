# Smart Sensor Monitoring & Alarm System (LPC11U14)

![Completed LPC11U14 monitoring prototype](images/board-finished.jpeg)

> **Academic Note:** This project was developed as part of the **PBLE02 - Board Bring Up and Electronic Prototype Validation** course, instructed by Professor [Rodrigo Maximiano](@rmaalmeida). The course provided a hands-on "from scratch" engineering experience: we received a bare PCB and loose components, requiring us to manually solder the board, perform physical hardware validation, and troubleshoot electrical issues before developing the firmware. The software utilizes base hardware abstraction libraries provided by [Osmar Bruno] (@BRun0442) [Anunciador de Alarmes] (https://github.com/BRun0442/Anunciador-de-alarmes).
## 🎥 Video Demonstration

[![Watch the demonstration video](images/board-finished.jpeg)](images/demo.mp4)

[▶ Watch the demonstration video (MP4, 1:37, no audio)](images/demo.mp4)

The video shows the completed board, LCD interface, physical controls, and serial communication with a PC.

---

## 📐 Architecture & UML

**Note:** The UML diagrams below are symbolic representations of the firmware modules and control flow. This project is written in C and does not use object-oriented classes or inheritance.

The project is structured into logical layers (Strict MVC) to guarantee maintainability and data shielding:

1. **Hardware Abstraction Layer (HAL):** `I2C.c`, `e2prom.c`, `Teclado.c`, and `Serial.c` interact directly with the LPC11U14 silicon registers.
2. **Event Router (Input Manager):** `event.c` centralizes and processes both physical clicks and Serial port strings.
3. **Controller (State Machine):** `stateMachine.c` evaluates events, executes business logic, and orchestrates EEPROM cycles.
4. **Model & View:** `var.c` acts as the Model, safely storing runtime data, while `output.c` acts as the View, consuming this data to render the LCD and drive the LEDs.

### Class Diagram

```mermaid
classDiagram
    namespace Hardware_Drivers {
        class Serial_UART {
            <<Driver>>
        }
        class Teclado {
            <<IO>>
        }
        class Encoder {
            <<IO>>
        }
        class E2PROM {
            <<Memoria_I2C>>
        }
        class LCD {
            <<Periferico>>
        }
        class LED {
            <<Periferico>>
        }
    }

    namespace Controle {
        class Event {
            <<Input_Router>>
            -unsigned int key_ant
            -unsigned int keyenc_ant
            -int enc_pos_ant
            +eventInit()
            +eventRead() unsigned int
            +processaSerial() unsigned int
        }
        class StateMachine {
            <<Controller>>
            +int modo_edicao
            +signed char lang
            +smInit()
            +smLoop()
        }
    }

    namespace Modelo_e_Visao {
        class Output {
            <<View>>
            -char* msgs
            +outputInit()
            +outputPrint(numTela, idioma, editando)
        }
        class Var {
            <<Model>>
            +getSensorLevel() int
            +getAlarmLevel_H() int
            +getAlarmLevel_L() int
            +setLanguage(val)
        }
    }

    Serial_UART --> Event : Manda comandos seriais
    Teclado --> Event : Manda cliques para navegacao
    Encoder --> Event : Manda clique e giro
    Event --> StateMachine : Retorna eventos EV_ENTER e outros
    StateMachine --> Output : Pede atualizacao da tela
    StateMachine --> E2PROM : Grava parametros via I2C
    StateMachine --> Var : Le e atualiza dados
    Output --> Var : Consulta valores para exibir
    Output --> LCD : Renderiza tela
    Output --> LED : Aciona indicadores
```

### State Machine

```mermaid
stateDiagram-v2
    direction TB

    [*] --> TENSAO: smInit()

    TENSAO --> STATE_ALARME: EV_RIGHT
    STATE_ALARME --> TENSAO: EV_LEFT [modo=0]
    STATE_ALARME --> STATE_IDIOMA: EV_RIGHT [modo=0]
    STATE_IDIOMA --> STATE_ALARME: EV_LEFT [modo=0]
    STATE_IDIOMA --> TENSAO: EV_RIGHT [modo=0]
    TENSAO --> STATE_IDIOMA: EV_LEFT
    STATE_ALARME --> TENSAO: EV_HOME
    STATE_IDIOMA --> TENSAO: EV_HOME

    state STATE_ALARME {
        [*] --> Leitura_Alarme
        Leitura_Alarme --> Edita_L: EV_ENTER / modo=1
        Edita_L --> Edita_H: EV_ENTER / modo=2
        Edita_H --> Leitura_Alarme: EV_ENTER / grava limites na EEPROM e modo=0
        Edita_L --> Edita_L: EV_ENC_CW ou EV_ENC_CCW
        Edita_H --> Edita_H: EV_ENC_CW ou EV_ENC_CCW
    }

    state STATE_IDIOMA {
        [*] --> Leitura_Idioma
        Leitura_Idioma --> Edita_Lang: EV_ENTER / modo=1
        Edita_Lang --> Leitura_Idioma: EV_ENTER / grava idioma na EEPROM e modo=0
        Edita_Lang --> Edita_Lang: EV_ENC_CW ou EV_ENC_CCW
    }

    note right of TENSAO
        UART getsensor responde com ADC e tensao
        e seleciona esta tela.
    end note
    note right of STATE_ALARME
        UART getalarmelow e getalarmehigh consultam limites.
        setalarmelow:value e setalarmehigh:value alteram
        um limite e simulam EV_ENTER para gravar ambos.
    end note
    note right of STATE_IDIOMA
        UART getlanguage consulta o idioma.
        setlanguage:value altera o idioma e simula
        EV_ENTER para gravar na EEPROM.
    end note

    TENSAO --> STATE_DISPARA: ADC acima de H ou abaixo de L
    STATE_ALARME --> STATE_DISPARA: ADC acima de H ou abaixo de L
    STATE_IDIOMA --> STATE_DISPARA: ADC acima de H ou abaixo de L
    STATE_DISPARA --> TENSAO: ADC normalizado
```

The nested editing nodes represent the `modo_edicao` flag, not separate values in the state enum. `TENSAO` represents `STATE_TENSAO`; `STATE_FIM` is an enum sentinel. The UART notes show the commands handled by `event.c`; a SET command saves to EEPROM only when the alarm check lets the simulated ENTER event run. In the current implementation, an alarm does not clear `modo_edicao`; press HOME after the reading returns to normal if editing was interrupted. `getlanguage` does not yet include a French reply.

---

## 🛠️ Hardware Setup

* **Microcontroller:** NXP LPC11U14 (ARM Cortex-M0)
* **External Memory:** 24LC512 EEPROM (I2C Address `0x50`)
* **Human-Machine Interface:** LCD Display, Rotary Encoder, and Push Buttons
* **Communication:** UART Interface (Serial via PC)

### Assembly Stages

1. **Power supply test:** The PCB connected to a bench supply before the microcontroller and user interface were installed. The lit LED shows this early electrical check.

   ![PCB during the power supply test](images/board-power-test.jpeg)

2. **Intermediate assembly:** The microcontroller and part of the supporting circuitry are mounted, while the display and remaining controls are still absent.

   ![PCB during intermediate assembly](images/board-assembly.jpeg)

3. **Completed prototype:** The LCD, push buttons, encoder, and other components are mounted on the board.

   ![Completed LPC11U14 prototype](images/board-finished.jpeg)

### Circuit Diagram

The circuit schematic can be added here when it is available.

---

## 🚀 Key Features

* **Resilient State Machine:** Menu navigation and parameter editing managed by a central State Machine, with automatic UI lockout during alarms.
* **Anti-Lockup I2C Driver:** Custom driver supporting timeouts and emergency bus release. Tolerant to physical connection failures.
* **Data Persistence (EEPROM):** Communicates with a 24LC512 chip to store alarm thresholds and language preferences. Includes silicon burn-time compensation (10ms).
* **Hybrid Interface (HMI & Serial):** Operable physically (Encoder/Buttons/LCD) or remotely (UART).
* **Mathematical Auto-Snapping:** Fine-tuning the alarm thresholds via the encoder snaps to multiples of 5 using modulo arithmetic.

---

## 💻 Serial Interface Commands

The system supports remote operation via a Serial terminal (e.g., PuTTY, Tera Term). Commands are case-insensitive.

**GET Commands (Read):**
* `getalarmelow` - Returns the current minimum voltage threshold.
* `getalarmehigh` - Returns the current maximum voltage threshold.
* `getsensor` - Returns the current ADC value and the converted voltage.
* `getlanguage` - Returns the current system language (Portuguese, English, French).

**SET Commands (Write & Physical Save):**
* `setalarmelow:[value]` - Sets a new minimum threshold, updates UI, and writes to EEPROM. (e.g., `setalarmelow:105`)
* `setalarmehigh:[value]` - Sets a new maximum threshold and saves it to EEPROM.
* `setlanguage:[0, 1 or 2]` - Changes global display language and saves to non-volatile memory.

---

## ⚙️ Build and Run

1. Clone this repository:
   ```bash
   git clone [https://github.com/SEU_USUARIO/LPC11U14-Smart-Monitor.git](https://github.com/SEU_USUARIO/LPC11U14-Smart-Monitor.git)
