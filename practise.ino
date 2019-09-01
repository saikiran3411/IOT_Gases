#include <SLFS.h>
#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad

#include <SPI.h>
#endif
#include <WiFi.h>
//Receiver variables
int actualTimeOn=0;
float ran;
//gas sensor variables
int pinNumber = A1;
int pinNumber2 = A2;
float loadResistance = 2.5;
int samplesTaken = 100;
int samplesTime = 5;//milliseconds
float cleanAirSensorRatio = 9.83; //from datasheet
float gasSensorResistance;
float cleanAirSensorResistance=1.86;
int lpg;
int propane;
int h2;
int alcohol;
int methane;
int smoke;
int co;
int lpg2;
int propane2;
int h22;
int alcohol2;
int methane2;
int smoke2;
int co2;
//wifi variables
char wifi_name[] = "default";
char wifi_password[] = "openfree";
WiFiServer myServer(80);
uint8_t oldCountClients = 0;
uint8_t countClients = 0;
char ssid[50];
char password[50];
  int ssidRead;
  int m;
  int passwordRead;
  int n;
String text;
String mac0;
String mac1;
String mac2;
String mac3;
String mac4;
String mac5;
const int button = PUSH1;
WiFiClient client;
//Server variables
char server[] = "166.62.28.97"; //stranger address
String GETtoStranger;
String GETtoStranger2;
void setup() {
   pinMode(button, INPUT_PULLUP);
 Serial.begin(115200);
 SerFlash.begin(); // <====== Essential! Initiates the SimpleLink API and DMA channels.
 SerFlash.open("ssid.txt", FS_MODE_OPEN_READ); 
  m=0;
  do
  {
    ssidRead=SerFlash.read(); 
    if  (ssidRead != -1)
    {
      ssid[m]= (char)ssidRead; //integer cast to char
 
      m++;
    }
  }
  while (ssidRead != -1);
  SerFlash.close();// close the file
  Serial.println(ssid);
 SerFlash.open("password.txt", FS_MODE_OPEN_READ); 
  n=0;
  do
  {
    passwordRead=SerFlash.read(); 
    if  (passwordRead != -1)
    {
      password[n]= (char)passwordRead; //integer cast to char
 
      n++;
    }
  }
  while (passwordRead != -1);
  Serial.println();
  SerFlash.close();// close the file
  Serial.println(password);
  //initRo();//Calculate Ro;
    if(digitalRead(button)){
    switchToAp();
    }
  if(ssid[0]==0&&password[0]==0){
      digitalWrite(YELLOW_LED,HIGH);
      startApMode();
      }else{
        digitalWrite(YELLOW_LED, LOW);
        switchToSta();
        }
       
}

void loop() {
  if(digitalRead(button)){
    switchToAp();
    }
   if(ssid[0]==0&&password[0]==0){
      digitalWrite(YELLOW_LED,HIGH);
     apmodeClientCheck();
      }else{
        digitalWrite(YELLOW_LED, LOW);
        if(WiFi.status() == WL_CONNECTED){
         getVolt();
        readppm();//calculates ppm
    GET();//Url of webpage
  insertppm();//insert into database
    insertppm2();//insert into database
  delay(1000);}
        }
  
  
}
//void initRo(){
//Serial.print("Calibrating..\n");
// cleanAirSensorResistance = 3.82;
//Serial.print("R0 = ");
//Serial.print(cleanAirSensorResistance);
//Serial.print("kohm \n");
//Serial.print("Calibrating the hazardeous gases...\n");
//}
float SensorResistance2(){
    int i;
    float val = 0;
        val = ResistanceCalculation2();
      return val;
}
float ResistanceCalculation2(){
int temp = analogRead(pinNumber2);
 float rs2 = (float) loadResistance*(4095-temp)/temp ;
  return rs2;
}
float SensorResistance(){
    int i;
    float val = 0;
        val = ResistanceCalculation();
      return val;
}
float ResistanceCalculation(){
 float rs = (float) loadResistance*(5-ran)/ran ;
  return rs;
}
float getVolt(){
  int i,c,val;
while(actualTimeOn<50){
val = analogRead(pinNumber);
  while(val<2000){
    val = analogRead(pinNumber);
    c=1;
    }
    if(c==1){
    while(val<2000){
    val = analogRead(pinNumber);
    }
   int timeIn = millis();
    while(val>2000){
      val = analogRead(pinNumber);
      }
     int timeOut = millis();
     int timeOn = timeOut-timeIn;
      if(timeOn>50){
        actualTimeOn = timeOn;
         Serial.println(actualTimeOn);
      }
       c=0;
       }
}
   ran = (float) 5*actualTimeOn/1000;
   actualTimeOn = 0;
  }
