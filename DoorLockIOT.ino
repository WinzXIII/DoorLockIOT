//ProJectIOT
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "MFRC522.h"
#include <Servo.h>

#define RST_PIN  D1
#define SS_PIN  D2

const char* ssid = "Winz";
const char* password = "123456789win";
const char* host = "api.thingspeak.com";
unsigned long time1;
int LED = 16; //D0
int BUTTON = 0; //D3
int button;
int BUZZER = 15; //D8
String rfid_in = "";
boolean toneTry = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);
WiFiClient client;
DynamicJsonDocument doc(2048);
DeserializationError error;
Servo myservo;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.disconnect();
  delay(1000);

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("");
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);
  myservo.attach(2, 500, 2400);
}


void loop() {
  if (millis() - time1 >= 5000) { // delay 5 sec
    Serial.println();
    time1 = millis();
    connectHost();
    String url = "https://api.thingspeak.com/channels/1905374/feeds";
    url = url + ".json?api_key=DACSTO8X3I00IRBI&results=";
    url = url + "1";
    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.println(String("GET ") + url + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Connection: close");
    client.println();

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    String line = "";
    while (client.available()) {
      line = line + char(client.read());
      delay(1);
    }
    delay(10);
    client.stop();
    delay(10);

    int JSfirst = line.indexOf('{');
    int JSlast = line.lastIndexOf('}');
    line.remove(JSlast + 2);
    line.remove(0, JSfirst);
    error = deserializeJson(doc, line);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    String feeds = doc["feeds"];
    feeds.replace("[", "");
    feeds.replace("]", "");

    error = deserializeJson(doc, feeds);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    String field1 = doc["field1"];
    Serial.print("Door:");
    Serial.print(field1);
    //---------------------------------------------------------------------------------IOT HERE!!!
    if (field1 == "1") {
      digitalWrite(LED, 1);
      myservo.write(199);
    } else if (field1 == "0") {
      digitalWrite(LED, 0);
      myservo.write(0);
    }
  }
  delay(1);
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) { //-----------RFID HERE!!!
    rfid_in = rfid_read();
    Serial.println("\n>>>> " + rfid_in);
    if (rfid_in == "79 01 1C BD") { //บัตรขาว
      digitalWrite(LED, 1);
      tone(BUZZER, 2380, 2000);
      myservo.write(199);
      delay(15000);
      tone(BUZZER, 2380, 500);
      delay(1000);
      tone(BUZZER, 2380, 600);
      delay(1000);
      tone(BUZZER, 2380, 800);
      delay(1000);
      digitalWrite(LED, 0);
      myservo.write(0);
      delay(2000);
    }
    else if (rfid_in == "8A 1A B6 3B") { //พวกกุญแจ
      digitalWrite(LED, 1);
      tone(BUZZER, 2380, 2000);
      myservo.write(199);
      delay(15000);
      tone(BUZZER, 2380, 500);
      delay(1000);
      tone(BUZZER, 2380, 600);
      delay(1000);
      tone(BUZZER, 2380, 800);
      delay(1000);
      digitalWrite(LED, 0);
      myservo.write(0);
      delay(2000);
    }
    else if (rfid_in == "96 B9 40 41") { //บัตรนศ.
      digitalWrite(LED, 1);
      tone(BUZZER, 2380, 2000);
      myservo.write(199);
      delay(15000);
      tone(BUZZER, 2380, 500);
      delay(1000);
      tone(BUZZER, 2380, 600);
      delay(1000);
      tone(BUZZER, 2380, 800);
      delay(1000);
      digitalWrite(LED, 0);
      myservo.write(0);
      delay(2000);
    }
    else {
      digitalWrite(LED, 1);
      tone(BUZZER, 2380, 300);
      delay(500);
      digitalWrite(LED, 0);
      delay(500);
      digitalWrite(LED, 1);
      tone(BUZZER, 2380, 300);
      delay(500);
      digitalWrite(LED, 0);
      delay(2000);
    }
    delay(1000);
  }
  delay(1);

  button = digitalRead(BUTTON); //--------------------------------------------------BUTTON HERE!!!
  if (button == LOW) {
    digitalWrite(LED, 1);
    tone(BUZZER, 2380, 2000);
    myservo.write(199);
    delay(15000);
    tone(BUZZER, 2380, 500);
    delay(1000);
    tone(BUZZER, 2380, 600);
    delay(1000);
    tone(BUZZER, 2380, 800);
    delay(1000);
    digitalWrite(LED, 0);
    myservo.write(0);
    delay(2000);
  }
}

int connectHost() {
  if (!client.connect(host, 80)) {
    Serial.println(" !!! Connection failed !!! ");
    delay(10);
    return 0;
  } else {
    return 1;
  }
}

String rfid_read() {
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  return content.substring(1);
}
