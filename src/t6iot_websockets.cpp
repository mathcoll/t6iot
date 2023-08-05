/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/
#include "t6iot_websockets.h"
extern t6iot_Websockets t6iotWebsockets;
#include <WebSocketsClient.h>
#include <ArduinoJWT.h>
#include <base64.h>
WebSocketsClient webSocket;

t6iot_Websockets::t6iot_Websockets() {
	Serial.println("t6 > t6iot_Websockets Constructor");
	_lastUpdate = millis();
}

bool WsConnected = false;
bool claimed = false;
const int MAX_PAYLOAD_SIZE = 8 * 1024;
const int MAX_FILE_SIZE = 10240;
char file_buffer[MAX_FILE_SIZE];
int buffer_index = 0;

void t6iot_Websockets::claimObject() {
  Serial.println("[WSc] claimObject to WS server. object_id: " + _object_id);
  String payload = "{\"object_id\": \"" + _object_id + "\"}";
  ArduinoJWT jwt = ArduinoJWT(_object_secret);
  DynamicJsonDocument json(256);
  String databuf;
  json["command"] = "claimObject";
  json["object_id"] = _object_id;
  json["signature"] = jwt.encodeJWT(payload);
  serializeJson(json, databuf);
  webSocket.sendTXT(databuf);
}

bool t6iot_Websockets::sendTXT(String data) {
	if (claimed) {
		webSocket.sendTXT(data);
		return 1;
	} else {
		return 0;
	}
}

bool subscribe(String channel) {
  Serial.println("[WSc] subscribe object to WS server. Channel: " + channel);
  if (claimed) {
    DynamicJsonDocument json(256);
    String databuf;
    json["command"] = "subscribe";
    json["channel"] = channel;
    serializeJson(json, databuf);
    webSocket.sendTXT(databuf);
    return true;
  } else {
    Serial.println("[WSc] Error: Object is not yet claimed, can't subscribe.");
    return false;
  }
}

