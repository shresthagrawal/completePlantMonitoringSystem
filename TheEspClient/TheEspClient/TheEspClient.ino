//////////////////////////////////////////////////////////////////////////////
// Complete Plant Health and Environment Monitoring System NodeMCU Client ///
////////////////////////////////////////////////////////////////////////////

/// GOOGLE CODE-IN///////////////////////////////////////////////




#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

/* Sample Input Packet POST from Arduino
{
  "SSID": "KabelBox-2D74",
  "Password":"53704221317725974186",
  "RequestMode":"POST",
  "Uri":"http://192.168.0.5:9200/twitter/_doc/",
  "RequestBody":"" 
}
*/

String rawData;
HTTPClient http;
DynamicJsonDocument doc;

void setup() {
  // Setup Serial Communication
  Serial.begin(115200);

}


  
 
void loop() {

  while (!Serial.available());
  rawData= Serial.readString();// read the incoming Json String
  deserializeJson(doc, rawData);
  JsonObject obj = doc.as<JsonObject>();

  //Check if Wifi is not connected
  if(WiFi.status() != WL_CONNECTED){
    //Connect to Wifi using the wifi Credentials recieved in first Json Packet
    //Convert the string into Char * for SSID and Password
    char SSID[sizeof(obj["SSID"].as<String>())+10]; 
    (obj["SSID"].as<String>()).toCharArray(SSID,sizeof(obj["SSID"].as<String>())+10);
    char password[sizeof(obj["Password"].as<String>())+10]; 
    (obj["Password"].as<String>()).toCharArray(password,sizeof(obj["Password"].as<String>())+10);
    WiFi.begin(SSID,password);   //WiFi connection
    while (WiFi.status() != WL_CONNECTED) { 
      //Serial.println(SSID); 
      //Wait for the WiFI connection completion
      delay(500);
    }
  }
  
  // if the method specified is post 
  if((obj["RequestMode"].as<String>()).equals("POST")){
    // pass the URI from the Json request
    http.begin(obj["Uri"].as<String>());
    http.addHeader("Content-Type", "application/json");
    //Serial.println(obj["RequestBody"].as<String>());
    // pass the post request body from the Json request
    int httpCode = http.POST(obj["RequestBody"].as<String>());
    Serial.println(http.getString());

  }
  else if ((obj["RequestMode"].as<String>()).equals("GET"))
  {
    // pass the URI from the Json request
    http.begin(obj["Uri"].as<String>());
    http.addHeader("Content-Type", "application/json");
    //Serial.println(obj["RequestBody"].as<String>());
    // pass the post request body from the Json request
    int httpCode = http.GET();
    Serial.println(http.getString());
  }

  
  
  

}
