# GFX4dESP32

The GFX4dESP32 library is provided by 4D Systems for use with gen4-ESP32-XX product series. This library provides users access to the graphics, touch, and WiFi functionalities of 4D Systems' ESP32-S3 display modules.

Note however that some functionalities might not be supported by a certain product types, depending on its specifications. For instance, non-touch variants have no access to all touch-related functions. For more information on the specifications of a product, refer to its datasheet.

To install the library, please refer to the instructions in [Workshop4 ESP32 Development Manual](https://resources.4dsystems.com.au/manuals/workshop4/esp32/#install-via-library-manager)

It is recommended to use Workshop4 IDE to get the most of out the library functions.

### Notes

1. Workshop4 is a **Windows-only** application.
2. This is tested against Arduino ESP32 Core version 2.0.13. It might also work with 2.0.14 or higher but if any issues occur, it is recommend to try to set the boards package version to 2.0.13. If trying to use board package v3.0.0 or higher, please see library version v1.1.0 or higher. If the issue persists, please don't hesitate to [write an issue](https://github.com/4dsystems/GFX4dESP32/issues/new) or contact us directly.
3. This version doesn't support 7-inch and 9-inch variants and also 4.3 QSPI variant. For these displays, please use Arduino-ESP32 core 3.0.0 or higher and GFX4dESP32 library v1.1.0 or higher.
