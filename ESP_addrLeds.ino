/*
  Скетч создан на основе FASTSPI2 EFFECTS EXAMPLES автора teldredge (www.funkboxing.com)
  А также вот этой статьи https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#cylon
  Доработан, переведён и разбит на файлы 2017 AlexGyver
  Смена выбранных режимов в случайном порядке через случайное время
*/

#include <FastLED.h>          // библиотека для работы с лентой
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>

#ifdef ESP32
//For ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
//For ESP8266
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#else
#error "Board not supported"
#endif




#define APSSID "LedControl"
//#define APPSK  "thereisnospoon"
IPAddress apIP(192, 168, 4, 1);

#define LED_COUNT 16          // число светодиодов в кольце/ленте
#define LED_DT 2             // пин, куда подключен DIN ленты

int max_bright = 100;          // максимальная яркость (0 - 255)

volatile byte ledMode = 3;


// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS  VAR
int thishue = 0;             //-FX LOOPS  VAR
int thissat = 255;           //-FX LOOPS  VAR
int thisval = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1)
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
boolean bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

volatile uint32_t btnTimer;
volatile byte modeCounter;
volatile boolean changeFlag;
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

DNSServer dnsServer;
const char *server_name = "led.com";  // Can be "*" to all DNS requests

void setup()
{
  Serial.begin(115200);              // открыть порт для связи
  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость

  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  one_color_all(0, 0, 0);          // погасить все светодиоды
  LEDS.show();
  randomSeed(analogRead(0));              // отослать команду

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.print("Configuring access point... ");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APSSID);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  const byte DNS_PORT = 53;
  dnsServer.start(DNS_PORT, server_name, apIP);
  Serial.println("DNS started");

  handlers();

  // Start server
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  if (Serial.available() > 0) {
    change_mode(Serial.parseInt());               // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)
    Serial.println("-> " + String(ledMode));
    Serial.flush();
  }
  ledEffect(ledMode);
}

// функция эффектов
void ledEffect(int ledMode) {

  // цвета мячиков для режима
  byte ballColors[3][3] = {
    {0xff, 0, 0},
    {0xff, 0xff, 0xff},
    {0   , 0   , 0xff}
  };

  switch (ledMode) {
    case 999: break;                            // пазуа
    case  2: color_bounce(); break;             // бегающий светодиод
    case 3: rwb_march(); break;                 // белый синий красный бегут по кругу (ПАТРИОТИЗМ!)
    case 4: ems_lightsSTROBE(); break;          // полицейская мигалка
    case 5: rainbowCycle(thisdelay); break;                                        // очень плавная вращающаяся радуга
    case 6: Strobe(0xff, 0xff, 0xff, 10, thisdelay, 1000); break;                  // стробоскоп

    case 45: BouncingBalls(0xff, 0, 0, 3); break;                                   // прыгающие мячики
    case 46: BouncingColoredBalls(3, ballColors); break;                            // прыгающие мячики цветные
  }
}

void change_mode(int newmode) {
  switch (newmode) {
    case 0: one_color_all(0, 0, 0); LEDS.show(); break; //---ALL OFF
    case 1: one_color_all_HSV(thishue, thissat, thisval); LEDS.show(); break; //---ALL ON
    case 2: thisdelay = 20; thissat = 255; thishue = 0; break;        //---CYLON v1
    case 3: thisdelay = 80; thissat = 255; break;                     //---MARCH RWB COLORS
    case 4: thisdelay = 25; thissat = 255; thishue = 0; break;        //---EMERGECNY STROBE
    case 5: thisdelay = 20; thissat = 255; break;                     // rainbowCycle
    case 6: thisdelay = 100; thissat = 255; break;                    // Strobe
  }
  bouncedirection = 0;
  one_color_all(0, 0, 0);
  ledMode = newmode;
}
