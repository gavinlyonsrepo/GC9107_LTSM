# GC9107 Readme

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/paypalme/whitelight976)

## Table of contents

* [Overview](#overview)
* [Installation](#installation)
* [Dependency](#dependency)
* [Documentation](#documentation)
* [Test](#test)
* [Software](#software)
  * [User Options](#user-options)
* [Hardware](#hardware)

## Overview

* Name: GC9107_LTSM
* Description:

Arduino C++ library for a TFT SPI LCD, GC9107 Driver IC.

The GC9107 is a 262,144-color single-chip SOC driver for a-TFT liquid crystal display with resolution of 128RGBx160dots, comprising a 384-channel source driver, a 160-channel gate driver, 46,080 bytes GRAM for graphic display data of 128RGBx160 dots, and power supply circuit.

1. Power modes, Invert, Scroll, Rotate, Bitmaps supported.
2. Hardware & software SPI
3. Tested on 60x160 Bar Display
4. Arduino eco-system library.
5. 16 ASCII fonts included, fonts can easily be removed or added.
6. Advanced graphics class included.
7. Advanced frame buffer mode included.
8. Bitmaps supported: 1, 8 and 16 bit.
9. Sprites supported.
10. [Project url link](https://github.com/gavinlyonsrepo/GC9107_LTSM)

* [DataSheet](https://www.buydisplay.com/download/ic/GC9107.pdf)

## Installation

The library is included in the official Arduino library manger and the optimum way to install it is using the library manager which can be opened by the *manage libraries* option in Arduino IDE.

## Dependency

This library requires the Arduino library 'display16_LTSM' as a dependency. display16_LTSM library contains
the graphics, bitmaps, and font methods as well as font data and bitmap test data. Its also
where the user sets options(debug, advanced graphics and frame buffer mode).
When you install 'GC9107_LTSM' with Arduino IDE. It should install 'display16_LTSM' as well after
a prompt, if it does not you have to install it same way as 'GC9107_LTSM'.
The 'display16_LTSM' project and readme is at [URL github link.](https://github.com/gavinlyonsrepo/display16_LTSM)
'display16_LTSM' is also written by author of this library.

## Documentation

Code is commented for the 'doxygen' API generation tool.
Documents on fonts, bitmaps and graphics can be found at
the dependency 'display16_LTSM' repository, [URL github link](https://github.com/gavinlyonsrepo/display16_LTSM)

## Test

There are example files included.

| Filename | Function | Note |
| --- | --- | --- |
| HELLO | Hello world | Portrait 0 degrees rotation |
| HELLO_90 | Hello world | Landscape 90 degrees rotation |
| FRAME_BUFFER | Testing frame buffer mode | dislib16_ADVANCED_SCREEN_BUFFER_ENABLE must be enabled, user option 2 |
| BITMAP | bitmaps tests | Bitmap data is stored in arrays |
| TEXT_GRAPHIC_FUNC | Text + Graphics + Functions | Some graphics test require dislib16_ADVANCED_GRAPHICS_ENABLE to be enabled, user option 1 |
| DEMO_ONE | System Monitor Demo | - |
| DEMO_TWO | EMV Monitor Demo | - |
| DEMO_THREE | Flight Simulator Demo | - |

## Software

### User Options

In the ino example files. There are sections in "setup()" function
where user can make adjustments to select for SPI type used, PCB type used and screen size.

0. USER OPTION 0 SPI_SPEED, GPIO + TYPE
1. USER OPTION 1 Screen size + options
2. USER OPTION 2 Screen offsets

#### USER OPTION 0 SPI SPEED + GPIO + TYPE

A bool variable is used to decide of hardware or software init function is called
Here the user can pass the SPI Bus freq in Hertz, Currently set to 8 Mhz.
If users wants software SPI just call this method
with just one argument for the optional GPIO software uS delay,
which by default is zero. Setting this higher can be used to slow down Software SPI
which may be beneficial in some setups.

The 5 GPIO pins used, the clock and data lines must be the clock and data lines
of SPI bus in hardware SPI mode.

#### USER OPTION 1 Screen size + options

In USER OPTION 1.
User can adjust display size (width & height) in pixels, memory base
and colour order (RGB or BGR)

There are two memory base variants called "Display data GRAM mapping" in datasheet
Which I refer to as memory base A and B, These require different init routines.

1. 128x128 mapping (MEMORY_BASE_GM_128x128) A
2. 128x160 mapping (MEMORY_BASE_GM_128x160) B

#### USER OPTION 2 Screen offsets

In addition here are number of different truncated displays on market: These truncated displays require an offset.
Known displays types on market with GC9107 controller.
The offsets for truncated untested devices are untested and may be incorrect.

| Display Type | Memory Base | VRAM size setup (WbyH) | Offsets rotation | Tested |
| --- | --- | --- | --- | --- |
| 128x128 Square | MEMORY_BASE_GM_128x128 A | 128 , 128 | 0,0 | No device |
| 128x115 Round Truncated | MEMORY_BASE_GM_128x128 A | 128 , 128 | 0,7 ? | No device |
| 60x160 Truncated | MEMORY_BASE_GM_128x160 B | 128 , 160 | 34,0 | YES |
| 96x160 Truncated | MEMORY_BASE_GM_128x160 B | 128 , 160 | 16,0 ? | No device |
| 128x160 Full | MEMORY_BASE_GM_128x160 B | 128 , 160 | 0,0 | No device |

Example offset calculation for a truncated display(60x160) is as follows.

[![ offsets ](https://github.com/gavinlyonsrepo/displaylib_16bit_PICO/blob/main/extra/image/gc91.png)](https://github.com/gavinlyonsrepo/displaylib_16bit_PICO/blob/main/extra/image/gc91.png)

## Hardware

[![ display pic ](https://github.com/gavinlyonsrepo/displaylib_16bit_PICO/blob/main/extra/image/gc92.png)](https://github.com/gavinlyonsrepo/displaylib_16bit_PICO/blob/main/extra/image/gc92.png)

Connections as setup in ino examples files for ESP32:

| Pin description | HW SPI | SW SPI |
| --- | --- | --- |
| GND Ground | GND | GND |
| VCC | 3.3 VCC | 3.3 VCC |
| SCL Clock | 18 | 27 |
| SDA Data in -> MOSI | 23 | 26 |
| RST Reset | 4 | 4 |
| DC Data or command | 5 | 5 |
| CS Chip select | 15 | 15 |
| BL Backlight | VCC thru resistor | <- |

1. This is a 3.3V logic device do NOT connect the I/O logic lines to 5V logic device.
2. SW SPI pick any GPIO you like , HW SPI SCLK and SDA will be tied to SPI interface.
3. Backlight on/off control is left to user.
4. If no Reset pin present: pass -1 as argument for Reset pin number in SetupGPIO()functions
