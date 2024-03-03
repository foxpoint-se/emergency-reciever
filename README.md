# emergency-reciever
C-code (Arduino) for the emergency reciever module, responsible for reading GPS data from the emergency module and presents it on a OLED screen.

## Functionality
Purpose of the reciever module is to parse messages sent from the emergency module and present it on the oled display. There is not much else to it. Want to add checksum calculation and verification in the future.

## Components
Emergency reciever consists of the following components:

- Arduino MKR WAN 1310
- LCD OLED 0.91" 128x32px I2C

## Dependencies & installation
There are two external dependencies to other Arduino libraries. One for the LoRa tranciever and one for the Adafruit OLED display drivers.

LoRa module can be installed from this Git repository:
https://github.com/sandeepmistry/arduino-LoRa

Adafruit SSD1306:
https://github.com/adafruit/Adafruit_SSD1306

Also the official board software needs to be installed from the Arduino IDE.

## Data format
Emergency module transmits an aggregated data message separated by a newline character in the following format:

221835.00\n1\n17.981023\n59.307035\n35.00\n

1. UTC time parsed from the GPS module, ex: 221835.00
2. Status (0 - Normal operation, 1 - Emergency mode), ex: 1
3. Longitude parsed from the GPS module, ex: 17.981023
4. Latitude parsed form the GPS module, ex: 59.307035
5. Battery percentage, current votlage from INA sensor over max battery voltage, ex: 35.00

## Message validation
Some variants of message validation could be either, checking the length of the string (should be 43 chars?), checking the number of new line characters found in the message. Adding a XOR checksum to validate that the full content have been sent. OR even a combination of at least two of the above.