# ⚡ Smart IoT Analytics Dashboard

A modern, high-performance IoT solution featuring a stunning landing page and a real-time sensor dashboard. This project is designed to be **user-friendly for non-developers** using a mobile configuration portal.

## 🚀 Key Features

*   **Premium Home Page**: A beautiful introduction to the project with modern aesthetics.
*   **Real-time Dashboard**: Live charts for Temperature, Humidity, and Distance.
*   **Pause/Resume**: Freeze the data feed at any time to analyze readings.
*   **Smart Mobile Setup**: Non-developers can configure Wi-Fi and Server IP using a phone (no coding required!).

## 📂 Project Structure

```
iot/
├── public/
│   ├── index.html          # NEW: Premium Landing Page
│   └── dashboard.html      # NEW: Real-time Dashboard (with Pause feature)
├── esp32/
│   └── sensor.ino          # NEW: Smart Config ESP32 code (using WiFiManager)
├── server.js               # Node.js backend
└── package.json            # Project dependencies
```

## 🛠️ Setup Instructions

### 1. Start the Website (Developer)
1. Navigate to the project folder:
   ```bash
   cd iot
   npm install
   npm start
   ```
2. The website will be available at `http://localhost:3000`.

### 2. Prepare the Tool (Developer)
1. Open `esp32/sensor.ino` in Arduino IDE.
2. Install these libraries via Library Manager:
   - **WiFiManager** (by tablatronix)
   - **DHT sensor library** (by Adafruit)
3. Upload the code to your ESP32.

### 3. Mobile Setup (For Anyone / Non-Developer)
1. **Power on the tool**: The ESP32 will create its own Wi-Fi network.
2. **Connect via Phone**: Look for a Wi-Fi named **"IoT-Setup-Portal"** and connect.
3. **Configure**: A page will pop up. Enter the local Wi-Fi password and your computer's **Server IP**.
4. **Done**: The tool will restart and start sending data to the website!

## 🔧 Hardware Wiring

| Component | Pin | Note |
| :--- | :---: | :--- |
| **DHT22** | GPIO 4 | Temp & Humidity |
| **Ultrasonic TRIG** | GPIO 5 | Distance |
| **Ultrasonic ECHO** | GPIO 18 | Distance |

---
*Created with passion for modern IoT Architecture.*