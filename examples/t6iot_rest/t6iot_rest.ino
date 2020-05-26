#include <ESP8266WiFi.h>
#include <t6iot.h>

// t6 Server
char* t6HttpHost = "api.internetcollaboratif.info";               // t6 server IP Address, actually, it can't be updated only from this parameter, require SSL fingerprint in the library too
int t6HttpPort = 443;                                             // t6 port
int t6Timeout = 3000;                                             // t6 timeout to get an answer from server

// t6 JWT Authentication
const char* t6Username = "demo";                                  // Your t6 Username
const char* t6Password = "?[W{7kG'X63-e0N";                       // Your t6 Password
                                                                  // or :
const char* t6Key = "";                                           // Your t6 Key
const char* t6Secret = "";                                        // Your t6 Secret

// t6 Object
char* secret = "";                                                // The object secret, for signature
char* t6ObjectId = "092579ba-3dd6-4c03-982e-0ecc66033609";        // The Object uuid-v4 in t6
char* t6UserAgent = "nodeMCU.28";                                 // The userAgent used when calling t6 api
const char* t6ObjectWww_username = "admin";                       // Optional Username to call Object Api, set to "" to disable this authentication
const char* t6ObjectWww_password = "esp8266";                     // Optional Password to call Object Api, set to "" to disable this authentication

// t6 Flow container for Sensor data
char* t6FlowId = "7774c70a-551a-4f1c-b78c-efa836835b14";          // 
char* t6Mqtt_topic = "";                                          // 
char* t6Unit = "%";                                               // 
char* t6Save = "true";                                            // 
char* t6Publish = "true";                                         // 

const char* ssid = "";                                            // Your own Wifi ssi to connect to
const char* password = "";                                        // Your wifi password

const long POSTInterval = 180000;                                 // Interval between each POST -> 30 minutes
const long READInterval = 60 * 1000;                              // Interval between each READ -> 1 minute
float sensorValue = -1.0;                                         // Init the sensor value
unsigned long POSTlast = -1;                                      // This is just to know when last Post was called
unsigned long READlast = -1;                                      // This is just to know when last sensor read was done

String html = "<html>\n"
  "<head></head>\n"
  "<body>\n"
  "<h1>ESP demo</h1>\n"
  "<ul>\n"
  "<li><a href='/open'>open</a></li>\n"
  "<li><a href='/close'>close</a></li>\n"
  "<li><a href='/getVal'>getVal</a></li>\n"
  "<!--<li><a href='/setVal'>setVal</a></li>-->\n"
  "<li><a href='/on'>on</a></li>\n"
  "<li><a href='/off'>off</a></li>\n"
  "<li><a href='/upper'>upper</a></li>\n"
  "<li><a href='/lower'>lower</a></li>\n"
  "<li><a href='/upgrade'>upgrade</a></li>\n"
  "</ul>\n"
  "</body>\n"
  "</html>\n";

T6iot t6Client;                                                   // Init T6iot Client named "t6Client"

void setup() {
  Serial.println("Booting ESP..");
  Serial.begin(115200);

  startWiFi();                                                    // Obviously, the wifi initialization :-)
  printIPAddressOfHost(t6HttpHost);

  t6Client.init(t6HttpHost, t6HttpPort, t6UserAgent, t6Timeout);  // This will initialize the t6 Client according to server
  t6Client.DEBUG = false;                                         // Activate or disable DEBUG mode
  t6Client.setCredentials(t6Username, t6Password);                // This will define your own personal username/password to connect to t6
  t6Client.initObject(t6ObjectId, secret, t6UserAgent);           // 
  t6Client.activateOTA();                                         // Activating Over The Air (OTA) update procedure
  t6Client.setWebServerCredentials(t6ObjectWww_username, t6ObjectWww_password); // Define credentials for webserver on the Object
  //t6Client.setHtml(html);                                         // Set html into the Object
  t6Client.setHtml();                                             // Or fetch it from t6 api
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

void printIPAddressOfHost(const char* host) {
  IPAddress resolvedIP;
  if (!WiFi.hostByName(host, resolvedIP)) {
    Serial.print(F("Host lookup failed for "));
    Serial.println(host);
  }
  Serial.print(F("Host: "));
  Serial.print(host);
  Serial.print(", IP: ");
  Serial.println(resolvedIP.toString().c_str());
}
void startWiFi() {
  Serial.println();
  Serial.print("Connecting to Wifi SSID: ");
  Serial.println(ssid);
  /*
  IPAddress ip(192, 168, 0, 100);
  IPAddress gateway(192, 168, 0, 255);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns1(8.8.8.8);
  IPAddress dns2(8.8.8.4);
  WiFi.config(ip, gateway, dns1, dns2);
  */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    Serial.print(".");
    ESP.restart();
  }
  Serial.println("Wifi: OK");
  Serial.print("DNS address 1:");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS address 2:");
  Serial.println(WiFi.dnsIP(1));
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
  Serial.print("Signal Power:");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}
