/*
  t6iot.cpp - v2.0.1
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
 */

#include "t6iot.h"
#include <ArduinoHttpClient.h>
// nmap --script ssl-cert.nse -p 443 api.internetcollaboratif.info | grep SHA-1
// Not valid after:  2023-10-09T05:45:42
const char* fingerprint = "12 3f 14 75 f4 aa bf 13 ce e7 13 28 c8 d2 13 56 0c 9b 5f 34";
WiFiClientSecure wifiClient;

using namespace std;

t6iot::t6iot(String host, int port) {
	_userAgent = "Arduino/2.2.0 t6iot-library/2.0.1 (+https://www.internetcollaboratif.info)";
	_httpHost = host;
	_httpPort = port;
}
bool t6iot::wifiInit(const String& wifi_ssid, const String& wifi_password) {
  _ssid = wifi_ssid;
  _password = wifi_password;
  if (_httpPort == 443) {
    _httpProtocol = "https://";
  } else {
    _httpProtocol = "http://";
  }
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
  Serial.printf("t6 > Using protocol %s\n", _httpProtocol);
  return 1;
}
void t6iot::set_endpoint(const String& endpoint) {
  _endpoint = endpoint;
}
void t6iot::set_key(const char* key) {
  _key = key;
  Serial.println("t6 > set_key is DONE");
}
void t6iot::set_secret(const char* secret) {
  _secret = secret;
  Serial.println("t6 > set_secret is DONE");
}
void t6iot::set_object_secret(String object_id, String secret) {
  _object_id = object_id;
  _object_secret = secret;
  Serial.println("t6 > Using secret from Object to encrypt payload");
}
int t6iot::createDatapoint(DynamicJsonDocument& payload) {
  t6iot::set_endpoint("/v2.0.1/data/"); // Set the t6iot API endpoint.
  String payloadStr;
  serializeJson(payload, payloadStr);
  if(false) { // TODO
    payloadStr = _getSignedPayload(payloadStr, _object_id, _object_secret); // TODO
  }
  Serial.println("t6 > Adding datapoint(s) to: ");
  Serial.print(" * "); Serial.print(_httpProtocol); Serial.print(_httpHost); Serial.print(":"); Serial.print(_httpPort); Serial.println(_endpoint);
  Serial.println("t6 > payload: ");
  Serial.print(" * ");
  Serial.println(payloadStr);

  if (_httpPort == 443) {
    Serial.printf("t6 > Using fingerprint: %s\n", fingerprint);
    wifiClient.setFingerprint(fingerprint);
  } else {
    Serial.println("t6 > Not using fingerprint / setInsecure");
    wifiClient.setInsecure();
  }
  if (!wifiClient.connect(_httpHost, _httpPort)) {
    Serial.println("t6 > Connection failed");
    return 495;
  }
	HttpClient https = HttpClient(wifiClient, _httpHost, _httpPort);
	{
		https.beginRequest();
		https.post(_endpoint);
		https.sendHeader("Accept", "application/json");
		https.sendHeader("Content-Type", "application/json");
		https.sendHeader("Content-Length", payloadStr.length());
		if(_key && _secret) {
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
			Serial.print("t6 > Error httpCode: EOE>>");
			Serial.println(httpCode);
			Serial.println(payloadRes);
			Serial.print("<<EOE");
			return httpCode;
		}
	}
}
String t6iot::_getSignedPayload(String& payload, String& object_id, String& object_secret) { // TODO
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