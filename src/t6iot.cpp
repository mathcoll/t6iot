/*
  t6iot.cpp - 
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  Sample file to connect t6 api
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
 */

/*#include <t6iot.h>*/
#include "t6iot.h"


#define SLEEP_DURATION  1800
const size_t MAX_CONTENT_SIZE = 512;
String _JWTToken;
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

String responseA; // for authentication
String responseD; // for datapoints
String responseDeploy; // for OTA Deploy
String responseO; // for Object

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

T6iot::T6iot() {
	T6iot("192.168.0.15", 3000, "", 10000);
}
T6iot::T6iot(char* httpHost, int httpPort) {
	T6iot(httpHost, httpPort, "", 10000);
}
T6iot::T6iot(char* httpHost, int httpPort, char* userAgent) {
	T6iot(httpHost, httpPort, userAgent, 10000);
}
T6iot::T6iot(char* httpHost, int httpPort, char* userAgent, int timeout) {
	_httpHost = httpHost;
	_httpPort = httpPort;
	_userAgent = userAgent;
	_timeout = timeout;
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
	server.on("/favicon.ico", [=]() {
		static const uint8_t ico[] PROGMEM = {
			0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
			0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
			0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
			0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
			0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
		};
		server.send(200, "image/x-icon", ico, sizeof(ico));
	});


	/* MAINTENANCE ACTIONS ON OBJECT */
	server.on("/upgrade", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(201, "text/plain", "UNDERSTOOD, but this can take a while.. and might fail.");
		upgrade();
	});
	server.on("/refresh", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(201, "text/plain", "UNDERSTOOD");
		setHtml();
	});

	/* ACTIONS ON OBJECT */
	server.on("/open", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"open\", \"status\": \"ok\", \"snack\": \"Opened\"}"));
	});
	server.on("/close", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"close\", \"status\": \"ok\", \"snack\": \"Closed\"}"));
	});
	server.on("/on", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"on\", \"status\": \"ok\", \"snack\": \"Switched On\"}"));
	});
	server.on("/off", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"off\", \"status\": \"ok\", \"snack\": \"Switched Off\"}"));
	});
	server.on("/upper", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"upper\", \"status\": \"ok\", \"snack\": \"Increased\"}"));
	});
	server.on("/lower", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		server.send(200, "application/json", String("{\"action\": \"lower\", \"status\": \"ok\", \"snack\": \"Decreased\"}"));
	});


	/* SETTER/GETTER ON OBJECT */
	server.on("/getVal", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		float value = getValue();
		server.send(200, "application/json", String("{\"action\": \"getVal\", \"value\": \"")+value+String("\"}"));
	});
	server.on("/setVal", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); }
		float value = 10.00; //atof(server.arg("value"));
		//setValue(value);
		server.send(200, "application/json", String("{\"action\": \"setVal\", \"value\": \"")+value+String("\"}"));
	});


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

