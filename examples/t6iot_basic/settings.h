/*
  settings.h - 
  Created by mathieu@internetcollaboratif.info <Mathieu Lory>.

  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
  
*/

const char* ssid = ""; // Your Wifi SSID
const char* password = ""; // Your Wifi password

// t6 Server
char* t6HttpHost = "api.internetcollaboratif.info"; // t6 server IP Address
int httpPort = 3000;
int timeout = 1000;
char* userAgent = "Sample";

// JWT Authentication
const char* t6Username = ""; // Your t6 username
const char* t6Password = ""; // Your t6 password
const char* t6Key = ""; // Your t6 Key
const char* t6Secret = ""; // Your t6 Secret

// Object
char* objectId = "4"; // The current object ID
const char* secret = ""; // The current object secret for signature
char* t6FlowId = "6" // The flow to create datapoints
#define SLEEP_DELAY_IN_SECONDS 1800 // Delay before going to sleep mode. 1800 = 30 minutes

;
