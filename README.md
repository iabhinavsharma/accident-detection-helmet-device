# 🪖 Accident Detection Helmet

A smart helmet device that detects accidents in real-time using motion sensors and triggers an alert system with a manual cancel option.

---

## 🔍 What It Does

When the helmet detects a sudden impact or abnormal motion (indicating an accident), it activates a buzzer alert for 10 seconds. The wearer or a bystander can press a button to cancel the alert within that window — if no one cancels, it signals that help may be needed.

---

## 🔧 Components Used

| Component | Purpose |
|---|---|
| ESP32 | Main microcontroller |
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
6. The system then resets and resumes monitoring

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

---

## 📌 Future Improvements

- Send SMS alert with GPS coordinates via Mobile app.
- Mobile app integration.
- Cloud dashboard for fleet/workplace safety monitoring

---

## 👤 Author

**Abhinav Sharma** — 1st Year ECE Student  
[GitHub Profile](https://github.com/iabhinavsharma)