void startApMode(){
  Serial.println("*** LaunchPad CC3200 WiFi Web-Server in AP Mode");
    
    // Start WiFi and create a network with wifi_name as the network name
    // with wifi_password as the password.
    Serial.print("Starting AP... ");
    WiFi.beginNetwork(wifi_name, wifi_password);
    while (WiFi.localIP() == INADDR_NONE)
    {
        // print dots while we wait for the AP config to complete
        Serial.print('.');
        delay(300);
    }
    Serial.println("DONE");
    
    Serial.print("LAN name = ");
    Serial.println(wifi_name);
    Serial.print("WPA password = ");
    Serial.println(wifi_password);
    
    
    pinMode(RED_LED, OUTPUT);      // set the LED pin mode
    digitalWrite(RED_LED, LOW);
    pinMode(GREEN_LED, OUTPUT);      // set the LED pin mode
    digitalWrite(GREEN_LED, LOW);
     pinMode(YELLOW_LED, OUTPUT);      // set the LED pin mode
    digitalWrite(YELLOW_LED, LOW);
    IPAddress ip = WiFi.localIP();
    Serial.print("Webserver IP address = ");
    Serial.println(ip);
    
    Serial.print("Web-server port = ");
    myServer.begin();                           // start the web server on port 80
    Serial.println("80");
    Serial.println();
  }
  void switchToSta(){
  sl_Start(NULL, NULL, NULL);

  sl_WlanDisconnect();
  
  sl_NetAppMDNSUnRegisterService(0, 0);
  
  sl_WlanRxStatStart();
  
  sl_WlanSetMode(ROLE_STA);

  /* Restart Network processor */
  sl_Stop(30);

  sl_Start(NULL, NULL, NULL);

                // attempt to connect to Wifi network:
                Serial.print("Attempting to connect to Network named: ");
                delay(500);
                // print the network name (SSID);
                Serial.println(ssid);                
                // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
                WiFi.begin(ssid, password);
                int mcheck = millis();
                while ( WiFi.status() != WL_CONNECTED) {
                  // print dots while we wait to connect
                  Serial.print(".");
                  delay(300);
                  if(millis()-mcheck > 15000){
                    Serial.println("Timeout Cannot Find the wifi network Provided");
                    WiFi.disconnect();
                    Serial.println("Push Refresh to again connect to same wifi");
                    Serial.println("Push Reset to provide new Credentials");
                    delay(10000);
                    if(digitalRead(button)){
                      digitalWrite(YELLOW_LED, LOW);
                      switchToAp();
                      mcheck = 100000;
                      }
                      break;
                    }
                   
                }
                if(WiFi.status() == WL_CONNECTED){
                Serial.println("\nYou're connected to the network");
                Serial.println("Waiting for an ip address");

                while (WiFi.localIP() == INADDR_NONE) {
                  // print dots while we wait for an ip addresss
                  Serial.print(".");
                  delay(300);
                }

                Serial.println("\nIP Address obtained");

                // you're connected now, so print out the status
                printWifiStatus();}
    }
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
MACAddress mac = getMacAddress();
 Serial.print("Mac Address: ");
 Serial.println(mac);
  mac0 = mac[0];
  mac1 = mac[1];
  mac2 = mac[2];
  mac3 = mac[3];
  mac4 = mac[4];
  mac5 = mac[5];
 long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
