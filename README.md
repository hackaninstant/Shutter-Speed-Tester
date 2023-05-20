This shutter speed tester is based off of Cameradactyl's shutter timer, modified to display on an I2C SSD1306 128x64 display. It was forked to run automously without a computer connected. 

Features:

- Battery operated (4.5v)
- Reports shutter speed in fractions of a second as well as milliseconds
- Reports battery voltage

Requirements:

- Arduino Nano 16K or 32K version
- Laser sensor module
- 5Mw red laser module
- SSD1306 I2C 128x64 display
- 3 cell battery holder with batteries

Arduino Nano configuration:

- D2: Laser sensor data
- A4: SDA on I2C SSD1306
- A5: SCL on I2C SSD1306
- 5V: Battery holder pos, Laser module and laser sensor module pos
- GND: GND on I2C SSD1306, GND on battery holder, GND on laser module and sensor
- 3.3V: VVC on I2C SSD1306 display

Probably the most difficult part of the project is building a frame/housing for the sensor and laser. The sensor should be protected from ambient light.

When lining up the laser, it is best to try aiming the laser directly into the center of the lens along it's center axis, otherwise the laser will defract and will not wind up hitting the sensor. 
