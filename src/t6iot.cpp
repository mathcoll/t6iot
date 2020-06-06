/*
  t6iot.cpp - 
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  Sample file to connect t6 api
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
 */

#include <t6iot.h>
#include <IoAbstraction.h>
#include <FS.h>

#define SLEEP_DURATION  1800
const size_t MAX_CONTENT_SIZE = 512;
String _JWTToken;
String _refreshToken;
bool authorized = false;
int processes = 0;
unsigned long latestOTACall = 0;
bool authenticated = false;
const char* objectExpectedVersion;
char* currentVersion = "3"; // the OTA source version
unsigned long previousMillis = 0;
const long interval = 1000;
const long otaTimeout = 180000;
unsigned long latestPOSTCall = millis() + otaTimeout;
int ledState = LOW;
bool OTA_IN_Progress = false;
bool OTA_activated = false;
bool _lockedSleep = false;
const char* sourceId;
const char* sourceLatestVersion;
const char* buildVersions;
extern char* secret;
int _t6ObjectHttpPort = 80;
const char* defaultReal = "t6 Auth Realm";
String authFailResponse = "Authentication Failed";
String _messageArrived = "";
const char* www_username;
const char* www_password;
const char* www_realm;
bool DEBUG = false;
const char* fingerprint = "6C:6C:63:F9:1D:92:30:28:55:44:38:D3:F5:54:E0:6F:F9:8F:D0:77"; // SHA1 fingerprint of the certificate / Use web browser to view and copy

WiFiClient client;
HTTPClient https;

String defaultHtml = "<html>\n"
	"<head></head>\n"
	"<body>\n"
	"<h1>Default UI</h1>\n"
	"<ul>\n"
	"<li><a href='/open'>open</a></li>\n"
	"<li><a href='/close'>close</a></li>\n"
	"<li><a href='/getVal'>getVal</a></li>\n"
	"<!--<li><a href='/setVal'>setVal</a></li>-->\n"
	"<li><a href='/on'>on</a></li>\n"
	"<li><a href='/off'>off</a></li>\n"
	"<li><a href='/upper'>upper</a></li>\n"
	"<li><a href='/lower'>lower</a></li>\n"
	"<li><a href='/upgrade'>upgrade</a></li>\n"
	"</ul>\n"
	"</body>\n"
	"</html>\n";

ESP8266WebServer server(_t6ObjectHttpPort);
T6Object object;

T6Object::T6Object() {

}
void T6Object::setId(String objectId) {
	object.id = objectId;
	if (DEBUG) {
		Serial.println("Object "+objectId+" instantiated");
	}
}
void T6Object::setSecret(String objectSecret) {
	object.secret = objectSecret;
}
void T6Object::setUA(String objectUa) {
	object.userAgent = objectUa;
}





/* Events */
//IoAbstractionRef arduinoIo = ioUsingArduino();
char slotString[20] = { 0 };
int taskId = -1;
void T6iot::log(const char* logLine) {
	Serial.print(millis());
	Serial.print(": ");
	Serial.println(logLine);
}
/* End Events */




/* t6 IoT constructor */
T6iot::T6iot(): TaskManager() {
	T6iot("api.internetcollaboratif.info", 443, "", 10000);
}
T6iot::T6iot(char* httpHost, int httpPort): TaskManager() {
	T6iot(httpHost, httpPort, "", 10000);
}
T6iot::T6iot(char* httpHost, int httpPort, char* userAgent): TaskManager() {
	T6iot(httpHost, httpPort, userAgent, 10000);
}
T6iot::T6iot(char* httpHost, int httpPort, char* userAgent, int timeout): TaskManager() {
	_httpHost = httpHost;
	_httpPort = httpPort;
	_userAgent = userAgent;
	_timeout = timeout;
	https.setTimeout(_timeout);
}




int T6iot::setWebServerCredentials(const char* username, const char* password) {
	www_username = username;
	www_password = password;
	www_realm = defaultReal;
}
int T6iot::setWebServerCredentials(const char* username, const char* password, const char* realm) {
	www_username = username;
	www_password = password;
	www_realm = realm;
}
int T6iot::setHtml() {
	_getHtmlRequest(&client, _urlObjects+String(object.id)+"/show");
	return 1;
}
int T6iot::setHtml(String html) {
	defaultHtml = html;
	return 1;
}