MACAddress getMacAddress()
{
    uint8_t macAddressVal[SL_MAC_ADDR_LEN];
    uint8_t macAddressLen = SL_MAC_ADDR_LEN;
    sl_NetCfgGet(SL_MAC_ADDRESS_GET,NULL,&macAddressLen,macAddressVal);
    MACAddress MAC(macAddressVal);
    return MAC;
}
void apmodeClientCheck(){
   countClients = WiFi.getTotalDevices();
    
    // Did a client connect/disconnect since the last time we checked?
    if (countClients != oldCountClients)
    {
        if (countClients > oldCountClients)
        {  // Client connect
            //            digitalWrite(RED_LED, !digitalRead(RED_LED));
            Serial.println("Client connected to AP");
            for (uint8_t k = 0; k < countClients; k++)
            {
                Serial.print("Client #");
                Serial.print(k);
                Serial.print(" at IP address = ");
                Serial.print(WiFi.deviceIpAddress(k));
                Serial.print(", MAC = ");
                Serial.println(WiFi.deviceMacAddress(k));
                Serial.println("CC3200 in AP mode only accepts one client.");
            }
        }
        else
        {  // Client disconnect
            //            digitalWrite(RED_LED, !digitalRead(RED_LED));
            Serial.println("Client disconnected from AP.");
            Serial.println();
        }
        oldCountClients = countClients;
    }
    
    WiFiClient myClient = myServer.available();
    if (myClient)
    {                             // if you get a client,
        Serial.println(". Client connected to server");           // print a message out the serial port
        char buffer[150] = {0};                 // make a buffer to hold incoming data
        int8_t i = 0;
        while (myClient.connected())
        {            // loop while the client's connected
            if (myClient.available())
            {             // if there's bytes to read from the client,
                char c = myClient.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character
                    
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (strlen(buffer) == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        myClient.println("HTTP/1.1 200 OK");
                        myClient.println("Content-type:text/html");
                        myClient.println();
                        
                        // the content of the HTTP response follows the header:
                        myClient.println("<html><head><title>User Interface</title></head><body>");
                        myClient.println("<center><form style='text-align: center;line-height: 30px;width: 50%;border: 1px solid black;height: 500px;'><h1 style='color: red;opacity: .9;'>CONFIGURE TO YOUR WIFI</h1><input style='height: 20px;' type='text' name='ssid' placeholder='SSID'><br><input type='text' style='height: 20px;' name='pass' placeholder='Password'><br><button>Submit</button></form><br>");
          
                        
                        // The HTTP response ends with another blank line:
                        myClient.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    {      // if you got a newline, then clear the buffer:
                        memset(buffer, 0, 150);
                        i = 0;
                    }
                }
                else if (c != '\r')
                {    // if you got anything else but a carriage return character,
                    buffer[i++] = c;      // add it to the end of the currentLine
                }
                
                   text = buffer;
                 // Check to see if the client request was "GET /H" or "GET /L":
               
                if (text.startsWith("ssid",6))
                {
                  
                    digitalWrite(RED_LED, HIGH);  
                    Serial.println();     
             int textlength = strlen(buffer);       
              int i; int j =0; int k=0;
            int beforeSsid = text.indexOf("=");
            int ssidEnd = text.indexOf("&");
            int ssidStart = beforeSsid +1;
            int beforePass = text.indexOf("=", ssidStart);
            int passStart = beforePass+1;
            int passIdStart = ssidEnd + 1;
            if(text.startsWith("pass",passIdStart)){
              memset(ssid, 0, 50);
              memset(password, 0, 50);
              digitalWrite(GREEN_LED,HIGH);
              int firstSpace = text.indexOf(" ");
              int second = firstSpace +1;
              int secondSpace= text.indexOf(" ", second);
              int beforeSsid = text.indexOf("=");
            int ssidEnd = text.indexOf("&");
            int ssidStart = beforeSsid +1;
            int beforePass = text.indexOf("=", ssidStart);
            int passStart = beforePass+1;
               for(i=passStart; i<secondSpace;i++){
              password[k] = buffer[i];
              k++;
              }
              }
            for(i=ssidStart; i<ssidEnd;i++){
              ssid[j] = buffer[i];
              j++;
              }
           
            // GET /H turns the LED on
                }
            }
        }        
        // close the connection:
        myClient.stop();
        Serial.println(". Client disconnected from server");
        Serial.println();
        Serial.println(ssid);
          Serial.println(password);

    }     
    if(ssid[0]==0&&password[0]==0){
      digitalWrite(YELLOW_LED,HIGH);
      }else{
         digitalWrite(YELLOW_LED, LOW);
         SerFlash.open("ssid.txt", FS_MODE_OPEN_CREATE(1024, _FS_FILE_OPEN_FLAG_COMMIT));// create and open the file with this name

 
  // THIS OPENS THE FILE FOR WRITING -----------------
  SerFlash.open("ssid.txt", FS_MODE_OPEN_WRITE);  //open ile for writing to
  SerFlash.write(ssid);// write content to the file
  SerFlash.close();// close the file

   SerFlash.open("password.txt", FS_MODE_OPEN_CREATE(1024, _FS_FILE_OPEN_FLAG_COMMIT));// create and open the file with this name

 
  // THIS OPENS THE FILE FOR WRITING -----------------
  SerFlash.open("password.txt", FS_MODE_OPEN_WRITE);  //open ile for writing to
  SerFlash.write(password);// write content to the file
  SerFlash.close();// close the file
        switchToSta();
        }

  }
