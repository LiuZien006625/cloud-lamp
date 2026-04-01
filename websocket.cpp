#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h> 
#include <LittleFS.h>

// Change the ssid to your preferred WiFi network name
// Password not needed if you want the AP (Access Point) to be open
const char* ssid = "Arduino-ESP32";
const char* password = "88888888";

// Define LED GPIO pins
const int ledPin1 = 16;
const int ledPin2 = 17;
const int ledPin3 = 18;
// const int ledPin4 = 19;

// LED states
bool ledState1 = LOW;
bool ledState2 = LOW;
bool ledState3 = LOW;
// bool ledState4 = LOW;

// Configure an IP address for the Access Point
IPAddress ap_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress  subnet(255, 255, 255, 0);

AsyncWebServer server(80); // AsyncWebServer object on port 80
AsyncWebSocket ws("/ws"); // WebSocket server at /ws
JsonDocument receivedJson; // Json object to hold received data from clients

void notifyClients() {
    String jsonMessage1 = "{\"id\":1,\"state\":" + String(ledState1) + "}";
    String jsonMessage2 = "{\"id\":2,\"state\":" + String(ledState2) + "}";
//    String jsonMessage4 = "{\"id\":4,\"state\":" + String(ledState4) + "}"; 
    String jsonMessage3 = "{\"id\":3,\"state\":" + String(ledState3) + "}";
 
    ws.textAll(jsonMessage1);
    ws.textAll(jsonMessage2);
    ws.textAll(jsonMessage3);
    // ws.textAll(jsonMessage4);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char*)data;
    Serial.println(message);

    deserializeJson(receivedJson, message);

    int ledId = receivedJson["id"];

    switch (lampMode) {
            case 1:
                thunderstorm();
                break;
            case 2:
                breathing();
                break;
            case 3:
                rainbow();
                break;
            case 4:
                meteor();
                break;
    }

    notifyClients();
    }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        notifyClients(); // Send initial states to newly connected client
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    // Serve static UI files (index.html, script.js, style.css) from LittleFS.
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    server.begin();
}

// Initialize Wi-Fi Access Point and Web Server
void initAP() {
    Serial.print("Setting AP (Access Point)…");
    WiFi.mode(WIFI_AP);
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, password);
    if (!WiFi.softAPConfig(ap_ip, gateway, subnet)) {
        Serial.println("AP configuration failed");
    } else {
        // Print ESP IP Address
        IPAddress IP = WiFi.softAPIP();
        Serial.println("AP configuration successful");
        Serial.print("AP IP address: ");
        Serial.println(IP);
    }
}

void setup() {
    Serial.begin(115200);
    delay(5000);
    Serial.println("\nStarting LittleFS...");

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
    }

    // if (!LittleFS.begin(false, "/littlefs", 10, "ffat")){
    //     Serial.println("An Error occurred while mounting LittleFS");
    // } else {
    //     Serial.println("LittleFS mounted successfully");
    // }



    initAP();
    initWebSocket();
}

void loop() {
    ws.cleanupClients();
    // digitalWrite(ledPin1, ledState1);
    // digitalWrite(ledPin2, ledState2);
    // digitalWrite(ledPin3, ledState3);
    // digitalWrite(ledPin4, ledState4);
    Serial.println("LED1: " + String(ledState1) + " LED2: " + String(ledState2) + " LED3: " + String(ledState3) );
    delay(1000);
}