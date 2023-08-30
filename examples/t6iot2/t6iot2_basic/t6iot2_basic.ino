/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/

   Linux tools:
   - SSDP     : gssdp-discover -i eth0 -r 5
   - MDSN     : mdns-scan
   - Firewall : sudo tail -f /var/log/kern.log

   ESP32 & ESP8266 known compatible architectures:
   - ESP32-WROOM-DA module          / Minimal SPIFFS 1.9MB APP with OTA/190KB SPIFFS
   - NodeMCU 1.0 (ESP-12E) Module // Using nodemcu board v2.7.4: https://github.com/arrowhead-f/ArrowheadESP/issues/6#issuecomment-865923278
   - WEMOS D1 R32
   - ESP8266/Lolin(WEMOS) D1 Mini Pro
*/

#include <t6iot.h>
#include "t6config.h"

t6iot t6client;                                // Create a new t6iot client
struct sAverage {
  int32_t blockSum;
  uint16_t numSamples;
};
struct sAverage sampleAve;

void setup() {
  Serial.begin(115200);
  Serial.flush();
  pinMode(PWR_PROBE, OUTPUT);
  delay(200);
  Serial.println("t6 > BOOT Object");
  //t6client.lockSleep(t6Timeout);            // TODO, prevent chip from deepsleep
  //t6client.unlockSleep();                   // TODO, allow chip to deepsleep
  t6client.set_wifi(WIFI_SSID, WIFI_PASSWORD);// Connect to Wifi network

  if (USE_T6_CUSTOM_SERVER) {
    t6client.set_server(host, portHttp, "");  // Set custom server host, port and useragent
  } else {
    t6client.set_server();                    // Use host & port from default t6iot library
  }

  t6client.set_key(api_key);                  // Required to identify yourself on t6
  t6client.set_secret(api_secret);            // Required to identify yourself on t6

  t6client.set_object_id(object_id);          // Required for websockets & encryption and used in the user-agent
  t6client.set_object_secret(object_secret);  // Required for websockets & encryption

  t6client.startHttp(80);                     // Load to serve Http files with a user interface
  t6client.addStaticRoutes();                 // Load Http static routes on the ESP
  t6client.addDynamicRoutes();                // Load Http dynamic routes on the ESP
  t6client.startSsdp();                       // Start SSDP
  t6client.startMdns(object_name);            // Start MDNS
  t6client.startWebsockets(host, portWs);     // Connect to t6 web-socket server
  t6client.audioSetVol(3);                    // Set volume in a 1-10 range (I2S audio)
  //t6client.activateOTA();                     // Activating Over The Air (OTA) update procedure

                                              // Run only once
  t6client.scheduleOnce(0, readSample, TIME_SECONDS);
  Serial.println("t6 > readSample task as 'readTask' ; will be triggered each " + String(READINTERVAL) + "s...");

                                              // Read sensor Value regularly
  readTask = t6client.scheduleFixedRate(READINTERVAL, readSample, TIME_SECONDS);
  //t6client.cancelTask(readTask);            // Stop a task from executing again if it is a repeating task
}
void loop() {
  t6client.runLoop();                         // t6 TaskManager
  if(t6client._websockets_started) { t6client.webSockets_loop(); }
  if(t6client._audio_started) { t6client.audio_loop(); }
  delay(250);
  //t6client.goToSleep(SLEEP_DELAY_IN_SECONDS);
  //delay(SLEEP_DELAY_IN_SECONDS * 1000000);  // Use delay instead of deepSleep when HttpServer is enabled
}
int16_t addSampleToAverage(struct sAverage *ave, int16_t newSample) {
  ave->blockSum += newSample;
  ave->numSamples++;
  return newSample;
}
int16_t getAverage(struct sAverage *ave) {
  int16_t average = ave->blockSum / ave->numSamples;
  // get ready for the next block
  ave->blockSum = 0;
  ave->numSamples = 0;
  return average;
}
void readSample() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PWR_PROBE, HIGH); // Turn "On"
  t6client.lockSleep();
  delay(500);
  int count=1;
  Serial.println("t6 > Measuring:");
  do {
    sensorValue = constrain( map( analogRead(PIN_PROBE), 0, 1024, 100, 0 ) , 0.0, 1024.0);
    Serial.print(" * Measurement ");
    Serial.print(count);
    Serial.print("/");
    Serial.print(measurements);
    Serial.print(": ");
    Serial.println(sensorValue);
    
    if ( sensorValue != 0.0 && sensorValue != 10000.0 ) {
      addSampleToAverage(&sampleAve, sensorValue);
    } else {
      Serial.print("Weird sensorValue! Not added to average.");
      Serial.println(sensorValue);
    }
    count++;
    delay(500);
  } while (count <= measurements); // 10 valid measures expected
  delay(1000);
  Serial.println("------------------------------");
  digitalWrite(PWR_PROBE, LOW); // Turn "Off"
  digitalWrite(LED_BUILTIN, HIGH);

  if (getAverage(&sampleAve) > -1) {
    // Create a payload.
    DynamicJsonDocument payload(1024);  // 
    DynamicJsonDocument meta(64);       // 
    DynamicJsonDocument rules(1024);    // Rules require several nodes
    int index = 0;
    payload[index]["value"]        = sensorValue; // getAverage(&sampleAve);
    payload[index]["flow_id"]      = flow_id;
    payload[index]["mqtt_topic"]   = "t6/sensor/temperature";
    payload[index]["retention"]    = "retention1d";
    payload[index]["datatype_id"]  = "e7dbdc23-5fa8-4083-b3ec-bb99c08a2a35";
    payload[index]["object_id"]    = object_id;
    payload[index]["longitude"]    = -117.98329726717841;  // This can be dynamic and sent to t6 iot server
    payload[index]["latitude"]     = 35.06649625390798;    // This can be dynamic and sent to t6 iot server
    payload[index]["save"]         = false;
    payload[index]["publish"]      = true;

    meta[index]["sensor"]          = "sample sensor";
    rules[index][0]["conditions"]["all"][0]["fact"]        = "flow";
    rules[index][0]["conditions"]["all"][0]["operator"]    = "equal";
    rules[index][0]["conditions"]["all"][0]["value"]       = flow_id;
    rules[index][0]["conditions"]["all"][1]["fact"]        = "value";
    rules[index][0]["conditions"]["all"][1]["operator"]    = "distanceGreaterThan";
    rules[index][0]["conditions"]["all"][1]["value"]       = "1";
    rules[index][0]["event"]["type"]                       = "replaceWithDistance";
    rules[index][0]["priority"]                            = "1";
    rules[index][1]["conditions"]["all"][0]["fact"]        = "flow";
    rules[index][1]["conditions"]["all"][0]["operator"]    = "equal";
    rules[index][1]["conditions"]["all"][0]["value"]       = flow_id;
    rules[index][1]["conditions"]["all"][1]["fact"]        = "value";
    rules[index][1]["conditions"]["all"][1]["operator"]    = "distanceGreaterThan";
    rules[index][1]["conditions"]["all"][1]["value"]       = "50";
    rules[index][1]["event"]["type"]                       = "email";
    rules[index][1]["event"]["params"]["from"]             = "";
    rules[index][1]["event"]["params"]["bcc"]              = "";
    rules[index][1]["event"]["params"]["to"]               = "";
    rules[index][1]["event"]["params"]["subject"]          = "Geofencing > outside";
    rules[index][1]["event"]["params"]["text"]             = "Geofencing > outside by {distance} meter(s)";
    rules[index][1]["event"]["params"]["html"]             = "<h1>Geofencing</h1><br /> &gt; outside by {distance} meter(s)";
    rules[index][1]["priority"]                            = "2";

    payload[index]["meta"]          = meta[index];
    payload[index]["rules"]         = rules[index];
    // payload[index]["unit"]          = "26a4be78-1c02-4a41-acc1-14d8e6b29a84";
    // payload[index]["timestamp"]  = millis(); // By default, timestamp will be handled by server
  
    /*
    index++;
    payload[index]["value"]        = sensorValue; // getAverage(&sampleAve);
    payload[index]["flow_id"]      = flow_id2;
    payload[index]["mqtt_topic"]   = "t6/sensor/temperature2";
    payload[index]["retention"]    = "retention1d";
    payload[index]["datatype_id"]  = "e7dbdc23-5fa8-4083-b3ec-bb99c08a2a35";
    payload[index]["object_id"]    = object_id;
    payload[index]["longitude"]    = -117.98329726717841;
    payload[index]["latitude"]     = 35.06649625390798;
    payload[index]["save"]         = true;
    payload[index]["publish"]      = true;

    meta[index]["sensor"]          = "sample sensor2";
    payload[index]["meta"]         = meta[index];
    // payload[index]["unit"]         = "26a4be78-1c02-4a41-acc1-14d8e6b29a84";
    // payload[index]["timestamp"]  = millis(); // By default, timestamp will be handled by server
    */
  
    // TODO signature
  
    // Send the payload to the t6iot API.
    int status = t6client.createDatapoint(payload);
    Serial.println("t6 > Result status " + String(status));
    t6client.unlockSleep();
  }
}
