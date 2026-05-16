# 🪖 Accident Detection Helmet

A smart helmet device that detects accidents in real-time using motion sensors and triggers an alert system with a manual cancel option.

---

## 🔍 What It Does

When the helmet detects a sudden impact or abnormal motion (indicating an accident), it activates a buzzer alert for 10 seconds. The wearer or a bystander can press a button to cancel the alert within that window — if no one cancels, it signals that help may be needed. Accident data is also transmitted wirelessly to a paired mobile phone via Bluetooth.

---

## 🔧 Components Used

| Component | Purpose |
|---|---|
| ESP32 | Main microcontroller (with built-in Bluetooth) |
| MPU6050 | Accelerometer + Gyroscope to detect impact/fall |
| NEO-6M GPS Module | Tracks location at time of accident |
| Buzzer | Audio alert on accident detection |
| Push Button | Cancel alert within 10-second window |

---

## ⚙️ How It Works

1. The **MPU6050** continuously monitors acceleration and gyroscope data
2. If a sudden impact or abnormal tilt is detected beyond a threshold, an accident is flagged
3. The **buzzer** activates immediately for **10 seconds**
4. A **push button** allows the user to cancel the alert if it's a false alarm
5. If the alert is not cancelled, the **NEO-6M GPS module** logs the location for emergency reference
6. Accident event data (impact detected, GPS coordinates) is transmitted to a paired mobile phone via **Bluetooth (ESP32 built-in)**
7. The system then resets and resumes monitoring

---

## ⚠️ Known Limitations

- **Threshold values for accident detection are not yet calibrated.** The current MPU6050 sensitivity thresholds (acceleration/gyroscope limits used to flag an impact) are placeholder values and may result in false positives or missed detections. Proper calibration based on real-world impact testing is needed before deployment.

---

## 🛠️ Built With

- **Arduino IDE**
- **ESP32 Dev Board**

---

## 🚀 How to Run

1. Clone this repository
2. Open the `.ino` file in Arduino IDE
3. Install required libraries:
   - `MPU6050` by Electronic Cats
   - `TinyGPS++` by Mikal Hart
4. Select **ESP32 Dev Module** as your board
5. Upload and power the device
6. Pair the ESP32 with your mobile phone via Bluetooth to receive accident alerts

---

## 📌 Future Improvements

- Calibrate MPU6050 thresholds through real-world impact testing for reliable detection.
- Send SMS alert with GPS coordinates via Mobile app.
- Mobile app integration.
- Cloud dashboard for fleet/workplace safety monitoring.

---

## 👤 Author

**Abhinav Sharma** — 1st Year ECE Student  
[GitHub Profile](https://github.com/iabhinavsharma)

