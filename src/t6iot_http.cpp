/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/
#include "t6iot_http.h"
extern t6iot_Http t6iotHttp;
AsyncWebServer serverHttp(80);
const char* PARAM_INPUT_PIN               = "pin";
const char* PARAM_INPUT_VALUE             = "value";
const char* PARAM_INPUT_RESOLUTION        = "resolution";
const char* PARAM_INPUT_CHANNEL           = "channel";
const char* PARAM_INPUT_MODE              = "mode";
bool reboot                               = false;
static const char *ssdpTemplate = R"EOF(<?xml version="1.0"?>
<root xmlns="urn:schemas-upnp-org:device-1-0">
	<specVersion>
		<major>1</major>
		<minor>0</minor>
	</specVersion>
	<URLBase>http://%s/</URLBase>
	<device>
		<deviceType>%s</deviceType>
		<friendlyName>%s</friendlyName>
		<presentationURL>%s</presentationURL>
		<serialNumber>%s</serialNumber>
		<modelName>%s</modelName>
		<modelNumber>%s</modelNumber>
		<modelURL>%s</modelURL>
		<manufacturer>%s</manufacturer>
		<manufacturerURL>%s</manufacturerURL>
		<UDN>uuid:a774f8f2-c180-4e26-8544-cda0e6%02x%02x%02x</UDN>
		<iconList>
			<icon>
				<mimetype>image/format</mimetype>
				<width>32</width>
				<height>32</height>
				<depth>24h</depth>
				<url>icon-32x32.png</url>
			</icon>
		</iconList>
	</device>
</root>
)EOF";
int SSDP_localPortSSDP;
String SSDP_deviceType;
String SSDP_friendlyName;
String SSDP_modelName;
String SSDP_modelNumber;
String SSDP_modelURL;
String SSDP_manufacturer;
String SSDP_manufacturerURL;
int SSDP_advertiseInterval;

t6iot_Http::t6iot_Http() {
	Serial.println(F("t6 > t6iot_http Constructor"));
}

bool t6iot_Http::startHttp(int port) {
	if (!FILEFS.begin()) {
		Serial.println(F("t6 > An Error has occurred while mounting FS"));
		return 0;
	} else {
		Serial.println(F("t6 > FS mounted"));
		serverHttp.begin();
		Serial.println(F("t6 > HTTP listening to:"));
		Serial.print(F("t6 > http://"));
		Serial.print(WiFi.localIP().toString().c_str());
		Serial.print(F(":"));
		Serial.println(port);
		Serial.println(F("t6 > HTTP started"));
		return 1;
	}
}
void t6iot_Http::setSsdp(int localPortSSDP, String deviceType, String friendlyName, String modelName, String modelNumber, String modelURL, String manufacturer, String manufacturerURL, int advertiseInterval) {
	SSDP_localPortSSDP = localPortSSDP;
	SSDP_deviceType = deviceType;
	SSDP_friendlyName = friendlyName;
	SSDP_modelName = modelName;
	SSDP_modelNumber = modelNumber;
	SSDP_modelURL = modelURL;
	SSDP_manufacturer = manufacturer;
	SSDP_manufacturerURL = manufacturerURL;
	SSDP_advertiseInterval = advertiseInterval;
}
bool t6iot_Http::addStaticRoutes() {
	if(SSDP_friendlyName) {
		serverHttp.on("/ssdp/service-desc.xml", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->send(FILEFS, "/ssdp/service-desc.xml", "application/xml");
		});
	}
	serverHttp.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/index.html", "text/html");
	});
	serverHttp.on("/index", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/index.html", "text/html");
	});
	serverHttp.on("/sw.js", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/sw.js", "text/javascript");
	});
	serverHttp.on("/fonts/Material-Icons.woff2", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/fonts/Material-Icons.woff2", "font/woff2");
	});
	serverHttp.on("/t6show-min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/t6show-min.js", "text/javascript");
	});
	serverHttp.on("/t6show-min.js.map", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/t6show-min.js.map", "application/json");
	});
	serverHttp.on("/t6app.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/t6app.min.css", "text/css");
	});
	serverHttp.on("/object.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/object.css", "text/css");
	});
	serverHttp.on("/ui.js", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/ui.js", "application/javascript");
	});
	serverHttp.on("/robots.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/robots.txt", "plain/text");
	});
	serverHttp.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/favicon.ico", "image/x-icon");
	});
	serverHttp.on("/icon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/icon-16x16.png", "image/x-png");
	});
	serverHttp.on("/icon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/icon-32x32.png", "image/x-png");
	});
	serverHttp.on("/config.json", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/config.json", "application/json");
	});
	serverHttp.onNotFound([](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(FILEFS, "/404.html", "text/html");
		response->setCode(404);
		request->send(response);
	});
	return 1;
}
String t6iot_Http::getPinMode(int pin) {
	uint32_t bit = digitalPinToBitMask(pin);
	uint32_t port = digitalPinToPort(pin);
	volatile uint32_t *reg = portModeRegister(port);
	if (*reg & bit) {
		return "OUTPUT";
	} else {
		return "INPUT";
	}
}

