/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/
#include "t6iot_ssdp.h"

t6iot_Ssdp::t6iot_Ssdp() {
	Serial.println(F("t6 > t6iot_Ssdp Constructor"));
}

bool t6iot_Ssdp::startSsdp(int localPortSSDP, String deviceType, String friendlyName, String modelName, String modelNumber, String modelURL, String manufacturer, String manufacturerURL, int advertiseInterval) {
	SSDP.setSchemaURL("description.xml");
	SSDP.setHTTPPort(localPortSSDP);
	SSDP.setDeviceType(deviceType);
	SSDP.setName(friendlyName);
	SSDP.setSerialNumber(String(ESP_GETCHIPID));
	SSDP.setModelName(modelName);
	SSDP.setModelNumber(modelNumber);
	SSDP.setModelURL(modelURL);
	SSDP.setManufacturer(manufacturer);
	SSDP.setManufacturerURL(manufacturerURL);
	SSDP.setInterval(advertiseInterval);
	SSDP.setTTL(advertiseInterval);
	SSDP.setUUID("a774f8f2-c180-4e26-8544");
	SSDP.setURL("/");
	#if defined(ESP32)
		SSDP.setServerName("t6iot/2.0.4");
		SSDP.setModelDescription("Connect Arduino Sensors to t6 framework Saas or On Premise.");
	#endif
	SSDP.begin();
	Serial.println(F("t6 > SSDP started"));
	return 1;
}
