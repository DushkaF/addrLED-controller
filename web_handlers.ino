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

  // Route to additional content
  server.on("/effects_set.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/effects_set.json", "application/json");
  });

  server.on("/user_set.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/user_set.json", "application/json");
  });

  // Route to functional content
  server.on("/selectedEffects", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->params() > 0) {
      AsyncWebParameter* p = request->getParam(0);
      String json = p->value();
      Serial.println(json);
      StaticJsonDocument<256> doc;
      deserializeJson(doc, json);
      byte newLedMode = doc["effect"].as<int>();
      thishue = (int)(doc["hsv"][0].as<float>() / 360.0 * 255.0);   // Convert 0-360 to 0-255
      thissat = (int)(doc["hsv"][1].as<float>() * 255.0) ;          // Convert 0-1 to 0-255
      thisval = doc["hsv"][2].as<int>();
      effectSpeed = (float)(doc["speed"].as<float>() / 100.0);
      change_mode(newLedMode);                                         // меняем режим через change_mode (там для каждого режима стоят цвета и задержки)
      Serial.println("new led mode " + String(ledMode));
      Serial.println("H \t S \t V \tSpeed");
      Serial.println(String(thishue) + "\t" + String(thissat) + "\t" + String(thisval) + "\t" + String(effectSpeed));
    }
    request->send(200);
  });

  server.on("/update_user_set", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->params() > 0) {
      AsyncWebParameter* p = request->getParam(0);
      String json = p->value();
      Serial.println(json);
      StaticJsonDocument<384> doc;
      deserializeJson(doc, json);
      File f = SPIFFS.open("/user_set.json", "w");
      if (!f) {
        Serial.println("file open failed");
      }
      else
      {
        //Write data to file
        Serial.println("Writing Data to File");
        f.print(json);
        f.close();  //Close file
      }
    }
    request->send(200);
  });

}
