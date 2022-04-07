#include <ESP8266WiFi.h>

#include  <ArduinoJson.h>

#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include <WifiLocation.h>

char myssid[] = "BelongCB47AB";         // network SSID name
char mypass[] = "6hrfng3yf7db";

const char* googleAPI = "http://www.googleapis.com";
String endPoint = "/geolocation/v1/geolocate?key=";
String key = "AIzaSyDgTp1t3-xK9rv9VtGgpvGnstbz-58YfAM";

WifiLocation location(key);


String networks = "{\n";

void setup(){
  
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
  
    // Connect to WiFi network
    Serial.print("Trying to connect to ");
    Serial.println(myssid);
    
    WiFi.begin(myssid, mypass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
}

void loop() {
    char bssid[6];
    DynamicJsonBuffer jsonBuffer;

    // Return networks found
    int n = WiFi.scanNetworks();
    if (n == 0 ) {
    } else {
        Serial.print(n);
        Serial.println(" networks found");
    }

      for (int j = 0; j < n; ++j) {
          networks += "{\n";
          networks += "\"macAddress\" : \"";
          networks += (WiFi.BSSIDstr(j));
          networks += "\",\n";
          networks += "\"signalStrength\": ";
          networks += WiFi.RSSI(j);
          networks += "\n";
          if (j < n - 1) {
              networks += "},\n";
          } else {
              networks += "}\n";
          }
      }
      networks += ("]\n");
      networks += ("}\n");
      Serial.println(networks);

      
    // sending a request to insecure API  
      WiFiClient client;
    // client.setInsecure();         //use this instead of setting the footprint

      HTTPClient http;
      http.begin(client,"http://ip-api.com/json/" );
      Serial.println("connected to ip-api");
      int httpCode = http.GET();
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
      client.stop();

  // sending a request to google geolocation API    
  if (client.connect(googleAPI, 80)) {
    Serial.println("Sending a request to google geolocation API");
    client.println("POST " + endPoint + key + " HTTP/1.1");
    client.println("Host: " + (String)googleAPI);
    client.println("Connection: keep-alive");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(networks.length());
    client.println();
    client.print(networks);
    delay(1000);
   }
   
    while (client.available()) {
      String payload = client.readStringUntil('\r');
      JsonObject& root = jsonBuffer.parseObject(payload);
      Serial.println(payload);
    }

    location_t loc = location.getGeoFromWiFi();
    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));


      delay(5000);
}
