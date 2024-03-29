/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#include "t6iot.h"
// nmap --script ssl-cert.nse -p 443 api.internetcollaboratif.info | grep SHA-1
// openssl s_client -connect api.internetcollaboratif.info:443 -prexit -showcerts -state -status -tlsextdebug -verify 10

String VERSION = "2.0.8";

const char *fingerprint = "d057 5a5f 76b5 5e3f 8e56 89ca a69e 7bf0 87bb c0e2";
const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFkzCCBHugAwIBAgIRAOq5AVmXVoKBDqdAm7bv1xAwDQYJKoZIhvcNAQELBQAw
RjELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM
TEMxEzARBgNVBAMTCkdUUyBDQSAxUDUwHhcNMjQwMzAzMTEyNjI5WhcNMjQwNjAx
MTEyNjI4WjAkMSIwIAYDVQQDExlpbnRlcm5ldGNvbGxhYm9yYXRpZi5pbmZvMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtiJW1x21Y8M6ACoztp6MywzY
ekbFoqpRkigijGKPbcN8kswK1++MFFiIOBVfQ15eV38YZ6MkJbXzMytxkgaV29Xy
UQhF1hTveSOlYWaBcYzXJKHl0TkXfqBkHzpIUUKfaQ2496lO4rKKP9qJUz+MtWKi
i0RqtBmWsjudfs4kn4vG1REA14pG2csTSy7Gs3NlByuoZsgQiHFaY9LIeNwWkQxa
85EVEcyn4ZXB4IgKBEeRuo3IyP8o961Xo83B92H8FrNBnY4hFFvrHrUuxcSQprv5
zvpXQKFdcdYUoahC52eyWrBUMpqtqlYW8DDQnGsR3FWy+yG7coQrrQu3aJwQ8wID
AQABo4ICnDCCApgwDgYDVR0PAQH/BAQDAgWgMBMGA1UdJQQMMAoGCCsGAQUFBwMB
MAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFOfOe57YJuXXphEwtEL+QbnZJm3zMB8G
A1UdIwQYMBaAFNX8ng3fHsrdCJeXbivFX8Ur9ey4MHgGCCsGAQUFBwEBBGwwajA1
BggrBgEFBQcwAYYpaHR0cDovL29jc3AucGtpLmdvb2cvcy9ndHMxcDUvM29Qcm5I
Q2Q0UVEwMQYIKwYBBQUHMAKGJWh0dHA6Ly9wa2kuZ29vZy9yZXBvL2NlcnRzL2d0
czFwNS5kZXIwQQYDVR0RBDowOIIZaW50ZXJuZXRjb2xsYWJvcmF0aWYuaW5mb4Ib
Ki5pbnRlcm5ldGNvbGxhYm9yYXRpZi5pbmZvMCEGA1UdIAQaMBgwCAYGZ4EMAQIB
MAwGCisGAQQB1nkCBQMwPAYDVR0fBDUwMzAxoC+gLYYraHR0cDovL2NybHMucGtp
Lmdvb2cvZ3RzMXA1L2F4VzBlbGNZNkhzLmNybDCCAQMGCisGAQQB1nkCBAIEgfQE
gfEA7wB1AHb/iD8KtvuVUcJhzPWHujS0pM27KdxoQgqf5mdMWjp0AAABjgRHriwA
AAQDAEYwRAIgOU3q/qJxfVvdoc9tjavAnbbqG2FWsb1+q1uTHRktIbkCIGufHWqh
o2EEAo90jdF8b3XkJ0lt0hYmSgvSxoIM/TzxAHYA2ra/az+1tiKfm8K7XGvocJFx
bLtRhIU0vaQ9MEjX+6sAAAGOBEeuKAAABAMARzBFAiEA8BeERDFBAYE/vlNVocX0
S4J/cZOUusSBxP6S9YwBic4CIBETKn4NTj7oZfXDmIVfM0Jugcp1UNgomv2eF9bJ
r1FlMA0GCSqGSIb3DQEBCwUAA4IBAQCeVTFjsd/zEfgcRG86GdvsmllAMmXgN86h
WKn0oPs3LtfHn8WVyXvqKjLoygcmQMDlFZfWUn8CktG5+aw3rOGGg0FgYub2yJ/3
5C87f85NAZjGBsMek+wNWm/oF9mcH+nQld0ukZQ/pjrg57PQwLtzAIspQ5amLCHG
RI4QdyWXfSj5cE7EsZCPXcHAs+0n9fPNBE7pUGPj4i5R8NAoeou4OedyqXwfbvl+
hJLGQAjVQR7gJGG5dvlEydg0t8Z0YuhRRCIyHUYtjIgZ0KxFFJTaeYoscROX0cBy
2bTUS/UOhXPDvrQ2PiqU2NWmW94HM8cl+efG/LKBNEZtUx7HTG1x
-----END CERTIFICATE-----
)EOF";
IPAddress							dns(8, 8, 8, 8); //Google dns
String DEFAULT_useragent			= "t6iot-library/"+String(VERSION)+" (Arduino; rv:2.2.0; +https://www.internetcollaboratif.info)";
String DEFAULT_friendlyName 		= "t6ObjectLib";
String DEFAULT_host					= "api.internetcollaboratif.info";
String DEFAULT_host_ws				= "ws.internetcollaboratif.info";
int DEFAULT_port					= 443;
int DEFAULT_port_ws					= 443;
int DEFAULT_messageInterval 		= 15000;
int DEFAULT_reconnectInterval 		= 5000;
int DEFAULT_timeoutInterval 		= 3000;
int DEFAULT_disconnectAfterFailure	= 2;
int DEFAULT_localPortMDNS			= 80;
int DEFAULT_portHTTP				= 80;
int DEFAULT_portWEBSOCKETS			= 443;
bool _locked						= false;
bool _OTA_started					= false;
bool _http_started					= false;
bool _ssdp_started					= false;
bool _mdns_started					= false;
bool _audio_started					= false;
bool _websockets_started			= false;
int _DEBUG							= 0;

