# clockthenticator

A QR code(ish) based one-time password (OTP) generator for Arduino.

## Features

- Narrow non-standard compliant subset of QR code (Version 2 with 21x21 matrix, ECC 1)
- HOTP (HMAC-based One-Time Password) generation
- Support for 1-digit thru 9-digit OTPs
- Support for an optional prefix, such as a URL or device id
- Support for time hinting to allow for clock drift detection

## Parts List

Adafruit Matrix Portal S3 CircuitPython Powered Internet Display
https://www.adafruit.com/product/5778

64x32 RGB LED Matrix - 5mm pitch
https://www.adafruit.com/product/2277

Adafruit PCF8523 Real Time Clock Assembled Breakout Board
https://www.adafruit.com/product/3295

## Configuration

Copy Configuration_example.hpp to Configuration.hpp and modify the values to match your setup.

## Usage

Upload the code to your Arduino. Time will be set to your compile time with the offset in your Configuration.hpp.

We assume you're displaying UTC time for proper HOTP code generation. If you force a local time offset, your HOTP codes will be incorrect.

## TODO

Display local time that adjusts for DST.

## Thank You

Many thanks to the following projects for their inspiration and code:

- Adafruit - https://www.adafruit.com/
- ZXing Team - https://github.com/zxing/zxing
- Project Nayuki - https://www.nayuki.io/page/creating-a-qr-code-step-by-step
- Google - SHA-1 implementation that works on Arduino (included verbatim)
