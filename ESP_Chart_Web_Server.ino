#include "DHT.h"
#include "RTClib.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#define DHTPIN 2
#define DHTTYPE DHT11

const char* ssid = "DIR-612-F569";
const char* password = "BennettdanLala";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);




RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DHT dht(DHTPIN, DHTTYPE);



String readDHT11Temperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHT11Humidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return "";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

int Relay = 18;
const int OnHour = 10; //SET TIME TO ON RELAY (24 HOUR FORMAT)
const int OnMin = 00;
const int OffHour = 11; //SET TIME TO OFF RELAY
const int OffMin = 00;

int Relay2 = 19;
const int OnOverdrive = 75; //SET HUMIDITY TO ON RELAY 
const int OffOverdrive = 90; //SET HUMIDITY TO OFF RELAY

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
 
dht.begin();
   pinMode(Relay2, OUTPUT);
  digitalWrite(Relay2, HIGH);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  rtc.begin();
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, HIGH);

   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHT11Temperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHT11Humidity().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){

  DateTime now = rtc.now();
  Serial.print("ESP32 RTC Date Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);


  if(now.hour() == OnHour && now.minute() == OnMin){
    digitalWrite(Relay,LOW);
    Serial.println("LIGHT ON");
      }
    
    else if(now.hour() == OffHour && now.minute() == OffMin){
      digitalWrite(Relay,HIGH);
      Serial.println("LIGHT OFF");
    }

float temp = dht.readTemperature();
float hum = dht.readHumidity();

    if(hum <= OnOverdrive){
    digitalWrite(Relay2,LOW);
    Serial.println("MIST MAKER ON");
    }
    
    else if(hum >= OffOverdrive){
      digitalWrite(Relay2,HIGH);
      Serial.println("MIST MAKER OFF");
    }
 

}
