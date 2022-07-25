void handlers() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/html/index.html");
  });

  // Route to load styles file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/style.css", "text/css");
  });
  server.on("/wheel.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/wheel.css", "text/css");
  });
  server.on("/menu.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/menu.css", "text/css");
  });

  // Route to load scripts
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/script.css", "text/javascript");
  });
  server.on("/wheel.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/wheel.js", "text/javascript");
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
