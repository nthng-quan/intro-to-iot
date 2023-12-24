#include "wifi_http.h"

const char *ssid = "24hqiot";
const char *pass = "26102002";
const char* server_url = "http://192.168.1.5:5555/upload";
const char* server_url_fire = "http://192.168.1.5:5555/fire";

void initWiFi() {
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

String send_request(const char* endpoint, const char* method, String payload) {
    if ((WiFi.status() == WL_CONNECTED)) {
        WiFiClient wifi_client;
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        http.begin(wifi_client, endpoint);  // HTTP
        http.addHeader("Content-Type", "application/json");

        Serial.print("[HTTP] ");
        Serial.print(method);
        Serial.println("...");

        int httpCode;
        if (strcmp(method, "POST") == 0) {
            httpCode = http.POST(payload);
        } else if (strcmp(method, "GET") == 0) {
            httpCode = http.GET();
        } else {
            Serial.println("Invalid HTTP method");
            return "";
        }

        if (httpCode > 0) {
            Serial.printf("[HTTP] %s... code: %d\n", method, httpCode);

            if (httpCode == HTTP_CODE_OK) {
                String responsePayload = http.getString();
                Serial.println("received payload:\n<<");
                Serial.println(responsePayload);
                Serial.println(">>");
                return responsePayload;
            }
        } else {
            Serial.printf("[HTTP] %s... failed, error: %s\n", method, http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }

    return "";
}

void send_data(int IR, float temperature, float humidity, float rzero, float correctedRZero, float resistance, float ppm, float correctedPPM){
    String payload = String("{\n") +
                "    \"data\": {\n" +
                "        \"IR\": " + IR + ",\n" +
                "        \"MQ-135\": {\n" +
                "            \"correctedPPM\": " + correctedPPM + ",\n" +
                "            \"correctedRZero\": " + correctedRZero + ",\n" +
                "            \"ppm\": " + ppm + ",\n" +
                "            \"resistance\": " + resistance + ",\n" +
                "            \"rzero\": " + rzero + "\n" +
                "        },\n" +
                "        \"DHT\": {\n" +
                "            \"humidity\": " + humidity + ",\n" +
                "            \"temperature\": " + temperature + "\n" +
                "        }\n" +  // Remove the extra comma here
                "    }\n" +
                "}\n";

    String response;
    response = send_request(server_url, "POST", payload);
    Serial.println("sent");
    Serial.println(response);
    // return response;
}

String check_fire(int IR, float temperature, float humidity, float rzero, float correctedRZero, float resistance, float ppm, float correctedPPM) {
    String fire_response = "";
    if(temperature > 10){
        fire_response = send_request(server_url_fire, "GET", "");
        return fire_response;
    }
    return "";
}
