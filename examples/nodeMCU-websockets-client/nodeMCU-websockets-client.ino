/*
 * https://www.internetcollaboratif.info/features/sockets-connection/
 * https://github.com/mathcoll/t6iot/blob/master/examples/nodeMCU-websockets-client/nodeMCU-websockets-client.ino
 *
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJWT.h>
#include <WebSocketsClient.h>
#include <ESPAsyncWebServer.h>
#include <StreamString.h>
#include <base64.h>
#include <pins_arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>

#include <ESP8266SAM.h>
#include "AudioOutputI2SNoDAC.h"

// Wifi
const char *ssid                = "";
const char *password            = "";

// t6
char wsHost[]                   = "ws.internetcollaboratif.info";
uint16_t wsPort                 = 80;
char wsPath[]                   = "/ws";
String t6wsKey                  = "";
String t6wsSecret               = "";
long expiration                 = 1695148112505;
const char * t6Object_id        = "";
const char * t6ObjectSecretKey  = "";

// t6 web sockets
unsigned long messageInterval   = 15000; //  15 secs // 10000 * 60 * 10; // 10 mins
              // Once Claimed, reschedule next message not before a long time
unsigned long messageIntervalOnceClaimed   = 60000 * 10; //  15 secs // 60000 * 10; // 10 mins
unsigned long reconnectInterval = 5000;  //  5  secs
unsigned long timeoutInterval   = 3000;  //  3  secs
int disconnectAfterFailure      = 2;     // consider connection disconnected if pong is not received 2 times
const char* PARAM_INPUT_1       = "pin";
const char* PARAM_INPUT_2       = "value";

// SSDP
int localPort                   = 80;
const char * presentationURL    = "/";
const char * friendlyName       = "t6Object";
const char * modelName          = "t6 IoT";
const char * modelNumber        = "1.0.0";
const char * deviceType         = "upnp:rootdevice";
const char * modelURL           = "https://github.com/mathcoll/t6iot";
const char * manufacturer       = "Internet Collaboratif";
const char * manufacturerURL    = "https://www.internetcollaboratif.info";

String basic_token;
const char * t6wsBase64Auth;
bool connected = false;
bool claimed = false;
unsigned long lastUpdate = millis();

String numbers[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "height", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "heighteen", "nineteen", "twenty", "twenty one"};
#define serial Serial
WebSocketsClient webSocket;
ArduinoJWT jwt = ArduinoJWT(t6ObjectSecretKey);
AsyncWebServer server(localPort);
AudioOutputI2SNoDAC *audioOutput = NULL;

static const char* ssdpTemplate =
  "<?xml version=\"1.0\"?>"
  "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
    "<specVersion>"
      "<major>1</major>"
      "<minor>0</minor>"
    "</specVersion>"
    "<URLBase>http://%s/</URLBase>"
    "<device>"
      "<deviceType>%s</deviceType>"
      "<friendlyName>%s</friendlyName>"
      "<presentationURL>%s</presentationURL>"
      "<serialNumber>%u</serialNumber>"
      "<modelName>%s</modelName>"
      "<modelNumber>%s</modelNumber>"
      "<modelURL>%s</modelURL>"
      "<manufacturer>%s</manufacturer>"
      "<manufacturerURL>%s</manufacturerURL>"
      "<UDN>uuid:a774f8f2-c180-4e26-8544-cda0e6%02x%02x%02x</UDN>"
    "</device>"
  "</root>\r\n"
  "\r\n";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED: {
              serial.printf("[WSc] Disconnected!\n");
              connected = false;
          }
          break;
        case WStype_CONNECTED: {
            serial.printf("[WSc] Connected to url: %s\n", payload);
            connected = true;
            // send message to server when Connected
            //serial.println("[WSc] SENT: Connected");
            //webSocket.sendTXT("Connected");
          }
          break;
        case WStype_TEXT: {
          serial.printf("[WSc] RESPONSE: %s\n", payload);
          StaticJsonDocument <256> doc;
          DeserializationError error = deserializeJson(doc, payload);
            if (error) {
              //Serial.print(F("deserializeJson() failed: "));
              //Serial.println(error.f_str());
              return;
            } else {
              const char* arduinoCommand = doc["arduinoCommand"];
              uint8_t pin = doc["pin"];
              const char* val = doc["value"];

              serial.printf("payload: %s\n", payload);
              serial.printf("- arduinoCommand: %s\n", arduinoCommand);
              serial.printf("- pin: %d\n", pin);
              serial.printf("- value: %s\n", val);
              serial.println();

              if (strcmp(arduinoCommand, "claimed") == 0) {
                  serial.println("[WSc] claimObject is accepted on WS server. socket_id: " + String(doc["socket_id"]));
                  messageInterval = messageIntervalOnceClaimed;
                  claimed = true;

              } else if (strcmp(arduinoCommand, "claimRequest") == 0) {
                serial.println("claimRequest");
                claimObject( t6Object_id );

              } else if (strcmp(arduinoCommand, "analogWrite") == 0) {
                serial.println("analogWrite");
                analogWrite(pin, atoi(val));

              } else if (strcmp(arduinoCommand, "digitalWrite") == 0) {
                serial.println("digitalWrite");
                serial.printf("value ==> %d\n", atoi(val));
                digitalWrite(pin, atoi(val));

              } else if (strcmp(arduinoCommand, "analogRead") == 0) {
                String currentVal = String(analogRead(pin), DEC);
                serial.println("analogRead: "+currentVal);
                webSocket.sendTXT( currentVal );

              } else if (strcmp(arduinoCommand, "digitalRead") == 0) {
                String currentVal = String(digitalRead(pin), DEC);
                serial.println("digitalRead: "+currentVal);
                webSocket.sendTXT( currentVal );

              } else if (strcmp(arduinoCommand, "getPinMode") == 0) {
                serial.println("getPinMode");
                //getPinMode(pin);
                
              } else if (strcmp(arduinoCommand, "setPinMode") == 0) {
                serial.println("setPinMode");
                pinMode(pin, atoi(val));
                
              } else if (strcmp(arduinoCommand, "audioOutput") == 0) {
                serial.println("audioOutput");
                ESP8266SAM *sam = new ESP8266SAM;
                sam->Say(audioOutput, val);
                delete sam;   
              }
            }
          }
          break;
        case WStype_BIN: {
            serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);
          }
          break;
        case WStype_PING: {
            serial.printf("[WSc] get ping\n"); // pong will be send automatically
          }
          break;
        case WStype_PONG: {
            serial.printf("[WSc] get pong\n"); // answer to a ping we send
          }
          break;
        default:
          break;
    }
}

String processor(const String& var) {
  Serial.println(var);
}

String getPinMode(int pin) {
  uint32_t bit = digitalPinToBitMask(pin);
  uint32_t port = digitalPinToPort(pin);
  volatile uint32_t *reg = portModeRegister(port);
  if (*reg & bit) {
      return "OUTPUT";
  } else {
      return "INPUT";
  }
}
 
void setup() {
    serial.begin(115200);
    //serial.setDebugOutput(true);
    serial.flush();
    serial.println("[SETUP] BOOT");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);

    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    
    audioOutput = new AudioOutputI2SNoDAC();
    audioOutput->begin();
 
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED ) {
      delay(500);
      Serial.print(".");
    }
    serial.print("Local IP: ");
    serial.println(WiFi.localIP());
    
    MDNS.begin(friendlyName);
    MDNS.addService("http", "tcp", localPort);

    server.onNotFound([] (AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/404.html", "text/html");
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });

    server.on("/digitalWrite", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String inputMessage2;
      // GET input1 value on <ESP_IP>/digitalWrite?pin=<inputMessage1>&value=<inputMessage2>
      if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
      }
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Set to: ");
      Serial.println(inputMessage2.toInt());
      request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \""+String(inputMessage1.toInt())+"\", \"value\": \""+String(inputMessage2.toInt())+"\"}");
    });

    server.on("/analogWrite", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String inputMessage2;
      // GET input1 value on <ESP_IP>/analogWrite?pin=<inputMessage1>&value=<inputMessage2>
      if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        analogWrite(inputMessage1.toInt(), inputMessage2.toInt());
      }
      Serial.print("GPIO: ");
      Serial.print(inputMessage1);
      Serial.print(" - Set to: ");
      Serial.println(inputMessage2.toInt());
      request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \""+String(inputMessage1.toInt())+"\", \"value\": \""+String(inputMessage2.toInt())+"\"}");
    });

    server.on("/setPinModeOutput", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      // GET input1 value on <ESP_IP>/setPinModeOutput?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        pinMode(inputMessage1.toInt(), OUTPUT);
      }
      request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \""+inputMessage1+"\", \"value\": \"OUTPUT\"}");
    });

    server.on("/setPinModeInput", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      // GET input1 value on <ESP_IP>/setPinModeOutput?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        pinMode(inputMessage1.toInt(), INPUT);
      }
      request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \""+inputMessage1+"\", \"value\": \"INPUT\"}");
    });

    server.on("/analogRead", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String currentVal;
      // GET input1 value on <ESP_IP>/analogRead?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        currentVal = String(analogRead(inputMessage1.toInt()), DEC);
      }
      serial.println("analogRead: "+currentVal);
      request->send(200, "application/json", "{\"status\": \"OK\", \"pin\": \""+inputMessage1+"\", \"value\": \""+currentVal+"\"}");
    });

    server.on("/digitalRead", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String currentVal;
      // GET input1 value on <ESP_IP>/digitalRead?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        currentVal = String(digitalRead(inputMessage1.toInt()), DEC);
      }
      serial.println("digitalRead: "+currentVal);
      request->send(200, "application/json", "{\"status\": \"OK\", \"pin\": \""+inputMessage1+"\", \"value\": \""+currentVal.toInt()+"\"}");
    });

    server.on("/getValues", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String currentVal;
      String output;
      const byte allPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
      const byte COUNT = sizeof(allPins);
      // GET input1 value on <ESP_IP>/getValues?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = ( request->getParam(PARAM_INPUT_1)->value() ).c_str();
        StaticJsonDocument<1024> jsonValues;
        JsonArray pins = jsonValues.createNestedArray("pins");
        for (int p = 0; p < COUNT; p++) {
          JsonObject npin = pins[p].createNestedObject("pin"+String(allPins[p]));
          npin["mode"] = getPinMode(allPins[p]);
          npin["type"] = "digital";
          npin["value"] = String(digitalRead((allPins[p])), DEC);
        }
        serializeJson(jsonValues, output);
        serial.println("");
        serial.println(output);
      }
      request->send(200, "application/json", output);
    });

    server.on("/sw.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/sw.js", "text/javascript");
    });

    server.on("/t6show-min.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/t6show-min.js", "text/javascript");
    });

    server.on("/t6show-min.js.map", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/t6show-min.js.map", "application/json");
    });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/style.css", "text/css");
    });

    server.on("/t6app.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/t6app.min.css", "text/css");
    });

    server.on("/object.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/object.css", "text/css");
    });

    server.on("/ui.json", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/ui.json", "application/json");
    });

    server.on("/robots.txt", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/robots.txt", "plain/text");
    });

    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/favicon.ico", "image/x-icon");
    });

    server.on("/icon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/icon-16x16.png", "image/x-png");
    });

    server.on("/icon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/icon-32x32.png", "image/x-png");
    });

    server.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest *request) {
      StreamString output;
      //t6Object_id
      if(output.reserve(1024)) {
        uint32_t chipId = ESP.getChipId();
        output.printf(ssdpTemplate,
          WiFi.localIP().toString().c_str(),
          deviceType,
          friendlyName,
          presentationURL,
          String(ESP.getChipId()),
          modelName,
          modelNumber,
          modelURL,
          manufacturer,
          manufacturerURL,
          (uint8_t) ((chipId >> 16) & 0xff),
          (uint8_t) ((chipId >>  8) & 0xff),
          (uint8_t)   chipId        & 0xff
        );
        request->send(200, "text/xml", (String)output);
      } else {
        request->send(500);
      }
    });

    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(localPort);
    SSDP.setDeviceType(deviceType);
    SSDP.setName(friendlyName);
    SSDP.setSerialNumber(String(ESP.getChipId()));
    SSDP.setModelName(modelName);
    SSDP.setModelNumber(modelNumber);
    SSDP.setModelURL(modelURL);
    SSDP.setManufacturer(manufacturer);
    SSDP.setManufacturerURL(manufacturerURL);
    SSDP.setURL("/");
    SSDP.begin();

    server.begin();
    webSocket.begin(wsHost, wsPort, wsPath);
    delay(500);
    webSocket.onEvent(webSocketEvent);

    basic_token = String("Basic " + base64::encode(String(t6wsKey+":"+t6wsSecret).c_str()));
    t6wsBase64Auth = basic_token.c_str();
    webSocket.setAuthorization(t6wsBase64Auth);
    
    webSocket.setReconnectInterval(reconnectInterval);
    webSocket.enableHeartbeat(messageInterval, timeoutInterval, disconnectAfterFailure);
    delay(500);
}

void claimObject(const char * id) {
  serial.println("[WSc] claimObject to WS server. object_id: " + String(id));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "claimObject";
  json["object_id"] = String(id);
  String payload = "{\"object_id\": \""+String(t6Object_id)+"\"}";
  String signature = jwt.encodeJWT(payload);
  json["signature"] = signature;
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

void loop() {
    webSocket.loop();
    if ( connected && lastUpdate+messageInterval<millis() ) {
      claimObject( t6Object_id );
      lastUpdate = millis();
    }
}
