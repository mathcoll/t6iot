/*
 * https://www.internetcollaboratif.info/features/sockets-connection/
 * https://github.com/mathcoll/t6iot/blob/master/examples/nodeMCU-websockets-client/nodeMCU-websockets-client.ino
 * 
 * Set CPU Frequency to at least 240MHz
 * https://github.com/lorol/arduino-esp32littlefs-plugin
 * ESP32-WROOM-DA
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJWT.h>
#include <WebSocketsClient.h>
#include <StreamString.h>
#include <base64.h>
#include <pins_arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266SAM.h>
#include <LittleFS.h>
#include "AudioOutputI2SNoDAC.h"
#include "config.h"

#define FILEFS LittleFS
#define ASYNC_TCP_SSL_ENABLED true
#define serial Serial

#ifdef ESP8266
  #define ESP_GETCHIPID ESP.getChipId()
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266SSDP.h>
#elif ESP32
  #define ESP_GETCHIPID (uint32_t)ESP.getEfuseMac()
  #define LED_BUILTIN 2
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <ESP32SSDP.h>
#endif

bool connected = false;
bool claimed = false;
unsigned long lastUpdate = millis();

String numbers[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "height", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "heighteen", "nineteen", "twenty", "twenty one"};
WebSocketsClient webSocket;
ArduinoJWT jwt = ArduinoJWT(t6ObjectSecretKey);
AsyncWebServer server(localPortHTTP);
AudioOutputI2SNoDAC *audioOutput = NULL;

static const char* configTemplate = R"EOF(
let config={
  "object_ip": "%s",
  "object_port": %s,
  "wsHost": "%s",
  "wsPort": "%s",
  "wsPath": "%s",
  "t6Object_id": "%s",
  "token": "%s"
};
)EOF";

static const char* ssdpTemplate = R"EOF(<?xml version="1.0"?>
<root xmlns="urn:schemas-upnp-org:device-1-0">
  <specVersion>
    <major>1</major>
    <minor>0</minor>
  </specVersion>
  <URLBase>https://%s/</URLBase>
  <device>
    <deviceType>%s</deviceType>
    <friendlyName>%s</friendlyName>
    <presentationURL>%s</presentationURL>
    <serialNumber>%u</serialNumber>
    <modelName>%s</modelName>
    <modelNumber>%s</modelNumber>
    <modelURL>%s</modelURL>
    <manufacturer>%s</manufacturer>
    <manufacturerURL>%s</manufacturerURL>
    <UDN>uuid:a774f8f2-c180-4e26-8544-cda0e6%02x%02x%02x</UDN>
  </device>
</root>
)EOF";

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

void subscribe(const char * channel) {
  serial.println("[WSc] subscribe object to WS server. " + String(channel));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "subscribe";
  json["channel"] = String(channel);
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

void unsubscribe(const char * channel) {
  serial.println("[WSc] unsubscribe object to WS server. " + String(channel));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "unsubscribe";
  json["channel"] = String(channel);
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

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
          StaticJsonDocument <256> jsonPayload;
          DeserializationError error = deserializeJson(jsonPayload, payload);
            if (error) {
              //Serial.print(F("deserializeJson() failed: "));
              //Serial.println(error.f_str());
              return;
            } else {
              const char* arduinoCommand;
              const char* val = "";
              const char* message = "";
              const char* socket_id = "";
              uint8_t pin;

              if (jsonPayload["arduinoCommand"]) {
                arduinoCommand = jsonPayload["arduinoCommand"].as<char*>();
              }
              if (jsonPayload["value"]) {
                val = jsonPayload["value"].as<char*>();
              }
              if (jsonPayload["message"]) {
                val = jsonPayload["message"].as<char*>();
              }
              if (jsonPayload["socket_id"]) {
                socket_id = jsonPayload["socket_id"].as<char*>();
              }
              if (jsonPayload["pin"]) {
                pin = jsonPayload["pin"];
              }

              serial.printf("payload: %s\n", jsonPayload);
              serial.printf("- arduinoCommand: %s\n", arduinoCommand);
              serial.printf("- pin: %d\n", pin);
              serial.printf("- value: %s\n", val);
              serial.println();

              if (strcmp(arduinoCommand, "claimed") == 0) {
                  serial.printf("[WSc] claimObject is accepted on WS server. socket_id: %s\n", socket_id);
                  messageInterval = messageIntervalOnceClaimed;
                  claimed = true;

              } else if (strcmp(arduinoCommand, "info") == 0) {
                serial.println("[WSc] info ===================");
                serial.printf("- pin: %d\n", pin);
                serial.printf("- value: %s\n", val);
                serial.printf("- message: %s\n", message);
                serial.printf("- socket_id: %s\n", socket_id);
                serial.println("[WSc] /info ==================");

              } else if (strcmp(arduinoCommand, "claimRequest") == 0) {
                serial.println("claimRequest");
                claimObject( t6Object_id );
                
                subscribe("demo");
                //subscribe("channel_1");
                //subscribe("channel_2");
                //subscribe("channel_3");
                //unsubscribe("channel_1");
                //unsubscribe("channel_2");
                //unsubscribe("channel_3");

              } else if (strcmp(arduinoCommand, "analogWrite") == 0) {
                serial.println("analogWrite");
                analogWrite(pin, atoi(val));

              } else if (strcmp(arduinoCommand, "digitalWrite") == 0) {
                serial.println("digitalWrite");
                serial.printf("value ==> %d\n", atoi(val));
                pinMode(pin, OUTPUT);
                if(atoi(val) == 0) {
                  digitalWrite(pin, LOW);
                } else if(atoi(val) == 1) {
                  digitalWrite(pin, HIGH);
                }

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
            //hexdump(payload, length); // https://github.com/timum-viw/socket.io-client/issues/15
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
  serial.println(var);
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

unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String urldecode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {
      encodedString += c;
    }
    yield();
  }
  return encodedString;
}

void setup() {
  serial.begin(115200);
  //serial.setDebugOutput(true);
  serial.flush();
  serial.println("[t6IoT] BOOT");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
  }
  serial.print("[t6IoT] Local IP: ");
  serial.println(WiFi.localIP());
  serial.println("[t6IoT] Wifi started");

  if(!FILEFS.begin()) {
    serial.println("[t6IoT] An Error has occurred while mounting FS");
    return;
  } else {
    serial.println("[t6IoT] FS mounted");
  }
  
  if( audio_active ) {
    audioOutput = new AudioOutputI2SNoDAC();
    audioOutput->begin();
    serial.println("[t6IoT] audioOutput started");
  }

  if( mdns_active ) {
    MDNS.begin(friendlyName);
    MDNS.addService("http", "tcp", localPortHTTP);
    serial.println("[t6IoT] MDNS started");
  }

  if( sockets_active ) {
    webSocket.begin(wsHost, wsPort, wsPath);
    webSocket.onEvent(webSocketEvent);
    webSocket.setAuthorization(( String("Basic " + base64::encode(String(t6wsKey+":"+t6wsSecret).c_str())) ).c_str());
    webSocket.setReconnectInterval(reconnectInterval);
    webSocket.enableHeartbeat(messageInterval, timeoutInterval, disconnectAfterFailure);
    serial.println("[t6IoT] WebSockets started");
  }

  if( http_active ) {
    server.begin();
    server.on("/digitalWrite", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String inputMessage2;
      // GET input1 value on <ESP_IP>/digitalWrite?pin=<inputMessage1>&value=<inputMessage2>
      if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        
        Serial.print("pinMode: ");
        pinMode(inputMessage2.toInt(), OUTPUT);
        Serial.println("OUTPUT");
        if(inputMessage2.toInt() == 0) {
          digitalWrite(inputMessage2.toInt(), LOW);
        } else if(inputMessage2.toInt() == 1) {
          digitalWrite(inputMessage2.toInt(), HIGH);
        }
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
  
      //set the resolution to 12 bits (0-4096)
      analogReadResolution(12);
      int analogValue = analogRead(inputMessage1.toInt());
      int analogVolts = analogReadMilliVolts(inputMessage1.toInt());
      
      // GET input1 value on <ESP_IP>/analogRead?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
        currentVal = String(analogVolts, DEC);
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
    server.on("/audioOutput", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage2;
      // GET input1 value on <ESP_IP>/audioOutput?value=<inputMessage2>
      if (request->hasParam(PARAM_INPUT_2)) {
        //inputMessage2 = ( request->getParam(PARAM_INPUT_2)->value() ).c_str();
        ESP8266SAM *sam = new ESP8266SAM;
        String message_decoded = urldecode(request->getParam(PARAM_INPUT_2)->value());
        const char* inputMessage2 = message_decoded.c_str();
        sam->Say(audioOutput, inputMessage2); // String(inputMessage2).c_str()
        delete sam;
      }
      request->send(201, "application/json", "{\"status\": \"OK\", \"snack\": \"Message played "+String(inputMessage2)+"\"}");
    });
    server.on("/getValues", HTTP_GET, [](AsyncWebServerRequest *request) {
      String inputMessage1;
      String currentVal;
      String output;
      const byte allPins[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
      const byte COUNT = sizeof(allPins);
      // GET input1 value on <ESP_IP>/getValues?pin=<inputMessage1>
      if (request->hasParam(PARAM_INPUT_1)) {
        inputMessage1 = ( request->getParam(PARAM_INPUT_1)->value() ).c_str();
        StaticJsonDocument<4096> jsonValues;
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
    server.on("/object-conf.js", HTTP_GET, [](AsyncWebServerRequest *request) {
      StreamString output;
      output.printf(configTemplate,
        WiFi.localIP().toString().c_str(),
        String(localPortHTTP),
        String(wsHost).c_str(),
        String(wsPort),
        wsPath,
        t6Object_id,
        String(t6wsKey+":"+t6wsSecret).c_str()
      );
      request->send(200, "text/javascript", (String)output);
    });
    server.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest *request) {
      StreamString output;
      //t6Object_id
      if(output.reserve(1024)) {
        output.printf(ssdpTemplate,
          WiFi.localIP().toString().c_str(),
          deviceType,
          friendlyName,
          presentationURL,
          String(ESP_GETCHIPID),
          modelName,
          modelNumber,
          modelURL,
          manufacturer,
          manufacturerURL,
          (uint8_t) ((ESP_GETCHIPID >> 16) & 0xff),
          (uint8_t) ((ESP_GETCHIPID >>  8) & 0xff),
          (uint8_t)   ESP_GETCHIPID        & 0xff
        );
        request->send(200, "text/xml", (String)output);
      } else {
        request->send(500);
      }
    });
    server.on("/index", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/index.html", "text/html");
    });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/index.html", "text/html");
    });
    server.on("/sw.js", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/sw.js", "text/javascript");
    });
    server.on("/fonts/Material-Icons.woff2", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/fonts/Material-Icons.woff2", "font/woff2");
    });
    server.on("/t6show.js", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/t6show.js", "text/javascript");
    });
    server.on("/t6show-min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/t6show-min.js", "text/javascript");
    });
    server.on("/t6show-min.js.map", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/t6show-min.js.map", "application/json");
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/style.css", "text/css");
    });
    server.on("/t6app.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/t6app.min.css", "text/css");
    });
    server.on("/object.css", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/object.css", "text/css");
    });
    server.on("/ui.js", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/ui.js", "application/javascript");
    });
    server.on("/robots.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/robots.txt", "plain/text");
    });
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/favicon.ico", "image/x-icon");
    });
    server.on("/icon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/icon-16x16.png", "image/x-png");
    });
    server.on("/icon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(FILEFS, "/icon-32x32.png", "image/x-png");
    });
    server.onNotFound([] (AsyncWebServerRequest *request) {
      request->send(FILEFS, "/404.html", "text/html");
    });
    serial.println("[t6IoT] HTTP started");
  }

  if( ssdp_active ) {
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(localPortHTTP);
    SSDP.setDeviceType(deviceType);
    SSDP.setName(friendlyName);
    SSDP.setSerialNumber(String(ESP_GETCHIPID));
    SSDP.setModelName(modelName);
    SSDP.setModelNumber(modelNumber);
    SSDP.setModelURL(modelURL);
    SSDP.setManufacturer(manufacturer);
    SSDP.setManufacturerURL(manufacturerURL);
    SSDP.setURL("/");
    SSDP.setInterval(advertiseInterval);
    SSDP.begin();
    serial.println("[t6IoT] SSDP started");
  }

  serial.println("[t6IoT] READY");
}

void loop() {
  webSocket.loop();
  if ( connected && lastUpdate+messageInterval<millis() ) {
    claimObject( t6Object_id );
    lastUpdate = millis();
  }
  int analogVolts = analogReadMilliVolts(2);
  Serial.printf("ADC millivolts value = %d\n",analogVolts);
  delay(500);
}
