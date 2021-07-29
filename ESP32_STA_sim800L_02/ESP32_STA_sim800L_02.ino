  #include <WiFi.h>
#include <WebServer.h>
#include <String.h>
#include <SoftwareSerial.h>

#include "index.h"

// --------------------------  sim800L setUp -------------------

#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER   1024       // Receiver buffer 1024
#define DEBUG                           //enable debug mode
#include <TinyGsmClient.h>
 
#define RXD2 16
#define TXD2 17

#define APN_NAME "internet"             // Megafon internet
#define APN_USER "gdata"
#define APN_PSWD "gdata"

const char cloud_server[] = "kot60.online";       // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/index.php";             // path
const int  port = 80;                             // port number 

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 38400

SoftwareSerial SIM800L(RXD2, TXD2);         // RXD2 - RX esp32 (TX SIM800L), 9 - TXD2 esp32 (RX SIM800L)

TinyGsm sim800modem(SIM800L);               // modem control
  
TinyGsmClient client(sim800modem);          // http client

// --------------------------  sim800L setUp End -------------------


/* Local WiFi network credentials. AP */
const char* local_ssid      = "ESP32";  
const char* local_password  = "01234567";  

/* Internet access WiFi credentials. STA */
const char* ssid            = "FABIAN";       
const char* password        = "Larissa1453";  

/* Настройки IP адреса */
IPAddress local_ip(192,168,2,1);   // IP-network for local clients
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);              // web server on board of esp32

uint8_t LED1pin = 2;
bool LED1status = LOW;

int weight(0), storedWeight(0);

typedef struct {                  // structure of sensor values
  uint8_t sensNum;
  uint16_t weight;  
} sensorValues, *psensorValues;

sensorValues sValues[4];          // sensorValue instance

uint16_t Weights[4];
uint16_t *pWeights;

uint32_t connection_time(0);      // time marker for cloud connecting


void setup() {
  Serial.begin(115200);

  // SIM800L module restart. If isn't needed - insert init() instead of restart()
  
  Serial.println("Initializing modem...");
  sim800modem.restart();

  SIM800L.begin(9600);               // Скорость обмена данными с модемом
  SIM800L.println("AT");             // Автонастройка скорости
  SIM800L.println("AT+CLVL?");       // Запрашиваем громкость динамика
  SIM800L.println("AT+CMGF=1");      // Включить TextMode для SMS
  SIM800L.println("AT+DDET=1,0,1");  // Включить DTMF
  SIM800L.println("AT+CLIP=1");      // Включить АОН

  
  pinMode(LED1pin, OUTPUT);          // For esp32. No need ??
  // pinMode(LED2pin, OUTPUT);

  /*---------------- AP mode. Local WiFi network initializing ------------------*/
  WiFi.softAP(local_ssid, local_password);        // "ESP32", "01234567"
  WiFi.softAPConfig(local_ip, gateway, subnet);   // local_ip, gateway, 
  
  Serial.println();
  Serial.print("Soft AP SSID:\t");  Serial.println( WiFi.softAPSSID() );  
  Serial.print("Soft AP IP:\t");    Serial.println( WiFi.softAPIP() );  
  Serial.print("Soft AP MAC:\t");   Serial.println( WiFi.softAPmacAddress() );
  
  delay(100);

   /*-------------- STA mode. Simultaneous AP and STA communication ------------------------*/

  WiFi.begin(ssid, password);                     // Internet access via WiFi. STA mode
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) { delay(1000);  Serial.print("."); }
  
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print( "WiFi SSID:\t");    Serial.println(WiFi.SSID());
  Serial.print( "Got IP:\t\t");     Serial.println(WiFi.localIP());
  Serial.print( "GateWay IP:\t");   Serial.println(WiFi.gatewayIP());  
  Serial.print( "DNS IP:\t\t");     Serial.println(WiFi.dnsIP());  
  Serial.print( "MAC address:\t");  Serial.println(WiFi.macAddress()); 
  Serial.print( "Host name:\t");    Serial.println(WiFi.getHostname()); 

  /*-------------- AT and STA module end ------------------------*/  

  /*-------------- Local WEB server initializing ----------------*/  
  server.on("/",            handle_OnConnect );
  server.on("/led1on",      handle_led1on );
  server.on("/led1off",     handle_led1off );
  server.on("/readWeight",  handle_readWeight );
  server.on("/SetUp",       handle_SetUp );
  /*server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);*/
  server.onNotFound(        handle_NotFound );
  
  server.begin();
  Serial.println("HTTP server started");
  /*-------------- END Local WEB server initializing ----------------*/  
  
  // Serial.println(GPIO_OUT0_31);
  Serial.print("GPIO_MODE_OUTPUT ");    Serial.println(GPIO_MODE_OUTPUT, BIN);
  Serial.print("GPIO_FUNC0_OUT_SEL ");  Serial.println(GPIO_FUNC0_OUT_SEL, BIN);
  //  REG_WRITE(GPIO_ENABLE_W1TC_REG, 0xFF << 12 ); // It works!!! PARALLEL_0); // io_conf.mode); // = GPIO_MODE_OUTPUT;
 // Serial.println(GPIO_FUNC0_IN_SEL_CFG);
 Serial.print("GPIO_OUT_DATA ");        Serial.println(GPIO_OUT_DATA);

  connection_time = millis();  // I-net connection timer
}  // End setup()


