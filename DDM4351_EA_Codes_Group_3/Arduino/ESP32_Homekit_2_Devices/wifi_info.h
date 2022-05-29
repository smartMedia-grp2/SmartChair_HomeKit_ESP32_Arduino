/*
 * wifi_info.h
 *
 *  Revised on: 2022-05-16
 *      Author: Cyrus Lau
 */

// mainProg 5
  #ifndef WIFI_INFO_H_
  #define WIFI_INFO_H_
  
  #if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #elif defined(ESP32)
  #include <WiFi.h>
  #endif
  
  const char *ssid = "Cyrus";
  const char *password = "14june2021";
  
  float temperature_value = 0; // TEMPERATURE_SENSOR 2
  float humidity_value = 0; // HUMIDITY_SENSOR 2
  bool motion_value = false; // MOTION_SENSOR 2
  bool obstacle_value = false; // OBSTACLE_SENSOR 2
// mainProg 5 end






// insertIntoMySQL
  #include <HTTPClient.h>
  #include <Wire.h>
  // REPLACE with your Domain name and URL path or IP address with path
  const char* serverName = "http://192.168.60.190/ddm4351ea/post-esp-data.php";
  // Keep this API Key value to be compatible with the PHP code provided in the project page.
  // If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
  String apiKeyValue = "tPmAT5Ab3j7F9";
// insertIntoMySQL end






// plotChartWebServer
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
  // Create AsyncWebServer object on port 80
  AsyncWebServer server(80);
  
  String read_tempareture() {
    if (temperature_value == NULL) {
      Serial.println("Failed to read from temperature sensor!");
      return "";
    } else {
      Serial.println("Read tempareture: ");
      Serial.print("\t");
      Serial.print(temperature_value);
      Serial.print("\n");
      return String(temperature_value);
    }
  }
  String read_humidity() {
    if (humidity_value == NULL) {
      Serial.println("Failed to read from humidity sensor!");
      return "";
    } else {
      Serial.println("Read humidity: ");
      Serial.print("\t");
      Serial.print(humidity_value);
      Serial.print("\n");
      return String(humidity_value);
    }
  }
  String read_motion() {
    if (motion_value == NULL) {
      Serial.println("Failed to read from motion sensor!");
      return "";
    } else {
      Serial.println("Read motion: ");
      Serial.print("\t");
      Serial.print(motion_value);
      Serial.print("\n");
      return String(motion_value);
    }
  }
  String read_obstacle() {
    if (obstacle_value == NULL) {
      Serial.println("Failed to read from obstacle sensor!");
      return "";
    } else {
      Serial.println("Read obstacle: ");
      Serial.print("\t");
      Serial.print(obstacle_value);
      Serial.print("\n");
      return String(obstacle_value);
    }
  }
// plotChartWebServer end




// mainProg 6
void wifi_connect() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  Serial.println("WiFi connecting...");
  while (!WiFi.isConnected()) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\n");
  //Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
  Serial.print("[WIFI] Connected, IP: ");
  //Serial.println(WiFi.localIP());
  Serial.printf("%s\n", WiFi.localIP().toString().c_str());
}
// mainProg 6 end




// plotChartWebServer
  void updateRealtimeWebChart() {
      // Initialize SPIFFS
      if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
      }
      
      // Print ESP32 Local IP Address
      Serial.println(WiFi.localIP());
      
      // Route for root / web page
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/ddm4351ea/index.html");
      });
      server.serveStatic("/", SPIFFS, "/");
      server.on("/T", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", read_tempareture().c_str());
      });
      server.on("/H", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", read_humidity().c_str());
      });
      server.on("/M", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", read_motion().c_str());
      });
      server.on("/O", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", read_obstacle().c_str());
      });
      
      // Start server
      server.begin();
  }
// plotChartWebServer end






//insertIntoMySQL
  void insertIntoMySQL() {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
  
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
  
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(temperature_value) + 
                                "&value2=" + String(humidity_value) + "&value3=" + String(motion_value) + 
                               "&value4=" + String(obstacle_value);
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);
  
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
  
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
  
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      
      // Free resources
      http.end();
      
    } else {
      Serial.println("WiFi Disconnected");
    }
  }
//insertIntoMySQL end

#endif /* WIFI_INFO_H_ */ // mainProg 7
