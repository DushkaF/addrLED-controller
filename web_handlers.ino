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
      ledMode = doc["effect"];
      thishue = doc["hue"][0];
      thissat = doc["hue"][1];
      thisval = doc["hue"][2];
      change_mode(ledMode);               // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)  
      Serial.println("new led mode " + String(ledMode));
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
      Serial.println("new led count " + String(doc["led-count"]));
    }
    request->send(200);
  });
}
