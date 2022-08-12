void handlers() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/html/index.html");
  });

  server.on("/upd", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/html/update.html");
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
  server.on("/style/update.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style/update.css", "text/css");
  });

  // Route to load scripts
  server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/script.js", "text/javascript");
  });
  server.on("/js/wheel.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/wheel.js", "text/javascript");
  });
  server.on("/js/update.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/update.js", "text/javascript");
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

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    int params = request->params();
    Serial.println("IN RESPONSE");
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) { //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 501 : 200);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    delay(1000);
    restartRequired = !Update.hasError();;  // Tell the main loop to restart the ESP
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    //Upload handler chunks in data
    if (!index) { // if index == 0 then this is the first frame of data
      Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.setDebugOutput(true);
      if (filename.indexOf("ino") != -1) {
        Serial.println("CODE ");
        Update.runAsync(true);
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
          Update.printError(Serial);
        }
      }
      else if (filename.indexOf("spiffs") != -1) {
        Serial.println("SPIFFS");
        Update.runAsync(true);
        if (!Update.begin(0xFFFFFFFF, U_FS)) {
          Update.printError(Serial);
        }
      }
    }

    //    Write chunked data to the free sketch space
    if (!Update.hasError()) {
      if (Update.write(data, len) != len) {
        Update.printError(Serial);
      }
    }

    if (final) { // if the final flag is set then this is the last frame of data
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update %s Success: %u B\nRebooting...\n", filename.c_str(), index + len);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
      Serial.printf("Update Success: %u B\nEnd\n", index + len);
    }
  });

}
