/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

// #include "HX711.h"  


#ifndef STASSID
#define STASSID "ESP32"                  // "FABIAN"
#define STAPSK  "01234567"                    // "Larissa1453"
#endif

#define sensNum 2

// #define LED_BUILTIN2 16

/*
#define DT  D7                                  // Data pin
#define SCK D6                                   // Clock pin
*/

uint8_t ADC_DT = 7; // 0
uint8_t ADC_SCK = 6; // 2


const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.2.1";                  // "kot60.online"; //"djxmmx.net";
const uint16_t port = 80; //17;

String url = "/led1off"; 
bool ledStat = false;

uint16_t weight(0);

IPAddress ip, dns;

// HX711 scale;   

float calibration_factor = 1.39; // 1.391// 38.14;           // вводим калибровочный коэффициент
float units;                                                  // задаём переменную для измерений в граммах
float ounces;                                                 // задаём переменную для измерений в унциях



void setup() {

  // pinMode(LED_BUILTIN, OUTPUT); 
  // pinMode(LED_BUILTIN2, OUTPUT); 
  
  // pinMode(7, INPUT);
  // pinMode(6, OUTPUT);
  
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print(ssid); Serial.println(" ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  ip = WiFi.gatewayIP();
  dns = WiFi.dnsIP();
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");   Serial.println(WiFi.localIP());  
  Serial.print("GateWay IP: ");   Serial.println(WiFi.gatewayIP());  
  Serial.print("DNS IP: ");   Serial.println(WiFi.dnsIP());  
  Serial.print("MAC address: ");  Serial.println(WiFi.macAddress());
  Serial.print("WiFi SSID: ");    Serial.println(WiFi.SSID());

  Serial.println();
  Serial.print("DT: ");    Serial.println(ADC_DT);
  Serial.print("SCK: ");    Serial.println(ADC_SCK);
  Serial.println();

/*
  // scale.begin(ADC_DT, ADC_SCK);                            // инициируем работу с датчиком
  scale.begin(13, 12); 
  scale.set_scale();                                          // выполняем измерение значения без калибровочного коэффициента
  scale.tare();                                               // сбрасываем значения веса на датчике в 0
  scale.set_scale(calibration_factor);                        // устанавливаем калибровочный коэффициент
*/

  randomSeed(analogRead(0));

}

void loop() {
  // Serial.print("\nBuilt in LED: "); Serial.println(LED_BUILTIN);
  Serial.print("\nConnecting to ");
  Serial.print(ip); Serial.print(':'); Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(ip, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }  
/*
  Serial.print("Reading weight: ");                                  // выводим текст в монитор последовательного порта
  for (int i = 0; i < 10; i ++) {                             // усредняем показания, считав значения датчика 10 раз
    units = + scale.get_units(), 10;                          // суммируем показания 10 замеров
  }
  units = units / 10;                                         // усредняем показания, разделив сумму значений на 10
  ounces = units * 0.035274;                                  // переводим вес из унций в граммы
  Serial.print(ounces); // ounces                              // выводим в монитор последовательного порта вес в граммах
  Serial.println(" grams");                                   // выводим текст в монитор последовательного порта
  */
  weight = random(500, 30000);
  // weight = ounces;

  if (ledStat) {
    url = "/led1off";                    //   "/pics/Kot60_IoT.txt"; //"/index.html"
    ledStat = false;
  } else {
    url = "/led1on";                      //  "/pics/Kot60_IoT.txt"; //"/index.html"
    ledStat = true;
  }

  url += "?weight=";
  url += weight;
  url += "&sensor=";
  url += sensNum;
 
  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected()) {
    client.println(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");  //"hello from ESP8266");
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }  

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server\n");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
    // for (int i; i < 1000000; i++) {}
    delay(1);
  }

  // Close the connection
  Serial.println("closing connection");
  client.stop();

  // digitalWrite(LED_BUILTIN, HIGH);
  // digitalWrite(LED_BUILTIN2, LOW);

  delay(5000); // execute once every 5 minutes, don't flood remote service

  // digitalWrite(LED_BUILTIN, LOW);
  // digitalWrite(LED_BUILTIN2, HIGH);
}
