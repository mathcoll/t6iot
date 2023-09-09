/*
  t6iot - v2.0.4
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iot_h
	#define t6iot_h
	#include "Arduino.h"
	#include <ArduinoOTA.h>
	#include <ArduinoJWT.h>
	#include <TaskManager.h>
	#include <WiFiClientSecure.h>
	#include "t6iot_ssdp.h"
	#include "t6iot_mdns.h"
	#include "t6iot_http.h"
	#include "t6iot_audio.h"
	#include "t6iot_websockets.h"

	#if defined(ESP8266)
		#include <ESP8266WiFi.h>
		#include <ESP8266HTTPClient.h>
		#include <ArduinoHttpClient.h>
		#include <WiFiClientSecureBearSSL.h>
	#elif ESP32
		#include <WiFi.h>
		#include <HTTPClient.h>
//		#include <ArduinoBearSSL.h>
		#include <WiFiClientSecure.h>
//		#include <WiFiClientSecureBearSSL.h>
		#define LED_BUILTIN 2
	#endif

	class t6iot: public TaskManager {
		public:
			t6iot();
			void set_server();
			void set_server(String host);
			void set_server(String host, int port);
			void set_server(String host, int port, String useragent);
			void set_useragent(String useragent);
			void set_key(const char* key);
			void set_secret(const char* secret);
			void set_object_id(String object_id);
			void set_object_secret(String secret);
			void set_wifi(const String& ssid, const String& pass);
			int createDatapoint(DynamicJsonDocument& payload);
			int createDatapoints(DynamicJsonDocument& payload);

			bool isLocked();
			void lockSleep();
			void lockSleep(const long dur);
			bool setSleepDuration(const long dur);
			void unlockSleep();
			void goToSleep();
			void goToSleep(const long dur);

			bool startSsdp();
			bool startSsdp(String friendlyName);

			bool startMdns();
			bool startMdns(String friendlyName);
			bool startMdns(String friendlyName, int DEFAULT_portHTTP);
			bool startMdns(String friendlyName, int DEFAULT_portHTTP, int DEFAULT_portWEBSOCKETS);
			void mdns_loop();

			bool startWebsockets();
			bool startWebsockets(String host, int port);
			bool webSockets_sendTXT(String data);
			bool isClaimed();
			void webSockets_loop();

			bool startAudio();
			bool audioListenTo(const char* url);
			bool audioSetVol(int vol);
			void audio_loop();

			bool startHttp();
			bool startHttp(int port);
			bool addStaticRoutes();
			bool addDynamicRoutes();
			void activateOTA();
			bool _http_started;
			bool _ssdp_started;
			bool _mdns_started;
			bool _audio_started;
			bool _websockets_started;

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
			bool _locked;
			bool _OTA_activated;
			const char* _key;
			const char* _secret;
			long _sleepDuration;
			String _getSignedPayload(String& payload, String& object_id, String& object_secret);
	};
	extern t6iot t6client;
#endif
