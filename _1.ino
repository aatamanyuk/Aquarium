#include <DHT.h>

/*
  Arduino Starter Kit example
 Project 3  - Love-O-Meter
 
 This sketch is written to accompany Project 3 in the
 Arduino Starter Kit
 
 Parts required:
 1 TMP36 temperature sensor 
 3 red LEDs
 3 220 ohm resistors
 
 Created 13 September 2012
 by Scott Fitzgerald
 
 http://arduino.cc/starterKit
 
 This example code is part of the public domain 
 */
 
#include <SPI.h>
#include <WiFi.h>
#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "JayaDigital";     //  your network SSID (name) 
char pass[] = "0123456789";    // your network password

int status = WL_IDLE_STATUS;

boolean door_status = false;

WiFiServer server(80);



// named constant for the pin the sensor is connected to
const int sensorPin = A0;  //Внешний ДТ
const int sensorWPin = A1;  // Температура воды
const int ledPin = A2; //Датчик света

void setup(){
  // open a serial connection to display values
  Serial.begin(9600);
   dht.begin();
   // Attach interrupt to pin 2
  attachInterrupt(0, setDoorStatus, RISING);

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  } 
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
  
}

void loop(){
  // переменные датчика темп и влажности
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  /*int sensorVal = analogRead(sensorPin);
  float voltage = (sensorVal/1024.0) * 5.0;
  //Serial.print("degrees C: "); 
  float temperature = (voltage - .5) * 100;
  Serial.println(temperature);*/
  
  //Вода
  int sensorWVal = analogRead(sensorWPin);
  float voltageW = (sensorWVal/1024.0) * 5.0;
  //Serial.print("degrees C: "); 
  float temperatureW = (voltageW - .5) * 100;
  Serial.println(temperatureW);
  
  //Свет
  int ledPinVal = analogRead(ledPin);
  

  //delay(10);
  
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          
          //вывод датчика темп и влажности
           if (isnan(t) || isnan(h)) {
          client.println("Failed to read from DHT");
          } else {
          client.println("Humidity: "); 
          client.println(h);
          client.println(" %\t");
          client.println("Temperature: "); 
          client.println(t);
          client.println(" *C");
          }
          
          
          client.println("<br />");
          client.print(temperatureW);
          client.println("<br />");
          client.print(ledPinVal);
          client.println("</html>");
           break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
      // close the connection:
      client.stop();
      //Serial.println(temperature);
  }
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setDoorStatus() {
  door_status = true;
}
