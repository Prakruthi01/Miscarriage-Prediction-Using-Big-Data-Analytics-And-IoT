#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<LiquidCrystal.h>
int rs=13,en=12,d4=25,d5=26,d6=27,d7=14;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

#include <WiFi.h>
#include "ThingSpeak.h"

const char* ssid = "Good Day";   // your network SSID (name) 
const char* password = "19201920";   // your network password

WiFiClient  client;

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 33 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature;

unsigned long myChannelNumber = 2476924;
const char * myWriteAPIKey = "U3KRBZ7GWK9HV7PP";
void Connect_wifi();

char mystr[20];
char tempStr[8];
char ch;

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

//unsigned long lastTime = 0;
//unsigned long timerDelay = 30000;

float BPM, SpO2,Pulse;

PulseOximeter pox;
uint32_t tsLastReport = 0;
int smokeval;


void Init_spo2();

void onBeatDetected()
{
  Serial.println("Beat Detected!");
}

void setup() 
{

  Serial.begin(9600); 
  lcd.begin(16,2);//Initialize serial
  lcd.clear();
  
   
  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  Connect_wifi();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  MISSCARRIAGE ");
  lcd.setCursor(0,1);
  lcd.print("  PREDICTION ");
  
    Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {  
    Serial.println("SUCCESS");

    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
   Init_spo2();
}


void Connect_wifi()
{
//  if ((millis() - lastTime) > timerDelay) {
//    
//    // Connect or reconnect to WiFi
//    if(WiFi.status() != WL_CONNECTED){
//      Serial.print("Attempting to connect");
//      while(WiFi.status() != WL_CONNECTED){
//        WiFi.begin(ssid, password); 
//        delay(5000);     
//      } 
//      Serial.println("\nConnected.");
////      Init_spo2();
//    }
//  }


  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
}

void Init_spo2()
{
   Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");

    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);



}



void loop() 
{


  // Print The Readings
  
   SPO2();
   TEMPERATURE(); 
   SMOKE_SENSOR();
   
  

    // set the fields with the values
//    ThingSpeak.setField(1, tempStr);
    ThingSpeak.setField(1,Pulse);
    ThingSpeak.setField(2, tempStr);
   
    ThingSpeak.setField(3, smokeval);
//    ThingSpeak.setField(3, bpstr);
//     ThingSpeak.setField(4, count);
    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
      Connect_wifi();
//      Init_spo2();
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    
//  }
delay(3000);
} 

void SPO2()
{
  Init_spo2();
  int i=0;
    while(i<10000)
    {
        pox.update();
        Pulse = pox.getHeartRate();
        SpO2 = pox.getSpO2();
      
        if (millis() - tsLastReport > REPORTING_PERIOD_MS)
        {
      
          Serial.print("Pulse: ");
          Serial.println(Pulse);
      
//          Serial.print("SpO2: ");
//          Serial.print(SpO2);
//          Serial.println("%");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Pulse:");
          lcd.setCursor(7,0);
          lcd.print(Pulse); 
//          lcd.setCursor(0,1);
//          lcd.print("Spo2:");
//          lcd.setCursor(6,1);
//          lcd.print(SpO2);   // print the temperature in °C
//          lcd.print("%");
      
          Serial.println("*");
          Serial.println();
      
          tsLastReport = millis();
        }
//        delay(500);
        i++;
    }
}


void TEMPERATURE()
{
  sensors.requestTemperatures(); 
      temperature = sensors.getTempCByIndex(0);
      Serial.print("Temperature: ");
      Serial.println(temperature);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TEMP:");
      lcd.setCursor(0,1);
      lcd.print(temperature);
      delay(1000);
  
  
  delay(1000);
  dtostrf(temperature, 5, 3, tempStr);

  
}

void SMOKE_SENSOR()
{
  smokeval=analogRead(39);
  Serial.print("SMOKE:");
  Serial.println(smokeval);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SMOKE:");
  lcd.setCursor(0,1);
  lcd.print(smokeval);
  delay(1000);
  
}

