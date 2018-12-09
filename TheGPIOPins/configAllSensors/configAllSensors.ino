/////////////////////////////////////////////////////////////////////
// Complete Plant Health and Environment Monitoring System    //////
///////////////////////////////////////////////////////////////////

/// GOOGLE CODE-IN///////////////////////////////////////////////


/* Wiring For Node MCU 
MEGA/DUE Pin 18(TX1) -> NODE MCU RX
GND -> GND
MEGA/DUE Pin 19(RX1) -> NODE MCU TX
*/


#include <SimpleDHT.h>
#include <ArduinoJson.h>


// Declaring the Different Sensor pins
int DHT11Pin =7;
int pirSensor = 11; // PIR sensor
int hydrogenSensor = A5; // Hydrogen Sensor
int alcoholSensor = A4; // Alcohol Sensor
int ldrSensor= A3; 
int defaultLedPin = 13;
int redLedPin = 12;
int buzzerPin = 6;
int motorPin = 10;
int pumpPin = 9;

// Other Variables
int alcoholGasThres = 400;
int hydrogenGasThres = 400;
int delayThreshold = 5000;
int humidityThres = 40;
int temperatureThres = 40;

String requestTemplate = "{ \"SSID\": \"\", \"Password\":\"\", \"RequestMode\":\"POST\", \"Uri\":\"\", \"RequestBody\":\"\"}";
String requestBodyTemplate = "{\"NodeId\":\"\",\"TimeStamp\":\"\",\"MotionStatus\":0, \"LDR\":0, \"AlcoholGas\": 0, \"Temperature\": 0, \"Humidity\": 0 }";
String SSID ="KabelBox-2D74";
String Password="53704221317725974186";
String nodeId="node1"; // this should be unique for every device
String URI="http://192.168.0.5:9200/sensor-logs/_doc";
String timeApi = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";

void setup(){
  Serial.begin(115200);
  Serial1.begin(115200);
  
  // Attaching the sensors to the corresponding input pins
  pinMode(pirSensor, INPUT); //Declare PIR sensor as the input
  pinMode(ldrSensor, INPUT);
  pinMode(hydrogenSensor, INPUT); // Declare Hydrogen Sensor for input
  pinMode(alcoholSensor, INPUT); // Declare Alcohol Sensor for input
  pinMode(DHT11Pin, INPUT);// Declare DHT 11 for Input
  pinMode(defaultLedPin, OUTPUT); // Declare Default LedPin
  pinMode(redLedPin, OUTPUT); // Declare Red LedPin
  pinMode(motorPin, OUTPUT); //Motor Pin
  pinMode(pumpPin, OUTPUT); //Motor Pin
  pinMode(buzzerPin, OUTPUT); //Motor Pin
}


SimpleDHT11 dht1(DHT11Pin);

void loop(){
  //get the different sensors values
  byte temperature = 0;
  byte humidity = 0;
  dht1.read(&temperature, &humidity, NULL);
  float motionStatus = digitalRead(pirSensor); 
  float temperatureVal = temperature;
  float humidityVal = humidity;
  float ldrVal = analogRead(ldrSensor);
  float hydrogenVal = analogRead(hydrogenSensor); //hydrogen values
  float alcoholVal = analogRead(alcoholSensor); //alcohol values

  //get current Time Stamp
  Serial.println("GettingTimestamp...");
  //Send request to the NodeMCU using serial commmand
  Serial1.println(getDateRequest());
  // wait until the reply 
  while (!Serial1.available());
  // parse the Json result from the API 
  DynamicJsonDocument doc;
  deserializeJson(doc, Serial1.readString());
  JsonObject obj = doc.as<JsonObject>();
  String timeStamp= (obj["datetime"].as<String>());
  //get the TimeStamp*/
  Serial.println(timeStamp);
  //exit if time stamp was not found
  if(timeStamp.equals("null")) return;
  //get The Json String for sending request
  generateFakeData(motionStatus,ldrVal, hydrogenVal, alcoholVal, temperatureVal, humidityVal);
  String jsonRequest = getRequest("POST",getRequestBody(motionStatus, ldrVal, hydrogenVal, alcoholVal, temperatureVal, humidityVal, timeStamp.substring(0,19)));
  Serial.println("Sending Request:"+ jsonRequest);
  //Send request to the NodeMCU using serial commmand
  Serial1.println(jsonRequest);
  // wait until the reply 
  while (!Serial1.available());
  // print the reply 
  Serial.println(Serial1.readString());
  checkAndDisplay(motionStatus, hydrogenVal, alcoholVal, temperatureVal, humidityVal);

}