void readppm(){
   float gasesResistance = (float)SensorResistance()/cleanAirSensorResistance;
   float gasesResistance2 = (float)SensorResistance2()/;
  Serial.println(gasesResistance);
   lpg = ppmCalculation(gasesResistance, 0);
   propane = ppmCalculation(gasesResistance, 1);
   h2 = ppmCalculation(gasesResistance, 2);
   alcohol = ppmCalculation(gasesResistance, 3);
   methane = ppmCalculation(gasesResistance, 4);
   smoke = ppmCalculation(gasesResistance, 5);
   co = ppmCalculation(gasesResistance, 6);   
   lpg2 = ppmCalculation(gasesResistance2, 0);
   propane2 = ppmCalculation(gasesResistance2, 1);
   h22 = ppmCalculation(gasesResistance2, 2);
   alcohol2 = ppmCalculation(gasesResistance2, 3);
   methane2 = ppmCalculation(gasesResistance2, 4);
   smoke2 = ppmCalculation(gasesResistance2, 5);
   co2 = ppmCalculation(gasesResistance2, 6);
  Serial.print("LPG = ");
  Serial.print(lpg);
  Serial.println("ppm ");
  Serial.print("Propane = ");
  Serial.print(propane);
  Serial.println("ppm ");
  Serial.print("H2 = ");
  Serial.print(h2);
  Serial.println("ppm ");
  Serial.print("Alcohol = ");
  Serial.print(alcohol);
  Serial.println("ppm ");
  Serial.print("CH4 = ");
  Serial.print(methane);
  Serial.println("ppm ");
  Serial.print("Smoke = ");
  Serial.print(smoke);
  Serial.println("ppm ");
  Serial.print("CO = ");
  Serial.print(co);
  Serial.println("ppm ");
  }
