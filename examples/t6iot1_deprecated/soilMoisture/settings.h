/*
  settings.h - 
  Created by mathieu@internetcollaboratif.info <Mathieu Lory>.

  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
  
*/

const char* ssid = ""; // Your Wifi SSID
const char* password = ""; // Your Wifi password

// t6 Server
char* httpHost = "192.168.0.21";
int httpPort = 3000;
int timeout = 1000;
char* userAgent = "soilMoisture";

// JWT Authentication
const char* t6Username = ""; // Your t6 username
const char* t6Password = ""; // Your t6 password
const char* t6Key = ""; // Your t6 Key
const char* t6Secret = ""; // Your t6 Secret

// Object
char* objectId = ""; // The current object ID
const char* secret = ""; // The current object secret for signature
char* t6FlowId = ""; // The flow to create datapoints
const int VAL_PROBE = 0; // Analog pin 0
#define SLEEP_DELAY_IN_SECONDS 1800 // Delay before going to sleep mode. 1800 = 30 minutes

;
