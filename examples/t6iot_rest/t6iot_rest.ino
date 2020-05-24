#include <ESP8266WiFi.h>
#include <t6iot.h>

// t6 Server
char* t6HttpHost = "api.internetcollaboratif.info";            // t6 server IP Address
int t6HttpPort = 443;                                          // t6 port
int t6Timeout = 3000;                                          // t6 timeout to get an answer from server

// t6 JWT Authentication
const char* t6Username = "";                                   // Your t6 Username
const char* t6Password = "";                                   // Your t6 Password
                                                               // or :
const char* t6Key = "";                                        // Your t6 Key
const char* t6Secret = "";                                     // Your t6 Secret

// t6 Object
char* secret = "";                                             // The current object secret for signature
char* t6ObjectId = "d3ba128c-61c4-4110-8a71-2a83461b86ef";     // The Object uuid-v4 in t6
char* t6UserAgent = "nodeMCU.28";                              // 
const char* t6ObjectWww_username = "admin";                    // 
const char* t6ObjectWww_password = "esp8266";                  // 
const char* t6ObjectWww_realm = "ESP Auth Realm";              // 

// t6 Flow container for Sensor data
char* t6FlowId = "75317bf5-001f-49cb-ad91-626cf43aa0cf";       // 
char* t6Mqtt_topic = "";                                       // 
char* t6Unit = "mBar";                                         // 
char* t6Save = "true";                                         // 
char* t6Publish = "true";                                      // 

const char* ssid = "";                                         // Your own Wifi ssi to connect to
const char* password = "";                                     // Your wifi password

const long POSTInterval = 180000;                              // interval between each POST -> 30 minutes
const long READInterval = 60 * 1000;                           // interval between each READ -> 1 minute
float sensorValue = -1.0;                                      // Init the sensor value
unsigned long POSTlast = -1;
unsigned long READlast = -1;

T6iot t6Client;                                                // Init T6iot Client named "t6Client"

void setup() {
  Serial.println("Booting ESP..");
  Serial.begin(115200);
  
  startWiFi();                                                    // Obviously, the wifi initialization :-)

  t6Client.init(t6HttpHost, t6HttpPort, t6UserAgent, t6Timeout);  // This will initialize the t6 Client according to server
  t6Client.setCredentials(t6Username, t6Password);                // This will define your own personal username/password to connect to t6
  t6Client.activateOTA();                                         // Activating Over The Air (OTA) update procedure
  t6Client.setWebServerCredentials(t6ObjectWww_username, t6ObjectWww_password, t6ObjectWww_realm); // Define redentials for webserver on the Object
  t6Client.initObject(t6ObjectId, secret, t6UserAgent);           // 
  //t6Client.setFlow();                                             // 
  t6Client.startWebServer();                                      // Starting to listen from the Object on Http Api
}

void loop() {

  if (millis() - READlast >= READInterval) {                      // Reading only when necessary
    readSample();
    READlast = millis();
  }
  
  t6Client.handleClient();                                        // Handling t6 Object http connexion, only when WebServer is activated

  if (sensorValue > -1 && ((millis() - POSTlast >= POSTInterval) || POSTlast == -1)) {
    t6Client.lockSleep(t6Timeout);                                // Lock the sleep, so the Object can't get into deep sleep mode when posting
    
    t6Client.authenticate();                                      // Generate a JWT from your personnal credential on t6 server

    // Building payload to post
    const int BUFFER_SIZE = JSON_OBJECT_SIZE(6);
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
    JsonObject& payload = jsonBuffer.createObject();
    payload["value"] = sensorValue;
    payload["flow_id"] = t6FlowId;
    payload["mqtt_topic"] = t6Mqtt_topic;
    payload["unit"] = t6Unit;
    payload["save"] = t6Save;
    payload["publish"] = t6Publish;
    t6Client.createDatapoint(t6FlowId, payload);                  // Create a datapoint on t6
    t6Client.unlockSleep();                                       // Unlock the sleep mode

    POSTlast = millis();
  }
}

void readSample() {
  Serial.println("readSample");
  sensorValue++;                                                 // For the example file, the sensor read an incremented value
  Serial.println(String("Updating sensorValue to: ")+sensorValue);
  t6Client.setValue(sensorValue);                                // Updating t6 with the sensor value
}

void startWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
}
