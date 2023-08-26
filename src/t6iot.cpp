/*
 t6iot.cpp - v2.0.4
 Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
 - t6 website: https://www.internetcollaboratif.info
 - t6 iot: https://api.internetcollaboratif.info
 - Api doc: https://api.internetcollaboratif.info/docs/
 */

#include "t6iot.h"
// nmap --script ssl-cert.nse -p 443 api.internetcollaboratif.info | grep SHA-1
// Not valid after:  2023-10-09T05:45:42

const char *fingerprint				= "12 3f 14 75 f4 aa bf 13 ce e7 13 28 c8 d2 13 56 0c 9b 5f 34";
//const char *fingerprint				= "E5 8C 1C C4 91 3B 38 63 4B E9 10 6E E3 AD 8E 6B 9D D9 81 4A";
String DEFAULT_useragent			= "t6iot-library/2.0.4 (Arduino; rv:2.2.0; +https://www.internetcollaboratif.info)";
String DEFAULT_host					= "api.internetcollaboratif.info";
int DEFAULT_port					= 443;
String DEFAULT_host_ws				= "ws.internetcollaboratif.info";
int DEFAULT_port_ws					= 443;
int DEFAULT_messageInterval 		= 15000;
int DEFAULT_reconnectInterval 		= 5000;
int DEFAULT_timeoutInterval 		= 3000;
int DEFAULT_disconnectAfterFailure	= 2;
int DEFAULT_localPortMDNS			= 80;
String DEFAULT_friendlyName 		= "t6ObjectLib";
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
void t6iot::set_server() {
	Serial.println("t6 > Using DEFAULT host, port  & UA");
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
	Serial.print("t6 > Connecting to Wifi SSID: ");
	Serial.println(_ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(_ssid, _password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("t6 > WiFi Connect Failed! Rebooting...");
		delay(1000);
		Serial.print(".");
	} else {
		Serial.println("t6 > WiFi Connected...");
		#if defined(ESP8266)
			Serial.print("t6 > WiFi getPhyMode(): ");
			Serial.println(WiFi.getPhyMode());
		#endif
		Serial.print("t6 > WiFi IP Address: ");
		Serial.println(WiFi.localIP());
		delay(1000);
	}
}
void t6iot::set_endpoint(const String &endpoint) {
	_endpoint = endpoint;
}
void t6iot::set_key(const char *key) {
	_key = key;
	Serial.println("t6 > set_key is DONE");
}
void t6iot::set_secret(const char *secret) {
	_secret = secret;
	Serial.println("t6 > set_secret is DONE");
}
void t6iot::set_object_id(String object_id) {
	_object_id = object_id;
	_userAgent = String(_userAgent + " +oid:" + _object_id);
	Serial.print("t6 > Using User-Agent: "); Serial.println(_userAgent);
}
void t6iot::set_object_secret(String secret) {
	_object_secret = secret;
	Serial.println("t6 > Using secret from Object to encrypt payload");
}
int t6iot::createDatapoint(DynamicJsonDocument &payload) {
	t6iot::set_endpoint("/v2.0.1/data/"); // Set the t6iot API endpoint.
	String payloadStr;
	serializeJson(payload, payloadStr);

	if (false) { // TODO
		payloadStr = _getSignedPayload(payloadStr, _object_id, _object_secret); // TODO
	}
	Serial.println("t6 > Adding datapoint(s) to: "); Serial.print(" * "); Serial.print(_httpProtocol); Serial.print(_httpHost); Serial.print(":"); Serial.print(_httpPort); Serial.println(_endpoint);
	Serial.println("t6 > User-Agent: "); Serial.print(" * "); Serial.println(_userAgent);
	Serial.println("t6 > payload: "); Serial.print(" * "); Serial.println(payloadStr);

	if (_httpPort == 443) {
		Serial.printf("t6 > HTTPS / Using fingerprint: %s\n", fingerprint);
		#ifdef ESP8266
			Serial.println("t6 > ESP8266");
			std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
			HTTPClient https;
			int conn = https.begin(*client, String(_httpHost).c_str(), _httpPort, _endpoint, true);
			client->setFingerprint(fingerprint);
			/*
			22:56:45.510 -> BSSL:_connectSSL: start connection
			22:56:45.510 -> BSSL:_connectSSL: OOM error
			22:56:45.510 -> [HTTP-Client] failed connect to api.internetcollaboratif.info:443
			22:56:45.510 -> [HTTP-Client][returnError] error(-1): connection failed
			22:56:45.543 -> t6 > httpCode failure aaaaa: [HTTP-Client][end] tcp is closed
			*/
			if (conn > 0) {
				Serial.print(F("t6 > https.begin conn success: "));
				Serial.println(conn);
				https.setUserAgent(String(_httpHost).c_str());
				https.addHeader("User-Agent", "t6iot-library " + String(_userAgent));
				https.addHeader("Accept", "application/json");
				https.addHeader("Content-Type", "application/json");
				https.addHeader("Cache-Control", "no-cache");
				https.addHeader("Accept-Encoding", "gzip, deflate, br");
				https.addHeader("x-api-key", _key);
				https.addHeader("x-api-secret", _secret);
				https.addHeader("Content-Length", String((payloadStr).length()));
				https.addHeader("Connection", "Close");
				int httpCode = https.POST(payloadStr);
				if (httpCode == 200 && payloadStr != "") {
					String payloadRes = https.getString();
					DynamicJsonDocument doc(2048);
					DeserializationError error = deserializeJson(doc, payloadRes);
					if (!error) {
						return httpCode;
					} else {
						Serial.print(F("t6 > DeserializeJson() failed: "));
						Serial.println(error.c_str());
						return 500;
					}
				} else {
					Serial.print(F("t6 > httpCode failure aaaaa: "));
					String payloadRes = https.getString();
					Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
					Serial.println(payloadRes);
					Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
					return httpCode;
				}
			} else {
				Serial.print(F("t6 > https.begin conn failure: "));
				Serial.println(conn);
				return conn;
			}
		#elif ESP32
			Serial.println("t6 > ESP32");
			return 501;
		#endif
	} else {
		Serial.println("t6 > HTTP / Not using fingerprint / setInsecure");
		WiFiClient client;
		HTTPClient http;
		http.begin(client, String(_httpProtocol) + String(_httpHost).c_str() + ":" + String(_httpPort).c_str() + String( _endpoint ));
		http.addHeader("User-Agent", "t6iot-library " + String(_userAgent));
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
			Serial.print("t6 > Error Response: ");
			Serial.println(httpCode);
			Serial.println("t6 > payloadRes: EOE21>>");
			Serial.println(payloadRes);
			Serial.println("<<EOE2");
			return httpCode;
		}
		http.end();
		return httpCode;
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
	_ssdp_started = true;
	return t6iotSsdp.startSsdp(80, "upnp:rootdevice", "t6Object", "t6 IoT", "1.0.0", "https://github.com/mathcoll/t6iot", "Internet Collaboratif", "https://www.internetcollaboratif.info", 600);
}
bool t6iot::startMdns() {
	_mdns_started = true;
	return t6iotMdns.startMdns(DEFAULT_friendlyName, DEFAULT_localPortMDNS);
}
bool t6iot::startMdns(String friendlyName) {
	_mdns_started = true;
	return t6iotMdns.startMdns(friendlyName, DEFAULT_localPortMDNS);
}
bool t6iot::startMdns(String friendlyName, int localPortMDNS) {
	_mdns_started = true;
	return t6iotMdns.startMdns(friendlyName, localPortMDNS);
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
		t6iotAudio);
	return false;
}
bool t6iot::startWebsockets(String host, int port) {
	_websockets_started = true;
	Serial.print(host);
	Serial.print(port);
	return t6iotWebsockets.startWebsockets(host, port, "/", _key, _secret, DEFAULT_messageInterval, DEFAULT_reconnectInterval, DEFAULT_timeoutInterval, DEFAULT_disconnectAfterFailure, _object_id, _object_secret, t6iotAudio);
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
void t6iot::audio_loop() {
	t6iotAudio.audio_loop();
}
bool t6iot::audioListenTo(const char* url) {
	_audio_started = true;
	return t6iotAudio.audioListenTo(url);
}
bool t6iot::audioSetVol(int vol) {
	_audio_started = true;
	return t6iotAudio.audioSetVol(vol);
}
bool t6iot::isLocked() {
	return _locked;
}
void t6iot::lockSleep() {
	_locked = true;
}
void t6iot::lockSleep(const long dur) {
	Serial.println(dur);
	_locked = true;
	//t6iot::scheduleOnce(dur, [] {_locked = false;}); // TODO
}
void t6iot::unlockSleep() {
	_locked = false;
}
void t6iot::goToSleep(const long dur) {
	if (!_locked) {
		Serial.println("t6 > Sleeping ; will wake up in " + String(dur) + "s...");
		Serial.println("t6 > Sleeping DISABLED - hardcoded");
		//ESP.deepSleep(dur * 1000000, WAKE_RF_DEFAULT);
	}
}
void t6iot::activateOTA() {
	_OTA_activated = true;
	_locked = true;
	ArduinoOTA.handle();
	ArduinoOTA.begin();
}
