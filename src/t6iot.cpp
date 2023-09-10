/*
  t6iot - v2.0.4
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#include "t6iot.h"
// nmap --script ssl-cert.nse -p 443 api.internetcollaboratif.info | grep SHA-1
// Not valid after:  2023-10-09T05:45:42
// openssl s_client -connect api.internetcollaboratif.info:443 -prexit -showcerts -state -status -tlsextdebug -verify 10

const char *fingerprint = "12 3f 14 75 f4 aa bf 13 ce e7 13 28 c8 d2 13 56 0c 9b 5f 34";
const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx
OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63
ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS
iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k
KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ
DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk
j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5
cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW
CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499
iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei
Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap
sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b
9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP
BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf
BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw
JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH
MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al
oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy
MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF
AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9
NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9
WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw
9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy
+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi
d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=
-----END CERTIFICATE-----
)EOF";
IPAddress							dns(8, 8, 8, 8); //Google dns
String DEFAULT_useragent			= "t6iot-library/2.0.4 (Arduino; rv:2.2.0; +https://www.internetcollaboratif.info)";
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
bool _OTA_activated					= false;
bool _http_started					= false;
bool _ssdp_started					= false;
bool _mdns_started					= false;
bool _audio_started					= false;
bool _websockets_started			= false;

WiFiClientSecure					wifiClient;
t6iot_Ssdp							t6iotSsdp;
t6iot_Mdns							t6iotMdns;
t6iot_Http							t6iotHttp;
t6iot_Audio							t6iotAudio;
t6iot_Websockets					t6iotWebsockets;

using namespace std;

t6iot::t6iot(): TaskManager() {
	Serial.println("t6 > Constructor");
	#ifdef ESP8266
		Serial.print("t6 > getResetReason"); Serial.println(ESP.getResetReason());
		Serial.print("t6 > getHeapFragmentation"); Serial.println(ESP.getHeapFragmentation());
		Serial.print("t6 > getMaxFreeBlockSize"); Serial.println(ESP.getMaxFreeBlockSize());
		Serial.print("t6 > getChipId"); Serial.println(ESP.getChipId());
		Serial.print("t6 > getCoreVersion"); Serial.println(ESP.getCoreVersion());
		Serial.print("t6 > getFlashChipId"); Serial.println(ESP.getFlashChipId());
		Serial.print("t6 > getFlashChipRealSize"); Serial.println(ESP.getFlashChipRealSize());
		Serial.print("t6 > checkFlashCRC"); Serial.println(ESP.checkFlashCRC());
		Serial.print("t6 > getVcc"); Serial.println(ESP.getVcc());
	#elif ESP32
		Serial.print("t6 > getFreeHeap"); Serial.println(ESP.getFreeHeap());
		Serial.print("t6 > getChipModel"); Serial.println(ESP.getChipModel());
	#endif
	Serial.print("t6 > getFlashChipMode"); Serial.println(ESP.getFlashChipMode());
	Serial.print("t6 > getSdkVersion"); Serial.println(ESP.getSdkVersion());
	Serial.print("t6 > getCpuFreqMHz"); Serial.println(ESP.getCpuFreqMHz());
	Serial.print("t6 > getSketchSize"); Serial.println(ESP.getSketchSize());
	Serial.print("t6 > getFreeSketchSpace"); Serial.println(ESP.getFreeSketchSpace());
	Serial.print("t6 > getSketchMD5"); Serial.println(ESP.getSketchMD5());
	Serial.print("t6 > getFlashChipSize"); Serial.println(ESP.getFlashChipSize());
	Serial.print("t6 > getFlashChipSpeed"); Serial.println(ESP.getFlashChipSpeed());
}
void t6iot::set_useragent(String useragent) {
	if ( !useragent.isEmpty() ) {
		Serial.println("t6 > Using CUSTOM UA");
		_userAgent = useragent;
	} else {
		Serial.println("t6 > Using DEFAULT UA");
		_userAgent = DEFAULT_useragent;
	}
}
void t6iot::set_server() {
	Serial.println("t6 > Using DEFAULT host, port & UA");
	set_server(DEFAULT_host, DEFAULT_port, DEFAULT_useragent);
}
void t6iot::set_server(String host) {
	Serial.println("t6 > Using DEFAULT port & UA");
	set_server(host, DEFAULT_port, DEFAULT_useragent);
}
void t6iot::set_server(String host, int port) {
	Serial.println("t6 > Using DEFAULT UA");
	set_server(host, port, DEFAULT_useragent);
}
void t6iot::set_server(String host, int port, String useragent) {
	Serial.println("t6 > Setting host, port & UA");
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
	Serial.print("t6 > Using protocol: ");
	Serial.println(_httpProtocol);
}
void t6iot::set_wifi(const String &wifi_ssid, const String &wifi_password) {
	_ssid = wifi_ssid;
	_password = wifi_password;
	Serial.println();
	Serial.print(F("t6 > Connecting to Wifi SSID: "));
	Serial.println(_ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(_ssid, _password);
	WiFi.waitForConnectResult();
	#ifdef ESP32
		// ESP32 require a dns ?
		WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), dns);
	#endif
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println(F("t6 > WiFi Connect Failed! Rebooting..."));
		delay(1000);
		Serial.print(F("."));
	} else {
		Serial.println(F("t6 > WiFi Connected..."));
		delay(1000);
	}
}
void t6iot::set_endpoint(const String &endpoint) {
	_endpoint = endpoint;
}
void t6iot::set_key(const char *key) {
	_key = key;
	Serial.println(F("t6 > set_key is DONE"));
}
void t6iot::set_secret(const char *secret) {
	_secret = secret;
	Serial.println(F("t6 > set_secret is DONE"));
}
void t6iot::set_object_id(String object_id) {
	_object_id = object_id;
	_userAgent = String(_userAgent + " +oid:" + _object_id);
	Serial.print(F("t6 > Using User-Agent: ")); Serial.println(_userAgent);
}
void t6iot::set_object_secret(String secret) {
	_object_secret = secret;
	Serial.println(F("t6 > Using secret from Object to encrypt payload"));
}
int t6iot::createDatapoint(DynamicJsonDocument &payload) {
	t6iot::set_endpoint("/v2.0.1/data/"); // Set the t6iot API endpoint.
	String payloadStr;
	serializeJson(payload, payloadStr);
	if (false) { // TODO
		payloadStr = _getSignedPayload(payloadStr, _object_id, _object_secret); // TODO
	}
	Serial.println(F("t6 > Adding datapoint(s) to: ")); Serial.print(" * "); Serial.print(_httpProtocol); Serial.print(_httpHost); Serial.print(":"); Serial.print(_httpPort); Serial.println(_endpoint);
	Serial.println(F("t6 > User-Agent: ")); Serial.print(" * "); Serial.println(_userAgent);
	Serial.println(F("t6 > payload: ")); Serial.print(" * "); Serial.println(payloadStr);

	if (_httpPort == 443) {
		Serial.printf("t6 > HTTPS / Using fingerprint: %s\n", fingerprint);
		#if defined(ESP8266)
			Serial.println(F("t6 > ESP8266"));
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
				Serial.print(F("t6 > payload: "));
				Serial.println(payload);
				Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				return httpCode;
			} else {
				Serial.print(F("t6 > httpCode failure httpCode: "));
				Serial.println(httpCode);
				String payload = https.getString();
				Serial.println(payload);
				Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				return httpCode;
			}
		#elif ESP32
			Serial.println(F("t6 > ESP32"));
			wifiClient.setCACert(root_ca);
			int conn = wifiClient.connect(String(_httpHost).c_str(), _httpPort);
			if (conn > 0) {
				Serial.print(F("t6 > https.begin conn success: "));
				Serial.println(conn);
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
				Serial.print(F("t6 > https.begin conn failure: "));
				Serial.println(conn);
				return conn;
			}
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

			int httpCode = http.POST(payloadStr);
			if (httpCode == 200 && payloadStr != "") {
				String payload = http.getString();
				Serial.print(F("t6 > payload: "));
				Serial.println(payload);
				Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				return httpCode;
			} else {
				Serial.print(F("t6 > httpCode failure httpCode: "));
				Serial.println(httpCode);
				String payload = http.getString();
				Serial.println(payload);
				Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				return httpCode;
			}
/*
			client.beginRequest();
			client.post("/v2.0.1/data/");
			client.sendHeader("User-Agent", String(_userAgent));
			client.sendHeader("Accept", "application/json");
			client.sendHeader("Content-Type", "application/json");
			client.sendHeader("Cache-Control", "no-cache");
			client.sendHeader("Accept-Encoding", "gzip, deflate, br");
			client.sendHeader("x-api-key", _key);
			client.sendHeader("x-api-secret", _secret);
			client.sendHeader("Content-Length", (payloadStr).length());
			//client.beginBody();
			client.print(payloadStr);
			client.endRequest();

			int httpCode = client.responseStatusCode();

			if (httpCode == 200 && payloadStr != "") {
				return httpCode;
			} else {
				Serial.print(F("t6 > httpCode failure httpCode: "));
				Serial.println(httpCode);
				String payloadRes = client.responseBody();
				Serial.println(payloadRes);
				Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				return httpCode;
			}
*/
		#elif ESP32
			Serial.println(F("t6 > ESP32"));
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
				Serial.print("t6 > OK Response: ");
				Serial.println(httpCode);
				Serial.println(payloadRes);
				return httpCode;
			} else {
				const String& payloadRes = http.getString();
				Serial.print(F("t6 > Error Response: "));
				Serial.println(httpCode);
				Serial.println(F("t6 > payloadRes: EOE21>>"));
				Serial.println(payloadRes);
				Serial.println(F("<<EOE2"));
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
	Serial.println(payload);
	Serial.println(object_id);
	Serial.println(object_secret);
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
		DEFAULT_port_ws, "/",
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
		Serial.println("t6 > Sleeping ; will wake up in " + String(dur) + "s...");
		#if defined(ESP8266)
			ESP.deepSleep(dur * 1000000, WAKE_RF_DEFAULT);
		#elif ESP32
			esp_sleep_enable_timer_wakeup(dur * 1000000);
		#endif
	}
}
void t6iot::activateOTA() {
	_OTA_activated = true;
	_locked = true;
	ArduinoOTA.handle();
	ArduinoOTA.begin();
}
