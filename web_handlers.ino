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
      Serial.println("new led mode " + String(ledMode));
    }
    request->send(200);
  });
}
