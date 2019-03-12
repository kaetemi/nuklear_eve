# Nuklear EVE

Nuklear on EVE.

## Supported EVE targets
- FT80X (EVE) graphics controller
- FT81X (EVE2) graphics controller
- BT81X (EVE3) graphics controller

## Supported platforms
- FT90X mcu
- MPSSE cable using Windows
- FT4222 adapter using Windows
- BT8XXEMU emulator on Windows

## Running the demos

Run CMake in a `build` subdirectory to generate the project.

Build and run the generated project.

## Dependencies
- Nuklear (https://github.com/vurtun/nuklear/)
- EVE Hardware Abstraction Layer
- EVE Screen Designer Framework Core
- FT900 Toolchain (optional)
- BT8XX Emulator (optional)
- LibMPSSE (optional)
- LibFT4222 (optional)

## Licenses

This library is distributed under the MIT License.

```
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

Nuklear is distributed both under the MIT License and as public domain.

FTDI libraries follow the https://www.ftdichip.com/Drivers/FTDriverLicenceTerms.htm license.

Bridgetek source code is distributed under the https://brtchip.com/brtsourcecodelicenseagreement/ license.

## Known limitations

- Nuklear only supports single touch
- Right click is not supported
- Total amount of objects on the screen is capped by hardware limits
- Oval shapes are not supported, only plain circles are
- Bitmap support is not yet implemented
- Unicode font support for EVE3 target is not yet implemented