void T6iot::_handleAuthenticateResponse() {
	const int A_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer A_jsonRequestBuffer(A_BUFFER_SIZE);
	JsonObject& authenticate = A_jsonRequestBuffer.parseObject(responseA);
	if (!authenticate.success()) {
		if (DEBUG) {
			Serial.println("Failure on parsing json.");
			Serial.println(responseA);
		}
	} else {
		const char* Aerror = authenticate["error"];
		const char* Atoken = authenticate["token"];
		const char* Astatus = authenticate["status"];
		const char* Arefresh_token = authenticate["refresh_token"];
		const char* ArefreshTokenExp = authenticate["refreshTokenExp"];
		if ( Aerror ) {
			if (DEBUG) {
				Serial.println("Failure on:");
				Serial.println(responseA);
			}
			authenticated = false;
		} else {
			authenticated = true;
			if (DEBUG) {
				Serial.println();
				Serial.print("\tToken: ");
				Serial.println( Atoken );
				Serial.print("\tStatus: ");
				Serial.println( Astatus );
				Serial.print("\tRefresh Token: ");
				Serial.println( Arefresh_token );
				Serial.print("\tRefresh Token Exp: ");
				Serial.println( ArefreshTokenExp );
				Serial.println();
			}
		}
	}
}
void T6iot::_handleDatapointResponse() {
	const int D_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer D_jsonRequestBuffer(D_BUFFER_SIZE);
	JsonObject& datapoint = D_jsonRequestBuffer.parseObject(responseD);
	if (!datapoint.success()) {
		if (DEBUG) {
			Serial.println("Failure on parsing json.");
			Serial.println(responseD);
		}
	} else {
		const char* Derror = datapoint["error"];
		if ( Derror && DEBUG ) {
			Serial.println("Failure on:");
			Serial.println(responseD);
			Serial.println();
			Serial.println();
		}
	}
}
void T6iot::_handleOTALatestVersionResponse() {
	const int O_BUFFER_SIZE = JSON_OBJECT_SIZE(64);
	DynamicJsonBuffer O_jsonRequestBuffer(O_BUFFER_SIZE);
	JsonObject& ota = O_jsonRequestBuffer.parseObject(responseO);
	if (!ota.success() && DEBUG) {
		Serial.println("Failure on parsing json.");
		Serial.println(responseO);
	} else if(ota["data"]["type"] == "errors") {
		/*
		if (DEBUG) {
			Serial.println();
			Serial.print("\tError: ");
			Serial.println( ota["data"]["type"]["attributes"]["code"] );
			Serial.println( ota["data"]["type"]["attributes"]["message"] );
			Serial.println();
		}
		*/
	} else {
		objectExpectedVersion = ota["objectExpectedVersion"];
		sourceLatestVersion = ota["sourceLatestVersion"];
		buildVersions = ota["buildVersions"];
		sourceId = ota["source_id"];
		if (DEBUG) {
			Serial.println();
			Serial.print("\tcurrentVersion: ");
			Serial.println( currentVersion );
			Serial.print("\tobjectExpectedVersion: ");
			Serial.println( objectExpectedVersion );
			Serial.print("\tsourceLatestVersion: ");
			Serial.println( sourceLatestVersion );
			Serial.print("\tsourceId: ");
			Serial.println( sourceId );
			Serial.println();
		}
	}
}
void T6iot::_handleOTADeployResponse() {
	const int O2_BUFFER_SIZE = JSON_OBJECT_SIZE(32);
	DynamicJsonBuffer O2_jsonRequestBuffer(O2_BUFFER_SIZE);
	JsonObject& otadeploy = O2_jsonRequestBuffer.parseObject(responseDeploy);
	if (!otadeploy.success() && DEBUG) {
		Serial.println("Failure on parsing json.");
		Serial.println(responseDeploy);
	} else {
		const char* data = otadeploy["data"];
		if (DEBUG) {
			Serial.println();
			Serial.print("\tdata: ");
			Serial.println( data );
			Serial.println();
		}
	}
}

void T6iot::_handleShowResponse() {
	String* res = &responseO;
	const int O_BUFFER_SIZE = JSON_OBJECT_SIZE(32);
	DynamicJsonBuffer O_jsonRequestBuffer(O_BUFFER_SIZE);
	JsonObject& show = O_jsonRequestBuffer.parseObject(responseO);
	if (!show.success()) {
		if (DEBUG) {
			Serial.println("Failure on parsing.");
			Serial.println(responseO);
		}
	} else {
		defaultHtml = responseO;
	}
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
void T6iot::authenticate() {
	authenticate(_t6Username, _t6Password, &responseA);
}
void T6iot::authenticate(const char* t6Username, const char* t6Password) {
	_t6Username = t6Username;
	_t6Password = t6Password;
	authenticate(_t6Username, _t6Password, &responseA);
}
void T6iot::authenticate(const char* t6Username, const char* t6Password, String* res) {
	if (DEBUG) {
		Serial.println("Authenticating to t6 using a Username/Password:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during authenticate");
	}
	_t6Username = t6Username;
	_t6Password = t6Password;
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);
	JsonObject& payload = jsonRequestBuffer.createObject();
	payload["username"] = _t6Username;
	payload["password"] = _t6Password;

	_postRequest(&client, _urlJWT, payload, false);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
		JsonObject& response = jsonRequestBuffer.parseObject(lineChars);
		_JWTToken = response.get<char*>("token");
	}
	_handleAuthenticateResponse();
}