void loop() {

  // ---------------- Data exchange Serial monitor <--> sim800L modem ------------------
  if (SIM800L.available())           // Ожидаем прихода данных (ответа) от модема...
    Serial.write(SIM800L.read());    // ...и выводим их в Serial
  if (Serial.available())           // Ожидаем команды по Serial...
    SIM800L.write(Serial.read());    // ...и отправляем полученную команду модему
    
  // -------------------- 2G i-net connection / data sending ---------------------
  if ( millis() - connection_time > 300000 ) {                    // once in 5 minutes

    unsigned long sendingDuration = millis();
    Serial.print("System time: ");          Serial.println( sendingDuration ); // Just to measure sending duration
  
    Serial.print("Connecting to APN: ");   Serial.print(APN_NAME);
    
    if (!sim800modem.gprsConnect(APN_NAME, APN_USER, APN_PSWD)) { Serial.println(" fail"); }
    else {
      Serial.println(" OK");      
      Serial.print("Connecting to ");    Serial.print(cloud_server);
      
      if (!client.connect(cloud_server, port)) { Serial.println(" fail"); }
      else {
        Serial.println(" OK");      
        // POST request
        Serial.println( "Performing HTTP POST request..." );
        // Data to send: 
        // String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(bme.readTemperature())
        //                        + "&value2=" + String(bme.readHumidity()) + "&value3=" + String(bme.readPressure()/100.0F) + "";
  
        String httpRequestData = "api_key=" + String(1234) + "&value1=" + String(25)                    // some data to send to the cloud server
                                  + "&value2=" + String(80) + "&value3=" + String(1024/100.0F) + "";                
                
        client.print(String("POST ") + resource + " HTTP/1.1\r\n");
        client.print(String("Host: ") + cloud_server + "\r\n");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(httpRequestData.length());
        client.println();
        client.println(httpRequestData);

        Serial.print( "Data sent within: " ); Serial.print( ( millis() - sendingDuration ) / 1000 ); Serial.println(" Sec");  // actual data transfer duration
   
        unsigned long timeout = millis();
        while (client.connected() && millis() - timeout < 10000L) {
          
          while (client.available()) {          // Data to serial (Server HTTP answer)
            char c = client.read();
            Serial.print(c);
            timeout = millis();
          }
        }
        Serial.println();
      
        // Closing client and switch off
        client.stop();
        Serial.println(F("Server disconnected"));
        sim800modem.gprsDisconnect();
        Serial.println(F("GPRS disconnected"));
        
        // delay (600000);
        connection_time = millis();
      }
    }
  }


  // --------------------------  WiFi functionality part ---------------------
  
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}

  // --------------------------  WiFi functionality part End ---------------------
  
}  // END loop()

void handle_OnConnect() {
  String w  = MAIN_page;
  Serial.print("Active clients: ");  Serial.println( WiFi.softAPgetStationNum() ); // Number of current clients
  LED1status = LOW;
  // LED2status = LOW;
  Serial.println("GPIO4 Status: OFF");                    //  | GPIO5 Status: OFF
  
  client_data(Weights);
  
  server.send(200, "text/html", w );    //  SendHTML(LED1status, 777) );    // ,LED2status
  }

void handle_led1on() {
  String w  = MAIN_page; // = "";
  Serial.println();
  
  // softAPdata();   // SSID, IP, MAC, clients#
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");

  client_data(Weights);
  w = "Sensor 1.: " + String(Weights[0]) + " Kg<br>" +
      "Sensor 2.: " + String(Weights[2]) + " Kg<br>";
  
  server.send(200, "text/html", w);     // SendHTML(true, weight) );    // ,LED2status
}

void handle_led1off() {
  String w  = MAIN_page;   // = "";
  Serial.print("Active clients: ");  Serial.println( WiFi.softAPgetStationNum() ); // Number of current clients
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");  
  
  client_data(Weights);
  
  server.send(200, "text/html", w);    // SendHTML(false, weight) );   // ,LED2status
}

