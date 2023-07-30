#include <t6iot.h>

#define WIFI_SSID             ""        //WIFI SSID
#define WIFI_PASSWORD         ""        //WIFi Password

String host = "192.168.0.15";           // You can use your own t6iot On Premise application
int port = 3000;                        // .. and using a custom port

String object_id              = "";     // t6 Object Id
String object_secret          = "";     // optional t6 Object secret (32bits hexa) when using payload encryption
const char* api_key           = "";     //
const char* api_secret        = "";     //
String flow_id                = "FAKE";

float sensorValue             = -1.0;   // Value read by the sensor
const int VAL_PROBE           = 0;      // Analog pin 0
const int power               = 13;     // VCC connected to sensor
const int measurements        = 10;     // Number of measurements to make sure to sensor is OK
#define SLEEP_DELAY_IN_SECONDS 1800     // Sleep duration. // 1800=30 minutes / 3600=60 minutes / 2700=45min

t6iot t6client;                         // Create a new t6iot client

struct sAverage {
  int32_t blockSum;
  uint16_t numSamples;
};
struct sAverage sampleAve;

void setup() {
  Serial.begin(115200);
  pinMode(power, OUTPUT);
  
  //t6client.set_server();              // Leave host&port by default
  t6client.set_server(host, port);      // Set custom server host & port

  // Set the API key and secret.
  t6client.set_key(api_key);
  t6client.set_secret(api_secret);

  // Set Object Id
  t6client.set_object_id(object_id);

  // TODO encryption
  //t6client.set_object_secret(object_secret);

  // Set the Wifi
  t6client.set_wifi(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  // Create a payload.
  DynamicJsonDocument payload(1024);
  DynamicJsonDocument meta(64);

  readSample(); // Read the sensors.
  if (getAverage(&sampleAve) > -1) {
    int index = 0;
    payload[index]["value"]        = sensorValue; // getAverage(&sampleAve);
    payload[index]["flow_id"]      = flow_id;
    payload[index]["mqtt_topic"]   = "t6/sensor/temperature";
    payload[index]["retention"]    = "retention1d";
    payload[index]["datatype_id"]  = "e7dbdc23-5fa8-4083-b3ec-bb99c08a2a35";
    payload[index]["object_id"]    = object_id;
    payload[index]["longitude"]    = -117.98329726717841;
    payload[index]["latitude"]     = 35.06649625390798;
    payload[index]["save"]         = false;
    payload[index]["publish"]      = true;

    meta[index]["sensor"]          = "sample sensor";
    payload[index]["meta"]         = meta[index];
    // payload[index]["unit"]         = "26a4be78-1c02-4a41-acc1-14d8e6b29a84";
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
  }
  Serial.println("t6 > Sleeping ; will wake up in " + String(SLEEP_DELAY_IN_SECONDS) + "s...");
  delay(500);
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
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
  digitalWrite(power, HIGH); //turn "On"
  delay(500);
  int count=1;
  do {
    sensorValue = constrain( map( analogRead(VAL_PROBE), 0, 1024, 100, 0 ) , 0.0, 1024.0);
    Serial.print(" * Measurement ");
    Serial.print(count);
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
  digitalWrite(power, LOW); //turn "Off"
  digitalWrite(LED_BUILTIN, HIGH);
}
