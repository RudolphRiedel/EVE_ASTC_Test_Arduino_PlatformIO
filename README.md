# Example for ASTC compression with BT81x graphics controllers. 
This is a small example based on my EVE code library that displays the same image compressed with 12 different ASTC profiles.

For conveniance this is supplied as PlatformIO Arduino project since that allows to compile the same
code for a huge variety of boards.

|Environment|Status|
|---|---|
|Uno|SUCCESS|
|avr_pro|SUCCESS|
|nano328|SUCCESS|
|adafruit_metro_m4|SUCCESS|
|samd21_m0-mini|SUCCESS|
|ESP32 |SUCCESS|
|ESP8266_d1_mini|SUCCESS|
|nucleo_f446re|SUCCESS|
|teensy41|SUCCESS|
|bbcmicrobit_v2|SUCCESS|

I tested this with an ESP32 board but it should work on all of the above.

The pin settings for the different boards are in "EVE_target.h".

For example Arduino / ESP32 uses these settings:
|#define|pin|
|---|---|
|EVE_CS|13|
|EVE_PDN|12|
|EVE_SCK|18|
|EVE_MISO|19|
|EVE_MOSI|23|

The display to be used is configured in EVE_config.h.

The setting in use is "EVE_EVE3_50G" for a Matrix Orbital EVE3-50G with a BT815 chip on it and 32MBit of onboard NOR flash.

To use this example you need to copy the supplied file "flash.bin" file from the "FLASH_Data" drawer to your display, for example using the "Flash Programmer" option in the "FLASH UTILITIES" section of Bridgeteks EVE Asset Builder.

In the example the images are first copied to RAM in the BT81x and displayed from there. This is not exactly necessary but is done to avoid any potential influency by the module itself, the bandwidth of the supplied NOR flash or the board layout.

Recommended for this example are displays with 800x480 and up, using a 480x272 would be fine as well but this line needs to be adjusted:

EVE_cmd_dl_burst(VERTEX2F(0, 0));

Change that to for example:

EVE_cmd_dl_burst(VERTEX2F(-200, -200));

