/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/
#include "t6iot_mdns.h"

t6iot_Mdns::t6iot_Mdns() {
	Serial.println("t6 > t6iot_Mdns Constructor");
}

bool t6iot_Mdns::startMdns(String friendlyName, int localPortMDNS) {
	MDNS.begin(String(friendlyName).c_str());
	MDNS.addService("http", "tcp", localPortMDNS);
	Serial.println("t6 > MDNS started");
	return 1;
}