void T6iot::authenticateKS(const char* t6Key, const char* t6Secret) {
	return authenticateKS(t6Key, t6Secret, NULL);
}
void T6iot::authenticateKS(const char* t6Key, const char* t6Secret, String* res) {
	if (DEBUG) {
		Serial.println("Authenticating to t6 using a Key/Secret:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during authenticateKS");
	}
	_t6Key = t6Key;
	_t6Secret = t6Secret;
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);
	JsonObject& payload = jsonRequestBuffer.createObject();
	payload["key"] = t6Key;
	payload["secret"] = t6Secret;
	payload["grant_type"] = "access_token";

	_postRequest(&client, _urlJWT, payload, false);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
		JsonObject& response = jsonRequestBuffer.parseObject(lineChars);
		_JWTToken = response.get<char*>("token");
	}
}
void T6iot::getStatus(String* res) {
	if (DEBUG) {
		Serial.println("Getting t6 Api Status:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getStatus");
	}
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);

	_getRequest(&client, _urlStatus);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
}
void T6iot::getDatatypes(String* res) {
	if (DEBUG) {
		Serial.println("Getting datatypes:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getDatatypes");
	}
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);

	_getRequest(&client, _urlDatatypes);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
}
void T6iot::getUnits(String* res) {
	if (DEBUG) {
		Serial.println("Getting units:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getUnits");
	}
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);

	_getRequest(&client, _urlUnits);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
}
void T6iot::getIndex(String* res) {
	if (DEBUG) {
		Serial.println("Getting index:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getIndex");
	}
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);

	_getRequest(&client, _urlIndex);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
}
void T6iot::createUser() {

}
void T6iot::getUser(char* userId) {

}
void T6iot::editUser() {

}
void T6iot::createDatapoint(char* flowId, JsonObject& payload) {
	return createDatapoint(flowId, payload, false, &responseD);
}
void T6iot::createDatapoint(char* flowId, JsonObject& payload, String* res) {
	return createDatapoint(flowId, payload, false, res);
}
void T6iot::createDatapoint(char* flowId, JsonObject& payload, bool useSignature, String* res) {
	if (DEBUG) {
		Serial.println("Adding datapoint to t6:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during createDatapoint");
	}

	_postRequest(&client, _urlDataPoint+String(flowId), payload, useSignature);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
	_handleDatapointResponse();
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
void T6iot::getOtaLatestVersion(String objectId, String* res) {
	if (DEBUG) {
		Serial.println("Getting t6 OTA Latest Version for an Object:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getOtaLatestVersion");
	}
	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);

	_getRequest(&client, _urlObjects+String(objectId)+String("/latest-version"));

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
	_handleOTALatestVersionResponse();
}
void T6iot::otaDeploy(const char* sourceId, String objectId, String* res) {
	if (DEBUG) {
		Serial.println("Deploying a source to Object: "+String(objectId)+" ("+String(sourceId)+")");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during otaDeploy");
	}

	StaticJsonBuffer<400> jsonBuffer;
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(1);
	DynamicJsonBuffer jsonRequestBuffer(BUFFER_SIZE);
	JsonObject& payload = jsonRequestBuffer.createObject();
	_postRequest(&client, _urlOta+String(sourceId)+String("/deploy/")+String(objectId), payload);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line.length() == 1) { //empty line means end of headers
			break;
		}
	}
	//read first line of body
	while (client.available()) {
		String line = client.readStringUntil('\n');
		const char* lineChars = line.c_str();
		res->concat(line);
	}
	_handleOTADeployResponse();
}
void T6iot::_getHtmlRequest(WiFiClient* client, String url) {
	if (DEBUG) {
		Serial.print("GETing from: ");
		Serial.println(url);
	}
	// TODO: bearSSL is only being used in this _getHtmlRequest. It should also be used in all the other methods
	BearSSL::WiFiClientSecure newSecure;
	newSecure.setFingerprint(fingerprint);
	int checkBegin = https.begin(newSecure, _httpHost, _httpPort, url, false);
	int code = https.GET();
	/*
	Serial.print("checkBegin:");
	Serial.println(checkBegin);
	Serial.print("code:");
	Serial.println(code);
	*/
	defaultHtml = https.getString();
	if (DEBUG) {
		if (defaultHtml != "") {
			Serial.println("Youooouuuu, I got some Html to serve.");
			Serial.println(defaultHtml);
		} else {
			Serial.println("Oh nooooo, I don't have any Html to serve.");
		}
	}
	delay(_timeout);
	https.end();
	newSecure.stop();
}
void T6iot::_getRequest(WiFiClient* client, String url) {
	if (DEBUG) {
		Serial.print("GETing from: ");
		Serial.println(url);
	}
	client->print("GET ");
	client->print(url);
	client->println(" HTTP/1.1");
	client->print("Host: ");
	client->println(_httpHost);
	client->print("User-Agent: Arduino/2.2.0/t6iot-library/");
	client->println(_userAgent);
	if (_JWTToken) {
		client->print("Authorization: Bearer ");
		client->println(_JWTToken);
	}
	client->println("Accept: application/json");
	client->println("Content-Type: application/json");
	client->println("Connection: close");
	client->println();

	delay(_timeout);
}

