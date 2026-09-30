# Third-Party Notices

GFX4dESP32 is licensed under the Apache License, Version 2.0 (see `LICENSE`).
The following components are third-party works and remain under their own terms.
Their notices must be retained when you redistribute this library.

## Espressif JPEG decoder wrapper (esp_jpeg)
- Files: `src/jpeg_decoder4D.c`, `src/jpeg_decoder4D.h`
- Upstream: https://github.com/espressif/idf-extra-components (`esp_jpeg`)
- Copyright: Espressif Systems (Shanghai) CO LTD
- License: Apache-2.0 (SPDX identifier in each file header)
- Modifications by 4D Systems: `jpeg_decoder4D.c` has renamed includes, ROM-decoder
  selection disabled, and default `JD_FORMAT` changed to RGB565 (noted in the file).
  `jpeg_decoder4D.h` is unchanged from upstream apart from the file name.

## TJpgDec - Tiny JPEG Decompressor
- Files: `src/tjpgd4D.c`, `src/tjpgd4D.h` (and configuration `src/tjpgdcnf4D.h`)
- Obtained via Espressif's esp_jpeg; code is identical to upstream except renamed includes.
  `tjpgdcnf4D.h` is 4D's fixed-value adaptation of Espressif's `tjpgdcnf.h`.
- Copyright: (C) 2021, ChaN, all rights reserved
- Terms (from the file header): free software with no warranty; no restriction
  on use, modification or redistribution for personal, non-profit or commercial
  products under your own responsibility; redistributions of source code must
  retain the original copyright notice, which is kept in those files.

## GT911 touch driver
- Files: `src/GT911.h`, `src/GT911.cpp`, `src/GT911_Structs.h`
- Derived from https://github.com/alex-code/GT911, which is based on
  https://github.com/nik-sharky/arduino-goodix
- Verified: `GT911_Structs.h` matches alex-code's file apart from blank lines. `GT911.h` and
  `GT911.cpp` keep its structure and API, with 4D additions (firmware/resolution
  helpers, rotation and HRES/VRES accessors, direct `Wire` use).
- License: MIT. Both upstream notices are reproduced below and must be retained.

### Copyright (c) 2021 alex-code

MIT License

Copyright (c) 2021 alex-code

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### Copyright (c) 2017 Nik

MIT License

Copyright (c) 2017 Nik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
