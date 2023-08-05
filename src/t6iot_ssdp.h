/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iotSsdp_h
	#define t6iotSsdp_h
	#ifdef ESP8266
	  #define ESP_GETCHIPID ESP.getChipId()
	  #include <ESP8266SSDP.h>
	#elif ESP32
	  #define ESP_GETCHIPID (uint32_t)ESP.getEfuseMac()
	  #include <ESP32SSDP.h>
	#endif

	class t6iot_Ssdp {
		public:
			t6iot_Ssdp();
			bool startSsdp(int localPortSSDP, String deviceType, String friendlyName, String modelName, String modelNumber, String modelURL, String manufacturer, String manufacturerURL, int advertiseInterval);

		private:
	};
#endif