/*
  Скетч создан на основе FASTSPI2 EFFECTS EXAMPLES автора teldredge (www.funkboxing.com)
  А также вот этой статьи https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#cylon
  Доработан, переведён и разбит на файлы 2017 AlexGyver
  Смена выбранных режимов в случайном порядке через случайное время
*/

#include <FastLED.h>          // библиотека для работы с лентой
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

#define APSSID "LedControl"
//#define APPSK  "thereisnospoon"

#define LED_COUNT 16          // число светодиодов в кольце/ленте
#define LED_DT 4             // пин, куда подключен DIN ленты

int max_bright= 100;          // максимальная яркость (0 - 255)
int ledMode = 3;

uint8_t delayValue = 20; // задержка
uint8_t stepValue = 10; // шаг по пикселям
uint8_t hueValue = 0; // тон цвета

uint8_t flag = 1; // флаг отмены эффекта

CRGBArray<LED_COUNT> leds;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup()
{
  Serial.begin(9600);              // открыть порт для связи
  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость

  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  updateColor(0,0,0);          // погасить все светодиоды
  LEDS.show();       
  randomSeed(analogRead(0));              // отослать команду

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(APSSID);

  handlers();
  
  // Start server
  server.begin();
}

void loop() {
  
  if (Serial.available() > 0) {
    ledMode = Serial.parseInt();
    Serial.println("-> " + String(ledMode));
  }
  ledEffect(ledMode);
}

// функция эффектов
void ledEffect(int ledMode){ 
    switch(ledMode){
      case 0: updateColor(0,0,0); break;
      case 1: rainbow_fade(); delayValue = 20; break;       
      case 2: rainbow_loop(); delayValue = 20; break;
      case 3: new_rainbow_loop(); delayValue = 5; break;
      case 4: random_march(); delayValue = 40; break;  
      case 5: rgb_propeller(); delayValue = 25; break;
      case 6: rotatingRedBlue(); delayValue = 40; hueValue = 0; break;
      case 7: Fire(55, 120, delayValue); delayValue = 15; break; 
      case 8: blueFire(55, 250, delayValue); delayValue = 15; break;  
      case 9: random_burst(); delayValue = 20; break;
      case 10: flicker(); delayValue = 20; break;
      case 11: random_color_pop(); delayValue = 35; break;                                      
      case 12: Sparkle(255, 255, 255, delayValue); delayValue = 0; break;                   
      case 13: color_bounce(); delayValue = 20; hueValue = 0; break;
      case 14: color_bounceFADE(); delayValue = 40; hueValue = 0; break;
      case 15: red_blue_bounce(); delayValue = 40; hueValue = 0; break;
      case 16: rainbow_vertical(); delayValue = 50; stepValue = 15; break;
      case 17: matrix(); delayValue = 50; hueValue = 95; break; 
  
      // тяжелые эффекты
      case 18: rwb_march(); delayValue = 80; break;                         
      case 19: flame(); break;
      case 20: theaterChase(255, 0, 0, delayValue); delayValue = 50; break;
      case 21: Strobe(255, 255, 255, 10, delayValue, 1000); delayValue = 100; break;
      case 22: policeBlinker(); delayValue = 25; break;
      case 23: kitt(); delayValue = 100; break;
      case 24: rule30(); delayValue = 100; break;
      case 25: fade_vertical(); delayValue = 60; hueValue = 180; break;
      case 26: fadeToCenter(); break;
      case 27: runnerChameleon(); break;
      case 28: blende(); break;
      case 29: blende_2();

    }
}
