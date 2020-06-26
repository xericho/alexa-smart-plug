/*
 Version 0.1 - June 25 2020
 Eric Ho
 NOTES:
  - This code uses ArduinioJson 6. See https://arduinojson.org/v6/doc/upgrade/
  - Source: https://github.com/esp8266/Arduino
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

// Uncomment the following line to enable serial debug output
#define ENABLE_DEBUG

/******************************************************
 *                  General Setup                     *
 ******************************************************/

#define API_KEY "e76e30a3-dfe4-4d53-8513-cde2afffd15b" 
#define WIFI_SSID "ASUS_08_2G" 
#define WIFI_PASS "eternity_3224" 
#define LED_PIN LED_BUILTIN  // let library figure out which is LED pin :)

/******************************************************/

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == "5ef572336733ac69e481a72b") // Device ID of first device
  {  
#ifdef ENABLE_DEBUG
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
#endif
    digitalWrite(LED_PIN, LOW);
  } 
//  else if (deviceId == "5axxxxxxxxxxxxxxxxxxx") // Device ID of second device
//  { 
//    Serial.print("Turn on device id: ");
//    Serial.println(deviceId);
//  }
//  else {
//    Serial.print("Turn on for unknown device id: ");
//    Serial.println(deviceId);    
//  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5ef572336733ac69e481a72b") // Device ID of first device
   {  
#ifdef ENABLE_DEBUG
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
#endif
     digitalWrite(LED_PIN, HIGH);
   }
//   else if (deviceId == "5axxxxxxxxxxxxxxxxxxx") // Device ID of second device
//   { 
//     Serial.print("Turn off Device ID: ");
//     Serial.println(deviceId);
//  }
//  else {
//     Serial.print("Turn off for unknown device id: ");
//     Serial.println(deviceId);    
//  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
#ifdef ENABLE_DEBUG
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
#endif
      break;
    case WStype_CONNECTED: {
      isConnected = true;
#ifdef ENABLE_DEBUG
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
#endif
      }
      break;
    case WStype_TEXT: {
#ifdef ENABLE_DEBUG
        Serial.printf("[WSc] get text: %s\n", payload);
#endif
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
          
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
#ifdef ENABLE_DEBUG
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(WIFI_SSID);
#endif

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
#ifdef ENABLE_DEBUG
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
#endif

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", API_KEY);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}

// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonDocument root(1024);     
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  serializeJson(root, databuf);
  
  webSocket.sendTXT(databuf);
}

//eg: setPowerStateOnServer("deviceid", "CELSIUS", "25.0")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonDocument root(1024);     
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject valueObj = root.createNestedObject("value");
  JsonObject targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  serializeJson(root, databuf);
  
  webSocket.sendTXT(databuf);
}
