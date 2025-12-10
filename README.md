# 💡 ESP32 Voice-Controlled Door Lock

An IoT project that allows you to unlock your door using voice commands via Google Assistant. This system integrates cloud services with an ESP32 microcontroller to control a servo motor or solenoid lock remotely.

---

## 🛠️ Frameworks & Technologies

This project uses the following technologies and hardware ecosystem:

* **Voice Control:** Google Assistant / Google Home
* **Development Platform:** Arduino IDE
* **Microcontroller:** ESP32 (or you can use ESP8266 instead)
* **Middleware:** IFTTT (If This Then That)
* **IoT Cloud:** Adafruit IO / Blynk (configurable)
* **Connectivity:** Wi-Fi

---

## ⚙️ The Architecture

### How it Works
Because Google Assistant runs in the cloud, it cannot communicate directly with the ESP32 board on your local network. 

### The Flow
Here is the step-by-step process of how the command travels from your voice to the physical lock:

**Trigger:** You say, *"Hey Google, Open the Door."*

1.  **Google Assistant:** Captures the voice command and sends a webhook/trigger to **IFTTT**.
2.  **IFTTT:** Receives the trigger and sends a signal (updates a value) to the **IoT Cloud** (e.g., Adafruit IO Feed).
3.  **ESP32:** The microcontroller is constantly subscribed to the IoT Cloud feed. When it detects the specific "OPEN" signal:
4.  **Hardware Action:** The ESP32 sends a PWM signal to the **Servo Motor** (to rotate) or a HIGH signal to the **Solenoid Lock** (to pull back), effectively unlocking the door.

---

## 🔌 Hardware Requirements

| Component | Description |
| :--- | :--- |
| **ESP32** | The main controller handling Wi-Fi and logic. |
| **Servo Motor** | (SG92 or SG92R) Used to physically turn the latch. |
| **5V Relay** | (If using a Solenoid) To switch the high current for the lock. |
| **Power Supply** | 5V USB or External Battery for the ESP32/Motor. |
| **Jumper Wires** | For connections. |
| **Custom Lever Arm** | To create a folding mechanism. |

---


### 2. IFTTT Configuration
1.  Create a new Applet.
2.  **IF:** Select **Google Assistant** -> "Say a simple phrase" (e.g., "Open the door").
3.  **THEN:** Select **Adafruit IO** -> "Send data to Feed" -> Send value `1` or `ON`.

### 3. Firmware (Arduino IDE)
1.  Install the required libraries (e.g., `Adafruit_MQTT`, `WiFi`).
2.  Update the code with your Wi-Fi credentials and API Keys.
3.  Upload the code to the ESP32.

---


## 🤝 Contributing

3 Partners from UIT