WiFiClientSecure					wifiClient;
t6iot_Ssdp							t6iotSsdp;
t6iot_Mdns							t6iotMdns;
t6iot_Http							t6iotHttp;
t6iot_Audio							t6iotAudio;
t6iot_Websockets					t6iotWebsockets;

using namespace std;

t6iot::t6iot(): TaskManager() {
	if (_DEBUG > 0) {
		Serial.println(F("t6 > Constructor"));
	}

}
void t6iot::set_useragent(String useragent) {
	if ( !useragent.isEmpty() ) {
		if (_DEBUG > 0) {
			Serial.println("t6 > Using CUSTOM UA");
		}
		_userAgent = useragent;
	} else {
		if (_DEBUG > 0) {
			Serial.println("t6 > Using DEFAULT UA");
		}
		_userAgent = DEFAULT_useragent;
	}
}
void t6iot::set_server() {
	if (_DEBUG > 0) {
		Serial.println("t6 > Using DEFAULT host, port & UA");
	}
	set_server(DEFAULT_host, DEFAULT_port, DEFAULT_useragent);
}
void t6iot::set_server(String host) {
	if (_DEBUG > 0) {
		Serial.println("t6 > Using DEFAULT port & UA");
	}
	set_server(host, DEFAULT_port, DEFAULT_useragent);
}
void t6iot::set_server(String host, int port) {
	if (_DEBUG > 0) {
		Serial.println("t6 > Using DEFAULT UA");
	}
	set_server(host, port, DEFAULT_useragent);
}
void t6iot::set_server(String host, int port, String useragent) {
	if (_DEBUG > 0) {
		Serial.println("t6 > Setting host, port & UA");
	}
	if ( !host.isEmpty() ) {
		_httpHost = host;
	} else {
		_httpHost = DEFAULT_host;
	}
	if ( port > 1 ) {
		_httpPort = port;
	} else {
		_httpPort = DEFAULT_port;
	}
	if ( !useragent.isEmpty() ) {
		_userAgent = useragent;
	} else {
		_userAgent = DEFAULT_useragent;
	}
	if (_httpPort == 443) {
		_httpProtocol = "https://";
	} else {
		_httpProtocol = "http://";
	}
	if (_DEBUG > 0) {
		Serial.print("t6 > Using protocol: ");
		Serial.println(_httpProtocol);
	}
}
void t6iot::set_wifi(const String &wifi_ssid, const String &wifi_password) {
	_ssid = wifi_ssid;
	_password = wifi_password;
	Serial.println();
	if(_DEBUG) {
		Serial.print(F("t6 > Connecting to Wifi SSID: "));
		Serial.println(_ssid);
	}
	WiFi.mode(WIFI_STA);
	WiFi.begin(_ssid, _password);
//	WiFi.setSleep(false);
	#ifdef ESP32
		// ESP32 require a dns ?
		WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), dns);
		if(_DEBUG) {
			Serial.print(F("t6 > Wifi is using DNS: ")); Serial.println(WiFi.dnsIP());
		}
	#endif
