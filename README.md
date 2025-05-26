# Smart Aquarium Temperature Control System (ESP32 + ThingSpeak)

This project uses an **ESP32** to automatically control a **fan based on temperature**, and periodically sends sensor data to **ThingSpeak**. It utilizes a **Dallas DS18B20 temperature sensor**, a **relay module**, and **Wi-Fi** for data transmission.

![Başlıksız](https://github.com/user-attachments/assets/707cb132-148b-4c97-aeff-bf70f1857f05)

## Features

* **Automatic fan control**:
  If the temperature exceeds **22.5°C**, the fan turns on. Below that, it turns off.

* **ThingSpeak integration**:
  Temperature data is sent to **Field 2**, and fan status (on/off) to **Field 3**.

* **Wi-Fi recovery**:
  If Wi-Fi disconnects, the system attempts to reconnect or **restarts ESP32** after a timeout.

* **Power saving mode**:
  When the fan is off, the ESP32 enters **deep sleep mode** for **2 minutes**, reducing power usage.

## Hardware Used

* ESP32
* DS18B20 temperature sensor
* Relay module
* Fan
* Wi-Fi connection
  
## Notes

* If the sensor fails to respond (returns -127°C), the ESP32 will automatically reboot.
* Wi-Fi and ThingSpeak credentials should be stored in a separate `secrets.h` file.

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
