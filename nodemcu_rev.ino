// menerima single data dari node mcu ke arduino uno
//node mcu
#include <ESP8266WiFi.h>                                                // esp8266 library
#include <FirebaseArduino.h>                                             // firebase library
#define FIREBASE_HOST "dummy-ef627.firebaseio.com"                         // the project name address from firebase id
#define FIREBASE_AUTH "Q5QpybkeHQ5O6GZhEjYvXN0cDsWqlgkOX2Km5QJX"                    // the secret key generated from firebase
#define WIFI_SSID "OnePlus 6Ts"                                          // input your home or public wifi name 
#define WIFI_PASSWORD "segojagung" 
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Servo.h>

SoftwareSerial s(D6,D5); //rx tx
String fireStatus = ""; 
int data;
Servo myservo;
//const int motorServo = 5; //D1
int pos = 1;
int gerak = 0;
bool firstLoop = true;

float suhu = 0;
float kelembapan = 0;
int langit = 0;
bool atap = false;
bool manual = false;
bool action = true;

float Tsuhu = 0;
float Tkelembapan = 0;
int Tlangit = 0;
bool Tatap = false;
//bool Tmanual = false;
//bool Taction = true;


void connectWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address
}
void bukaAtap(){
 myservo.attach(5);
    for(pos = 1; pos < 150; pos +=1) {
        myservo.write(pos); 
        delay(8);               
    }
   myservo.detach();   
  }

void tutupAtap(){
  myservo.attach(5);
    for(pos = 150; pos > 1; pos -=1) {
      myservo.write(pos); 
      delay(8);               
    }
   myservo.detach();   
  }

void cekAtap(){
    if (!atap){
      if (pos == 150){
       Serial.println("MENUTUP ATAP");
       tutupAtap(); 
      }
    }
  
    if (atap){
       if (pos == 1){
         Serial.println("MEMBUKA ATAP");
         bukaAtap(); 
      }
    }
  }

void setFirebaseFirst(){
  Firebase.setFloat("suhu", suhu);  
  Firebase.setFloat("kelembapan", kelembapan);
  Firebase.setBool("atap", atap);
  Firebase.setInt("langit", langit);
  Firebase.setBool("manual", manual);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  s.begin(9600);
  connectWifi();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase
  tutupAtap();
  myservo.detach();
  while(!Serial) continue;

}

void loop() {
  // put your main code here, to run repeatedly:

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid()){
    return;
  }
  

  int hujan = root["hujan"];
  float ldr = root["ldr"];
  suhu = root["suhu"];
  kelembapan = root["kelembapan"];
  atap = root["atap"];
  langit = root["langit"];

  manual = Firebase.getBool("manual");
    
  if (!manual){
    cekAtap();
  } else {
    action = Firebase.getBool("action");
    if (action){
      atap = true;
      if (pos == 1){
         Serial.println("MEMBUKA ATAP");
         bukaAtap(); 
      }
    } else {
      atap = false; 
      if (pos == 150){
         Serial.println("MENUTUP ATAP");
         tutupAtap();
      }
    }
  }

  if(firstLoop){
    setFirebaseFirst();
  }
  
  Serial.println("JSON recieved and parsed");
  root.prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("");
 
  Serial.print("Sensor Hujan = ");
  //Firebase.setInt("hujan", hujan);
  Serial.print(hujan);
  Serial.println("");

  Serial.print("Sensor LDR = ");
  //Firebase.setFloat("ldr", ldr);
  Serial.print(ldr);
  Serial.println("");
    
  Serial.print("Suhu = ");
  if (!firstLoop && suhu != Tsuhu){
    Firebase.setFloat("suhu", suhu);  
  }
  Serial.print(suhu);
  Serial.println(" *C");

  Serial.print("Kelembapan = ");
  if (!firstLoop && kelembapan != Tkelembapan){
    Firebase.setFloat("kelembapan", kelembapan);
  }
  Serial.print(kelembapan);
  Serial.println(" %t");

  Serial.print("Kondisi Atap = ");
  if (!firstLoop && atap != Tatap){
    Firebase.setBool("atap", atap);
  }
  Serial.print(atap);
  Serial.println("");

  Serial.print("Kondisi Langit = ");
  if (!firstLoop && langit != Tlangit){
    Firebase.setInt("langit", langit);
  }
  Serial.print(langit);
  Serial.println("");

//  Serial.print("Gerak atap = ");
//  int gerak = root["gerak"];
//  Serial.print(gerak);
//  Serial.println("");
 
  Serial.println("========================");
  Serial.println(pos);

  Tsuhu = suhu;
  Tkelembapan = kelembapan;
  Tatap = atap;
  Tlangit = langit;
  firstLoop = false;

  
  //myservo.detach(); 
  delay(1500);

}