void T6iot::_postRequest(WiFiClient* client, String url, JsonObject& payload) {
	return _postRequest(client, url, payload, false);
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

void T6iot::_postRequest(WiFiClient* client, String url, JsonObject& payload, bool useSignature) {
	String payloadStr;
	payload.printTo(payloadStr);
	if (DEBUG) {
		Serial.print("POSTing (_postRequest) to: ");
		Serial.println(url);
	}
	client->print("POST ");
	client->print(url);
	client->println(" HTTP/1.1");
	client->print("Host: ");
	client->println(_httpHost);
	client->print("User-Agent: Arduino/2.2.0/t6iot-library/");
	client->println(_userAgent);
	if (_JWTToken) {
		client->print("Authorization: Bearer ");
		client->println(_JWTToken);
	}
	client->println("Accept: application/json");
	client->println("Content-Type: application/json");
	client->print("Content-Length: ");

	if (useSignature==true) {
		Serial.print("as signed payload:");
		payloadStr = _getSignedPayload(payload, _t6ObjectId, _t6ObjectSecret);
		client->println(payloadStr.length());
		client->println("Connection: close");
		client->println();
		client->println(payloadStr);
	} else {
		client->println(payloadStr.length());
		client->println("Connection: close");
		client->println();
		payload.printTo(*client);
	}
	client->println();
	delay(_timeout);
}
String T6iot::_getSignedPayload(JsonObject& payload, char* objectId, char* secret) {
	ArduinoJWT jwt = ArduinoJWT(secret);
	String signedJson;
	String payloadString;
	String signedPayloadAsString;

	payload.printTo(payloadString);
	signedJson = jwt.encodeJWT( payloadString );

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(25);
	StaticJsonBuffer<BUFFER_SIZE> jsonBufferSigned;
	JsonObject& signedPayload = jsonBufferSigned.createObject();
	signedPayload["signedPayload"] = signedJson;
	signedPayload["object_id"] = objectId;
	signedPayload.prettyPrintTo(Serial);

	signedPayload.printTo(signedPayloadAsString);
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
		getOtaLatestVersion(objectId, &responseO);
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
				otaDeploy(sourceId, objectId, &responseDeploy);
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
void T6iot::setValue(float sensorValue) {
	_sensorValue = sensorValue;
}
float T6iot::getValue() {
	return _sensorValue;
}
