#include "AIS_NB_BC95.h"
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "SoftwareSerial.h"
#include <Arduino.h>
static const int RXPin = 10, TXPin = 11;//D1 -> Tx, D2 -> RX
SoftwareSerial GPSModule(RXPin, TXPin); // TX, RX
//AltSoftSerial mega;
String filename = "yourfilename";

String serverIP = ""; // Your Server IP
String serverPort = ""; // Your Server Port
String jsonData ="";
float t,h;


AIS_NB_BC95 AISnb;
LiquidCrystal_I2C lcd(0x27,16,4); 
const long interval = 15000;  //millisecond
unsigned long previousMillis = 0;
#include "DHT.h"
#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int cnt =0;
int dispCounter =0;
float tDHT;
float hDHT;
char response[50];
float latitude = 0.000000;
float longitude = 0.000000;
String tDHTStr = "";String hDHTStr = "";
String output;
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};

void setup()
{ 
  Serial.begin(9600);lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("NB IOT Project..");
  lcd.setCursor(0,1);
  lcd.print("+ udp Server    ");
  delay(1000);
  
  lcdBlink(3,500);
  AISnb.debug = true;
  AISnb.setupDevice(serverPort);
  String ip1 = AISnb.getDeviceIP();  
  pingRESP pingR = AISnb.pingIP(serverIP);
  previousMillis = millis();
  GPSModule.begin(9600);
  delay(1000);
}
void lcdBlink(int a,int b){
    for (int f = 0; f <a;f++){
    lcd.noBacklight();
    delay(b);
    lcd.backlight();
    delay(b);
  }
}
void readDHT11(){
      // get data from DHT22 sensor
      float htempo = dht.readHumidity();
      float ttempo = dht.readTemperature();
      delay(50);
        if (isnan(htempo)){
          Serial.println("h = NAN");
        }
        else {
          hDHT = htempo;
          //Serial.print(" Humi= ");Serial.println(hDHT);
        }
        if (isnan(ttempo)){
          Serial.println("t = NAN");
        }
        else {
          tDHT = ttempo;
          //Serial.print("Temp= ");Serial.println(tDHT);
        }
}
void loop()
{ 
  // Add values in the document
  readDHT11(); 
  output = "";
    
  StaticJsonDocument<500> doc;
      //doc["lat"] = nmea[2];//value to sterilize
      //doc["long"] = nmea[4];
      doc["date"] = nmea[8];
      doc["speed"] = nmea[6];
      doc["ambT"] = String(t);
      //doc["ambH"] = String(h);
      JsonArray data = doc.createNestedArray("data");
        data.add(nmea[2]);
        data.add(nmea[4]);
      doc["cnt"] = String(cnt);
     
  serializeJson(doc, output);
  Serial.print("Output= ");
  Serial.println(output);
  //serializeJsonPretty(doc,Serial);
  //Serial.println();
  Serial.print("jsonMeasure= ");
  Serial.println(measureJsonPretty(doc));
  unsigned long currentMillis = millis();
  //Serial.print("...Time to read sht10= ");
  long timeLeft = (interval - (currentMillis - previousMillis))/1000;
  char timebuff[3] = ""; ///< This is the buffer for the string the sprintf outputs to
  sprintf(timebuff, "%03d", timeLeft); 
  //Serial.println(timeLeft); //seconds to Post
  lcd.setCursor(13,1);
  lcd.println(timebuff);
  if (currentMillis - previousMillis >= interval)
    {    
      cnt ++;
      if (cnt>=100) cnt =0; 
      // Send data in String 
      UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, jsonData);
      //Send data in HexString     
      //udpDataHEX = AISnb.str2HexStr(udpData);
      //UDPSend udp = AISnb.sendUDPmsg(serverIP, serverPort, udpDataHEX);
      previousMillis = currentMillis;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sent data to Serv");
      lcd.setCursor(0,1);
      lcd.print("-----------------");
      //delay(1000);
    }
  UDPReceive resp = AISnb.waitResponse();
  gpsRead();
  jsonFormat();
  lcdDisplay();
  delay(1000);
}
void jsonFormat(){
  jsonData = '{';
/*
  jsonData += '"';
  jsonData += "latitude";
  jsonData += '"';
  jsonData += ':';
  jsonData += '"';
  jsonData += nmea[2];
  jsonData += '"';
  jsonData += ',';

  jsonData += '"';
  jsonData += "longitude";
  jsonData += '"';
  jsonData += ':';
  jsonData += '"';
  jsonData += nmea[4];
  jsonData += '"';
  jsonData += ',';
*/
  jsonData += '"';
  jsonData += "speed";
  jsonData += '"';
  jsonData += ':';
  jsonData += '"';
  jsonData += String(nmea[6]);
  jsonData += '"';
  jsonData += ',';

  jsonData += '"';
  jsonData += "ddate";
  jsonData += '"';
  jsonData += ':';
  jsonData += '"';
  jsonData += String(nmea[8]);
  jsonData += '"';
  jsonData += ',';

//nested array

  jsonData += '"';
  jsonData += "position";
  jsonData += '"';
  jsonData += ":[";
  jsonData += '"';
  jsonData += String(nmea[2]);
  jsonData += '"';
  jsonData += ',';
  jsonData += '"';
  jsonData += String(nmea[4]);
  jsonData += '"';
  jsonData += "],";
 
//  jsonData += ',';
  jsonData += '"';
  jsonData += "temp";
  jsonData += '"';
  jsonData += ':';
  jsonData += '"';
  jsonData += String(t);
  jsonData += '"';
  jsonData += ',';
/*  
  jsonData += '"';
  jsonData += "humi";
  jsonData += '"';  
  jsonData += ':';
  jsonData += '"';  
  jsonData += String(h);
  jsonData += '"';
  jsonData += ',';
*/  
  jsonData += '"';
  jsonData += "cnt";
  jsonData += '"';  
  jsonData += ':';
  jsonData += '"';  
  jsonData += String(cnt);
  jsonData += '"';
    
  jsonData += "}";
  
  Serial.print("jsonData= ");
  Serial.println(jsonData);
  Serial.print(".length= ");
  Serial.println(jsonData.length());

  if (cnt>=100) cnt =0;
}
String ConvertLat() {
    String posneg = "";
    if (nmea[3] == "S") {
        posneg = "-";
    }
    String latfirst;
    float latsecond;
    for (int i = 0; i < nmea[2].length(); i++) {
        if (nmea[2].substring(i, i + 1) == ".") {
          latfirst = nmea[2].substring(0, i - 2);
          latsecond = nmea[2].substring(i - 2).toFloat();
        }
    }
    latsecond = latsecond / 60;
    String CalcLat = "";
    
    char charVal[9];
    dtostrf(latsecond, 4, 6, charVal);
    for (int i = 0; i < sizeof(charVal); i++)
    {
      CalcLat += charVal[i];
    }
    latfirst += CalcLat.substring(1);
    latfirst = posneg += latfirst;
    return latfirst;
}