int T6iot::startWebServer(int port) {
	_t6ObjectHttpPort = port;
	SPIFFS.begin();
	server.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico");
	server.on("/", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) {
			//Basic Auth Method with Custom realm and Failure Response
			//return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
			//Digest Auth Method with realm="Login Required" and empty Failure Response
			//return server.requestAuthentication(DIGEST_AUTH);
			//Digest Auth Method with Custom realm and empty Failure Response
			//return server.requestAuthentication(DIGEST_AUTH, www_realm);
			//Digest Auth Method with Custom realm and Failure Response
			return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
		}
		server.send(200, "text/html; charset=UTF-8", defaultHtml);
	});

	/* MAINTENANCE ACTIONS ON OBJECT */
	server.on("/upgrade", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(201, "application/json", String("{\"action\": \"upgrade\", \"status\": \"UNDERSTOOD\", \"snack\": \"Upgrade OTA is pending. It might take a long time.\"}"));
		upgrade();
	});
	server.on("/refresh", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(201, "application/json", String("{\"action\": \"refresh\", \"status\": \"UNDERSTOOD\", \"snack\": \"Refresh is pending. It might take a long time.\"}"));
		setHtml();
	});

	/* ACTIONS ON OBJECT */
	server.on("/open", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"open\", \"status\": \"ok\", \"snack\": \"Opened\"}"));
		_messageArrived = "open";
	});
	server.on("/close", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"close\", \"status\": \"ok\", \"snack\": \"Closed\"}"));
		_messageArrived = "close";
	});
	server.on("/on", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"on\", \"status\": \"ok\", \"snack\": \"Switched On\"}"));
		_messageArrived = "on";
	});
	server.on("/off", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"off\", \"status\": \"ok\", \"snack\": \"Switched Off\"}"));
		_messageArrived = "off";
	});
	server.on("/upper", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"upper\", \"status\": \"ok\", \"snack\": \"Increased\"}"));
		_messageArrived = "upper";
	});
	server.on("/lower", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"lower\", \"status\": \"ok\", \"snack\": \"Decreased\"}"));
		_messageArrived = "lower";
	});
	server.on("/true", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"true\", \"status\": \"ok\", \"snack\": \"True\"}"));
		_messageArrived = "true";
	});
	server.on("/false", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"false\", \"status\": \"ok\", \"snack\": \"False\"}"));
		_messageArrived = "false";
	});

	/* SETTER/GETTER ON OBJECT */
	server.on("/getVal", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		float value = getValue();
		server.send(200, "application/json", String("{\"action\": \"getVal\", \"value\": \"")+value+String("\"}"));
		_messageArrived = "getVal";
	});
	//server.on(UriBraces("/setVal/{}/{}"), [=]() {
	server.on("/setVal", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		//String variable = server.pathArg(0);
		//float value = server.pathArg(1);
		float value = 10.0;
		//setValue(value);
		server.send(200, "application/json", String("{\"action\": \"setVal\", \"value\": \"")+value+String("\"}"));
		_messageArrived = "setVal";
	});
	/*
	server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), [=]() {
		String user = server.pathArg(0);
		String device = server.pathArg(1);
		server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'");
	});
	*/

	server.onNotFound([]() {
		String message = "File Not Found\n\n";
		message += "URI: ";
		message += server.uri();
		message += "\nMethod: ";
		message += (server.method() == HTTP_GET) ? "GET" : "POST";
		message += "\nArguments: ";
		message += server.args();
		message += "\n";
		for (uint8_t i = 0; i < server.args(); i++) {
			message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
		}
		server.send(404, "text/plain", message);
	});
	server.begin();
	Serial.print("ESP available at http://");
	Serial.println(WiFi.localIP());
	return 1;
}
int T6iot::startWebServer(int port, const char* username, const char* password, const char* realm) {
	_t6ObjectHttpPort = port;
	www_username = username;
	www_password = password;
	www_realm = realm;
	startWebServer(port);
}
int T6iot::startWebServer() {
	startWebServer(_t6ObjectHttpPort);
}
String T6iot::pollWebServer() {
	if(_messageArrived) {
		String msg = _messageArrived;
		_messageArrived = "";
		return msg;
	} else {
		return "";
	}
}
void T6iot::handleClient() {
	if (OTA_activated) {
		ArduinoOTA.handle();
	}
	server.handleClient();
}
void T6iot::activateOTA() {
	OTA_activated = true;
	ArduinoOTA.begin();
}
int T6iot::init(char* host, int port) {
	init(host, port, "", 3000);
}
int T6iot::init(char* host, int port, char* ua) {
	init(host, port, ua, 3000);
}
int T6iot::init(char* host, int port, char* userAgent, int timeout) {
	_httpHost = host;
	_httpPort = port;
	_userAgent = userAgent;
	_timeout = timeout;
	https.setTimeout(_timeout);
	_urlJWT = "/v2.0.1/authenticate";
	_urlStatus = "/v2.0.1/status";
	_urlIndex = "/v2.0.1/index";
	_urlDataPoint = "/v2.0.1/data/";
	_urlObjects = "/v2.0.1/objects/";
	_urlFlows = "/v2.0.1/flows/";
	_urlSnippets = "/v2.0.1/snippets/";
	_urlDashboards = "/v2.0.1/dashboards/";
	_urlRules = "/v2.0.1/rules/";
	_urlMqtts = "/v2.0.1/mqtts/";
	_urlUsers = "/v2.0.1/users/";
	_urlDatatypes = "/v2.0.1/datatypes/";
	_urlUnits = "/v2.0.1/units/";
	_urlOta = "/v2.0.1/ota/";

	if (!client.connect(_httpHost, _httpPort)) {
		if (DEBUG) {
			Serial.println("Http connection failed during init");
		}
		return 0;
	} else {
		if (DEBUG) {
			Serial.println("Http connection succeed");
			Serial.print(_httpHost);
			Serial.print(":");
			Serial.println(_httpPort);
		}
		return 1;
	}
	return 1;
}
void T6iot::setObject(char* t6ObjectSecret, char* t6ObjectId, char* t6ObjectUA) {
	_t6ObjectSecret = t6ObjectSecret;
	_t6ObjectId = t6ObjectId;
	_t6ObjectUA = t6ObjectUA;
}
void T6iot::setCredentials(const char* t6Username, const char* t6Password) {
	_t6Username = t6Username;
	_t6Password = t6Password;
	//_t6Key;
	//_t6Secret;
}
void T6iot::refreshToken() {
	if (DEBUG) {
		Serial.println("Refreshing JWT using a refresh Token:"+_refreshToken);
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during authenticate");
	}
	{
		HTTPClient https;
		BearSSL::WiFiClientSecure newSecure;
		newSecure.setFingerprint(fingerprint);
		int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlJWT);
		newSecure.setFingerprint(fingerprint);
		https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
		https.addHeader("Accept", "application/json");
		https.addHeader("Content-Type", "application/json");

		const int BUFFER_SIZE = JSON_OBJECT_SIZE(6);
		DynamicJsonDocument payload(500);
		payload[String("username")] = _t6Username;
		payload[String("refresh_token")] = _refreshToken;
		payload[String("grant_type")] = "refresh_token";

		String payloadStr;
		serializeJson(payload, payloadStr);
		if (DEBUG) {
			Serial.print("payloadStr: ");
			Serial.println(payloadStr);
		}

		int httpCode = https.POST(payloadStr);
		if (httpCode == 200 && payloadStr != "") {
			String payload = https.getString();
			if (DEBUG) {
				Serial.println("Result HTTP Status=200");
				Serial.println(payload);
			}
			DynamicJsonDocument doc(1500);
			DeserializationError error = deserializeJson(doc, payload);
			if (error && DEBUG) {
				Serial.print(F("deserializeJson() failed: "));
				Serial.println(error.c_str());
				return;
			} else {
				_JWTToken = (doc["token"]).as<String>();
				_refreshToken = (doc["refresh_token"]).as<String>();
				if (DEBUG) {
					Serial.print(F("deserializeJson() succeed: "));
					Serial.println(_JWTToken);
					Serial.println(_refreshToken);
				}
			}
		} else {
			if (DEBUG) {
				Serial.print("Error using payloadStr on refreshAuth: ");
				Serial.println(payloadStr);
			}
			_JWTToken = "";
		}
	}
}
void T6iot::authenticate() {
	authenticate(_t6Username, _t6Password);
}
void T6iot::authenticate(const char* t6Username, const char* t6Password) {
	if (DEBUG) {
		Serial.println("Authenticating to t6 using a Username/Password:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during authenticate");
	}
	_t6Username = t6Username;
	_t6Password = t6Password;
	{
		HTTPClient https;
		BearSSL::WiFiClientSecure newSecure;
		newSecure.setFingerprint(fingerprint);
		int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlJWT);
		newSecure.setFingerprint(fingerprint);
		https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
		https.addHeader("Accept", "application/json");
		https.addHeader("Content-Type", "application/json");

		const int BUFFER_SIZE = JSON_OBJECT_SIZE(6);
		DynamicJsonDocument payload(500);
		payload[String("username")] = _t6Username;
		payload[String("password")] = _t6Password;

		String payloadStr;
		serializeJson(payload, payloadStr);
		if (DEBUG) {
			Serial.print("payloadStr: ");
			Serial.println(payloadStr);
		}

		int httpCode = https.POST(payloadStr);
		if (httpCode == 200 && payloadStr != "") {
			String payload = https.getString();
			if (DEBUG) {
				Serial.println("Result HTTP Status=200");
				Serial.println(payload);
			}
			DynamicJsonDocument doc(1500);
			DeserializationError error = deserializeJson(doc, payload);
			if (error && DEBUG) {
				Serial.print(F("deserializeJson() failed: "));
				Serial.println(error.c_str());
				return;
			} else {
				_JWTToken = (doc["token"]).as<String>();
				_refreshToken = (doc["refresh_token"]).as<String>();
				if (DEBUG) {
					Serial.print(F("deserializeJson() succeed: "));
					Serial.println(_JWTToken);
					Serial.println(_refreshToken);
				}
			}
		} else {
			if (DEBUG) {
				Serial.print("Error using payloadStr: ");
				Serial.println(payloadStr);
			}
			_JWTToken = "";
		}
	}
}
void T6iot::authenticateKS() {
	return authenticateKS(_t6Key, _t6Secret);
}
void T6iot::authenticateKS(const char* t6Key, const char* t6Secret) {
	if (DEBUG) {
		Serial.println("Authenticating to t6 using a Key/Secret:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during authenticateKS");
	}
	_t6Key = t6Key;
	_t6Secret = t6Secret;
	{
		HTTPClient https;
		BearSSL::WiFiClientSecure newSecure;
		newSecure.setFingerprint(fingerprint);
		int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlJWT);
		newSecure.setFingerprint(fingerprint);
		https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
		https.addHeader("Accept", "application/json");
		https.addHeader("Content-Type", "application/json");

		const int BUFFER_SIZE = JSON_OBJECT_SIZE(6);
		DynamicJsonDocument payload(500);
		payload[String("username")] = _t6Username;
		payload[String("password")] = _t6Password;
		payload[String("grant_type")] = "access_token";

		String payloadStr;
		serializeJson(payload, payloadStr);
		if (DEBUG) {
			Serial.print("payloadStr: ");
			Serial.println(payloadStr);
		}

		int httpCode = https.POST(payloadStr);
		if (httpCode == 200 && payloadStr != "") {
			String payload = https.getString();
			if (DEBUG) {
				Serial.println("Result HTTP Status=200");
				Serial.println(payload);
			}
			DynamicJsonDocument doc(1500);
			DeserializationError error = deserializeJson(doc, payload);
			if (error && DEBUG) {
				Serial.print(F("deserializeJson() failed: "));
				Serial.println(error.c_str());
				return;
			} else {
				_JWTToken = (doc["token"]).as<String>();
				_refreshToken = (doc["refresh_token"]).as<String>();
				if (DEBUG) {
					Serial.print(F("deserializeJson() succeed: "));
					Serial.println(_JWTToken);
					Serial.println(_refreshToken);
				}
			}
		} else {
			if (DEBUG) {
				Serial.print("Error using payloadStr: ");
				Serial.println(payloadStr);
			}
			_JWTToken = "";
		}
	}
}
void T6iot::getStatus() {
	if (DEBUG) {
		Serial.println("Getting t6 Api Status:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getStatus");
	}
	BearSSL::WiFiClientSecure newSecure;
	newSecure.setFingerprint(fingerprint);
	int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlStatus, false);
	https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
	int httpCode = https.GET();
	if (httpCode == 200) {
		String payload = https.getString();
		if (DEBUG) {
			Serial.println("Result HTTP Status=200");
			Serial.println(payload);
		}
		DynamicJsonDocument doc(750);
		DeserializationError error = deserializeJson(doc, payload);
		if (error && DEBUG) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			return;
		} else {
			String version = (doc["version"]).as<String>();
			String t6BuildVersion = (doc["t6BuildVersion"]).as<String>();
			String t6BuildDate = (doc["t6BuildDate"]).as<String>();
			String status = (doc["status"]).as<String>();
			String mqttInfo = (doc["mqttInfo"]).as<String>();
			String appName = (doc["appName"]).as<String>();
			String started_at = (doc["started_at"]).as<String>();
			if (DEBUG) {
				Serial.print(F("deserializeJson() succeed: "));
			}
		}
	} else {
		if (DEBUG) {
			Serial.print("Result HTTP Status=");
			Serial.println(httpCode);
		}
	}
	https.end();
	newSecure.stop();
}
void T6iot::getDatatypes() {
	if (DEBUG) {
		Serial.println("Getting datatypes:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getDatatypes");
	}
	BearSSL::WiFiClientSecure newSecure;
	newSecure.setFingerprint(fingerprint);
	int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlDatatypes, false);
	https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
	int httpCode = https.GET();
	if (httpCode == 200) {
		String payload = https.getString();
		if (DEBUG) {
			Serial.println("Result HTTP Status=200");
			Serial.println(payload);
		}
		DynamicJsonDocument doc(500);
		DeserializationError error = deserializeJson(doc, payload);
		if (error && DEBUG) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			return;
		} else {
			if (DEBUG) {
				Serial.print(F("deserializeJson() succeed: "));
			}
		}
	} else {
		if (DEBUG) {
			Serial.print("Result HTTP Status=");
			Serial.println(httpCode);
		}
	}
	https.end();
	newSecure.stop();
}
void T6iot::getUnits() {
	if (DEBUG) {
		Serial.println("Getting units:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getUnits");
	}
	BearSSL::WiFiClientSecure newSecure;
	newSecure.setFingerprint(fingerprint);
	int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlUnits, false);
	https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
	int httpCode = https.GET();
	if (httpCode == 200) {
		String payload = https.getString();
		if (DEBUG) {
			Serial.println("Result HTTP Status=200");
			Serial.println(payload);
		}
		DynamicJsonDocument doc(500);
		DeserializationError error = deserializeJson(doc, payload);
		if (error && DEBUG) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			return;
		} else {
			if (DEBUG) {
				Serial.print(F("deserializeJson() succeed: "));
			}
		}
	} else {
		if (DEBUG) {
			Serial.print("Result HTTP Status=");
			Serial.println(httpCode);
		}
	}
	https.end();
	newSecure.stop();
}
void T6iot::getIndex() {
	if (DEBUG) {
		Serial.println("Getting index:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getIndex");
	}
	BearSSL::WiFiClientSecure newSecure;
	newSecure.setFingerprint(fingerprint);
	int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlIndex, false);
	https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
	int httpCode = https.GET();
	if (httpCode == 200) {
		String payload = https.getString();
		if (DEBUG) {
			Serial.println("Result HTTP Status=200");
			Serial.println(payload);
		}
		DynamicJsonDocument doc(5000);
		DeserializationError error = deserializeJson(doc, payload);
		if (error && DEBUG) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			return;
		} else {
			if (DEBUG) {
				Serial.print(F("deserializeJson() succeed: "));
			}
		}
	} else {
		if (DEBUG) {
			Serial.print("Result HTTP Status=");
			Serial.println(httpCode);
		}
	}
	https.end();
	newSecure.stop();
}
void T6iot::createUser() {

}
void T6iot::getUser(char* userId) {

}
void T6iot::editUser() {

}
void T6iot::createDatapoint(char* flowId, DynamicJsonDocument& payload) {
	return createDatapoint(flowId, payload, false);
}
void T6iot::createDatapoint(char* flowId, DynamicJsonDocument& payload, bool useSignature) {
	if (DEBUG) {
		Serial.println("Adding datapoint to t6:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during createDatapoint");
	}
	{
		HTTPClient https;
		BearSSL::WiFiClientSecure newSecure;
		newSecure.setFingerprint(fingerprint);
		int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlDataPoint+String(flowId));
		newSecure.setFingerprint(fingerprint);
		https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
		https.addHeader("Accept", "application/json");
		https.addHeader("Content-Type", "application/json");
		if(_JWTToken) {
			https.addHeader("Authorization", "Bearer "+String(_JWTToken));
		}

		String payloadStr;
		serializeJson(payload, payloadStr);

		if(useSignature) {
			payloadStr = _getSignedPayload(payloadStr, object.id, object.secret); // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
		}
		if (DEBUG) {
			Serial.println("payloadStr:");
			Serial.println(payloadStr);
		}
		
		int httpCode = https.POST(payloadStr);
		if (httpCode == 200 && payloadStr != "") {
			String payloadRes = https.getString();
			if (DEBUG) {
				Serial.println("Result HTTP Status=200");
				Serial.println(payloadRes);
			}
			DynamicJsonDocument doc(1500);
			DeserializationError error = deserializeJson(doc, payloadRes);
			if (error && DEBUG) {
				Serial.print(F("deserializeJson() failed: "));
				Serial.println(error.c_str());
				return;
			} else {
				
			}
		} else {
			//if (DEBUG) {
				String payloadRes = https.getString();
				Serial.print("Error:");
				Serial.print("httpCode: ");
				Serial.println(httpCode);
				Serial.print("request:");
				Serial.println(payloadStr);
				Serial.print("result:");
				Serial.println(payloadRes);
			//}
		}
	}
}
void T6iot::getDatapoints() {

}
void T6iot::createObject() {

}
void T6iot::getObjects() {

}
void T6iot::editObject() {

}
void T6iot::deleteObject() {

}
void T6iot::createFlow() {

}
void T6iot::getFlows() {

}
void T6iot::editFlow() {

}
void T6iot::deleteFlow() {

}
void T6iot::createDashboard() {

}
void T6iot::getDashboards() {

}
void T6iot::editDashboard() {

}
void T6iot::deleteDashboard() {

}
void T6iot::createSnippet() {

}
void T6iot::getSnippets() {

}
void T6iot::editSnippet() {

}
void T6iot::deleteSnippet() {

}
void T6iot::createRule() {

}
void T6iot::getRules() {

}
void T6iot::editRule() {

}
void T6iot::deleteRule() {

}
void T6iot::createMqtt() {

}
void T6iot::getMqtts() {

}
void T6iot::editMqtt() {

}
void T6iot::deleteMqtt() {

}
void T6iot::getOtaLatestVersion(String objectId) {
	if (DEBUG) {
		Serial.println("Getting t6 OTA Latest Version for an Object:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getOtaLatestVersion");
	}
	//_urlObjects+String(objectId)+String("/latest-version")
}
void T6iot::otaDeploy(const char* sourceId, String objectId) {
	if (DEBUG) {
		Serial.println("Deploying a source to Object: "+String(objectId)+" ("+String(sourceId)+")");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during otaDeploy");
	}
	//_urlOta+String(sourceId)+String("/deploy/")+String(objectId)
}
void T6iot::_getHtmlRequest(WiFiClient* client, String url) {
	if (DEBUG) {
		Serial.print("GETing from: ");
		Serial.println(url);
	}
	BearSSL::WiFiClientSecure newSecure;
	newSecure.setFingerprint(fingerprint);
	int checkBegin = https.begin(newSecure, _httpHost, _httpPort, url, false);
	https.setUserAgent("Arduino/2.2.0/t6iot-library/"+String(object.id));
	int code = https.GET();
	defaultHtml = https.getString();
	if (DEBUG) {
		if (defaultHtml != "") {
			Serial.println("Youooouuuu, I got some Html to serve.");
			//Serial.println(defaultHtml);
		} else {
			Serial.println("Oh nooooo, I don't have any Html to serve.");
		}
	}
	https.end();
	newSecure.stop();
}

