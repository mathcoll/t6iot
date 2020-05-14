
#define SLEEP_DURATION_IN_SECONDS  1800    // 3600=60 minutes / 2700=45min

// Wifi
const char* ssid = ""; // Your Wifi SSID
const char* password = ""; // Your Wifi password

// t6 Server
char* httpHost = "api.internetcollaboratif.info;
int httpPort = 3000;
int timeout = 1000;
char* userAgent = "ST-8563+soilMoisture";

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
const int soilPower = 13; //Variable for Soil moisture Power
