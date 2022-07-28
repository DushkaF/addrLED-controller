void handlers() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/html/index.html");
  });

  // Route to load styles file
  server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/style.css", "text/css");
  });
  server.on("/style/wheel.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/wheel.css", "text/css");
  });
  server.on("/style/menu.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/menu.css", "text/css");
  });

  // Route to load scripts
  server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/script.js", "text/javascript");
  });
  server.on("/js/wheel.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/wheel.js", "text/javascript");
  });

  // Route additional content
  server.on("/effects_set.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/effects_set.json", "application/json");
  });

  server.on("/selectedEffects", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->params() > 0) {
      AsyncWebParameter* p = request->getParam(0);
      String json = p->value();
      Serial.println(json);
      StaticJsonDocument<256> doc;
      deserializeJson(doc, json);
      byte newLedMode = doc["effect"].as<int>();
      thishue = (int)(doc["hue"][0].as<float>() / 360.0 * 255.0);   // Convert 0-360 to 0-255 
      thissat = (int)(doc["hue"][1].as<float>() * 255.0) ;          // Convert 0-1 to 0-255 
      thisval = doc["hue"][2].as<int>();
      change_mode(newLedMode);               // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)  
      Serial.println("new led mode " + String(ledMode));
      Serial.println("H \t S \t V");
      Serial.println(String(thishue) + "\t" + String(thissat) + "\t" + String(thisval));
    }
    request->send(200);
  });

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->params() > 0) {
      AsyncWebParameter* p = request->getParam(0);
      String json = p->value();
      Serial.println(json);
      StaticJsonDocument<256> doc;
      deserializeJson(doc, json);
//      LED_COUNT = doc["led-count"];
      Serial.println("new led count " + String(doc["led-count"].as<int>()));
    }
    request->send(200);
  });
}
