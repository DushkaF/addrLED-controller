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
//#include <Update.h>

#ifdef ESP32
//For ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
//For ESP8266
#include <ESPAsyncTCP.h>
#else
#error "Board not supported"
#endif




String APSSID = "LedControl";
//#define APPSK  "thereisnospoon"
IPAddress apIP(192, 168, 4, 1);

#define MAX_LED_COUNT 30          // число светодиодов в кольце/ленте
#define LED_DT 13             // пин, куда подключен DIN ленты

int max_bright = 50;          // максимальная яркость (0 - 255)

volatile byte ledMode = 9;


byte LED_COUNT = 16;

// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[MAX_LED_COUNT];
int ledsX[MAX_LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS  VAR
int thishue = 0;             //-FX LOOPS  VAR
int thissat = 255;           //-FX LOOPS  VAR
int thisval = 0;
float effectSpeed = 1.0;      // variable for changing effect speed

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1)
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
boolean bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

bool restartRequired = false;  // Set this flag in the callbacks to restart ESP in the main loop
boolean settingsChanged = false;


uint32_t effectTimer;        // main loop timer variable
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

DNSServer dnsServer;
const char *server_name = "led.com";  // Can be "*" to all DNS requests

void setup()
{
  Serial.begin(115200);
  Serial.println();
  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, MAX_LED_COUNT);  // set all line


  one_color_all(0, 0, 0);
  LEDS.show();
  randomSeed(analogRead(0));

  
//  setLedsSettings();
  uploadSettings();
  uploadUserPresets();

  Serial.print("Configuring access point... ");
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
    Serial.flush();
  }
  ledEffect(ledMode);

  if (settingsChanged) {
    uploadSettings();
    settingsChanged = false;
  }


  // check the flag here to determine if a restart is required
  if (restartRequired) {
    Serial.printf("Rebooting... \n\r");
    restartRequired = false;
    ESP.restart();
  }
}

// функция эффектов
void ledEffect(int ledMode) {

  // цвета мячиков для режима
  byte ballColors[3][3] = {
    {0xff, 0, 0},
    {0xff, 0xff, 0xff},
    {0   , 0   , 0xff}
  };

  //  2,4,7,15, 16, 22, 24, 25, 28, 30, 37, 40,
  //  5, 6, 9, 10, 26, 29, 33, 34, 39 44 - with color

  switch (ledMode) {
    case 999: break;                            // пазуа
    case 0: one_color_all(0, 0, 0); LEDS.show(); break;
    case  2: rainbow_fade(); break;            // плавная смена цветов всей ленты
    case  4: random_burst(); break;            // случайная смена цветов
    case  7: ems_lightsONE(); break;           // вращаются красный и синий // Color can be changed
    case 15: rwb_march(); break;               // белый синий красный бегут по кругу (ПАТРИОТИЗМ!)
    case 22: flame(); break;                   // эффект пламени
    case 25: random_color_pop(); break;        // безумие случайных вспышек
    case 26: ems_lightsSTROBE(); break;        // полицейская мигалка
    case 30: rainbowCycle(thisdelay, true); break;        // крутая плавная вращающаяся радуга
    case 37: rainbowCycle(thisdelay, false); break;                                        // очень плавная вращающаяся радуга
    case 38: TwinkleRandom(20, thisdelay, 1); break;                                // случайные разноцветные включения (1 - танцуют все, 0 - случайный 1 диод)

    // with color choising
    case  5: color_bounce(); break;            // бегающий светодиод
    case  6: color_bounceFADE(); break;        // бегающий паровозик светодиодов
    case  9: flicker(); break;                 // случайный стробоскоп
    case 10: pulse_one_color_all(); break;     // пульсация одним цветом
    case 16: radiation(); break;               // пульсирует значок радиации
    case 24: pacman(); break;                  // пакман
    case 28: kitt(); break;                    // случайные вспышки красного в вертикаьной плоскости
    case 29: matrix(); break;                  // зелёненькие бегают по кругу случайно
    case 33: colorWipe(thishue, thissat, thisval, thisdelay);
      colorWipe(0, 0, 0, thisdelay); break;                                // плавное заполнение цветом
    case 34: CylonBounce(thishue, thissat, thisval, 4, 10, thisdelay); break;                      // бегающие светодиоды
    case 39: RunningLights(thishue, thissat, thisval, thisdelay); break;                     // бегущие огни
    case 40: Sparkle(thisdelay); break;                           // случайные вспышки белого цвета
    case 44: Strobe(thishue, thissat, thisval, 10, thisdelay, 1000); break;         // стробоскоп
  }
}