void handle_readWeight() {
  String w = "";
  Serial.print("Active clients: ");  Serial.println( WiFi.softAPgetStationNum() ); // Number of current clients
    
  client_data(Weights);

  w = "Sensor 1.: " + String(Weights[0]) + " Kg<br>" +
      "Sensor 2.: " + String(Weights[2]) + " Kg<br>";
  Serial.print("Weight1: "); Serial.print(String(Weights[0]));
  Serial.print("\tWeight2: "); Serial.println(String(Weights[2]));
  
  server.send(200, "text/html", w);   // String(weight) ,LED2status
}

void handle_SetUp(){
  // E.g. Tara, null mode
  server.send(200, "text/html", SendHTMLsetUp()); 
}


void handle_NotFound(){
  server.send(404, "text/plain", "404 Not found");
}

void client_data(uint16_t* sensorWeights) {                  // Read velue in ARG(0), ggf. Weight detected by HX711
  String argValue = "";
  uint8_t sensorN = 0;
  
  if ( server.args() ) {
    argValue =  server.arg(0);
    weight =    argValue.toInt();         // Global variable, current measurement data
    argValue =  server.arg(1);
    sensorN =   argValue.toInt();         // could be 0, 1, 2, 3
    sensorWeights[sensorN] = weight;      // store the received data imto the array of weights
    
    Serial.print(server.arg(0)); Serial.print("\t"); Serial.print(server.arg(1)); Serial.print("\t"); Serial.println(server.arg(2));

    Serial.print("sensor No. "); Serial.print(sensorN);
    Serial.print("  received Value "); Serial.println(weight);  // Weight in Kg, Tonns
    storedWeight = weight;              // Global variable to store the measurement data. Just for compilation purposes!!!
  }
  else weight = storedWeight;           // Previous measurement data to show something on the WEB interface  
  
  Serial.print("Weight: "); Serial.print(weight); Serial.println(" Kg"); // or Tonns
  
  Serial.print("Sensor 0.\tSensor 1.\tSensor 2.\n"); 
  Serial.print(sensorWeights[0]); Serial.print("\t\t"); Serial.print(sensorWeights[1]); Serial.print("\t\t"); Serial.println(sensorWeights[2]);
  
}

void softAPdata() {
  Serial.print("Soft AP SSID:\t");    Serial.println( WiFi.softAPSSID() );  
  Serial.print("Soft AP IP:\t");      Serial.println( WiFi.softAPIP() ); 
  // Serial.print("Soft AP gateWay:\t");      Serial.println( WiFi.softAPgateway() ); 
  
  Serial.print("Soft AP MAC:\t");     Serial.println( WiFi.softAPmacAddress() );  
  Serial.print("Active clients: ");   Serial.println( WiFi.softAPgetStationNum() ); // Number of current clients
}


String SendHTMLsetUp(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Sea container weight measurement</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Scale server</h1>\n";
  ptr +="<h3>WiFi client and Access Point mode (AP+STA)</h3>\n";  
  
  ptr += "<h3>Set up section</h3>";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String SendHTML(uint8_t led1stat, int weightKg){           // ,uint8_t led2stat
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Sea container weight measurement</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 20px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Scale server</h1>\n";
  ptr +="<h3>WiFi client and Access Point mode (AP+STA)</h3>\n";  
  
  if(led1stat)
    {ptr +="<p>Indicator state: ON</p><a class=\"button button-off\" href=\"/led1off\">Switch OFF</a>\n";}
  else
    {ptr +="<p>Indicator state: OFF</p><a class=\"button button-on\" href=\"/led1on\">Switch ON  </a>\n";}
  /*  
  ptr +="<h3>Weight: ";
  ptr += weightKg;
  ptr += " Kg</h3>\n";*/

  ptr +="<h3>Weight:<br><span id=\"Weight\">___</span></h3>\n";

    // JS script Ajax
    ptr +="<script>";
    
    ptr +="setInterval(function() {";
    ptr +="  getData();";
    ptr +="  }, 2000);";  // every 2000 mS
    
    ptr +="function getData() {";
    ptr +="  var xhttp = new XMLHttpRequest();";
    ptr +="  xhttp.onreadystatechange = function() {";
    ptr +="    if (this.readyState == 4 && this.status == 200) {";
    ptr +="      document.getElementById(\"Weight\").innerHTML = this.responseText;";
    ptr +="    }";
    ptr +="  };";
    ptr +="  xhttp.open(\"GET\", \"/readWeight\", true);";
    ptr +="  xhttp.send();";
    ptr +="}";
    ptr +="</script>";  
  
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