void T6iot::lockSleep(int timeout) {
	_lockedSleep = true;
}

void T6iot::unlockSleep() {
	_lockedSleep = false;
}

bool T6iot::sleep(String command) {
	if(!_lockedSleep) {
		if (DEBUG) {
			Serial.println();
			Serial.println();
			Serial.println("Sleeping in few seconds...");
		}
		delay(500);
		ESP.deepSleep(SLEEP_DURATION * 1000000, WAKE_RF_DEFAULT);
		return true;
	}
	return false;
}

String T6iot::_urlEncode(String str) {
	String encodedString="";
	char c;
	char code0;
	char code1;
	char code2;
	for (int i =0; i < str.length(); i++){
		c=str.charAt(i);
		if (c == ' '){
			encodedString+= '+';
		} else if (isalnum(c)){
			encodedString+=c;
		} else{
			code1=(c & 0xf)+'0';
			if ((c & 0xf) >9){
				code1=(c & 0xf) - 10 + 'A';
			}
			c=(c>>4)&0xf;
			code0=c+'0';
			if (c > 9){
				code0=c - 10 + 'A';
			}
			code2='\0';
			encodedString+='%';
			encodedString+=code0;
			encodedString+=code1;
			//encodedString+=code2;
		}
		yield();
	}
	return encodedString;
}
String T6iot::_getSignedPayload(String& payload, String& objectId, String& secret) { // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	ArduinoJWT jwt = ArduinoJWT(secret);
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
	signedPayload["object_id"] = objectId;
	signedPayload.prettyPrintTo(Serial);

	signedPayload.printTo(signedPayloadAsString);
	*/
	return signedPayloadAsString;
}
int T6iot::ledControl(String command) {
	int state = command.toInt();
	digitalWrite(6,state);
	return 1;
}
int T6iot::activateOTA(String command) {
	ArduinoOTA.handle();
	return 1;
}
int T6iot::deployOTA() {
	if (DEBUG) {
		Serial.print("deploying using");
		Serial.println(object.id);
	}
	return deployOTA(object.id);
}
int T6iot::deployOTA(String objectId) {
	object.id = objectId;
	if (DEBUG) {
		Serial.println("Called deployOTA");
	}
	if (latestOTACall == 0 || millis() - latestOTACall > 1800000) { // Call OTA only once every 30 min
		if (!authenticated) {
			authenticate();
		}
		if (DEBUG) {
			Serial.println("Calling Api to get latest version");
		}
		latestOTACall = millis();
		getOtaLatestVersion(objectId);
		if ( strlen(sourceId) ) {
			if ( String(objectExpectedVersion) != String(currentVersion) ) {
				//if ( newVersionStatus === "200 Ready to deploy" ) { // Commented as we consider the Build is already OK on t6 server
				unsigned long currentMillis = millis();
				while (currentMillis - previousMillis >= interval && currentMillis < otaTimeout) {
					if (DEBUG) {
						Serial.println(currentMillis);
					}
					previousMillis = currentMillis;
					ledState = not(ledState);
					digitalWrite(LED_BUILTIN, ledState);
					currentMillis = millis();
				}
				// call OTA Deploy
				otaDeploy(sourceId, objectId);
				//}
			} else if (DEBUG) {
				Serial.println("No OTA needed, going to sleep now.");
			}
		} else if (DEBUG) {
			Serial.println("No sourceId detected!");
		}
	}
	return deployOTA(objectId);
}
int T6iot::upgrade() {
	if (DEBUG) {
		Serial.print("upgrading using");
		Serial.println(object.id);
	}
	return deployOTA();
}
void T6iot::startRest() {
	
}
void T6iot::handle(WiFiClient& client) {
	
}
void T6iot::setValue(float sensorValue) {
	_sensorValue = sensorValue;
}
float T6iot::getValue() {
	return _sensorValue;
}

/* T6Object */
T6Object T6iot::initObject() {
	T6Object object;
	return object;
}
T6Object T6iot::initObject(String id) {
	T6Object object;
	object.setId(id);
	return object;
}
T6Object T6iot::initObject(String id, String secret) {
	T6Object object;
	object.setId(id);
	object.setSecret(secret);
	return object;
}
T6Object T6iot::initObject(String id, String secret, String ua) {
	T6Object object;
	object.setId(id);
	object.setSecret(secret);
	object.setUA(ua);
	return object;
}
