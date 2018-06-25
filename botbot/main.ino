#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <Adafruit_NeoPixel.h>

// Update these with values suitable for your network.

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PSK";
const char* mqtt_server = "io.adafruit.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define PIN 12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(50, PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  strip.begin();
  rainbowCycle(50);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String red = "Red: ";
String green = "Green: ";
String blue = "Blue: ";

int redColor, greenColor, blueColor;

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

#define MAX_WORD_COUNT 3
#define MIN_WORD_COUNT 3
char *Words[MAX_WORD_COUNT];

byte split_message(char* str) {
  byte word_count = 0; //number of words
  char * item = strtok (str, ","); //getting first word (uses space & comma as delimeter)

  while (item != NULL) {
    if (word_count >= MAX_WORD_COUNT) {
      break;
    }
    Words[word_count] = item;
    item = strtok (NULL, ","); //getting subsequence word
    word_count++;
  }
  return  word_count;
}

char payloadString[30];

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void rainbowCycle(uint8_t wait) {
  uint16_t i;

  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i * 256 / strip.numPixels()) & 255));
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
int messageLength, payloadlength;
void callback(char* topic, byte* payload, unsigned int length) {
  sprintf(payloadString, "%s", payload);
  messageLength = sizeof(payloadString);
  payloadString[messageLength] = '\0';
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(payloadString);

  if(strstr(payloadString, "rainbow")){
    Serial.println("Rainbow!!!");
    rainbowCycle(50);
  } else {
    split_message(payloadString);

    redColor = atoi(Words[0]);
    greenColor = atoi(Words[1]);
    blueColor = atoi(Words[2]);

    Serial.println(red + redColor);
    Serial.println(green + greenColor);
    Serial.println(blue + blueColor);
    colorWipe(strip.Color(redColor, greenColor, blueColor), 50);
  }
  for(uint8_t i = 0; i < length; i++) {
    payload[i] = 0x00;
  }
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", "YOUR_AIO_USERNAME", "YOUR_AIO_KEY")) {
      Serial.println("connected");
      client.subscribe("YOUR_AIO_FEED");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
