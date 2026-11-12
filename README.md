Smart LPG Gas Monitoring System Using ESP32

Project Overview

An IoT-based LPG monitoring system using ESP32 to estimate the remaining gas quantity, calculate daily consumption and predict the approximate cylinder empty date. The system displays information on an LCD and sends refill alerts through a Telegram bot.

Features

LPG cylinder weight monitoring.

Remaining gas quantity estimation.

Daily consumption calculation.

Predicted cylinder empty date.

Real-time LCD display.

Telegram notifications for low LPG levels.

Hardware Requirements

ESP32 Development Board

Load Cell

HX711 Amplifier

LCD Display

Connecting Wires

Power Supply

Software Requirements

Arduino IDE

Embedded C/C++

ESP32 Board Support Package

Telegram Bot API

Required sensor and LCD libraries

Working Principle

The load cell measures the cylinder weight.

ESP32 processes the sensor readings.

Remaining LPG quantity is estimated using the cylinder tare weight.

Daily consumption is calculated from recorded measurements.

The expected empty date is predicted.

Information is displayed on the LCD.

Telegram sends alerts when the LPG level becomes low.

Technologies Used

ESP32

Embedded C/C++

Arduino IDE

Wi-Fi

Telegram Bot API

Project Status

In Progress

Safety Not
