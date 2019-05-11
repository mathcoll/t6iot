/*
  settings.h - 
  Created by mathieu@internetcollaboratif.info <Mathieu Lory>.

  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
  
*/

const char* ssid = ""; // Your Wifi SSID
const char* password = ""; // Your Wifi password

// t6 Server
char* httpHost = "api.internetcollaboratif.info";
int httpPort = 80;
int timeout = 1000;
char* userAgent = "Sample";

// JWT Authentication
const char* t6Username = ""; // Your t6 username
const char* t6Password = ""; // Your t6 password

// Object
char* objectId = "xxxxxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"; // The current object ID
const char* secret = ""; // The current object secret for signature
char* t6FlowId = "xxxxxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" // The flow to create datapoints
#define SLEEP_DELAY_IN_SECONDS 1800 // Delay before going to sleep mode. 1800 = 30 minutes

;
