// including libraries
#include <Timer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

// callback function definition
typedef void (*CallBackFunction)();

// timer
Timer t;

// pins
#define LED_PIN 2
#define ANALOG_PIN 17

// declaring functions
bool detectClap(int threshold = 300);
bool onClapDetected(CallBackFunction callBack, int threshold = 300);
int getAnalogData(int sample = 7);
void startTimer();
void stopTimer();
void ledON();
void ledOFF();
void ledToggle();
void clap();
void Tweet();
String buildTweetString();

// declaring global variables
int analog_value = 0; // analog value
bool led_on; // led light status; false:OFF, true:ON
bool timer_running = false;
int timeOutEvent;
int clapCount = 0;
int clapWaitingTime = 3000; // ms

// Tweet Settings
String API = "C5DR39LUZFQGFA1J"; // api key for tweets
const char* ssid = "AndroidAP"; // wireless modem ssid
const char* password = "djpb2458"; // wireless password
String tweet = "";
WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  ledOFF();
}

// the loop function runs over and over again forever
void loop() {
  analog_value = getAnalogData();
  if (timer_running) {
    onClapDetected(clap);
  } else if (detectClap()) {
    startTimer();
    clap();
    delay(100);
  }
  t.update();
}

void startTimer() {
  timer_running = true;
  timeOutEvent = t.every(clapWaitingTime, stopTimer);
  Serial.println("Timer has been STARTED.");
}

void stopTimer() {
  timer_running = false;
  Serial.println("Timer has been STOPPED.");
  Serial.println("A Tweet is being sent..");
  Tweet();
  t.stop(timeOutEvent);
}

bool onClapDetected(CallBackFunction callBack, int threshold) {
  bool detected = detectClap(threshold);
  if (detected) {
    callBack();
  }

  return detected;
}

bool detectClap(int threshold) {
  //analog_value = analogRead(ANALOG_PIN);
  if (analog_value >= threshold) {

    return true;
  } else {

    return false;
  }
}

int getAnalogData(int sample) {
  int a = 0, t;
  for (int i; i < sample; i++) {
    t = analogRead(ANALOG_PIN);
    if (t > a) {
      a = t;
    }
  }
  return a;
}

// led-on
void ledON() {
  led_on = true;
  digitalWrite(LED_PIN, LOW);
}

// led-off
void ledOFF() {
  led_on = false;
  digitalWrite(LED_PIN, HIGH);
}

// led toggle
void ledToggle() {
  if (led_on == true) {
    ledOFF();
  } else {
    ledON();
  }
}

void clap() {
  Serial.println("CLAP!");
  ledON();
  clapCount++;
  delay(100);
  ledOFF();
  // reinitialize timer
  t.stop(timeOutEvent);
  timeOutEvent = t.every(clapWaitingTime, stopTimer);
}

void Tweet() {
  // connect to wifi
  WiFi.begin(ssid, password);

  // allow time to make connection
  while (WiFi.status() != WL_CONNECTED)
    delay(500);

  // if connection to thingspeak.com is successful, send your tweet!
  if (client.connect("184.106.153.149", 80))
  {
    client.print("GET /apps/thingtweet/1/statuses/update?key=" + API + "&status=" + buildTweetString() + " HTTP/1.1\r\n");
    client.print("Host: api.thingspeak.com\r\n");
    client.print("Accept: */*\r\n");
    client.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n");
    client.print("\r\n");
  }
}

String buildTweetString() {
  String tweet = "";
  for (int i = 0; i < clapCount; i++) {
    tweet  += "ŞAK!";
  }
  clapCount = 0;
  return tweet;
}

