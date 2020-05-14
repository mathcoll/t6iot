/*
  t6iot.h - 
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  Sample file to connect t6 api
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iot_h
#define t6iot_h
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <aREST.h>

class t6iot {
  public:
    int begin(char* httpHost, int httpPort);
    int begin(char* httpHost, int httpPort, char* _userAgent);
    int begin(char* httpHost, int httpPort, char* _userAgent, int timeout);
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

    void authenticate(const char* t6Username, const char* t6Password);
    void authenticate(const char* t6Username, const char* t6Password, String* response);

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
    
    void getOtaLatestVersion(char* objectId, String* response);
		void otaDeploy(const char* sourceId, char* objectId, String* response);
	
  private:
    char* _httpHost;
    int _httpPort;
    int _timeout;
    const char* _t6Username;
    const char* _t6Password;
    const char* _t6Key;
    const char* _t6Secret;
    void _getRequest(WiFiClient* client, String url);
    void _postRequest(WiFiClient* client, String url, JsonObject& payload);
    void _postRequest(WiFiClient* client, String url, JsonObject& payload, bool useSignature);
    void _putRequest(WiFiClient* client, String url, JsonObject& payload);
    void _deleteRequest(WiFiClient* client, String url);
    String _getSignedPayload(JsonObject& payload, char* objectId, char* secret);
};

#endif
