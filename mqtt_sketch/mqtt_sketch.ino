#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <Servo.h>

const char ssid[] = "ssid";
const char pass[] = "science@78";

WiFiClient net;
MQTTClient client;
Servo myservo;  // create servo object to control a servo


unsigned long lastMillis = 0;
int pos=0;
const int light= D0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("mqtt", "xxxx", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("xxxx/feeds/servo");   //Change xxxx with Adafruit.io username
  client.subscribe("xxxx/feeds/light");   //Change xxxx with Adafruit.io username
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if(topic=="iot_guy/feeds/light"){
    if(payload=="1")
      digitalWrite(light,LOW);
    else
      digitalWrite(light,HIGH);
  }
  if(topic=="iot_guy/feeds/servo"){
        pos = payload.toInt();
        Serial.println("SERVO POSITION: "+pos);
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
  }

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass); 
  pinMode(light, OUTPUT);
  digitalWrite(light,HIGH);
  myservo.attach(D8);  // attaches the servo on D8 to the servo object
  
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("io.adafruit.com", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(5000);  // <- fixes some issues with WiFi stability
  
  if (!client.connected()) {
    connect();
  }
}
