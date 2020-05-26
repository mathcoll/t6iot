/*
  t6iot.h - 
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  Sample file to connect t6 api
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iot_h
#define t6iot_h
#include <Arduino.h>
#include <ArduinoJWT.h>
#include <sha256.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

class T6Object {
	public:
		T6Object();
		void setId(String id);
		void setSecret(String secret);
		void setUA(String ua);
		String id;
		String secret;
		String userAgent;
		int httpPort;
	private:
};
class T6Flow {
	public:
		T6Flow();
		void setId(String id);
		String id;
		String mqtt_topic;
		String unit;
		boolean save;
		boolean publish;
	private:
};

class T6iot {
  public:
    char* _userAgent;
    char* _urlJWT;
    char* _urlIndex;
    char* _urlDataPoint;
    char* _urlObjects;
    char* _urlFlows;
    char* _urlSnippets;
    char* _urlDashboards;
    char* _urlRules;
    char* _urlMqtts;
    char* _urlUsers;
    char* _urlDatatypes;
    char* _urlUnits;
    char* _urlStatus;
    char* _urlOta;
    bool DEBUG;
    String html;

    T6iot();
    T6iot(char* httpHost, int httpPort);
    T6iot(char* httpHost, int httpPort, char* _userAgent);
    T6iot(char* httpHost, int httpPort, char* _userAgent, int timeout);
    int init(char* host, int port);
    int init(char* host, int port, char* ua);
    int init(char* host, int port, char* userAgent, int timeout);
    int setWebServerCredentials(const char* t6ObjectWww_username, const char* t6ObjectWww_password);
    int setWebServerCredentials(const char* t6ObjectWww_username, const char* t6ObjectWww_password, const char* t6ObjectWww_realm);
    int webServerAllowCommand(String command);
    int startWebServer();
    int startWebServer(int port);
    int startWebServer(int port, const char* t6ObjectWww_username, const char* t6ObjectWww_password, const char* t6ObjectWww_realm);
    int setHtml();
    int setHtml(String html);
    void lockSleep(int timeout);
    void unlockSleep();
    void handleClient();
    void activateOTA();
    bool sleep(String command);

    void setCredentials(const char* t6Username, const char* t6Password);

    void authenticate();
    void authenticate(const char* t6Username, const char* t6Password);
    void authenticate(const char* t6Username, const char* t6Password, String* response);

    void setObject(char* t6ObjectId);
    void setObject(char* t6ObjectId, char* t6ObjectUA);
    void setObject(char* t6ObjectId, char* t6ObjectSecret, char* t6ObjectUA);
	T6Object initObject();
	T6Object initObject(String id);
	T6Object initObject(String id, String secret);
	T6Object initObject(String id, String secret, String ua);

    void authenticateKS(const char* t6Key, const char* t6Secret);
    void authenticateKS(const char* t6Key, const char* t6Secret, String* response);

    void getStatus(String* response);
    void getDatatypes(String* response);
    void getUnits(String* response);
    void getIndex(String* response);

    void createUser();
    void getUser(char* userId);
    void editUser();

    void createDatapoint(char* flowId, JsonObject& payload);
    void createDatapoint(char* flowId, JsonObject& payload, String* response);
    void createDatapoint(char* flowId, JsonObject& payload, bool useSignature, String* response);
    void getDatapoints();

    void createObject();
    void getObjects();
    void editObject();
    void deleteObject();

    void createFlow();
    void getFlows();
    void editFlow();
    void deleteFlow();

    void createDashboard();
    void getDashboards();
    void editDashboard();
    void deleteDashboard();

    void createSnippet();
    void getSnippets();
    void editSnippet();
    void deleteSnippet();

    void createRule();
    void getRules();
    void editRule();
    void deleteRule();

    void createMqtt();
    void getMqtts();
    void editMqtt();
    void deleteMqtt();

    void getOtaLatestVersion(String objectId, String* response);
    void otaDeploy(const char* sourceId, String objectId, String* response);

	void startRest();
	void handle(WiFiClient& client);
	int ledControl(String command);
	int activateOTA(String command);
	int deployOTA();
	int deployOTA(String objectId);
	int upgrade();

	float getValue();
	void setValue(float sensorValue);

  private:
	float _sensorValue;
    char* _httpHost;
    int _httpPort;
    int _timeout;
    bool _lockedSleep;
    const char* _t6Username;
    const char* _t6Password;
    const char* _t6Key;
    const char* _t6Secret;
    char* _t6ObjectSecret;
    char* _t6ObjectId;
    char* _t6ObjectUA;
    int _t6ObjectHttpPort;
    const char* _t6ObjectWww_username;
    const char* _t6ObjectWww_password;
    const char* _t6ObjectWww_realm;
    void _getRequest(WiFiClient* client, String url);
    void _getHtmlRequest(WiFiClient* client, String url);
    void _postRequest(WiFiClient* client, String url, JsonObject& payload);
    void _postRequest(WiFiClient* client, String url, JsonObject& payload, bool useSignature);
    void _putRequest(WiFiClient* client, String url, JsonObject& payload);
    void _deleteRequest(WiFiClient* client, String url);
    String _getSignedPayload(JsonObject& payload, char* objectId, char* secret);
    void _handleAuthenticateResponse();
    void _handleOTALatestVersionResponse();
    void _handleDatapointResponse();
    void _handleOTADeployResponse();
    void _handleShowResponse();
};

#endif
