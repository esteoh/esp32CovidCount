#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SSD1306Wire.h>

// const char* url = "https://services1.arcgis.com/0MSEUqKaxRlEPj5g/arcgis/rest/services/Coronavirus_2019_nCoV_Cases/FeatureServer/1/query?where=Country_Region%20like%20'%25MALAYSIA%25'&outFields=Last_Update,Confirmed,Deaths,Recovered&returnGeometry=false&outSR=4326&f=json";
// change recovered to active case
const char* url = "https://services1.arcgis.com/0MSEUqKaxRlEPj5g/arcgis/rest/services/Coronavirus_2019_nCoV_Cases/FeatureServer/1/query?where=Country_Region%20like%20'%25MALAYSIA%25'&outFields=Last_Update,Confirmed,Deaths,Active&returnGeometry=false&outSR=4326&f=json";

const char* ssid = "xxx";
const char* password = "xxxyyyzzz";

// const int redLED = 10;
SSD1306Wire display(0x3c, 5, 4); // instance for the OLED. Addr, SDA, SCL


void setup() {

  // pinMode(redLED, OUTPUT);
  Serial.begin(115200);
  delay(2000);

  display.init(); // initialise the OLED
  display.flipScreenVertically(); // does what is says
  display.setFont(ArialMT_Plain_16); // does what is says
  // Set the origin of text to top left
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.clear(); // clear the display
    // prep a string in the screen buffer
    display.drawString(0, 10, "WiFi connecting...");
    display.display(); // display whatever is in the buffer
    // Serial.println("WiFi connecting...");                  // for Debug
  }

  display.clear(); // clear the display
  // prep a string in the screen buffer
  display.drawString(0, 10, "WiFi connected");
  display.display(); // display whatever is in the buffer
  // Serial.println("WiFi connected");                           // for Debug
}

void loop() {

  HTTPClient http;
  String data;
  int _strLen;

  http.begin(url);
  
  int httpCode = http.GET();

  if (httpCode > 0) { //Check for the returning code

    String payload = http.getString();
    char charBuf[500];
    payload.toCharArray(charBuf, 500);
    
    // Serial.println(payload);                         // for Debug

    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 3 * JSON_OBJECT_SIZE(6) + 2 * JSON_OBJECT_SIZE(7) + 690;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, payload);

    JsonArray fields = doc["fields"];

    JsonObject features_0_attributes = doc["features"][0]["attributes"];

    long features_0_attributes_Last_Update = features_0_attributes["Last_Update"];
    String features_0_attributes_Confirmed = features_0_attributes["Confirmed"];
    String features_0_attributes_Deaths = features_0_attributes["Deaths"];
    // String features_0_attributes_Recovered = features_0_attributes["Recovered"];
    String features_0_attributes_Active = features_0_attributes["Active"];

    // === for Debug ===
    // Serial.println(features_0_attributes_Confirmed);
    // Serial.println(features_0_attributes_Deaths);
    // Serial.println(features_0_attributes_Recovered);

    _strLen = display.getStringWidth("Active: ");
    display.clear(); // clear the display
    display.drawString(0, 0, "== MALAYSIA ==");
    display.drawString(0, 16, "Total: ");
    display.drawString(_strLen, 16, features_0_attributes_Confirmed);
    display.drawString(0, 32, "Active: ");
    display.drawString(_strLen, 32, features_0_attributes_Active);
    display.drawString(0, 48, "Death: ");
    display.drawString(_strLen, 48, features_0_attributes_Deaths);
    display.display(); // display whatever is in the buffer
    
  } else {

    display.clear(); // clear the display
    display.drawString(0, 20, "HTTPreq error");
    
  }

  http.end(); //Free the resources

  delay(300000);
}