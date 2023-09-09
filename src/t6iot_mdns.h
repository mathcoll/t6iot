/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iotMdns_h
	#define t6iotMdns_h
	#ifdef ESP8266
		#include <ESP8266mDNS.h>
	#elif ESP32
		#include <ESPmDNS.h>
	#endif



	class t6iot_Mdns {
		public:
			t6iot_Mdns();
			bool startMdns();
			bool startMdns(String friendlyName);
			bool startMdns(String friendlyName, int portHTTP);
			bool startMdns(String friendlyName, int portHTTP, int portWEBSOCKETS);
			bool startMdns(String friendlyName, int portHTTP, int portWEBSOCKETS, bool http_started, bool websockets_started);
			void mdns_loop();

		private:
	};
#endif
