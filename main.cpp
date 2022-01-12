#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "JOSEPH";
const char* password = "00000000";

// Initialize Telegram BOT
#define BOTtoken "2035773795:AAGLoBUrKI0_vIk1f63RWhjaDt7mXd6peLM"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1135395154"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

#define sensor 2
const int fan = 12;
const int bulb = 14;
const int AC = 26;
const int door = 27;
bool ledState = LOW;

const int trigPin = 32;
const int echoPin = 33;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;


int ldr(){
  int val = analogRead(sensor); 
  if(val < 3500){
    digitalWrite(fan, LOW);
    delay(500);
     bot.sendMessage(chat_id, "THE LIGHT IS NOW ON", "");
    } 
    else{
      digitalWrite(Fan, HIGH);
      delay(500);
       bot.sendMessage(chat_id, "THE LIGHT IS OFF", "");
      }
  
  }

  

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "YOU WILL RECIEVE UPDATE ON YOUR DEVICES.\n\n";
      welcome += "/fan_on to turn FAN ON \n";
      welcome += "/fan_off to turn FAN OFF \n";
     
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/fan_on") {
      bot.sendMessage(chat_id, "The fan is now ON", "");
      ledState = HIGH;
      digitalWrite(fan, ledState);
    }
    
    if (text == "/fan_off") {
      bot.sendMessage(chat_id, " Fan is OFF", "");
      ledState = LOW;
      digitalWrite(fan, ledState);
    }

    
    if (text == "/state") {
      if (digitalRead(fan)){
        bot.sendMessage(chat_id, "FAN is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "FAN is OFF", "");
      }
    }
  }
}

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(fan, OUTPUT);
  digitalWrite(fan, ledState);
  pinMode(AC, OUTPUT);
  digitalWrite(AC, ledState);
  pinMode(bulb, OUTPUT);
  digitalWrite(bulb, ledState);
  pinMode(door, OUTPUT);
  digitalWrite(door, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void Ultra() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
 if(distanceCm < 30){
    digitalWrite(fan, LOW);
    delay(500);
     bot.sendMessage(chat_id, "YOU ARE TOO CLOSE TO THE DEVICE", "");
  }
  else{
     digitalWrite(fan, HIGH);
     delay(500);
     bot.sendMessage(chat_id, "KEEP DISTANCE", "");
    }
  
 
}

void loop() {
  ldr()
   Ultra()
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
