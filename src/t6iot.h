/*
  t6iot.h - v2.0.2
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iot_h
#define t6iot_h
#include "Arduino.h"
#include <ArduinoJson.h>
#include <ArduinoJWT.h>
#include <WiFiClientSecure.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#else
  #include <WiFi.h>
  #include <ArduinoHttpClient.h>
#endif

class t6iot {
	public:
		t6iot();
		void set_server();
		void set_server(String host);
		void set_server(String host, int port);
		void set_server(String host, int port, String useragent);
		void set_key(const char* key);
		void set_secret(const char* secret);
		void set_object_id(String object_id);
		void set_object_secret(String secret);
		void set_wifi(const String& ssid, const String& pass);
		int createDatapoint(DynamicJsonDocument& payload);
		int createDatapoints(DynamicJsonDocument& payload);

	private:
		void set_endpoint(const String& endpoint);
		String _ssid;
		String _password;
		String _endpoint;
		String _httpHost;
		String _httpProtocol;
		String _object_id;
		String _object_secret;
		String _userAgent;
		int _httpPort;
		const char* _key;
		const char* _secret;
		String _getSignedPayload(String& payload, String& object_id, String& object_secret);
};
#endif
