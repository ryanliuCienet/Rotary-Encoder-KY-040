# Rotary Encoder Project for nRF52833 DK

This project implements a rotary encoder (KY-040) interface for the nRF52833 DK development board using Zephyr RTOS.

## Hardware Connection

Connect the KY-040 rotary encoder to the nRF52833 DK as follows:

| KY-040 Pin | nRF52833 DK Pin | Description |
|------------|-----------------|-------------|
| CLK        | P0.03          | Clock signal (Rotary A) |
| DT         | P0.04          | Data signal (Rotary B) |
| SW         | P0.15          | Push button |
| VCC        | 3.3V           | Power supply |
| GND        | GND            | Ground |

## Features

- Detects clockwise and counter-clockwise rotation
- Counts rotation steps
- Detects button press
- Outputs rotation direction and counter value via serial console
