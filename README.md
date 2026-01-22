# iot_based_water_problem_solution_that_sends_update_msg_to_author_by_telegram
# 🌊 Water Quality Monitoring System (ESP32)

A **complete IoT-based Water Quality Monitoring System** using **ESP32**, featuring:

* 🌐 **Real-time Web Dashboard** (No Blynk)
* 🤖 **Telegram Alert System**
* 💧 **Turbidity Monitoring**
* 📊 **TDS (Total Dissolved Solids) Measurement**
* 📏 **Water Level Monitoring (Ultrasonic Sensor)**
* 🚨 **LED & Buzzer Alerts**

This project is suitable for **academic projects, research, smart campus, hostel, or water tank monitoring systems**.

---

## 📌 Features

* ESP32-based standalone system
* Built-in **HTTP Web Server**
* Mobile & PC friendly dashboard
* Automatic **Telegram notifications** on alert conditions
* Median filtering for stable TDS readings
* No third-party cloud (No Blynk, No Firebase)
* Real-time serial monitoring

---

## 🛠️ Hardware Requirements

| Component                   | Quantity    |
| --------------------------- | ----------- |
| ESP32 Dev Module            | 1           |
| Turbidity Sensor            | 1           |
| TDS Sensor                  | 1           |
| Ultrasonic Sensor (HC-SR04) | 1           |
| Red LED                     | 1           |
| Green LED                   | 1           |
| Buzzer                      | 1           |
| Jumper Wires                | As required |
| Breadboard / PCB            | Optional    |
| 5V / USB Power Supply       | 1           |

---

## 🔌 Pin Configuration

| Sensor / Module  | ESP32 Pin |
| ---------------- | --------- |
| Turbidity Sensor | GPIO 34   |
| TDS Sensor       | GPIO 33   |
| Ultrasonic TRIG  | GPIO 23   |
| Ultrasonic ECHO  | GPIO 22   |
| Red LED          | GPIO 25   |
| Green LED        | GPIO 26   |
| Buzzer           | GPIO 21   |

---

## 🌐 Software & Libraries

* **Arduino IDE** (Latest version recommended)
* ESP32 Board Package

### Required Libraries (Install via Library Manager)

```text
WiFi.h
WebServer.h
HTTPClient.h
```

> All libraries are default ESP32 libraries (no external dependency).

---

## ⚙️ Configuration

### 1️⃣ WiFi Credentials

Edit in code:

```cpp
const char* wifi_ssid = "YOUR_WIFI_NAME";
const char* wifi_password = "YOUR_WIFI_PASSWORD";
```

---

### 2️⃣ Telegram Bot Setup

1. Open Telegram
2. Search **@BotFather**
3. Create a bot and copy **BOT TOKEN**
4. Get your **CHAT ID** using @userinfobot

Update in code:

```cpp
const char* telegramBotToken = "YOUR_BOT_TOKEN";
const char* telegramChatID = "YOUR_CHAT_ID";
```

---

## 📊 Web Dashboard

* After uploading code, open **Serial Monitor**
* Copy the ESP32 IP address
* Open in browser:

```text
http://ESP32_IP_ADDRESS/
```

### Dashboard Shows:

* Turbidity (NTU)
* TDS (ppm)
* Water Level (cm)
* Sensor Voltages
* System Alert Status
* Live Update Every 2 Seconds

---

## 🚨 Alert Conditions

### Turbidity Alerts

| Voltage Range | Status         |
| ------------- | -------------- |
| > 2.10V       | Normal         |
| 2.00 – 2.10V  | Slightly Dirty |
| 1.50 – 2.00V  | Polluted       |
| < 1.50V       | Emergency      |

### Water Level Alerts

| Distance (cm) | Status       |
| ------------- | ------------ |
| ≤ 10 cm       | Normal       |
| 10 – 13 cm    | Slightly Low |
| 13 – 16 cm    | Very Low     |
| > 16 cm       | Emergency    |

---

## 🔔 Alert Actions

When alert occurs:

* 🔴 Red LED ON
* 🟢 Green LED OFF
* 🔊 Buzzer ON
* 📩 Telegram message sent

When normal:

* 🟢 Green LED ON
* 🔴 Red LED OFF
* 🔕 Buzzer OFF

---

## 🧪 Serial Monitor Output

* Sensor voltages
* NTU & TDS values
* Water level distance
* WiFi status
* Alert status

Updates every **2 seconds**.

---

## 🚀 How to Upload

1. Open Arduino IDE
2. Select **Board → ESP32 Dev Module**
3. Select correct COM Port
4. Paste the code
5. Click **Upload**
6. Open Serial Monitor (115200 baud)

---

## 📚 Applications

* Smart Water Tank Monitoring
* Hostel / Campus Water Quality
* Smart City Projects
* Academic Research
* IoT Demonstrations

---

## ⚠️ Notes

* Calibrate Turbidity & TDS sensors before real deployment
* Ultrasonic sensor distance depends on tank size
* Telegram alerts may repeat if condition remains active

---

## 🧑‍💻 Author

**Shamem Miah**
ESP32 | IoT | Embedded Systems | MERN Stack

---

## 📜 License

This project is open-source and free to use for **educational and research purposes**.

---

⭐ If you like this project, don't forget to **star the repository**!
