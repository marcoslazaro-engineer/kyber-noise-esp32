# Kyber Noise ESP32 Project
This project implements key components (GET NOISE) of the CRYSTALS-Kyber post-quantum cryptographic scheme for the ESP32-S3 platform. It is focused on low-level experimentation and hardware-level testing, including side-channel and fault injection analysis. Kyber512-only implementation optimized for memory-constrained ESP32-S3 hardware.
## Project Structure
The repository assumes the following structure:

esp/

├── esp-idf/ # ESP-IDF toolchain (not included)

└── kyber-noise-esp32/ # This project

Only `kyber-esp32/` is included in this repository. You must install `esp-idf` manually by following the official Espressif installation guide:

- https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/

## Features

Written in standard C for ESP32-S3 target.

Generates Kyber noise polynomials from a random seed using the official Kyber noise sampling routine.

Output is printed to UART via printf().

Can be extended for full Kyber KeyGen / Encapsulation / Decapsulation routines.

Structured for hardware-level testing (e.g., power analysis, fault injection).

## Building and Flashing

Make sure you are in the `esp/kyber-esp32` directory and that your `esp-idf` environment is correctly sourced.

```bash
. ../esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
Replace /dev/ttyACM0 with the actual port of your ESP32-S3 board.
```
## License
No license is granted.

This repository is published without any license. This means:

No one is permitted to copy, modify, distribute, or use this code in any form.

All rights are reserved by the author.

Unauthorized usage, even privately, is legally prohibited.

If you wish to use this project or parts of it, you must contact the author for explicit permission.





