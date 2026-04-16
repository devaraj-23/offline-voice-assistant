# 🔊 Offline Voice Assistant for Home Appliances

### 📌 Overview
This project presents an **offline voice-controlled home automation system** using the ESP32. It allows users to control home appliances through voice commands without requiring an internet connection, ensuring **privacy, low latency, and high reliability.**

---

### 🎯 Features
* **📴 100% Offline:** No internet or cloud dependency required.
* **🎤 Voice Recognition:** High-fidelity input via I2S microphone.
* **⚡ Real-time Control:** Instant appliance switching.
* **📡 ESP-NOW Protocol:** Low-power, high-speed communication between nodes.
* **🔒 Privacy-Focused:** Voice data never leaves the local device.
* **📺 Visual Feedback:** Real-time status updates via OLED display.

---

### 🛠️ Hardware Stack
| Component | Role |
| :--- | :--- |
| **ESP32** | Main Transmitter / Speech Processing |
| **ESP8266 (ESP-01)** | Remote Receiver Node |
| **INMP441** | I2S Digital Microphone |
| **Relay Module** | Appliance Switching (High Voltage) |
| **0.96" OLED** | I2C Display for System Status |
| **Power Supply** | 5V/2A DC Adapter |

---

### 💻 Software & Technologies
* **Framework:** Embedded C (ESP-IDF)
* **Speech Engine:** [ESP-Skainet](https://github.com/espressif/esp-skainet) (Offline Wake-word & Command Recognition)
* **Communication:** ESP-NOW (Connectionless Data Transfer)
* **Development:** Arduino IDE (for ESP-01) & VS Code (for ESP32)

---

### ⚙️ Working Principle
1.  **Capture:** The **INMP441** captures acoustic signals and sends them to the ESP32 via the I2S interface.
2.  **Processing:** The **ESP-Skainet** library running on the ESP32 performs keyword spotting and command recognition.
3.  **Transmission:** Once a command is identified, the ESP32 broadcasts a trigger signal via **ESP-NOW**.
4.  **Action:** The **ESP-01** receives the packet and toggles the **Relay Module**.
5.  **Feedback:** The **OLED** provides visual confirmation of the recognized command.

---

### 📂 Project Structure
```text
Offline-Voice-Assistant/
├── en_speech_commands_recognition/      # ESP32 Source (ESP-IDF)
├── Receiver_code/         # ESP8266 Source (Arduino)
├── Images/                  # Circuit diagrams & Schematics
└── README.md              # Project Documentation
```

---

### 🚀 Getting Started

1.  **Flash the Transmitter:** Use ESP-IDF to flash the code onto the **ESP32**.
2.  **Flash the Receiver:** Use Arduino IDE to upload the logic to the **ESP-01**.
3.  **Wiring:** Follow the schematics in the `/docs` folder for I2S and I2C pin mapping.
4.  **Commands:** Try saying:
    * *"Light ON"*
    * *"Light OFF"*

---

### 🔮 Future Improvements
* [ ] Integration of noise-cancellation algorithms (AEC/NS).
* [ ] Support for custom wake-word training.
* [ ] Multi-node mesh control for whole-house coverage.

---

### 👨‍💻 Author
**Devaraj M**
*B.E. Instrumentation and Control Engineering*
*PSG College of Technology*

---
> **License:** This project is for academic and learning purposes.
