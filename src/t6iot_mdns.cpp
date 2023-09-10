/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/
#include "t6iot_mdns.h"

t6iot_Mdns::t6iot_Mdns() {
	Serial.println(F("t6 > t6iot_Mdns Constructor"));
}

bool t6iot_Mdns::startMdns(String friendlyName, int portHTTP) {
	return startMdns(friendlyName, portHTTP, 0, true, false);
}

bool t6iot_Mdns::startMdns(String friendlyName, int portHTTP, int portWEBSOCKETS) {
	return startMdns(friendlyName, portHTTP, portWEBSOCKETS, true, true);
}

bool t6iot_Mdns::startMdns(String friendlyName, int portHTTP, int portWEBSOCKETS, bool http_started, bool websockets_started) {
	int attempt = 10;

	#if defined(ESP8266)
		if ( !MDNS.begin(friendlyName) ) {
			Serial.println(F("t6 > MDNS error!"));
			while (attempt>0) { attempt--; delay(1000); }
		}
	#elif ESP32
		if ( !MDNS.begin(friendlyName.c_str()) ) {
			Serial.println(F("t6 > MDNS error!"));
			while (attempt>0) { attempt--; delay(1000); }
		}
	#endif
	if (http_started) {
		MDNS.addService("http", "tcp", portHTTP);
		Serial.println(F("t6 > MDNS Http service announced"));
	} else {
		Serial.println(F("t6 > MDNS Http service disabled, not announced!"));
	}
	if (websockets_started) {
		MDNS.addService("socket", "tcp", portWEBSOCKETS);
		Serial.println(F("t6 > MDNS Socket service announced"));
	} else {
		Serial.println(F("t6 > MDNS Socket service disabled, not announced!"));
	}
	Serial.println(F("t6 > MDNS started"));
	return 1;
}
void t6iot_Mdns::mdns_loop() {
	#if defined(ESP8266)
		MDNS.update();
	#elif ESP32
	#endif
}
