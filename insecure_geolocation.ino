#include <ESP8266WiFi.h>

#include  <ArduinoJson.h>

#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
//Github pag of the library https://github.com/gmag11/WifiLocation
#include <WifiLocation.h>

char myssid[] = "BelongCB47AB";         // network SSID name
char mypass[] = "6hrfng3yf7db";

const char* googleAPI = "http://www.googleapis.com";
String endPoint = "/geolocation/v1/geolocate?key=";
String key = "****************"
WifiLocation location(key);


String networks = "{\n";

void setup() {

  Serial.begin(115200);

  // Code is derived form this resource was used to connect to WiFi https://circuitdigest.com/microcontroller-projects/how-to-track-location-with-nodemcu-using-google-map-api
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
  Serial.println("Connected To");
  Serial.println(myssid);

}

void loop() {
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;

  // scaning for nearby APs
  // Return details of the networks found
  int n = WiFi.scanNetworks();
  if (n == 0 ) {
  } else {
    Serial.print(n);
    Serial.println(" networks found");
  }

  //Code was derived from this resource to build the Json object for the post body: https://circuitdigest.com/microcontroller-projects/how-to-track-location-with-nodemcu-using-google-map-api
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

  //Set up an insecure client
  WiFiClient client;

  /**
      In this section we are communicating HTTP requests
      To ip-api which is a free API using IP address to get geolocation
      https://ip-api.com/
  **/

  HTTPClient http;
  http.begin(client, "http://ip-api.com/json/" );

  Serial.println("Connected to ip-api");

  // Sending Get request to get Json responce
  int httpCode = http.GET();

  String payload = http.getString();

  Serial.println(httpCode);

  Serial.println("this is the payload from ip-api");
  Serial.println(payload);
  client.stop();

  /**
      In this section we are trying to communicate with Google API over HTTP
      Get 400 "bad request" error
      Code for writing the POST request is derived from https://circuitdigest.com/microcontroller-projects/how-to-track-location-with-nodemcu-using-google-map-api
  **/

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

  /**
      In this section we use WiFiLocation library to send HTTPS requests to google API
      WiFiLocation does the work for setting the fingerprint and making a secure client
      For some reason get "0" for latitude and longitude ... not sure why?
      boiler plate code is from the GitHub page of the library https://github.com/gmag11/WifiLocation

  **/

  location_t loc = location.getGeoFromWiFi();
  Serial.println("Location request data");
  Serial.println(location.getSurroundingWiFiJson());
  Serial.println("Latitude: " + String(loc.lat, 7));
  Serial.println("Longitude: " + String(loc.lon, 7));
  Serial.println("Accuracy: " + String(loc.accuracy));

  delay(5000);
}
