/*
 * 
 * 
 * FS 256kb / OTA 374kb
 * openssl
 * req -x509 -newkey rsa:1024 -sha256 -keyout key.txt -out cert.txt -days 365 -nodes -subj "/C=FR/ST=CA/L=Paris/O=ESP [FR]/OU=ESP/CN=esp8266.local" -addext subjectAltName=DNS:esp8266.local,IP:192.168.0.28
 * 
 * 
*/
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
const char* wwwUsername = "admin";                                // Optional Username to call Object Api, set to "" to disable this authentication
const char* wwwPassword = "esp8266";                              // Optional Password to call Object Api, set to "" to disable this authentication

// t6 Flow container for Sensor data
char* t6FlowId = "7774c70a-551a-4f1c-b78c-efa836835b14";          // 
char* t6Mqtt_topic = "";                                          // 
char* t6Unit = "%";                                               // 
char* t6Save = "false";                                           // 
char* t6Publish = "false";                                        // 

const char* ssid = "";                                            // Your own Wifi ssid to connect to
const char* password = "";                                        // Your wifi password

const long READInterval = 3 * 60;                                 // Interval between each READ -> 3 minutes
const long JWTRefreshInterval = 4 * 60;                           // Should be less than server -> 4 minutes are fair
const long POSTInterval = 30 * 60;                                // Interval between each POST -> 30 minutes
const long POLLInterval = 100;                                    // Interval for polling ; it should be short -> 100ms
float sensorValue = -1.0;                                         // Value read from the sensor

uint8_t jwtrTask, pollTask, readTask, postTask;                   // All tasks that can be cancelled

String html = "<html>\n"
  "<head></head>\n"
  "<body>\n"
  "<h1>ESP demo FROM ino file</h1>\n"
  "<ul>\n"
  "<li><a href='/open'>open</a></li>\n"
  "<li><a href='/close'>close</a></li>\n"
  "<li><a href='/getVal'>getVal</a></li>\n"
  "<!--<li><a href='/setVal'>setVal</a></li>-->\n"
  "<li><a href='/on'>on</a></li>\n"
  "<li><a href='/off'>off</a></li>\n"
  "<li><a href='/upper'>upper</a></li>\n"
  "<li><a href='/lower'>lower</a></li>\n"
  "<li><a href='/true'>True</a></li>\n"
  "<li><a href='/false'>False</a></li>\n"
  "<li><a href='/refresh'>Refresh UI</a></li>\n"
  "<li><a href='/upgrade'>Upgrade OTA</a></li>\n"
  "</ul>\n"
  "</body>\n"
  "</html>\n";

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
-----END PRIVATE KEY-----
)EOF";

T6iot t6Client;                                                   // Init T6iot Client named "t6Client"

void setup() {
  Serial.println("Booting ESP..");
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);                                 // Turn the LED on ...
  startWiFi();                                                    // Obviously, the wifi initialization :-)
  printIPAddressOfHost(t6HttpHost);                               // Some custom wifi logs :-)
  digitalWrite(LED_BUILTIN, HIGH);                                // ... and then, wifi is set, turn light off

  t6Client.DEBUG = false;                                         // Activate or disable DEBUG mode
  t6Client.init(t6HttpHost, t6HttpPort, t6UserAgent, t6Timeout);  // This will initialize the t6 Client according to server
  t6Client.initObject(t6ObjectId, secret);                        // Initialize t6 Object with its uuid-v4, it's secret if you need to sign payload

  //t6Client.getStatus();                                           // Get t6 Status
  
  t6Client.setCredentials(t6Username, t6Password);                // This will define your own personal username/password to connect to t6
  t6Client.authenticate();                                        // Generate a JWT from your personnal credential on t6 server

  //t6Client.setHtml(html);                                         // Set html into the Object
  t6Client.setHtml();                                             // Or Fetch it from t6 api
  //t6Client.scheduleOnce(2, setHtml, TIME_SECONDS);                // Or delay it to few seconds

  //t6Client.activateOTA();                                         // Activating Over The Air (OTA) update procedure

  t6Client.setWebServerCredentials(wwwUsername, wwwPassword);     // Define credentials for webserver on the Object
  t6Client.startWebServer(serverCert, serverKey);                 // Starting to listen from the Object on Http Api

  jwtrTask = t6Client.scheduleFixedRate(JWTRefreshInterval, refreshToken, TIME_SECONDS);   // Refresh JWT and must be after an authenticate
  pollTask = t6Client.scheduleFixedRate(POLLInterval, doServerQueries, TIME_MILLIS);       // Polling library, using milliseconds as TimerUnit
  readTask = t6Client.scheduleFixedRate(READInterval, readSample, TIME_SECONDS);           // Read sensor Value regularly
  postTask = t6Client.scheduleFixedRate(POSTInterval, postSample, TIME_SECONDS);           // POST Value regularly
  t6Client.scheduleOnce(0, setupComplete, TIME_SECONDS); // Run only once the setup Complete method
}