//	WiFi.waitForConnectResult();
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		if(_DEBUG) {
			Serial.println(F("t6 > WiFi Connect Failed! Rebooting..."));
			delay(1000);
		}
		Serial.print(F("."));
	} else {
		if(_DEBUG) {
			Serial.println(F("t6 > WiFi Connected..."));
			Serial.print(F("t6 > WiFi localIP: ")); Serial.println(WiFi.localIP());
		}
		delay(1000);
	}
	#ifdef ESP8266
		if(_DEBUG) {
			Serial.print(F("t6 > getResetReason: ")); Serial.println(ESP.getResetReason());
			Serial.print(F("t6 > getHeapFragmentation: ")); Serial.println(ESP.getHeapFragmentation());
			Serial.print(F("t6 > getMaxFreeBlockSize: ")); Serial.println(ESP.getMaxFreeBlockSize());
			Serial.print(F("t6 > getChipId: ")); Serial.println(ESP.getChipId());
			Serial.print(F("t6 > getCoreVersion: ")); Serial.println(ESP.getCoreVersion());
			Serial.print(F("t6 > getFlashChipId: ")); Serial.println(ESP.getFlashChipId());
			Serial.print(F("t6 > getFlashChipRealSize: ")); Serial.println(ESP.getFlashChipRealSize());
			Serial.print(F("t6 > checkFlashCRC: ")); Serial.println(ESP.checkFlashCRC());
			Serial.print(F("t6 > getVcc: ")); Serial.println(ESP.getVcc());
		}
	#elif ESP32
		if(_DEBUG) {
			Serial.print(F("t6 > getFreeHeap: ")); Serial.println(ESP.getFreeHeap());
			Serial.print(F("t6 > getChipModel: ")); Serial.println(ESP.getChipModel());
		}
	#endif
	if(_DEBUG) {
		Serial.print(F("t6 > getFlashChipMode: ")); Serial.println(ESP.getFlashChipMode());
		Serial.print(F("t6 > getSdkVersion: ")); Serial.println(ESP.getSdkVersion());
		Serial.print(F("t6 > getCpuFreqMHz: ")); Serial.println(ESP.getCpuFreqMHz());
		Serial.print(F("t6 > getSketchSize: ")); Serial.println(ESP.getSketchSize());
		Serial.print(F("t6 > getFreeSketchSpace: ")); Serial.println(ESP.getFreeSketchSpace());
		Serial.print(F("t6 > getSketchMD5: ")); Serial.println(ESP.getSketchMD5());
		Serial.print(F("t6 > getFlashChipSize: ")); Serial.println(ESP.getFlashChipSize());
		Serial.print(F("t6 > getFlashChipSpeed: ")); Serial.println(ESP.getFlashChipSpeed());
	}
}
void t6iot::set_endpoint(const String &endpoint) {
	_endpoint = endpoint;
}
void t6iot::set_key(const char *key) {
	_key = key;
	if (_DEBUG > 0) {
		Serial.println(F("t6 > set_key is DONE"));
	}
}
void t6iot::set_secret(const char *secret) {
	_secret = secret;
	if (_DEBUG > 0) {
		Serial.println(F("t6 > set_secret is DONE"));
	}
}
void t6iot::set_object_id(String object_id) {
	_object_id = object_id;
	_userAgent = String(_userAgent + " +oid:" + _object_id);
	if (_DEBUG > 0) {
		Serial.print(F("t6 > Using User-Agent: ")); Serial.println(_userAgent);
	}
}
void t6iot::set_object_secret(String secret) {
	_object_secret = secret;
	if (_DEBUG > 0) {
		Serial.println(F("t6 > Using secret from Object to encrypt payload"));
	}
}
int t6iot::createDatapoint(DynamicJsonDocument &payload) {
	t6iot::set_endpoint("/v2.0.1/data/"); // Set the t6iot API endpoint.
	String payloadStr;
	serializeJson(payload, payloadStr);
	if (false) { // TODO
		payloadStr = _getSignedPayload(payloadStr, _object_id, _object_secret); // TODO
	}
	if (_DEBUG > 0) {
		Serial.println(F("t6 > Adding datapoint(s) to: ")); Serial.print(F("     * ")); Serial.print(_httpProtocol); Serial.print(_httpHost); Serial.print(":"); Serial.print(_httpPort); Serial.println(_endpoint);
		Serial.println(F("t6 > User-Agent: ")); Serial.print(F("     * ")); Serial.println(_userAgent);
		Serial.println(F("t6 > payload: ")); Serial.print(F("     * ")); Serial.println(payloadStr);
	}

	if (_httpPort == 443) {
		if (_DEBUG > 0) {
			Serial.printf("t6 > HTTPS / Using fingerprint: %s\n", fingerprint);
		}
		#if defined(ESP8266)
			if (_DEBUG > 0) {
				Serial.println(F("t6 > ESP8266"));
			}
			HTTPClient https;
			BearSSL::WiFiClientSecure newSecure;
			newSecure.setFingerprint(fingerprint);
			int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _endpoint);
//			https.setUserAgent(_userAgent);
			https.setUserAgent(String(_userAgent));
			https.addHeader("User-Agent", String(_userAgent));
			https.addHeader("Accept", "application/json");
			https.addHeader("Content-Type", "application/json");
			https.addHeader("Cache-Control", "no-cache");
			https.addHeader("Accept-Encoding", "gzip, deflate, br");
			https.addHeader("x-api-key", _key);
			https.addHeader("x-api-secret", _secret);
	//		https.addHeader("Content-Length", String((payloadStr).length()));

			int httpCode = https.POST(payloadStr);
			if (httpCode == 200 && payloadStr != "") {
				String payload = https.getString();
				if (_DEBUG > 0) {
					Serial.print(F("t6 > payload: "));
					Serial.println(payload);
					Serial.println("<");
				}
				return httpCode;
			} else {
				if (_DEBUG > 0) {
					Serial.print(F("t6 > httpCode failure httpCode: "));
					Serial.println(httpCode);
					String payload = https.getString();
					Serial.println(payload);
					Serial.println("<");
				}
				return httpCode;
			}
		#elif ESP32
			if (_DEBUG > 0) {
				Serial.println(F("t6 > ESP32"));
			}
			wifiClient.setCACert(root_ca);
			int conn = wifiClient.connect(String(_httpHost).c_str(), _httpPort);
			if (conn > 0) {
				if (_DEBUG > 0) {
					Serial.print(F("t6 > https.begin conn success: "));
					Serial.println(String(_httpHost).c_str());
					Serial.println(conn);
				}
				wifiClient.println("POST " + String(_httpProtocol) + String(_httpHost).c_str() + ":" + String(_httpPort).c_str() + String( _endpoint ) + " HTTP/1.0");
				wifiClient.print("User-Agent:"); wifiClient.println(_userAgent);
				wifiClient.print("Host:"); wifiClient.println(_httpHost);
				wifiClient.println("Accept: application/json");
				wifiClient.println("Content-Type: application/json");
				wifiClient.println("Cache-Control: no-cache");
				wifiClient.println("Accept-Encoding: gzip, deflate, br");
				wifiClient.print("x-api-key:"); wifiClient.println(_key);
				wifiClient.print("x-api-secret:"); wifiClient.println(_secret);
				wifiClient.print("Content-Length:"); wifiClient.println((payloadStr).length());
				wifiClient.println("Connection: Close");
				wifiClient.println();
				wifiClient.println(payloadStr);

				while (wifiClient.connected()) {
					String line = wifiClient.readStringUntil('\n');
					if (line == "\r") {
						Serial.println(line);
						break;
					}
				}
				while (wifiClient.available()) {
					char c = wifiClient.read();
					Serial.write(c);
				}
				wifiClient.stop();
				return conn;
			} else {
				if (_DEBUG > 0) {
					Serial.print(F("t6 > https.begin conn failure: "));
					Serial.println(String(_httpHost).c_str());
					Serial.println(conn);
				}
				return conn;
			}
		#endif
	} else {
		if (_DEBUG > 0) {
			Serial.println(F("t6 > HTTP / Not using fingerprint / setInsecure"));
		}
		#if defined(ESP8266)
			if (_DEBUG > 0) {
				Serial.println(F("t6 > ESP8266"));
			}
			WiFiClient wifi;
			HttpClient client = HttpClient(wifi, _httpHost, _httpPort);

			HTTPClient http;
			int checkBegin = http.begin(wifi, _httpHost, _httpPort, _endpoint);
			if (_DEBUG > 0) {
				Serial.print(F("t6 > checkBegin: ")); Serial.println(checkBegin);
			}
//			http.setUserAgent(_userAgent);
			http.addHeader("User-Agent", String(_userAgent));
			http.addHeader("Accept", "application/json");
			http.addHeader("Content-Type", "application/json");
			http.addHeader("Cache-Control", "no-cache");
			http.addHeader("Accept-Encoding", "gzip, deflate, br");
			http.addHeader("x-api-key", _key);
			http.addHeader("x-api-secret", _secret);

			int httpCode = http.POST(payloadStr);
			if (httpCode == 200 && payloadStr != "") {
				String payload = http.getString();
				if (_DEBUG > 0) {
					Serial.print(F("t6 > payload: "));
					Serial.println(payload);
					Serial.println("<");
				}
				return httpCode;
			} else {
				if (_DEBUG > 0) {
					Serial.print(F("t6 > httpCode failure httpCode: "));
					Serial.println(httpCode);
					String payload = http.getString();
					Serial.println(payload);
					Serial.println("<");
				}
				return httpCode;
			}
		#elif ESP32
			if (_DEBUG > 0) {
				Serial.println(F("t6 > ESP32"));
			}
			WiFiClient client;
			HTTPClient http;
			http.begin(client, String(_httpProtocol) + String(_httpHost).c_str() + ":" + String(_httpPort).c_str() + String( _endpoint ));
			http.addHeader("User-Agent", String(_userAgent));
			http.addHeader("Accept", "application/json");
			http.addHeader("Content-Type", "application/json");
			http.addHeader("Cache-Control", "no-cache");
			http.addHeader("Accept-Encoding", "gzip, deflate, br");
			http.addHeader("x-api-key", _key);
			http.addHeader("x-api-secret", _secret);
			http.addHeader("Content-Length", String((payloadStr).length()));
			http.addHeader("Connection", "Close");
			int httpCode = http.POST(payloadStr); //Body
			if (httpCode > 0) {
				const String& payloadRes = http.getString();
				if (_DEBUG > 0) {
					Serial.print("t6 > OK Response: ");
					Serial.println(httpCode);
					Serial.println(payloadRes);
				}
				return httpCode;
			} else {
				const String& payloadRes = http.getString();
				if (_DEBUG > 0) {
					Serial.print(F("t6 > Error Response: "));
					Serial.println(httpCode);
					Serial.print(F("t6 > payload: "));
					Serial.println(payloadRes);
					Serial.println(F("<"));
				}
				return httpCode;
			}
			http.end();
			return httpCode;
		#endif
	}
}
int t6iot::createDatapoints(DynamicJsonDocument &payload) {
	return createDatapoint(payload); // This is a shortcut
}
String t6iot::_getSignedPayload(String &payload, String &object_id, String &object_secret) { // TODO
	ArduinoJWT jwt = ArduinoJWT(object_secret);
	String signedJson;
	String payloadString;
	String signedPayloadAsString;
	if (_DEBUG > 0) {
		Serial.println(payload);
		Serial.println(object_id);
		Serial.println(object_secret);
	}
	/*
	payload.printTo(payloadString);
	signedJson = jwt.encodeJWT( payloadString );
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(25);
	StaticJsonBuffer<BUFFER_SIZE> jsonBufferSigned;
	JsonObject& signedPayload = jsonBufferSigned.createObject();
	signedPayload["signedPayload"] = signedJson;
	signedPayload["object_id"] = object_id;
	signedPayload.prettyPrintTo(Serial);
	signedPayload.printTo(signedPayloadAsString);
	*/
	return signedPayloadAsString;
}
bool t6iot::startSsdp() {
	return startSsdp(DEFAULT_friendlyName);
}
bool t6iot::startSsdp(String friendlyName) {
	_ssdp_started = true;
	DEFAULT_friendlyName = friendlyName;
	if (_http_started) {
		// modelName
		// modelNumber
		// modelURL
		// manufacturer
		// manufacturerURL
		t6iotHttp.setSsdp(80, "upnp:rootdevice", DEFAULT_friendlyName, "t6 IoT", "1.0.0", "https://github.com/mathcoll/t6iot", "Internet Collaboratif", "https://www.internetcollaboratif.info", 600);
	}
	return t6iotSsdp.startSsdp(80, "upnp:rootdevice", DEFAULT_friendlyName, "t6 IoT", "1.0.0", "https://github.com/mathcoll/t6iot", "Internet Collaboratif", "https://www.internetcollaboratif.info", 600);
}
bool t6iot::startMdns() {
	_mdns_started = true;
	return t6iotMdns.startMdns(DEFAULT_friendlyName, DEFAULT_portHTTP, DEFAULT_portWEBSOCKETS, _http_started, _websockets_started);
}
bool t6iot::startMdns(String friendlyName) {
	_mdns_started = true;
	DEFAULT_friendlyName = friendlyName;
	return t6iotMdns.startMdns(friendlyName, DEFAULT_portHTTP, DEFAULT_portWEBSOCKETS, _http_started, _websockets_started);
}
bool t6iot::startMdns(String friendlyName, int DEFAULT_portHTTP, int DEFAULT_portWEBSOCKETS) {
	_mdns_started = true;
	DEFAULT_friendlyName = friendlyName;
	return t6iotMdns.startMdns(friendlyName, DEFAULT_portHTTP, DEFAULT_portWEBSOCKETS, _http_started, _websockets_started);
}
void t6iot::mdns_loop() {
	if (_mdns_started) {
		t6iotMdns.mdns_loop();
	}
}
bool t6iot::startWebsockets() {
	_websockets_started = true;
	return t6iotWebsockets.startWebsockets(
		DEFAULT_host_ws,
		DEFAULT_port_ws, "/ws",
		_key, _secret,
		DEFAULT_messageInterval,
		DEFAULT_reconnectInterval,
		DEFAULT_timeoutInterval,
		DEFAULT_disconnectAfterFailure,
		_object_id, _object_secret,
		t6iotAudio, _audio_started);
}
bool t6iot::startWebsockets(String host, int port) {
	_websockets_started = true;
	Serial.print(host);
	Serial.print(port);
	return t6iotWebsockets.startWebsockets(host, port, "/", _key, _secret, DEFAULT_messageInterval, DEFAULT_reconnectInterval, DEFAULT_timeoutInterval, DEFAULT_disconnectAfterFailure, _object_id, _object_secret, t6iotAudio, _audio_started);
}
void t6iot::webSockets_loop() {
	t6iotWebsockets.webSockets_loop();
}
/*bool webSockets_sendTXT(String data) {
	return t6iotWebsockets.sendTXT(data);
}*/
bool t6iot::isClaimed() {
	return false; //t6iotWebsockets.isClaimed();
}
bool t6iot::startHttp() {
	_http_started = true;
	return t6iotHttp.startHttp(DEFAULT_localPortMDNS);
}
bool t6iot::startHttp(int port) {
	_http_started = true;
	return t6iotHttp.startHttp(port);
}
bool t6iot::addStaticRoutes() {
	return t6iotHttp.addStaticRoutes();
}
bool t6iot::addDynamicRoutes() {
	return t6iotHttp.addDynamicRoutes();
}
bool t6iot::startAudio() {
	_audio_started = true;
	return _audio_started;
}
void t6iot::audio_loop() {
	if (_audio_started) {
		t6iotAudio.audio_loop();
	}
}
bool t6iot::audioListenTo(const char* url) {
	if (_audio_started) {
		t6iotAudio.stopSong();
		return t6iotAudio.audioListenTo(url);
	} else {
		return false;
	}
}
bool t6iot::audioSetVol(int vol) {
	if (_audio_started) {
		return t6iotAudio.audioSetVol(vol);
	} else {
		return false;
	}
}
bool t6iot::isLocked() {
	return _locked;
}
bool t6iot::setSleepDuration(const long dur) {
	_sleepDuration = dur;
	return true;
}
void t6iot::lockSleep() {
	_locked = true;
}
void t6iot::lockSleep(const long dur) {
	Serial.println(dur);
	_locked = true;
	//t6iot::schedule(dur, [] {_locked = false;}); // TODO
}
void t6iot::unlockSleep() {
	_locked = false;
}
void t6iot::goToSleep() {
	return goToSleep(_sleepDuration);
}
void t6iot::goToSleep(const long dur) {
	if (!_locked) {
		if (_DEBUG > 0) {
			Serial.println("t6 > Sleeping ; will wake up in " + String(dur) + "s...");
		}
		#if defined(ESP8266)
			ESP.deepSleep(dur * 1000000, WAKE_RF_DEFAULT);
		#elif ESP32
			esp_sleep_enable_timer_wakeup(dur * 1000000);
		#endif
	}
}
void t6iot::activateOTA() {
	return activateOTA(DEFAULT_friendlyName);
}
void t6iot::activateOTA(String friendlyName) {
	_OTA_started = true;
	_locked = true;
	DEFAULT_friendlyName = friendlyName;
	ArduinoOTA.setHostname(DEFAULT_friendlyName.c_str());
	//ArduinoOTA.setPort(8266);
	//ArduinoOTA.setPassword("admin");
	//MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	//ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else {  // U_FS
			type = "filesystem";
		}
		// NOTE: if updating FS this would be the place to unmount FS using FS.end()
		Serial.println("t6 > Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("t6 > Progress: %u%%\r\n", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("t6 > Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) {
			Serial.println("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			Serial.println("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			Serial.println("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			Serial.println("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			Serial.println("End Failed");
		}
	});
	ArduinoOTA.begin();
}
void t6iot::deployOTA(String user_id, String object_id, String source_id) {
	t6iot::set_endpoint("/v2.0.1/ota/"+String(user_id)+"/deploy/"+String(object_id)); // Set the t6iot API endpoint.
	if (_DEBUG > 0) {
		Serial.println(F("t6 > Deploying t6 OTA Latest Version:"));
		Serial.print(F("     * Object: ")); Serial.println(object_id);
		Serial.print(F("     * User: ")); Serial.println(user_id);
		Serial.print(F("     * ")); Serial.print(_httpProtocol); Serial.print(_httpHost); Serial.print(":"); Serial.print(_httpPort); Serial.println(_endpoint);
		Serial.println(F("t6 > User-Agent: ")); Serial.print(F("     * ")); Serial.println(_userAgent);
	}
	if (_httpPort == 443) {
		if (_DEBUG > 0) {
			Serial.printf("t6 > HTTPS / Using fingerprint: %s\n", fingerprint);
		}
		#if defined(ESP8266)
			if (_DEBUG > 0) {
				Serial.println(F("t6 > ESP8266"));
			}
		#elif ESP32

		#endif
	} else {
		if (_DEBUG > 0) {
			Serial.println(F("t6 > HTTP / Not using fingerprint / setInsecure"));
		}
		#if defined(ESP8266)
			if (_DEBUG > 0) {
				Serial.println(F("t6 > ESP8266"));
			}

			WiFiClient wifi;
			HttpClient client = HttpClient(wifi, _httpHost, _httpPort);

			HTTPClient http;
			int checkBegin = http.begin(wifi, _httpHost, _httpPort, _endpoint);
			if (_DEBUG > 0) {
				Serial.print(F("t6 > checkBegin: ")); Serial.println(checkBegin);
			}
//			http.setUserAgent(_userAgent);
			http.addHeader("User-Agent", String(_userAgent));
			http.addHeader("Accept", "application/json");
			http.addHeader("Content-Type", "application/json");
			http.addHeader("Cache-Control", "no-cache");
			http.addHeader("Accept-Encoding", "gzip, deflate, br");
			http.addHeader("x-api-key", _key);
			http.addHeader("x-api-secret", _secret);

			int httpCode = http.POST("");
			if (httpCode == 201) {
				String payload = http.getString();
				if (_DEBUG > 0) {
					Serial.print(F("t6 > payload: "));
					Serial.println(payload);
					Serial.println("<");
				}
			} else {
				if (_DEBUG > 0) {
					Serial.print(F("t6 > httpCode failure httpCode: "));
					Serial.println(httpCode);
					String payload = http.getString();
					Serial.println(payload);
					Serial.println("<");
				}
			}
		#elif ESP32

		#endif
	}
}
void t6iot::getOtaLatestVersion(String object_id, int OTAcurrentVersion) {
	t6iot::set_endpoint("/v2.0.1/objects/"+String(object_id)+"/latest-version"); // Set the t6iot API endpoint.
	if (_DEBUG > 0) {
		Serial.println(F("t6 > Getting t6 OTA Latest Version for Object: "));
		Serial.print(F("     * ")); Serial.println(object_id);
		Serial.print(F("     * ")); Serial.print(_httpProtocol); Serial.print(_httpHost); Serial.print(":"); Serial.print(_httpPort); Serial.println(_endpoint);
		Serial.println(F("t6 > User-Agent: ")); Serial.print(F("     * ")); Serial.println(_userAgent);
	}

	if (_httpPort == 443) {
		Serial.printf("t6 > HTTPS / Using fingerprint: %s\n", fingerprint);
		#if defined(ESP8266)

		#elif ESP32

		#endif
	} else {
		Serial.println(F("t6 > HTTP / Not using fingerprint / setInsecure"));
		#if defined(ESP8266)
			Serial.println(F("t6 > ESP8266"));
			WiFiClient wifi;
			HttpClient client = HttpClient(wifi, _httpHost, _httpPort);

			HTTPClient http;
			int checkBegin = http.begin(wifi, _httpHost, _httpPort, _endpoint);
			Serial.print(F("t6 > checkBegin: "));
			Serial.println(checkBegin);
	//			http.setUserAgent(_userAgent);
			http.addHeader("User-Agent", String(_userAgent));
			http.addHeader("Accept", "application/json");
			http.addHeader("Content-Type", "application/json");
			http.addHeader("Cache-Control", "no-cache");
			http.addHeader("Accept-Encoding", "gzip, deflate, br");
			http.addHeader("x-api-key", _key);
			http.addHeader("x-api-secret", _secret);

			int httpCode = http.GET();
			if (httpCode == 200) {
				String payload = http.getString();
				Serial.print(F("t6 > payload: "));
				Serial.println(payload);
				Serial.println("<");

				//DynamicJsonDocument ota(64);
				StaticJsonDocument<2048> ota;
				DeserializationError error = deserializeJson(ota, payload);
				if (!error) {
					Serial.print(F("t6 > objectExpectedVersion: ")); Serial.println(ota["objectExpectedVersion"].as<int>());
					Serial.print(F("t6 > sourceLatestVersion: ")); Serial.println(ota["sourceLatestVersion"].as<int>());
					Serial.print(F("t6 > source_id: ")); Serial.println(ota["source_id"].as<String>());
					Serial.print(F("t6 > user_id: ")); Serial.println(ota["user_id"].as<String>());
					if( ota["objectExpectedVersion"].as<int>() != OTAcurrentVersion ) {
						//if ( newVersionStatus === "200 Ready to deploy" ) { // Commented as we consider the Build is already OK on t6 server
						if (_DEBUG > 0) {
							Serial.println(F("t6 > OTA new version is ready!"));
							Serial.print(F("     * OTAcurrentVersion: ")); Serial.println(OTAcurrentVersion);
							Serial.print(F("     * objectExpectedVersion: ")); Serial.println(ota["objectExpectedVersion"].as<int>());
							deployOTA(ota["user_id"].as<String>(), object_id, ota["source_id"].as<String>());
						}
					} else {
						Serial.println(F("t6 > OTA latest version already installed!"));
					}
				} else {
					Serial.print(F("t6 > deserializeJson() failed: "));
					Serial.println(error.f_str());
				}
			} else {
				Serial.print(F("t6 > httpCode failure httpCode: "));
				Serial.println(httpCode);
				String payload = http.getString();
				Serial.println(payload);
				Serial.println("<");
			}
			http.end();
		#elif ESP32

		#endif
	}
}
void t6iot::ota_loop() {
	ArduinoOTA.handle();
}
int t6iot::debug(int level) {
	_DEBUG = level;
	return _DEBUG;
}