bool unsubscribe(String channel) {
  Serial.println("[WSc] unsubscribe object to WS server. Channel: " + channel);
  if (claimed) {
    DynamicJsonDocument json(256);
    String databuf;
    json["command"] = "unsubscribe";
    json["channel"] = channel;
    serializeJson(json, databuf);
    webSocket.sendTXT(databuf);
    return true;
  } else {
    Serial.println("[WSc] Error: Object is not yet claimed, can't unsubscribe.");
    return false;
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("-----------------------------------");
  Serial.printf("[WSc] type: %d\n", type);
  //Serial.printf("[WSc] payload: %s\n", payload);
  switch (type) {
    case WStype_DISCONNECTED: {
        Serial.printf("[WSc] Disconnected!\n");
        WsConnected = false;
        break;
      }
    case WStype_CONNECTED: {
        //Serial.printf("[WSc] Connected to url: ws://%s:%s%s\n", String(config.wsHost), String(config.wsPort), String(config.wsPath));
        WsConnected = true;
        //Serial.println("[WSc] SENT: Connected");
        break;
      }
    case WStype_TEXT: {
        Serial.printf("[WSc] RESPONSE: %s\n", payload);
        StaticJsonDocument <256> jsonPayload;
        DeserializationError error = deserializeJson(jsonPayload, payload);
        if (error) {
          //Serial.print(F("deSerializeJson() failed: "));
          //Serial.println(error.f_str());
          return;
        } else {
          const char* arduinoCommand = "";
          const char* val = "";
          const char* message = "";
          const char* socket_id = "";
          const char* measurement = "";
          int hallVal = 0;
          uint8_t pin = 0;

          if (jsonPayload["arduinoCommand"]) {
            arduinoCommand = jsonPayload["arduinoCommand"].as<const char*>();
          }
          if (jsonPayload["value"]) {
            val = jsonPayload["value"].as<const char*>();
          }
          if (jsonPayload["message"]) {
            val = jsonPayload["message"].as<const char*>();
          }
          if (jsonPayload["socket_id"]) {
            socket_id = jsonPayload["socket_id"].as<const char*>();
          }
          if (jsonPayload["pin"]) {
            pin = jsonPayload["pin"];
          }

          //Serial.printf("payload: %s\n", jsonPayload);
          //Serial.printf("[WSc] - arduinoCommand: %s\n", arduinoCommand);
          //Serial.printf("[WSc] - pin: %d\n", pin);
          //Serial.printf("[WSc] - value: %s\n", val);
          //Serial.printf("[WSc] - channels: %s\n", channels);
          //Serial.println();

          if (strcmp(arduinoCommand, "claimed") == 0) {
            Serial.printf("[WSc] claimObject is accepted on WS server. socket_id: %s\n", socket_id);
            //config.messageInterval = config.messageIntervalOnceClaimed;
            claimed = true;

            /*
            file = new AudioFileSourcePROGMEM( development_mp3, sizeof(development_mp3) );
            
            Serial.println(">>>>>>>>>>>>>>>>>>>");
            Serial.println(file_buffer);
            Serial.println("<<<<<<<<<<<<<<<<<<<");
            AudioOutputI2SNoDAC *audioOutputi2s;
            audioOutputi2s = new AudioOutputI2SNoDAC(0);
            audioOutputi2s->SetGain(0.9);

            if(strcmp(config.audioEncoding, "MP3") == 0) {
              Serial.println("AudioGeneratorMP3");
              mp3 = new AudioGeneratorMP3();
              //mp3->begin(file, audioOutputi2s);
              Serial.println("AudioFileSourceID3");
              id3 = new AudioFileSourceID3(file);
              id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
              Serial.println("mp3->begin");
              mp3->begin(id3, audioOutputi2s);
              while(mp3->isRunning()) {
                if (!mp3->loop()) {
                  mp3->stop();
                  Serial.println("mp3->stopped");
                }
              }
        
            } else if(strcmp(config.audioEncoding, "LINEAR16") == 0) {
              wav = new AudioGeneratorWAV();
              wav->begin(file, audioOutputi2s);
            }
            */

            if ( subscribe("demo") ) {

            }

          } else if (strcmp(arduinoCommand, "info") == 0) {
            Serial.printf("[WSc] - socket_id: %s\n", socket_id);
            Serial.printf("[WSc] - message  : %s\n", message);

          } else if (strcmp(arduinoCommand, "claimRequest") == 0) {
            Serial.println("[WSc] claimRequest");
            Serial.printf("[WSc] - message  : %s\n", message);
            t6iotWebsockets.claimObject();

          } else if (strcmp(arduinoCommand, "analogWrite") == 0) {
            Serial.println("[WSc] analogWrite");
            analogWrite(pin, atoi(val));

          } else if (strcmp(arduinoCommand, "digitalWrite") == 0) {
            Serial.println("[WSc] digitalWrite");
            Serial.printf("[WSc] value ==> %d\n", atoi(val));
            pinMode(pin, OUTPUT);
            if (atoi(val) == 0) {
              digitalWrite(pin, LOW);
            } else if (atoi(val) == 1) {
              digitalWrite(pin, HIGH);
            }

          } else if (strcmp(arduinoCommand, "analogRead") == 0) {
            String currentVal = String(analogRead(pin), DEC);
            Serial.println("[WSc] analogRead: " + currentVal);
            webSocket.sendTXT( currentVal );

          } else if (strcmp(arduinoCommand, "digitalRead") == 0) {
            String currentVal = String(digitalRead(pin), DEC);
            Serial.println("[WSc] digitalRead: " + currentVal);
            webSocket.sendTXT( currentVal );

          } else if (strcmp(arduinoCommand, "getPinMode") == 0) {
            Serial.println("[WSc] getPinMode");
            //getPinMode(pin);

          } else if (strcmp(arduinoCommand, "setPinMode") == 0) {
            Serial.println("[WSc] setPinMode");
            pinMode(pin, atoi(val));

          } else if (strcmp(arduinoCommand, "audioOutput") == 0) {
            Serial.println("[WSc] audioOutput");
            /*
            AudioOutputI2SNoDAC *audioOutputi2s;
            audioOutputi2s = new AudioOutputI2SNoDAC();
            audioOutputi2s->begin();
            ESP8266SAM *sam = new ESP8266SAM;
            sam->Say(audioOutputi2s, val);
            delete sam;
            */
          } else if (strcmp(arduinoCommand, "tts") == 0) {
            Serial.println("[WSc] tts audio file ---------------------------------<---"); // not being used
          } else if (strcmp(arduinoCommand, "measureRequest") == 0) {
            if (jsonPayload["measurement"]) {
              measurement = jsonPayload["measurement"].as<const char*>();
              if (strcmp(measurement, "hallRead") == 0) {
#ifdef ESP32
                hallVal = hallRead();
                const char* t6FlowId = "fake-flow-id-hallVal";
                Serial.print("[WSc] hallRead:");
                Serial.println(String(hallVal));
                DynamicJsonDocument payload(1024);
                payload[String("value")] = hallVal;
                payload[String("flow_id")] = t6FlowId;
                payload[String("mqtt_topic")] = "";
                payload[String("unit")] = "";
                payload[String("save")] = "false";
                payload[String("publish")] = "true";
                //createDatapoint(t6FlowId, payload, false, config.t6wsKey, config.t6wsSecret);
#endif
              } else if (strcmp(measurement, "measurementConfig1") == 0) {
                Serial.println("[WSc] measurementConfig1:");

                DynamicJsonDocument payload(1024);
                const char* t6FlowId = "fake-flow-id-measurementConfig1";
                payload[String("value")] = 123456789; // TODO TODO TODO TODO TODO TODO TODO TODO TODO
                payload[String("flow_id")] = t6FlowId;
                payload[String("mqtt_topic")] = "";
                payload[String("unit")] = "";
                payload[String("save")] = "false";
                payload[String("publish")] = "true";
                //createDatapoint(t6FlowId, payload, false, config.t6wsKey, config.t6wsSecret);
                //triggerNextMeasure(10 * 60 * 1000, "remindMeToMeasure", "measurementConfig1", obj_id); // 10 minutes;

              } else if (strcmp(measurement, "measurementConfig2") == 0) {
                const char* t6FlowId = "fake-flow-id-measurementConfig2";
                Serial.print("[WSc] measurementConfig2:");
                Serial.println(t6FlowId);
                //getSSL();

              } else if (strcmp(measurement, "measurementConfig3") == 0) {
                const char* t6FlowId = "fake-flow-id-measurementConfig3";
                Serial.print("[WSc] measurementConfig3:");
                Serial.println(t6FlowId);

              }
            }
          }
        }
        break;
      }
    case WStype_BIN: {
        Serial.printf("[WSc] WStype_BIN get binary length: %u\n", length);
        //hexdump(payload, length); // https://github.com/timum-viw/socket.io-client/issues/15

        if (length == MAX_PAYLOAD_SIZE) {
          // Check if there is enough space in the buffer for the chunk
          if (buffer_index + MAX_PAYLOAD_SIZE <= MAX_FILE_SIZE) {
            // Append the chunk to the buffer
            memcpy(file_buffer + buffer_index, payload, MAX_PAYLOAD_SIZE);
            buffer_index += MAX_PAYLOAD_SIZE;
          } else {
            // Not enough space in the buffer for the chunk
            // Do something to handle the error, such as resetting the buffer
            buffer_index = 0;
          }
        } else {
          // This is the last chunk of the message
          // Append the chunk to the buffer and process the complete file
          int last_chunk_size = length;
          if (buffer_index + last_chunk_size <= MAX_FILE_SIZE) {
            memcpy(file_buffer + buffer_index, payload, last_chunk_size);
            buffer_index += last_chunk_size;
            /*
            file = new AudioFileSourcePROGMEM( file_buffer, sizeof(file_buffer) );
            AudioOutputI2SNoDAC *audioOutputi2s;
            audioOutputi2s = new AudioOutputI2SNoDAC();
            
            if(strcmp(config.audioEncoding, "MP3") == 0) {
              mp3 = new AudioGeneratorMP3();
              mp3->begin(file, audioOutputi2s);
            } else if(strcmp(config.audioEncoding, "LINEAR16") == 0) {
              wav = new AudioGeneratorWAV();
              wav->begin(file, audioOutputi2s);
            }
            */

            Serial.println("[WSc] tts audio stored to file_buffer");

            // Reset the buffer index for the next message
            buffer_index = 0;
          } else {
            // Not enough space in the buffer for the last chunk
            // Do something to handle the error, such as resetting the buffer
            buffer_index = 0;
          }
        }
        break;
      }
    case WStype_PING: {
        Serial.printf("[WSc] get ping\n"); // pong will be send automatically
        break;
      }
    case WStype_PONG: {
        Serial.printf("[WSc] get pong\n"); // answer to a ping we send
        break;
      }
    case WStype_ERROR: {
        Serial.printf("[WSc] WStype_ERROR\n");
        break;
      }
    case WStype_FRAGMENT_TEXT_START: {
        Serial.printf("[WSc] WStype_FRAGMENT_TEXT_START\n");
        break;
      }
    case WStype_FRAGMENT_BIN_START: {
        Serial.printf("[WSc] WStype_FRAGMENT_BIN_START\n");
        break;
      }
    case WStype_FRAGMENT: {
        Serial.printf("[WSc] WStype_FRAGMENT\n");
        break;
      }
    case WStype_FRAGMENT_FIN: {
        Serial.printf("[WSc] WStype_FRAGMENT_FIN\n");
        break;
      }
    default:
      break;
  }
}

bool t6iot_Websockets::startWebsockets(String wsHost, uint16_t wsPort, String wsPath, String t6wsKey, String t6wsSecret, int messageInterval, int reconnectInterval, int timeoutInterval, int disconnectAfterFailure, String o_id, String o_secret) {
	_object_id = o_id;
	_object_secret = o_secret;
	_messageInterval = messageInterval;
	_lastUpdate = millis();
	webSocket.begin(wsHost, wsPort, wsPath);
	webSocket.onEvent(webSocketEvent);
	webSocket.setAuthorization(( String("Basic " + base64::encode(String(t6wsKey + ":" + t6wsSecret).c_str())) ).c_str());
	webSocket.setReconnectInterval(reconnectInterval);
	//webSocket.enableHeartbeat(messageInterval, timeoutInterval, disconnectAfterFailure);
	Serial.print("t6 > WebSockets connecting to ");  Serial.print(wsHost); Serial.print(":"); Serial.print(wsPort); Serial.println(wsPath);
	Serial.println("t6 > Websockets started");
	return 1;
}

void t6iot_Websockets::webSockets_loop() {
	if ( WsConnected && _lastUpdate + _messageInterval < millis() ) {
		//claimObject();
		_lastUpdate = millis();
	}
	webSocket.loop();
}