void generateFakeData(float &motionStatus, float &ldrVal, float &hydrogenVal, float &alcoholVal, float &temperature, float &humidity)
{
  motionStatus = int(random(0,2));
  ldrVal = int(random(0,1024));
  hydrogenVal = int(random(200,300));
  alcoholVal =int(random(200,300));
  temperature = int(random(0,15)); // temp in berlin aprox 
  humidity = int(random(50,80));
  
}

String getDateRequest()
{
  //Create Json to send to Node MCU for Post Request
  DynamicJsonDocument doc;
  deserializeJson(doc, requestTemplate);
  JsonObject obj = doc.as<JsonObject>();
  obj["SSID"]=SSID;
  obj["Password"]=Password;
  obj["Uri"]=timeApi;
  obj["RequestMode"]="GET";
  String output;
  serializeJson(doc, output);
  return output;  
}

String getRequestBody(float motionStatus, float ldrVal, float hydrogenVal, float alcoholVal, float temperature, float humidity,String timeStamp){
  //Create Json to send to Node MCU for Post Request
  DynamicJsonDocument doc;
  deserializeJson(doc, requestBodyTemplate);
  JsonObject obj = doc.as<JsonObject>();
  obj["NodeId"]=nodeId;
  obj["MotionStatus"]=motionStatus;
  obj["LDR"]=ldrVal;
  obj["HydrogenGas"]=hydrogenVal;
  obj["AlcoholGas"]=alcoholVal;
  obj["Temperature"]=temperature;
  obj["TimeStamp"]=timeStamp;
  obj["Humidity"]=humidity;
  String output;
  serializeJson(doc, output);
  return output;
  
  }

String getRequest(String requestMode,String requestBody){
  //Create Json to send to Node MCU for Post Request
  DynamicJsonDocument doc;
  deserializeJson(doc, requestTemplate);
  JsonObject obj = doc.as<JsonObject>();
  obj["SSID"]=SSID;
  obj["Password"]=Password;
  obj["Uri"]=URI;
  obj["RequestMode"]=requestMode;
  obj["RequestBody"]=requestBody;
  
  
  String output;
  serializeJson(doc, output);
  return output;
}

void printAll(float motionStatus, float hydrogenVal, float alcoholVal, float temperature, float humidity, float ldrVal){
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Motion Sensor Status: ");
  Serial.println(motionStatus);
  Serial.print("Hygrogen Gas Concentration: ");
  Serial.println(hydrogenVal);
  Serial.print("Alcohol Gas Concentration: ");
  Serial.println(alcoholVal);
  Serial.print("LDR Val: ");
  Serial.println(ldrVal);
  delay(delayThreshold);
} 

void checkAndDisplay(float motionStatus, float hydrogenVal, float alcoholVal, float temperatureVal, float humidityVal){
  digitalWrite(defaultLedPin , HIGH);

  if (hydrogenVal>=hydrogenGasThres || alcoholVal>=alcoholGasThres ){
    
  	//when their is a abnormalgas concentration in air turn Red Led Onn to Indicate it.
    digitalWrite(defaultLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    
    Serial.println("Flammable gases are present above threshold!");
    Serial.print("Hydrogen Gas Concentration: ");
    Serial.println(hydrogenVal);
    Serial.print("Alcohol Gas Concentration: ");
    Serial.println(alcoholVal);
    }
  if (motionStatus == HIGH){

  	// if motion is detected from any outsider animal play music to scare them .
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Motion is Detected!!!");    
    }

  if (humidityVal<=humidityThres){

  	// if the humididty around plant is less turn on the pump
    Serial.println("I am Thirsty, I need Water");
    digitalWrite(pumpPin, HIGH);
    }

  if (temperatureVal>=temperatureThres){

  	// if the temp around plant is high turn on the fan to cool down the Temp
    Serial.println("Its very hot outside");
    digitalWrite(motorPin, HIGH);
    } 

  
}
