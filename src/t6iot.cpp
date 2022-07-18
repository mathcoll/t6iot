/*
  t6iot.cpp - v1.4.3
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
 */

#include <t6iot.h>
#include <IoAbstraction.h>

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
String _userAgent;
String _messageArrived = "";
String _parameterArrived = "";
const char* www_username;
const char* www_password;
const char* www_realm;
bool DEBUG = false;
bool LOGS = false;
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

WiFiClient client;
HTTPClient https;

ESP8266WebServer serverHttp(_t6ObjectHttpPort);
BearSSL::ESP8266WebServerSecure server(443);
T6Object object;

const char* fingerprint = "4A F8 05 93 7E 1B 59 0F CA 2C E4 C3 CB 9A 19 96 24 5D 56 05"; // SHA1 fingerprint of the certificate sni.cloudflaressl.com // valid until Tue, 13 Jun 2023 23:59:59 GMT
static const char serverCert[] PROGMEM = R"EOF(
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
)EOF";

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
	_userAgent = String(_userAgent)+String(userAgent);
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
	LittleFS.begin();
	if (LittleFS.exists("/ui.html")) {
		File UIFile = LittleFS.open("/ui.html", "r");
		Serial.println("UIFile=");
		Serial.println(UIFile);
		defaultHtml = UIFile.readString();
		Serial.println("defaultHtml from FS=");
		Serial.println(defaultHtml);
		UIFile.close();
	} else {
		defaultHtml = getShowUIHtml(_urlObjects+String(object.id)+"/show");
		if (DEBUG) {
			Serial.print("getShowUIHtml=");
			Serial.println(_urlObjects+String(object.id)+"/show");
			Serial.print("defaultHtml=");
			Serial.println(defaultHtml);
		}
	}
	return 1;
}
int T6iot::setHtml(String html) {
	defaultHtml = html;
	return 1;
}
String T6iot::getTimeDateString() {
	/*
	if (DEBUG) {
		Serial.print("timeClient.getEpochTime()=");
		Serial.println(timeClient.getEpochTime());
	}
	*/
	//"dd/mmm/YYYY:HH:mm:ss -Z";
	return String(timestamp + millis()/1000);
	//return (timeClient.getEpochTime() / 86400)%7+" "+timeClient.getFormattedTime();
}
int T6iot::startWebServer(int port) {
	_t6ObjectHttpPort = port;
	LittleFS.begin();
	//timeClient.begin();

	serverHttp.on("/", [=]() {
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+" -> Redirecting to https.");
		}
		serverHttp.sendHeader("Location ", "https://"+String(WiFi.localIP().toString().c_str()), true);
		serverHttp.send(301, "text/plain", "");
	});

	server.on("/", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+" -"+www_username+" ["+getTimeDateString()+"] \"GET / HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "text/html; charset=UTF-8", defaultHtml);
	});
	
	server.on("/favicon.ico", [=]() {
		if (LittleFS.exists("/favicon.ico")) {
			File f = LittleFS.open("/favicon.ico", "r");
			server.send(200, "image/x-icon", f.readString());
			if (LOGS) {
				String IPaddrstr = server.client().remoteIP().toString().c_str();
				String headerReferer = server.header("Referer");
				String headerUA = server.header("User-Agent");
				Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /favicon.ico HTTP/1.1\" 201 - \""+headerReferer+"\" \""+headerUA+"\"");
			}
			f.close();
		} else {
			server.send(404, "text/plain", "Not found");
		}
	});
	server.on("/icon-16x16.png", [=]() {
		if (LittleFS.exists("/icon-16x16.png")) {
			File f = LittleFS.open("/icon-16x16.png", "r");
			server.send(200, "image/png", f.readString());
			if (LOGS) {
				String IPaddrstr = server.client().remoteIP().toString().c_str();
				String headerReferer = server.header("Referer");
				String headerUA = server.header("User-Agent");
				Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /icon-16x16.png HTTP/1.1\" 201 - \""+headerReferer+"\" \""+headerUA+"\"");
			}
			f.close();
		} else {
			server.send(404, "text/plain", "Not found");
		}
	});
	server.on("/icon-32x32.png", [=]() {
		if (LittleFS.exists("/icon-32x32.png")) {
			File f = LittleFS.open("/icon-32x32.png", "r");
			server.send(200, "image/png", f.readString());
			if (LOGS) {
				String IPaddrstr = server.client().remoteIP().toString().c_str();
				String headerReferer = server.header("Referer");
				String headerUA = server.header("User-Agent");
				Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /icon-32x32.png HTTP/1.1\" 201 - \""+headerReferer+"\" \""+headerUA+"\"");
			}
			f.close();
		} else {
			server.send(404, "text/plain", "Not found");
		}
	});
	server.on("/sw.js", [=]() {
		if (LittleFS.exists("/sw.js")) {
			File f = LittleFS.open("/sw.js", "r");
			server.send(200, "application/javascript", f.readString());
			if (LOGS) {
				String IPaddrstr = server.client().remoteIP().toString().c_str();
				String headerReferer = server.header("Referer");
				String headerUA = server.header("User-Agent");
				Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /sw.js HTTP/1.1\" 201 - \""+headerReferer+"\" \""+headerUA+"\"");
			}
			f.close();
		} else {
			server.send(404, "text/plain", "Not found");
		}
	});

	/* MAINTENANCE ACTIONS ON OBJECT */
	server.on("/upgrade", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /upgrade HTTP/1.1\" 201 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(201, "application/json", String("{\"action\": \"upgrade\", \"status\": \"UNDERSTOOD\", \"snack\": \"Upgrade OTA is pending. It might take a long time.\"}"));
		upgrade();
	});
	server.on("/refresh", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /refresh HTTP/1.1\" 201 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(201, "application/json", String("{\"action\": \"refresh\", \"status\": \"UNDERSTOOD\", \"snack\": \"Refresh is pending. It might take a long time.\"}"));
		setHtml();
	});

	/* ACTIONS ON OBJECT */
	server.on("/open", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /open HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"open\", \"status\": \"ok\", \"snack\": \"Opened\"}"));
		_messageArrived = "open";
	});
	server.on("/close", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /close HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"close\", \"status\": \"ok\", \"snack\": \"Closed\"}"));
		_messageArrived = "close";
	});
	server.on("/on", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /on HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"on\", \"status\": \"ok\", \"snack\": \"Switched On\"}"));
		_messageArrived = "on";
	});
	server.on("/off", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /off HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"off\", \"status\": \"ok\", \"snack\": \"Switched Off\"}"));
		_messageArrived = "off";
	});
	server.on("/upper", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /upper HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"upper\", \"status\": \"ok\", \"snack\": \"Increased\"}"));
		_messageArrived = "upper";
	});
	server.on("/lower", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /lower HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"lower\", \"status\": \"ok\", \"snack\": \"Decreased\"}"));
		_messageArrived = "lower";
	});
	server.on("/true", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /true HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"true\", \"status\": \"ok\", \"snack\": \"True\"}"));
		_messageArrived = "true";
	});
	server.on("/false", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /false HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(200, "application/json", String("{\"action\": \"false\", \"status\": \"ok\", \"snack\": \"False\"}"));
		_messageArrived = "false";
	});

	/* SETTER/GETTER ON OBJECT */
	server.on("/getVal", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /getVal HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		float value = getValue();
		server.send(200, "application/json", String("{\"action\": \"getVal\", \"status\": \"ok\", \"sensorValue\": \"")+value+String("\", \"value\": \"")+value+String("\"}"));
		_messageArrived = "getVal";
	});
	//server.on(UriBraces("/setVal/{}/{}"), [=]() {
	server.on("/setVal", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /setVal HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		//String variable = server.pathArg(0);
		//float value = server.pathArg(1);
		float value = 10.0;
		//setValue(value);
		server.send(200, "application/json", String("{\"action\": \"setVal\", \"status\": \"ok\", \"value\": \"")+value+String("\"}"));
		_messageArrived = "setVal";
	});
	server.on("/setLight", [=]() {
		if (www_username!="" && www_password!="" && !server.authenticate(www_username, www_password)) { return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse); /* BASIC_AUTH */ }
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \"GET /setLight HTTP/1.1\" 200 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		//String variable = server.pathArg(0);
		String value = server.arg("value");
		//setValue(value);
		server.send(200, "application/json", String("{\"action\": \"setLight\", \"status\": \"ok\", \"value\": \"")+value+String("\"}"));
		_messageArrived = "setLight";
		_parameterArrived = value;
	});
	/*
	server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), [=]() {
		String user = server.pathArg(0);
		String device = server.pathArg(1);
		server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'");
	});
	*/

	//server.serveStatic("/favicon.ico", LittleFS, "/favicon.ico");
	//server.serveStatic("/icon-16x16.png", LittleFS, "/icon-16x16.png");
	//server.serveStatic("/icon-32x32.png", LittleFS, "/icon-32x32.png");
	//server.serveStatic("/sw.js", LittleFS, "/sw.js");
	//server.serveStatic("/", LittleFS, "/");

	server.onNotFound([=]() {
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
		if (LOGS) {
			String IPaddrstr = server.client().remoteIP().toString().c_str();
			String headerReferer = server.header("Referer");
			String headerUA = server.header("User-Agent");
			Serial.println(IPaddrstr+www_username+" ["+getTimeDateString()+"] \""+((server.method() == HTTP_GET) ? "GET" : "POST")+" "+server.uri()+" HTTP/1.1\" 404 - \""+headerReferer+"\" \""+headerUA+"\"");
		}
		server.send(404, "text/plain", message);
	});
	serverHttp.begin();

	server.getServer().setRSACert(new BearSSL::X509List(_serverCert), new BearSSL::PrivateKey(_serverKey));
	server.begin();
	Serial.print("ESP available at https://");
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
int T6iot::startWebServer(const char* cert, const char* key) {
	_serverCert = cert;
	_serverKey = key;
	startWebServer(_t6ObjectHttpPort);
}
String T6iot::pollWebServer() {
	if(_messageArrived) {
		String msg = _messageArrived;
		_messageArrived = "";
		//_parameterArrived = "";
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
	serverHttp.handleClient();
}
void T6iot::activateOTA() {
	OTA_activated = true;
	ArduinoOTA.begin();
}
int T6iot::init(const char* host, int port) {
	init(host, port, "", 3000);
}
int T6iot::init(const char* host, int port, const char* ua) {
	init(host, port, ua, 3000);
}
int T6iot::init(const char* host, int port, const char* userAgent, int timeout) {
	_httpHost = host;
	_httpPort = port;
	_userAgent = "Arduino/2.2.0/t6iot-library/"+String(userAgent);
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
	_userAgent = t6ObjectUA;
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
		https.setUserAgent(String(_userAgent));
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
				Serial.println("Result HTTP Code=200");
				Serial.println(payload);
			}
			DynamicJsonDocument doc(1500);
			DeserializationError error = deserializeJson(doc, payload);
			if (!error) {
				_JWTToken = (doc["token"]).as<String>();
				_refreshToken = (doc["refresh_token"]).as<String>();
				if (DEBUG) {
					Serial.print(F("deserializeJson() succeed: "));
					Serial.println(_JWTToken);
					Serial.println(_refreshToken);
				}
			} else {
				if (DEBUG) {
					Serial.print(F("deserializeJson() failed: "));
					Serial.println(error.c_str());
					return;
				}
			}
			doc.clear();
		} else if (httpCode == 401 || httpCode == 403) {
			authenticate(_t6Username, _t6Password);
		} else {
			if (DEBUG) {
				Serial.print("Error using payloadStr on refreshAuth: ");
				Serial.println(payloadStr);
				Serial.print("Result HTTP Code=");
				Serial.println(httpCode);
			}
			_JWTToken = "";
		}
		https.end();
		newSecure.stop();
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
		https.setUserAgent(String(_userAgent));
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
				Serial.println("Result HTTP Code=200");
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
			doc.clear();
		} else {
			if (DEBUG) {
				Serial.print("Error using payloadStr: ");
				Serial.println(payloadStr);
				Serial.print("Result HTTP Code=");
				Serial.println(httpCode);
			}
			_JWTToken = "";
		}
		https.end();
		newSecure.stop();
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
		https.setUserAgent(String(_userAgent));
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
			doc.clear();
		} else {
			if (DEBUG) {
				Serial.print("Error using payloadStr: ");
				Serial.println(payloadStr);
			}
			_JWTToken = "";
		}
		https.end();
		newSecure.stop();
	}
}
void T6iot::getStatus() {
	if (DEBUG) {
		Serial.println("Getting t6 Api Status:");
	}
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getStatus");
	}
	{
		HTTPClient https;
		BearSSL::WiFiClientSecure newSecure;
		newSecure.setFingerprint(fingerprint);
		int checkBegin = https.begin(newSecure, _httpHost, _httpPort, _urlStatus, false);
		https.setUserAgent(String(_userAgent));
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
}
void T6iot::createDatapoint(const char* flowId, DynamicJsonDocument& payload) {
	return createDatapoint(flowId, payload, false);
}
void T6iot::createDatapoint(const char* flowId, DynamicJsonDocument& payload, bool useSignature) {
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
		https.setUserAgent(String(_userAgent));
		https.addHeader("Accept", "application/json");
		https.addHeader("Content-Type", "application/json");
		if(_JWTToken) {
			https.addHeader("Authorization", "Bearer "+String(_JWTToken));
		}

		String payloadStr;
		serializeJson(payload, payloadStr);

		if(useSignature) {
			payloadStr = _getSignedPayload(payloadStr, object.id, object.secret); // TODO
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
			String payloadRes = https.getString();
			Serial.print("Error:");
			Serial.print("httpCode: ");
			Serial.println(httpCode);
			Serial.print("request:");
			Serial.println(payloadStr);
			Serial.print("result:");
			Serial.println(payloadRes);
		}
		https.end();
		newSecure.stop();
	}
}
void T6iot::getDatapoints() {

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
String T6iot::getShowUIHtml(String url) {
	if (!client.connect(_httpHost, _httpPort) && DEBUG) {
		Serial.println("Http connection failed during getShowUIHtml");
	}
	{
		WiFiClientSecure httpsClient;
		if (DEBUG) { Serial.printf("Using fingerprint '%s'\n", fingerprint); }
		httpsClient.setFingerprint(fingerprint);
		httpsClient.setTimeout(15000); // 15 Seconds
		delay(1000);
		if (DEBUG) { Serial.println("HTTPS Connecting"); }
		int r=0; //retry counter
		while((!httpsClient.connect(_httpHost, _httpPort)) && (r < 30)){
			delay(100);
			Serial.print(".");
			r++;
		}
		if (DEBUG) {
			if(r==30) {
				Serial.println("Connection failed");
			} else {
				Serial.println("Connected to web");
			}
			Serial.print("GETing from: ");
			Serial.print(_httpHost);
			Serial.println(url);
		}
		httpsClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
				"Host: " + _httpHost + "\r\n" +
				"User-Agent: " + String(_userAgent) + "\r\n" +
				"Accept: text/html\r\n" +
				"Content-Type: text/html; charset=UTF-8\r\n" +
				"Authorization: Bearer "+String(_JWTToken)+"\r\n" +
				"Connection: close\r\n\r\n");
		if (DEBUG) { Serial.println("Request sent."); }
		while (httpsClient.connected()) {
			String lineH = httpsClient.readStringUntil('\n');
			if (lineH == "\r") {
				if (DEBUG) { Serial.println("headers received :"); }
				break;
			}
		}
		if (DEBUG) {
		}
		String line;
		Serial.println("reply was: ->");
		while(httpsClient.available()){
			line += httpsClient.readStringUntil('\n');
			if (DEBUG) { Serial.println(line); }
		}
		Serial.println("<-");
		return line;
	}
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
