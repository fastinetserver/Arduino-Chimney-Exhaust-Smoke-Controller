# CHIMNEY EXHAUST SMOKE CONTROLLER with Arduino and Bluetooth

## Overview
Chimney_Exhaust_Smoke_Controller controles the flow of smoke going through a chimney and therefore adjusts temperature of the water in your Heating System.

## How to use:
1) Turn on Chimney Exhaust Smoke Controller (CESC);
2) Put some wood into your stove;
3) Set up fire;
4) Set up target temperature on your CESC either though BlueTooth or via Encoder;
5) Enjoy the automation.


## Features:
- Controlable by Bluetooth;
- Controlable by Encoder;
- Automatic mode where you define temperature and controller is adjusting postion of the valve;
- Manual mode where you specify the required Valve position (e.g 30% open);

## Components:

- Arduino (nano);
- DS18B20 temperature sensor (connected to the pipe with the hot water from your stove);
- LCD 2x16 screen;
- DC motor driver L298N or H-bridge;
- Geared DC motor JGB37-3530 12V;
- Bluetooth HC-05 or HC-06 module (Optional only if you want to control it via Blootooth);
- Potentiometer to read the position of the Valve;
- 12V power supply (2 Amps worked for my DC motor);
- 5V power supply (optional, I use 5 volts provided by L298N);
- Threaded rod;
- Bearing;
- Nuts;
- 3D Printer.


## License / Copyright 2022 Kostyantyn Ovechko

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.