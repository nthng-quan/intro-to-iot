#include "wifi_http.h"

const char *ssid = "24hqiot";
const char *pass = "26102002";
const char* server_url = "http://192.168.1.5:5555/upload";
const char* server_url_fire = "http://192.168.1.5:5555/fire";
const char* server_url_config = "http://192.168.1.5:5555/config";
const char* web_url = "http://192.168.1.5:5555/fire"; // IP node-red:

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
  DynamicJsonDocument jsonDoc(capacity); // Adjust the size based on your JSON data

  // Deserialize the JSON data
  DeserializationError error = deserializeJson(jsonDoc, jsonString);

  // Check for parsing errors
  if (error) {
    Serial.print(F("JSON parsing failed! Error code: "));
    Serial.println(error.c_str());
  }

  return jsonDoc;
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

    
    send_request(server_url, "POST", payload);
}

String init_check_fire(const SensorData &data) {
    String icf_response = send_request(server_url_fire, "POST", "check");
    return icf_response;
}

String check_fire(const SensorData& data, const int& neck_pos, const int& base_pos) {
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
    String fire_response = send_request(server_url_fire, "POST", payload);
    return fire_response;
}

Config get_config() {
    String config = send_request(server_url_config, "GET", "");
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
    
    send_request(web_url, "POST", payload);
}