void loop() {
  t6Client.runLoop();                                             // t6 TaskManager
  t6Client.handleClient();                                        // Handling t6 Object http connexion, only when WebServer is activated
}

void refreshToken() {
  Serial.println("refreshToken() called");
  t6Client.refreshToken();
}

void setOn() {
  Serial.println("setOn() called");
  jwtrTask = t6Client.scheduleFixedRate(JWTRefreshInterval, refreshToken, TIME_SECONDS);   // Refresh JWT and must be after an authenticate
  pollTask = t6Client.scheduleFixedRate(POLLInterval, doServerQueries, TIME_MILLIS);       // Polling library, using milliseconds as TimerUnit
  readTask = t6Client.scheduleFixedRate(READInterval, readSample, TIME_SECONDS);           // Read sensor Value regularly
  postTask = t6Client.scheduleFixedRate(POSTInterval, postSample, TIME_SECONDS);           // POST Value regularly
}

void setOff() {
  Serial.println("setOff() called");
  t6Client.cancelTask(postTask);                                // Stop a task from executing again if it is a repeating task
  t6Client.cancelTask(jwtrTask);                                // Stop a task from executing again if it is a repeating task
}

void readSample() {
  Serial.println("readSample() called");
  sensorValue++;
  Serial.println(String("Updating sensorValue to: ")+sensorValue);
  t6Client.setValue(sensorValue);                                // Updating t6 with the sensor value
}

void postSample() {
  Serial.println(String("Posting sensorValue to t6: ")+sensorValue);
  readSample();
  if (sensorValue > -1) {
    t6Client.lockSleep(t6Timeout);                                // Lock the sleep, so the Object can't get into deep sleep mode when posting
    
    DynamicJsonDocument payload(1024);                            // Building payload to post
    payload[String("value")] = sensorValue;
    payload[String("flow_id")] = t6FlowId;
    payload[String("mqtt_topic")] = t6Mqtt_topic;
    payload[String("unit")] = t6Unit;
    payload[String("save")] = t6Save;
    payload[String("publish")] = t6Publish;
    t6Client.createDatapoint(t6FlowId, payload);                  // Create a datapoint on t6
    t6Client.unlockSleep();                                       // Unlock the sleep mode
  }
}

void doServerQueries() {                                          // This section contains the triggers on www events
  String messageArrived = t6Client.pollWebServer();               // Poll Web Server to check if something happened
  if(messageArrived == "on") {                                    // Trigger if message is on
    //postSample();                                                 // Trigger postSample()
    setOn();

  } else if(messageArrived == "off") {                            // Trigger if message is off
    Serial.println("Set OFF and cancel all t6 api calls");        // But still readTask remain active so we can turn it on again
    setOff();

  } else if(messageArrived == "open") {                           // Trigger if message is open
    Serial.println("Opening something");

  } else if(messageArrived == "close") {                          // Trigger if message is close
    Serial.println("Closing something");

  } else if(messageArrived == "upper") {                          // Trigger if message is upper
    Serial.println("Increase volume");

  } else if(messageArrived == "lower") {                          // Trigger if message is lower
    Serial.println("Decrease volume");

  } else if(messageArrived == "true") {                           // Trigger if message is true
    Serial.println("Boolean value is Activated");
    digitalWrite(LED_BUILTIN, LOW);                               // Turn the LED on

  } else if(messageArrived == "false") {                          // Trigger if message is false
    Serial.println("Boolean value is Disabled");
    digitalWrite(LED_BUILTIN, HIGH);                              // Turn the LED off

  } else if(messageArrived != "") {
    Serial.print("UKN message arrived: ");
    Serial.println(messageArrived);
  }
}

void setupComplete() {
  Serial.println("Ready.");
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
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    Serial.print(".");
    ESP.restart();
  }
}
