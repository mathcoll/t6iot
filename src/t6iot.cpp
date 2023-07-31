/*
 t6iot.cpp - v2.0.2
 Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
 - t6 website: https://www.internetcollaboratif.info
 - t6 iot: https://api.internetcollaboratif.info
 - Api doc: https://api.internetcollaboratif.info/docs/
 */

#include "t6iot.h"
#include <ArduinoHttpClient.h>
// nmap --script ssl-cert.nse -p 443 api.internetcollaboratif.info | grep SHA-1
// Not valid after:  2023-10-09T05:45:42
const char *fingerprint		= "12 3f 14 75 f4 aa bf 13 ce e7 13 28 c8 d2 13 56 0c 9b 5f 34";
String DEFAULT_useragent	= "t6iot-library/2.0.2 (Arduino; rv:2.2.0; +https://www.internetcollaboratif.info)";
String DEFAULT_host			= "api.internetcollaboratif.info";
int DEFAULT_port			= 443;
WiFiClientSecure wifiClient;

using namespace std;

t6iot::t6iot() {
	Serial.println("t6 > Constructor");
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
		Serial.print("t6 > WiFi getPhyMode(): ");
		Serial.println(WiFi.getPhyMode());
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
		Serial.printf("t6 > Using fingerprint: %s\n", fingerprint);
		wifiClient.setFingerprint(fingerprint);
	} else {
		Serial.println("t6 > Not using fingerprint / setInsecure");
		wifiClient.setInsecure();
	}

	if (!wifiClient.connect(_httpHost, _httpPort)) {
		Serial.println("t6 > Connection failed / fallback to BearSSL");

		HTTPClient https;
		BearSSL::WiFiClientSecure newSecure;
		int checkBegin = https.begin(newSecure, _httpHost, _httpPort);
		Serial.print("t6 > checkBegin: ");
		Serial.println(checkBegin);
		newSecure.setFingerprint(fingerprint);
		https.setUserAgent(String(_userAgent));
		https.addHeader("Accept", "application/json");
		https.addHeader("Content-Type", "application/json");
		if (_key && _secret) {
			https.addHeader("x-api-key", _key);
			https.addHeader("x-api-secret", _secret);
		}

		int httpCode = https.POST(payloadStr);
		Serial.print("t6 > Result HTTP Status: ");
		Serial.println(httpCode);
		if (httpCode >= 200 && payloadStr != "") {
			String payloadRes = https.getString();
			Serial.print("t6 > Result HTTP Status: ");
			Serial.println(httpCode);
			Serial.println(payloadRes);
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
			String payloadRes = https.getString();
			Serial.print("t6 > Error httpCode: ");
			Serial.println(httpCode);
			Serial.println("t6 > payloadRes: EOE1>>");
			Serial.println(payloadRes);
			Serial.println("<<EOE1");
			return httpCode;
		}
	} else {
		HttpClient https = HttpClient(wifiClient, _httpHost, _httpPort);
		https.beginRequest();
		https.post(_endpoint);
		https.sendHeader("User-Agent", String(_userAgent));
		https.sendHeader("Accept", "application/json");
		https.sendHeader("Content-Type", "application/json");
		https.sendHeader("Content-Length", payloadStr.length());
		if (_key && _secret) {
			https.sendHeader("x-api-key", _key);
			https.sendHeader("x-api-secret", _secret);
		}
		https.beginBody();
		https.print(payloadStr);
		https.endRequest();

		int httpCode = https.responseStatusCode();
		if (httpCode >= 200 && payloadStr != "") {
			String payloadRes = https.responseBody();
			Serial.print("t6 > Result HTTP Status: ");
			Serial.println(httpCode);
			Serial.println(payloadRes);
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
			String payloadRes = https.responseBody();
			Serial.print("t6 > Error httpCode: ");
			Serial.println(httpCode);
			Serial.println("t6 > payloadRes: EOE2>>");
			Serial.println(payloadRes);
			Serial.println("<<EOE2");
			return httpCode;
		}
	}
}
int t6iot::createDatapoints(DynamicJsonDocument &payload) {
	return createDatapoint(payload); // This is a shortcut
}
String t6iot::_getSignedPayload(String &payload, String &object_id,
		String &object_secret) { // TODO
	ArduinoJWT jwt = ArduinoJWT(object_secret);
	String signedJson;
	String payloadString;
	String signedPayloadAsString;
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
