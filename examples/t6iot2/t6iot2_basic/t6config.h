#define WIFI_SSID             ""                // WIFI SSID & Password
#define WIFI_PASSWORD         ""
#define T6_USE_CUSTOM_SERVER  false             // You can use your own t6iot On Premise application
String host                   = "";             // .. Customer server IP or domain
int portHttp                  = 3000;           // .. and using a custom port for t6iot http
int portWs                    = 4000;           // .. and using a custom port for t6iot websockets

String object_name            = "customName";   // The name of the Object in the Mdns Local Area Network
String object_id              = "";             // t6 Object Id, this is used on the useragent as well
String object_secret          = "";             // optional t6 Object secret (32bits hexa) when using payload encryption
String flow_id                = "";             // t6 Flow Id that will contains and store the measurements
const char* api_key           = "";             // t6 api Key
const char* api_secret        = "";             // t6 api Secret

float sensorValue             = -1.0;           // This variable will store the measured value read by the sensor
const int PIN_PROBE           = 0;              // Analog data pin: 0
const int PWR_PROBE           = 13;             // VCC connected to sensor, this will be set as OUTPUT Power
const int measurements        = 10;             // Number of measurements to make sure measurement is accurate
const long READINTERVAL       = 30 * 60;        // Interval between each READ in seconds // 30 minutes
uint8_t                       readTask;         // All tasks that can be cancelled
const long SLEEP_DELAY_IN_SECONDS = 15 * 60;    // Sleep duration. // 1800=30 minutes / 3600=60 minutes / 2700=45min
