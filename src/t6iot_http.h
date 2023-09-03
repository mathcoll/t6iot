/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iotHttp_h
	#define t6iotHttp_h
	#include <LittleFS.h>
	#define FILEFS LittleFS
	#include <ESPAsyncWebServer.h>
	#include <ArduinoJson.h>
	#include <StreamString.h>
	#ifdef ESP8266
	  #define ESP_GETCHIPID ESP.getChipId()
	#elif ESP32
	  #define ESP_GETCHIPID (uint32_t)ESP.getEfuseMac()
	#endif

	class t6iot_Http {
		public:
			t6iot_Http();
			bool startHttp(int port);
			bool addStaticRoutes();
			bool addDynamicRoutes();
			void setSsdp(int localPortSSDP, String deviceType, String friendlyName, String modelName, String modelNumber, String modelURL, String manufacturer, String manufacturerURL, int advertiseInterval);

		private:
			String getPinMode(int pin);
			bool subscribe_chan(String channel);
			bool unsubscribe_chan(String channel);
	};
#endif
