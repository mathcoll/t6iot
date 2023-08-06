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



	class t6iot_Http {
		public:
			t6iot_Http();
			bool startHttp(int port);
			bool addStaticRoutes();
			bool addDynamicRoutes();

		private:
			String getPinMode(int pin);
			bool subscribe_chan(String channel);
			bool unsubscribe_chan(String channel);
	};
#endif