String ConvertLng() {
    String posneg = "";
    if (nmea[5] == "W") {
        posneg = "-";
    }
    String lngfirst;
    float lngsecond;
    for (int i = 0; i < nmea[4].length(); i++) {
        if (nmea[4].substring(i, i + 1) == ".") {
            lngfirst = nmea[4].substring(0, i - 2);
            //Serial.println(lngfirst);
            lngsecond = nmea[4].substring(i - 2).toFloat();
            //Serial.println(lngsecond);
        }
    }
    lngsecond = lngsecond / 60;
    String CalcLng = "";
    char charVal[9];
    dtostrf(lngsecond, 4, 6, charVal);
    for (int i = 0; i < sizeof(charVal); i++)
    {
        CalcLng += charVal[i];
    }
    lngfirst += CalcLng.substring(1);
    lngfirst = posneg += lngfirst;
    return lngfirst;
}
void gpsRead(){
    GPSModule.flush();
    while (GPSModule.available() > 0)
    {
      GPSModule.read();
    }
    if (GPSModule.find("$GPRMC,")) {
        String tempMsg = GPSModule.readStringUntil('\n');
        for (int i = 0; i < tempMsg.length(); i++) {
            if (tempMsg.substring(i, i + 1) == ",") {
              nmea[pos] = tempMsg.substring(stringplace, i);
              stringplace = i + 1;
              pos++;
            }
            if (i == tempMsg.length() - 1) {
              nmea[pos] = tempMsg.substring(stringplace, i);
            }
        }
        updates++;
        nmea[2] = ConvertLat();
        nmea[4] = ConvertLng();
        /*
        for (int i = 0; i < 9; i++) {
            Serial.print(labels[i]);
            Serial.print(nmea[i]);
            Serial.println("");
        }*/
    }
    else {
    failedUpdates++;
    }
    stringplace = 0;
    pos = 0;
    //GPSModule.flush();
}
void lcdDisplay(){
  switch (dispCounter){
    case 0:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Date:");
        lcd.print(nmea[8]);
        lcd.setCursor(0,1);
        lcd.print("Speed:");
        lcd.print(nmea[6]);
        dispCounter ++;
        break;  
    case 1:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Lat:");
        lcd.print(nmea[2]);
        lcd.setCursor(0,1);
        lcd.print("Lon:");
        lcd.print(nmea[4]);
        dispCounter ++;
        break;   
    case 2:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("ambT= ");
          lcd.setCursor(6,0);
          lcd.print(tDHT);
          lcd.setCursor(0,1);
          lcd.print("ambH= ");
          lcd.setCursor(6,1);
          lcd.print(hDHT);
        dispCounter =0;
        break;
      default:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Error !!!");
          dispCounter =0;
          break;
  }
}
