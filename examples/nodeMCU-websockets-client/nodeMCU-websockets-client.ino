/*
 *
 * 
 *
 */
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
 
WebSocketsClient webSocket;

const char *ssid     = "cxxxxxxxxxxxxxxxxxxxxxxxxxxs";
const char *password = "uxxxxxxxxxxxxxxxxxxxxxxxxxx0";
char wsHost[] = "ws.internetcollaboratif.info"; // "192.168.0.15"; //
uint16_t wsPort = 80; // 4000;
char wsPath[] = "/ws";
const char * wsKey    = "TSxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxaf";
const char * wsSecret = "RBxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxkS";
long expiration = 1695062423073;
const char * wsBase64Auth = "Basic VFxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxtT";
const char * t6Object_id = "aaaaaaaa-aaaa-4bbb-8888-aaaaaaaaaaaa";
const char * t6ObjectSecretKey = "FxxxxxxxxxxxxxxxxxxxxxxxxxxkeJ@@.@n'.@?o.%xxxxxxxxxxxxxxxxxxxxxxxxxxd?w*@y-f";

unsigned long messageInterval = 15000;   // 15 secs
unsigned long reconnectInterval = 5000; // 5 secs
bool connected = false;
 
#define serial Serial
#define UNKNOWN_PIN 0xFF
 
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

              if (strcmp(arduinoCommand, "analogWrite") == 0) {
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
 
void setup() {
    serial.begin(115200);
    //serial.setDebugOutput(true);
    serial.flush();
    serial.println("[SETUP] BOOT");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
 
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "." );
    }
    digitalWrite(LED_BUILTIN, HIGH);
    serial.print("Local IP: ");
    serial.println(WiFi.localIP());

    webSocket.begin(wsHost, wsPort, wsPath);
    delay ( 500 );
    webSocket.onEvent(webSocketEvent);
    webSocket.setAuthorization(wsBase64Auth);
    webSocket.setReconnectInterval(reconnectInterval);
    webSocket.enableHeartbeat(messageInterval, 3000, 2);
    delay ( 500 );
    claimObject( t6Object_id );
}

unsigned long lastUpdate = millis();

void claimObject(const char * id) {
  serial.println("[WSc] claimObject to WS server: " + String(id));
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "claimObject";
  json["object_id"] = String(id);
  // We should use secret (t6ObjectSecretKey) to sign a payload: {"object_id": t6Object_id}
  json["signature"] = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJvYmplY3RfaWQiOiJhMDZkZmE4YS1kZGViLTRiZjYtODg1YS02ZmI0ZjFmODRiMDEifQ.o_HkV9U8GKRdzq0S-5pFBlzkz38kcAb3VFkLTqtHKQc";
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

/*
uint8_t getPinMode(uint8_t pin) {
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);

  // I don't see an option for mega to return this, but whatever...
  if (NOT_A_PIN == port) return UNKNOWN_PIN;

  // Is there a bit we can check?
  if (0 == bit) return UNKNOWN_PIN;

  // Is there only a single bit set?
  if (bit & bit - 1) return UNKNOWN_PIN;

  volatile uint8_t *reg, *out;
  reg = portModeRegister(port);
  out = portOutputRegister(port);

  if (*reg & bit)
    return OUTPUT;
  else if (*out & bit)
    return INPUT_PULLUP;
  else
    return INPUT;
}
*/

void loop() {
    webSocket.loop();
    if ( connected && lastUpdate+messageInterval<millis() ) {
      // Do whatever is necessary based on the time interval
      claimObject( t6Object_id );
      lastUpdate = millis();
    }
}
