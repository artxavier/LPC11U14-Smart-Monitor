# Smart Sensor Monitoring & Alarm System (LPC11U14)

<!-- Placeholder para um banner ou foto real da placa rodando -->
![Project Cover](docs/images/cover_photo.jpg)

> **Academic Note:** This project was developed as part of the **PBLE02 - Board Bring Up and Electronic Prototype Validation** course, instructed by Professor [Rodrigo Maximiano](LINK_TO_PROFILE). The course provided a hands-on "from scratch" engineering experience: we received a bare PCB and loose components, requiring us to manually solder the board, perform physical hardware validation, and troubleshoot electrical issues before developing the firmware. The software utilizes base hardware abstraction libraries provided by the [Osmar Bruno repository](LINK_TO_REPO).
## 🎥 Video Demonstration

<!-- Placeholder para um GIF do projeto ou uma thumbnail clicável que leva para o YouTube -->
[![Watch the video](docs/images/video_thumbnail.png)](https://www.youtube.com/watch?v=SEU_LINK_AQUI)
*Click the image above to watch the system in action.*

---

## 📐 Architecture & UML

The project is structured into logical layers (Strict MVC) to guarantee maintainability and data shielding:

1. **Hardware Abstraction Layer (HAL):** `I2C.c`, `e2prom.c`, `Teclado.c`, and `Serial.c` interact directly with the LPC11U14 silicon registers.
2. **Event Router (Input Manager):** `event.c` centralizes and processes both physical clicks and Serial port strings.
3. **Controller (State Machine):** `stateMachine.c` evaluates events, executes business logic, and orchestrates EEPROM cycles.
4. **Model & View:** `var.c` acts as the Model, safely storing runtime data, while `output.c` acts as the View, consuming this data to render the LCD and drive the LEDs.

### State Machine (UML)

<!-- Placeholder para o diagrama UML da Máquina de Estados -->
![UML State Machine Diagram](docs/images/uml_diagram.png)
*UML diagram illustrating the core state machine transitions between voltage monitoring, alarm configuration, and language selection.*

---

## 🛠️ Hardware Setup

* **Microcontroller:** NXP LPC11U14 (ARM Cortex-M0)
* **External Memory:** 24LC512 EEPROM (I2C Address `0x50`)
* **Human-Machine Interface:** LCD Display, Rotary Encoder, and Push Buttons
* **Communication:** UART Interface (Serial via PC)

### Circuit Diagram

<!-- Placeholder para o esquemático elétrico do Proteus, KiCad, etc. -->
![Circuit Schematic](docs/images/schematic.png)
*Hardware wiring schematic showing I2C bus connections and pull-up resistors.*

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