//  2,4,7,15, 16, 22, 24, 25, 28, 30, 37, 40,
//  5, 6, 9, 10, 26, 29, 33, 34, 39 44 - with color

void change_mode(int newmode) {
  switch (newmode) {
    case 0: one_color_all(0, 0, 0);  LEDS.show(); break; //---ALL OFF
    case 1: one_color_all_HSV(thishue, thissat, thisval); LEDS.show(); break; //---ALL ON
    case 2: thisdelay = 60; break;                      //---STRIP RAINBOW FADE
    case 4: thisdelay = 20; thissat = 255; break;                      //---RANDOM BURST
    case 7: thisdelay = 40; thishue = 0; thissat = 255; break;         //---POLICE LIGHTS SINGLE
    case 15: thisdelay = 80; break;                     //---MARCH RWB COLORS
    case 25: thisdelay = 35; thissat = 255; break;                     //---RANDOM COLOR POP
    case 26: thisdelay = 25; thishue = 0; thissat = 255; break;        //---EMERGECNY STROBE
    case 30: thisdelay = 15; break;                      //---NEW RAINBOW LOOP
    case 37: thisdelay = 20; break;                     // rainbowCycle
    case 38: thisdelay = 10; break;                     // rainbowTwinkleSparkle

    case 5: thisdelay = 20; break;         //---CYLON v1
    case 6: thisdelay = 80; break;         //---CYLON v2
    case 9: thisdelay = 100; /*thishue = 160;  thissat = 50; */ break;         //---STRIP FLICKER
    case 10: thisdelay = 15; /* thishue = 0; */ break;        //---PULSE COLOR BRIGHTNESS
    case 16: thisdelay = 60; break;       //---RADIATION SYMBOL
    case 24: thisdelay = 50; break;                     //---PACMAN
    case 28: thisdelay = 100; break;       //---KITT
    case 29: thisdelay = 100; /* thishue = 95; */ break;       //---MATRIX RAIN
    case 33: thisdelay = 50; break;                     // colorWipe
    case 34: thisdelay = 50; break;                     // CylonBounce
    case 39: thisdelay = 50; break;                     // RunningLights
    case 40: thisdelay = 0; break;                      //
    case 44: thisdelay = 100; break;                    // Strobe
  }

  thisdelay = (int) (((float)thisdelay) / effectSpeed);

  if (ledMode != newmode) {
    one_color_all(0, 0, 0);
    effectTimer = millis();
    ledMode = newmode;
    Serial.println("-> " + String(ledMode));
  }
}

void uploadUserPresets() {
  String json;
  File file = SPIFFS.open("/user_set.json", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  while (file.available()) {
    //Lets read line by line from the file
    json += file.readStringUntil('\n');
  }
  file.close();

  Serial.println(json);
  StaticJsonDocument<384> doc;
  deserializeJson(doc, json);
  byte newLedMode = doc["turn"].as<boolean>() ? doc["effect"].as<int>() : 0;
  byte nowColorPreset = doc["now preset"].as<int>() + 1;
  thishue = (int)(doc["preset " + String(nowColorPreset)][0].as<float>() / 360.0 * 255.0);   // Convert 0-360 to 0-255
  thissat = (int)(doc["preset " + String(nowColorPreset)][1].as<float>() * 255.0) ;          // Convert 0-1 to 0-255
  thisval = doc["preset " + String(nowColorPreset)][2].as<int>();
  effectSpeed = (float)(doc["speed"].as<float>() / 100.0);
  change_mode(newLedMode);                                         // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)
}

void uploadSettings() {
  String json;
  File file = SPIFFS.open("/settings.json", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  while (file.available()) {
    //Lets read line by line from the file
    json += file.readStringUntil('\n');
  }
  file.close();

  Serial.println(json);
  StaticJsonDocument<1024> doc;
  deserializeJson(doc, json);
  LED_COUNT = doc["led-count"].as<byte>();
  APSSID = doc["ssid-name"].as<String>();

  setLedsSettings();

  // update visualise functions variables
  TOP_INDEX = int(LED_COUNT / 2);
  EVENODD = LED_COUNT % 2;
}

void setLedsSettings() {
  Serial.print("change LEDs ");
  Serial.println(LED_COUNT);
  //  leds.clearLedData();
  LEDS.clear();
  setAll(0, 0, 0, MAX_LED_COUNT);
  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость
  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  FastLED.setMaxPowerInVoltsAndMilliamps(12, 1800);
}