int ppmCalculation(float resValue, int id){
 if(id == 0){
     float ppmVal = (float) 575.62*pow(resValue, -2.12);
    if(ppmVal<200.0){ppmVal = 0;}else if(ppmVal>5000){ppmVal=5000;}
      return ppmVal;
      }
   else if(id == 1){
    float ppmVal = (float) 623.485*pow(resValue, -2.165);
   if(ppmVal<200){ppmVal = 0;}else if(ppmVal>5000){ppmVal=5000;}
      return ppmVal;
    }
    else if(id == 2){
    float ppmVal = (float) 966.496*pow(resValue, -2.1037);
   if(ppmVal<300.0){ppmVal = 0;}else if(ppmVal>5000){ppmVal=5000;}
      return ppmVal;
    }
    else if(id == 3){
    float ppmVal = (float) 3482.91*pow(resValue, -2.689);
   if(ppmVal<100.0){ppmVal = 0;}else if(ppmVal>2000){ppmVal=2000;}
      return ppmVal;
    }
    else if(id == 4){
    float ppmVal = (float) 4290.84*pow(resValue, -2.619);
      return ppmVal;
    }
    else if(id == 5){
    float ppmVal = (float) 3910.35*pow(resValue, -2.285);
      return ppmVal;
    }
    else if(id == 6){
    float ppmVal = (float) 28373*pow(resValue, -2.947);
      return ppmVal;
    }
}

void GET(){
  GETtoStranger = "GET /project/upload.php?macid="+mac0+":"+mac1+":"+mac2+":"+mac3+":"+mac4+":"+mac5+"&lpg="+String(lpg)+"&propane="+String(propane)+"&methane="+String(methane)+"&h2="+String(h2)+"&alcohol="+String(alcohol)+"&smoke="+String(smoke)+"&co="+String(co);
   GETtoStranger2 = "GET /project/upload.php?macid=2&lpg="+String(lpg2)+"&propane="+String(propane2)+"&methane="+String(methane2)+"&h2="+String(h22)+"&alcohol="+String(alcohol2)+"&smoke="+String(smoke2)+"&co="+String(co2);
  Serial.println(GETtoStranger);
  }
void insertppm(){
  Serial.println("\nConnecting to Stranger");
  if(client.connect(server, 80)){
    Serial.println("Connected");
     client.print(GETtoStranger);
     client.println(" HTTP/1.1");
    client.println("Host: strangerpro.com");
    client.println("Connection: close");
    client.println();
    client.println();
    }  
    while(client.connected()){
  while (client.available()){
    char c = client.read();
    Serial.write(c);
    }}
    if(!client.connected()){
      Serial.println();
      Serial.println("Disconnected");
      client.stop();
      }
}
void insertppm2(){
  Serial.println("\nConnecting to Stranger");
  if(client.connect(server, 80)){
    Serial.println("Connected");
     client.print(GETtoStranger2);
     client.println(" HTTP/1.1");
    client.println("Host: strangerpro.com");
    client.println("Connection: close");
    client.println();
    client.println();
    }  
    while(client.connected()){
  while (client.available()){
    char c = client.read();
    Serial.write(c);
    }}
    if(!client.connected()){
      Serial.println();
      Serial.println("Disconnected");
      client.stop();
      }
}
void switchToAp(){
    memset(ssid, 0 , 50);
  memset(password, 0 , 50);
  sl_Start(NULL, NULL, NULL);

  sl_WlanDisconnect();
  
  sl_NetAppMDNSUnRegisterService(0, 0);
  
  sl_WlanRxStatStart();

  sl_WlanSetMode(ROLE_AP);
  
  sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, strlen(wifi_name),
                              (unsigned char *)wifi_name);
  
  unsigned char  val = SL_SEC_TYPE_WPA;
  sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, 1, (unsigned char *)&val);

  sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, strlen(wifi_password),
                              (unsigned char *)wifi_password);

  /* Restart Network processor */
  sl_Stop(30);

  sl_Start(NULL, NULL, NULL);
  startApMode();

  }
