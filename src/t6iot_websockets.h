/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iotWebsockets_h
	#define t6iotWebsockets_h
	#include <WebSocketsClient.h>
	#include <ArduinoJson.h>
	#include <base64.h>
	#include "t6iot_audio.h"

	class t6iot_Websockets {
		public:
			t6iot_Websockets();
			bool startWebsockets(String wsHost, uint16_t wsPort, String wsPath, String t6wsKey, String t6wsSecret, int messageInterval, int reconnectInterval, int timeoutInterval, int disconnectAfterFailure, String o_id, String o_secret, t6iot_Audio t6iotWsAudio);
			bool isClaimed();
			void webSockets_loop();
			void claimObject();
			bool sendTXT(String data);

		private:
			String _object_id;
			String _object_secret;
			int _messageInterval;
			unsigned long _lastUpdate;
	};

	extern t6iot_Websockets t6iotWebsockets;
#endif