bool t6iot_Http::subscribe_chan(String channel) {
	Serial.println("[WSc] subscribe object to WS server. Channel: " + channel);
	DynamicJsonDocument json(256);
	String databuf;
	json["command"] = "subscribe";
	json["channel"] = channel;
	serializeJson(json, databuf);
	//t6client.webSockets_sendTXT(databuf);
	return true;
}

bool t6iot_Http::unsubscribe_chan(String channel) {
	Serial.println("[WSc] unsubscribe object to WS server. Channel: " + channel);
	DynamicJsonDocument json(256);
	String databuf;
	json["command"] = "unsubscribe";
	json["channel"] = channel;
	serializeJson(json, databuf);
	//t6client.webSockets_sendTXT(databuf);
	return true;
}

bool t6iot_Http::addDynamicRoutes() {
	serverHttp.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest *request) {
		StreamString output;
		if (output.reserve(512)) {
			output.printf(ssdpTemplate,
				WiFi.localIP().toString().c_str(),
				String(SSDP_deviceType).c_str(),
				String(SSDP_friendlyName).c_str(),
				String("/description.xml").c_str(),
				String("object_id_uuidv4").c_str(),
				String(SSDP_modelName).c_str(),
				String(SSDP_modelNumber).c_str(),
				String(SSDP_modelURL).c_str(),
				String(SSDP_manufacturer).c_str(),
				String(SSDP_manufacturerURL).c_str(),
				(uint8_t)((ESP_GETCHIPID >> 16) & 0xff),
				(uint8_t)((ESP_GETCHIPID >> 8) & 0xff),
				(uint8_t) ESP_GETCHIPID & 0xff);
			request->send(200, "text/xml", (String) output);
		} else {
			request->send(500);
		}
	});
	serverHttp.on("/digitalWrite", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _pin = (request->getParam((PARAM_INPUT_PIN))->value());
		String _value = (request->getParam((PARAM_INPUT_VALUE))->value());
		if (_pin && _value) {
			Serial.print(F("pinMode: "));
			pinMode(_pin.toInt(), OUTPUT);
			Serial.println(F("OUTPUT"));
			if (_value.toInt() == 0) {
				digitalWrite(_pin.toInt(), LOW);
			} else if (_value.toInt() == 1) {
				digitalWrite(_pin.toInt(), HIGH);
			}
		}
		Serial.print(F("GPIO: "));
		Serial.print(_pin);
		Serial.print(F(" is set to: "));
		Serial.println(_value);
		request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"" + String(_value) + "\"}");
	});
	serverHttp.on("/analogWrite", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _pin = (request->getParam((PARAM_INPUT_PIN))->value());
		String _value = (request->getParam((PARAM_INPUT_VALUE))->value());
		if (_pin && _value) {
			Serial.print(F("pinMode: "));
			pinMode(_pin.toInt(), OUTPUT);
			Serial.println(F("OUTPUT"));
			analogWrite(_pin.toInt(), _value.toInt());
		}
		Serial.print(F("GPIO: "));
		Serial.print(_pin);
		Serial.print(F(" is set to: "));
		Serial.println(_value);
		request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"" + String(_value) + "\"}");
	});
	serverHttp.on("/setPinMode", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _pin = (request->getParam((PARAM_INPUT_PIN))->value());
		String _mode = (request->getParam((PARAM_INPUT_MODE))->value());
		if (_pin && _mode) {
			if (_mode == "INPUT") {
				pinMode(_pin.toInt(), INPUT);
				request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"INPUT\"}");
			} else if (_mode == "OUTPUT") {
				pinMode(_pin.toInt(), OUTPUT);
				request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"OUTPUT\"}");
			} else {
				pinMode(_pin.toInt(), OUTPUT);
				request->send(201, "application/json", "{\"status\": \"OK\", \"pin\": \"" + String(_pin) + "\", \"value\": \"OUTPUT\"}");
			}
		}
	});
	serverHttp.on("/analogRead", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _pin = (request->getParam((PARAM_INPUT_PIN))->value());
		String currentVal = "";
		int analogValue = 0;
		Serial.print(analogValue);
		if (_pin) {
			#ifdef ESP32
				//set the resolution to 12 bits (0-4096)
				analogReadResolution(12);
				analogValue = analogReadMilliVolts(_pin.toInt());
			#elif ESP8266
				int analogValue = analogRead(_pin.toInt());
			#endif
			currentVal = String(analogValue, DEC);
		}
		Serial.println("analogRead: " + currentVal);
		request->send(200, "application/json", "{\"status\": \"OK\", \"pin\": \"" + _pin + "\", \"value\": \"" + currentVal + "\"}");
	});
	serverHttp.on("/digitalRead", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _pin = (request->getParam((PARAM_INPUT_PIN))->value());
		String currentVal;
		if (_pin) {
			currentVal = String(digitalRead(_pin.toInt()), DEC);
		}
		Serial.println("digitalRead: " + currentVal);
		request->send(200, "application/json", "{\"status\": \"OK\", \"pin\": \"" + _pin + "\", \"value\": \"" + currentVal.toInt() + "\"}");
	});
	serverHttp.on("/audioOutput", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _value = (request->getParam((PARAM_INPUT_VALUE))->value());
		String message_decoded;
		if (_value) {
			/*
			 AudioOutputI2SNoDAC *audioOutputi2s;
			 audioOutputi2s = new AudioOutputI2SNoDAC();
			 ESP8266SAM *sam = new ESP8266SAM;
			 message_decoded = urldecode(_value);
			 const char* inputMessage2 = message_decoded.c_str();
			 sam->Say(audioOutputi2s, inputMessage2); // String(inputMessage2).c_str()
			 delete sam;
			 */
		}
		request->send(201, "application/json", "{\"status\": \"OK\", \"snack\": \"Message played " + message_decoded + "\"}");
	});
	serverHttp.on("/getValues", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _pin = (request->getParam((PARAM_INPUT_PIN))->value());
		String currentVal;
		String output;
		const byte allPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 };
		const byte COUNT = sizeof(allPins);
		if (_pin) {
			StaticJsonDocument < 4096 > jsonValues;
			JsonArray pins = jsonValues.createNestedArray("pins");
			for (int p = 0; p < COUNT; p++) {
				JsonObject npin = pins[p].createNestedObject("pin" + String(allPins[p]));
				npin["mode"] = t6iotHttp.getPinMode(allPins[p]);
				npin["type"] = "digital";
				npin["value"] = String(digitalRead((allPins[p])), DEC);
			}
			serializeJson(jsonValues, output);
			Serial.println(F(""));
			Serial.println(output);
		}
		request->send(200, "application/json", output);
	});
	serverHttp.on("/subscribe", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _channel = (request->getParam((PARAM_INPUT_CHANNEL))->value());
		if (_channel) {
			if (t6iotHttp.subscribe_chan(_channel)) {
				request->send(201, "application/json","{\"status\": \"OK\", \"subscribe\": \""+ _channel+ "\", \"subscriptions\": \"undefined\", \"snack\": \"Subscribed to channel '" + _channel + "'\"}");
			} else {
				request->send(412, "application/json",
						"{\"status\": \"NOT\", \"subscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Failed to subscribe to channel '" + _channel + "'\"}");
			}
		}
	});
	serverHttp.on("/unsubscribe", HTTP_GET, [](AsyncWebServerRequest *request) {
		String _channel = (request->getParam((PARAM_INPUT_CHANNEL))->value());
		if (_channel) {
			if (t6iotHttp.unsubscribe_chan(_channel)) {
				request->send(201, "application/json",
						"{\"status\": \"OK\", \"unsubscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Unsubscribed from channel '" + _channel + "'\"}");
			} else {
				request->send(412, "application/json", "{\"status\": \"NOT\", \"unsubscribe\": \"" + _channel + "\", \"subscriptions\": \"undefined\", \"snack\": \"Failed to unsubscribe to channel '" + _channel + "'\"}");
			}
		}
	});
	serverHttp.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
		/*
		 int headers = request->headers();
		 int i;
		 for(i=0;i<headers;i++){
		 AsyncWebHeader* h = request->getHeader(i);
		 Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
		 }
		 */
		String heap = String(ESP.getFreeHeap());
		request->send(200, "application/json", "{\"status\": \"OK\", \"heap\": \"" + heap + "\", \"snack\": \"Heap: " + heap + "\"}");
	});
	serverHttp.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(FILEFS, "/reboot.html", "text/html");
		reboot = true;
	});

	return 1;
}
