# Alexa Smart Plug
Command any Alexa device to control applicances using this DIY smart plug! 

## Software
### You will need
- Arduino IDE (https://www.arduino.cc/en/Main/Software)
- A Mini USB Cable

### Steps
1. Go to Arduino IDE > Tools > Manage Libraries and install the following:
    - ArduinoJson v6+
    - WebSockets v2.2+
2. Go to Arduino IDE > Preferences and add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to Additional Boards Manager URLs
3. Go to Arduino IDE > Tools > Boards > Boards Manager and install the following:
    - ESP8266 v2.7+
4. Create an account at https://www.sinric.com/login, login, and create a Smart Home Device
5. Open `smart_plug.ino` and change the `WIFI_SSID, WIFI_PASS` variables
6. Update `API_KEY` and `deviceId` variables from Sinric 
7. Plug in the ESP8266, select the appropriate COM port, and upload
8. Go to Alexa app > Skills & Games and install the Sinric skill and login 
9. Go to Alexa app > Devices > (+) to add device
10. Done!

## Hardware
### You will need
- WeMos D1 Mini (ESP8266EX ESP-12)
- 5V Relay Module with Optocoupler Isolation
- Duplex outlet
- 14 AWG wire (because we're dealing with 120V @ 60Hz) 
- Any 5V phone charger
- Scrap parts for the plug

### Circuit Diagram
![Alt text](Circuit Diagram)
<img src="media/circuit_diagram.svg">

## References
- ESP8266 Circuit Diagram: https://www.wemos.cc/en/latest/d1/d1_mini.html
- ESP8266 Docs: https://github.com/esp8266/Arduino
