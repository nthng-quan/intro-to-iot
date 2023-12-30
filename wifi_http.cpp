#include "wifi_http.h"

const char *ssid = "24hqiot";
const char *pass = "26102002";
// const char* ssid = "hoangtri";
// const char* pass = "0768136778";

const char* server_url = "http://192.168.1.5:5555";
const char* system_route = "/system";
const char* fire_route = "/fire";
const char* config_route = "/config";
const char* capture_route = "/capture";

const char* nodered_url = "http://192.168.1.5:1880/fire"; // IP node-red:

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

DynamicJsonDocument parseJson(const String& jsonString, size_t capacity) {
  DynamicJsonDocument jsonDoc(capacity); // Memory pool
  // Deserialize the JSON data
  DeserializationError error = deserializeJson(jsonDoc, jsonString);
  // Check for parsing errors
  if (error) {
    Serial.print(F("JSON parsing failed! Error code: "));
    Serial.println(error.c_str());
  }

  return jsonDoc;
}

const char* feline(const char* base_url, const char* route) {
    size_t resultLength = strlen(base_url) + strlen(route) + 1;
    char* result = new char[resultLength];
    strcpy(result, base_url);
    strcat(result, route);
    return result;
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

void send_data(const SensorData& data) {
    String payload = String("{\n") +
                "    \"IR\": " + data.IR_value + ",\n" +
                "    \"MQ-135\": {\n" +
                "        \"correctedPPM\": " + data.corrected_ppm + ",\n" +
                "        \"correctedRZero\": " + data.corrected_rzero + ",\n" +
                "        \"ppm\": " + data.ppm + ",\n" +
                "        \"resistance\": " + data.resistance + ",\n" +
                "        \"rzero\": " + data.mq135_rzero + "\n" +
                "    },\n" +
                "    \"DHT\": {\n" +
                "        \"humidity\": " + data.humidity + ",\n" +
                "        \"temperature\": " + data.temperature + "\n" +
                "    }\n" +
                "}\n";

    const char* url_system = feline(server_url, system_route);
    send_request(url_system, "POST", payload);
    delete[] url_system;
}

String init_check_fire(const SensorData &data) {
    const char* url_fire = feline(server_url, fire_route);
    String icf_response = send_request(url_fire, "POST", "check");
    delete[] url_fire;
    return icf_response;
}

String check_fire(const SensorData& data, const int& base_pos, const int& neck_pos) {
    String payload = String("{\n") +
        "    \"IR\": " + data.IR_value + ",\n" +
        "    \"MQ-135\": {\n" +
        "        \"correctedPPM\": " + data.corrected_ppm + ",\n" +
        "        \"correctedRZero\": " + data.corrected_rzero + ",\n" +
        "        \"ppm\": " + data.ppm + ",\n" +
        "        \"resistance\": " + data.resistance + ",\n" +
        "        \"rzero\": " + data.mq135_rzero + "\n" +
        "    },\n" +
        "    \"DHT\": {\n" +
        "        \"humidity\": " + data.humidity + ",\n" +
        "        \"temperature\": " + data.temperature + "\n" +
        "    },\n" +
        "    \"servo\": {\n" +
        "        \"neck_pos\": " + neck_pos + ",\n" +
        "        \"base_pos\": " + base_pos + "\n" +
        "    }\n" +
        "}\n";
    
    const char* url_fire = feline(server_url, fire_route);
    String fire_response = send_request(url_fire, "POST", payload);
    delete[] url_fire;
    return fire_response;
}

Config get_config() {
    const char* url_config = feline(server_url, config_route);
    String config = send_request(url_config, "GET", "");
    delete[] url_config;

    DynamicJsonDocument config_response = parseJson(config, 256);
    Config config_data = {
        int(config_response["servo_neck"]),
        int(config_response["servo_base"]),
        float(config_response["corrected_ppm"]),
        float(config_response["corrected_rzero"]),
        int(config_response["temperature"]),
        int(config_response["humidity"])
    };
    return config_data;
}

void send_notification(const SensorData& data, const int& fire, const String& img_url) {
    String payload = String("{\n") +
                "    \"IR\": " + data.IR_value + ",\n" +
                "    \"MQ-135\": {\n" +
                "        \"correctedPPM\": " + data.corrected_ppm + ",\n" +
                "        \"correctedRZero\": " + data.corrected_rzero + ",\n" +
                "        \"ppm\": " + data.ppm + ",\n" +
                "        \"resistance\": " + data.resistance + ",\n" +
                "        \"rzero\": " + data.mq135_rzero + "\n" +
                "    },\n" +
                "    \"DHT\": {\n" +
                "        \"humidity\": " + data.humidity + ",\n" +
                "        \"temperature\": " + data.temperature + "\n" +
                "    },\n" +
                "    \"check_fire\": {\n" +
                "        \"fire\": " + fire + ",\n" +
                "        \"img_url\": " + img_url + "\n" +
                "    },\n" +
                "}\n";
    
    send_request(nodered_url, "POST", payload);
}

void send_capture() {
    const char* url = feline(server_url, capture_route);
    send_request(url, "GET", "");
    delete[] url;
}
