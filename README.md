# Light Based Toggle

A small educational project that toggles an output based on light conditions.

## Overview

Detect changes in light levels and trigger a toggle action when the lighting crosses a set threshold.

## Features

- Detects light levels
- Triggers a toggle or state change when the measured light crosses a threshold
- Simple and easy to adapt for different hardware setups
- Uses Simple Moving Average filter for more reliable and consistent measurements

## Hardware

Typical components may include:

- A light-dependent resistor (LDR)
- ESP32 microcontroller
- Supporting electronics such as resistors, wires, and power supply
- LED for an output device

## Setup

1. Assemble the scheme, using LDR and a regular resistor to create a voltage divider
2. Build the app and flash it to a microcontroller
3. Run the program and observe the behavior as lighting changes
