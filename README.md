
  

# EduGrid - Maximum Power Point Tracker

  

University project using an Espressif ESP32 controller to build a complete MPP tracker.

Performed at the Technical University of Munich during the winter semester 2023/2024.

  
  
  

## General

The EduGrid project aims to build a documented MPPT that can be used for educational purposes.

The aim is to create a basis on which interested users can understand the principles of voltage, current and power as well as power adjustment.

  

It is designed in such a way that the code is based on libraries and all self-created components are documented via Doxygen.

As the circuit board layout is also provided, nothing stands in the way of replication and further development for your own purposes.

  
  
  

## Supported features of EduGrid Tracker

- Connection of a solar module

- Charging a 12V battery (BMS not included)

- Monitoring of all relevant values on the integrated web server

- WLAN AccessPoint hosting

- Logging of voltage and current on the input and output side in CSV files

- Visualization of the functionality of an MPPT

- use of a MPPT algorithm or manual adjustments by the users

  
  
  

## Hardware

- Espressif ESP32 with 16 MB RAM

- LCD for PWM values and power measurements

- Encoder to varify PWM duty cycle and switching between the modes

- Current/Voltage measurements at IN/output

  
  
  

## Webserver

- Webserver (based on Websocket) to display all measurements and configure the controller

- Connect to WLAN AccessPoint with default credentials

```

SSID: edugrid_access_point

Password: 123456789

```

- Navigate to the web interface

```

http://192.168.1.1/

```

- Configurate WLAN credentials at:

```

http://192.168.1.1/admin

```

- Download logs and manage the file system:

```

http://192.168.1.1/file

```

  
  
  

## Logging

- $I_{IN}$, $I_{OUT}$, $V_{IN}$, $V_{OUT}$

- activate with clicking on LOG field at main webpage

- download log file at:

```

http://192.168.1.1/file

```

- logging time: 15min after activation


## Simulation
- Use simulation data to show the principe of a MPPT
- activate with clicking on SIMU field at main webpage

## Code Design
- Static Methods
- Getter / Setter - Methods
- ***edugrid_*** prefix indicates a self-created functionality

![Program cycle](https://gitlab.com/merhart/tum-edugrid/-/raw/main/propens_edugrid/doc/cycle.png?ref_type=heads&inline=false)
## Functions
### Measurements
- All measurements can be addressed by the class  ***edugrid_measurement***
### Pulse-Width Modulatio
- Set with command:
```
edugrid_pwm_control::setPWM(45);  
edugrid_pwm_control::setPWM(45, false);
edugrid_pwm_control::pwmIncrementDecrement(5); // CW, in abs. %  
edugrid_pwm_control::pwmIncrementDecrement(-5); // CCW, in abs. %
```
- Get frequency:
```
int freq = edugrid_pwm_control::getFrequency();  
int freq_kHz = edugrid_pwm_control::getFrequency_kHz();
```
### LCD
- Init LCD:
```
edugrid_lcd::lcdInit();
```
- Clear complete panel:
```
edugrid_lcd::lcdClear();
```
- Write text to LCD:
```
edugrid_lcd::lcdWriteText(digit, row, text);
```
![LCD](https://gitlab.com/merhart/tum-edugrid/-/raw/61bc85da0079fe8a159408e9309d43ed0c6701b0/propens_edugrid/doc/panel.png?inline=false)

