unsigned long change_time, last_change, last_bright;
int new_bright;

int effectJ;
boolean effectState, effectFrameState;


void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i].setRGB( cred, cgrn, cblu);
  }
}

void one_color_all_HSV(byte hue, byte sat, byte val) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i] = CHSV(hue, sat, val);
  }
}


//------------------------LED EFFECT FUNCTIONS------------------------

void rainbow_fade() {                         //-m2-FADE ALL LEDS THROUGH HSV RAINBOW
  byte rainbow_sat = 255;
  ihue++;
  if (ihue > 255) {
    ihue = 0;
  }
  for (int idex = 0 ; idex < LED_COUNT; idex++ ) {
    leds[idex] = CHSV(ihue, rainbow_sat, thisval);
  }
  LEDS.show();
  delay(thisdelay);
}

void random_burst() {                         //-m4-RANDOM INDEX/COLOR
  idex = random(0, LED_COUNT);
  ihue = random(0, 255);
  leds[idex] = CHSV(ihue, thissat, thisval);
  LEDS.show();
  delay(thisdelay);
}

void ems_lightsONE() {                    //-m7-EMERGENCY LIGHTS (TWO COLOR SINGLE LED)
  idex++;
  if (idex >= LED_COUNT) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  int thathue = (thishue + 160) % 255;
  for (int i = 0; i < LED_COUNT; i++ ) {
    if (i == idexR) {
      leds[i] = CHSV(thishue, thissat, thisval);
    }
    else if (i == idexB) {
      leds[i] = CHSV(thathue, thissat, thisval);
    }
    else {
      leds[i] = CHSV(0, 0, 0);
    }
  }
  LEDS.show();
  delay(thisdelay);
}

void radiation() {                   //-m16-SORT OF RADIATION SYMBOLISH-
  int N3  = int(LED_COUNT / 3);
  int N6  = int(LED_COUNT / 6);
  int N12 = int(LED_COUNT / 12);
  for (int i = 0; i < N6; i++ ) {    //-HACKY, I KNOW...
    tcount = tcount + .02;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount) * 255);
    int j0 = (i + LED_COUNT - N12) % LED_COUNT;
    int j1 = (j0 + N3) % LED_COUNT;
    int j2 = (j1 + N3) % LED_COUNT;
    leds[j0] = CHSV(thishue, thissat, ibright);
    leds[j1] = CHSV(thishue, thissat, ibright);
    leds[j2] = CHSV(thishue, thissat, ibright);
  }
  LEDS.show();
  delay(thisdelay);
}

void flame() {                                    //-m22-FLAMEISH EFFECT
  byte flame_sat = 255;
  int idelay = random(0, 35);
  float hmin = 0.1;
  float hmax = 45.0;
  float hdif = hmax - hmin;
  int randtemp = random(0, 3);
  float hinc = (hdif / float(TOP_INDEX)) + randtemp;
  int ihue = hmin;
  for (int i = 0; i <= TOP_INDEX; i++ ) {
    ihue = ihue + hinc;
    leds[i] = CHSV(ihue, flame_sat, thisval);
    int ih = horizontal_index(i);
    leds[ih] = CHSV(ihue, flame_sat, thisval);
    leds[TOP_INDEX].r = 255;
    leds[TOP_INDEX].g = 255;
    leds[TOP_INDEX].b = 255;
    LEDS.show();
    delay(idelay);
  }
}

void pacman() {                                  //-m24-REALLY TERRIBLE PACMAN CHOMPING EFFECT
  int s = int(LED_COUNT / 4);
  lcount++;
  if (lcount > 5) {
    lcount = 0;
  }
  if (lcount == 0) {
    for (int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, thisval);
    }
  }
  if (lcount == 1 || lcount == 5) {
    for (int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, thisval);
    }
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
  }
  if (lcount == 2 || lcount == 4) {
    for (int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, thisval);
    }
    leds[s - 1].r = 0; leds[s - 1].g = 0; leds[s - 1].b = 0;
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
    leds[s + 1].r = 0; leds[s + 1].g = 0; leds[s + 1].b = 0;
  }
  if (lcount == 3) {
    for (int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, thisval);
    }
    leds[s - 2].r = 0; leds[s - 2].g = 0; leds[s - 2].b = 0;
    leds[s - 1].r = 0; leds[s - 1].g = 0; leds[s - 1].b = 0;
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
    leds[s + 1].r = 0; leds[s + 1].g = 0; leds[s + 1].b = 0;
    leds[s + 2].r = 0; leds[s + 2].g = 0; leds[s + 2].b = 0;
  }
  LEDS.show();
  delay(thisdelay);
}

void random_color_pop() {                         //-m25-RANDOM COLOR POP
  idex = random(0, LED_COUNT);
  ihue = random(0, 255);
  one_color_all(0, 0, 0);
  leds[idex] = CHSV(ihue, thissat, thisval);
  LEDS.show();
  delay(thisdelay);
}

void kitt() {                                     //-m28-KNIGHT INDUSTIES 2000
  int rand = random(0, TOP_INDEX);
  for (int i = 0; i < rand; i++ ) {
    leds[TOP_INDEX + i] = CHSV(thishue, thissat, thisval);
    leds[TOP_INDEX - i] = CHSV(thishue, thissat, thisval);
    LEDS.show();
    delay(thisdelay / rand);
  }
  for (int i = rand; i > 0; i-- ) {
    leds[TOP_INDEX + i] = CHSV(thishue, thissat, 0);
    leds[TOP_INDEX - i] = CHSV(thishue, thissat, 0);
    LEDS.show();
    delay(thisdelay / rand);
  }
}

void new_rainbow_loop() {                      //-m88-RAINBOW FADE FROM FAST_SPI2
  ihue -= 1;
  fill_rainbow( leds, LED_COUNT, ihue );
  LEDS.show();
  delay(thisdelay);
}

//-------------------------------TwinkleRandom---------------------------------------
void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  if (!effectFrameState) {
    if (effectJ < Count) {
      if (effectTimer <= millis()) {
        if (OnlyOne) {
          setAll(0, 0, 0);
        }
        setPixel(random(LED_COUNT), random(0, thisval), random(0, thisval), random(0, thisval));
        FastLED.show();
        effectJ++;
        effectTimer = millis() + SpeedDelay;
      } else {
      }
    } else {
      effectFrameState = true;
      effectTimer = millis() + SpeedDelay;
      if (OnlyOne) {
        setAll(0, 0, 0);
      }
    }
  } else {
    if (effectTimer <= millis()) {
      setAll(0, 0, 0);
      effectFrameState = false;
      effectJ = 0;
    }
  }
}

void Sparkle(int SpeedDelay) {
  int Pixel = random(LED_COUNT);
  leds[Pixel] = CHSV(thishue, thissat, thisval);
  FastLED.show();
  delay(SpeedDelay);
  setPixel(Pixel, 0, 0, 0);
}

void color_bounceFADE() {                    //-m6-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2);
  for (int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {
      leds[i] = CHSV(thishue, thissat, 255);
    }
    else if (i == iL1) {
      leds[i] = CHSV(thishue, thissat, 150);
    }
    else if (i == iL2) {
      leds[i] = CHSV(thishue, thissat, 80);
    }
    else if (i == iL3) {
      leds[i] = CHSV(thishue, thissat, 20);
    }
    else if (i == iR1) {
      leds[i] = CHSV(thishue, thissat, 150);
    }
    else if (i == iR2) {
      leds[i] = CHSV(thishue, thissat, 80);
    }
    else if (i == iR3) {
      leds[i] = CHSV(thishue, thissat, 20);
    }
    else {
      leds[i] = CHSV(0, 0, 0);
    }
  }
  LEDS.show();
  delay(thisdelay);
}

void flicker() {                          //-m9-FLICKER EFFECT
  int random_bright = random(0, thisval);
  int random_delay = random(10, thisdelay);
  int random_bool = random(0, random_bright);
  if (random_bool < 10) {
    for (int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, random_bright);
    }
    LEDS.show();
    delay(random_delay);
  }
}

void pulse_one_color_all() {              //-m10-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR
  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {
      bouncedirection = 0;
    }
  }
  for (int idex = 0 ; idex < LED_COUNT; idex++) {
    leds[idex] = CHSV(thishue, thissat, ibright);
  }
  LEDS.show();
  delay(thisdelay);
}

void matrix() {                                   //-m29-ONE LINE MATRIX
  int rand = random(0, 100);
  if (rand > 90) {
    leds[0] = CHSV(thishue, thissat, thisval);
  }
  else {
    leds[0] = CHSV(thishue, thissat, 0);
  }
  copy_led_array();
  for (int i = 1; i < LED_COUNT; i++ ) {
    leds[i].r = ledsX[i - 1][0];
    leds[i].g = ledsX[i - 1][1];
    leds[i].b = ledsX[i - 1][2];
  }
  LEDS.show();
  delay(thisdelay);
}

void colorWipe(byte hue, byte sat, byte val, int SpeedDelay) {
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    leds[i] = CHSV(hue, sat, val);
    FastLED.show();
    delay(SpeedDelay);
  }
}

void CylonBounce(byte hue, byte sat, byte val, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < LED_COUNT - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    leds[i] = CHSV(hue, sat, val / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[i + j] = CHSV(hue, sat, val);
    }
    leds[i + EyeSize + 1] = CHSV(hue, sat, val / 10);
    FastLED.show();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = LED_COUNT - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    leds[i] = CHSV(hue, sat, val / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[i + j] = CHSV(hue, sat, val);
    }
    leds[i + EyeSize + 1] = CHSV(hue, sat, val / 10);
    FastLED.show();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}

void RunningLights(byte hue, byte sat, byte val, int WaveDelay) {
  int Position = 0;

  for (int i = 0; i < LED_COUNT * 2; i++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < LED_COUNT; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      leds[i] = CHSV(hue, sat, ((sin(i + Position) * 127 + 128) / 255) * val);
    }

    FastLED.show();
    delay(WaveDelay);
  }
}

// ------------------------------------ Rewtited --------------------------

void color_bounce() {                        //-m5-BOUNCE COLOR (SINGLE LED)
  if (effectTimer <= millis()) {
    if (!bouncedirection) {
      idex = idex + 1;
      if (idex == LED_COUNT) {
        bouncedirection = true;
        idex = idex - 1;
      }
    } else {
      idex = idex - 1;
      if (idex == 0) {
        bouncedirection = false;
      }
    }

    for (int i = 0; i < LED_COUNT; i++ ) {
      if (i == idex) {
        leds[i] = CHSV(thishue, thissat, thisval);
      }
      else {
        leds[i] = CHSV(0, 0, 0);
      }
    }
    LEDS.show();
    effectTimer = millis() + thisdelay;
  }
}

void rwb_march() {                    //-m15-R,W,B MARCH CCW
  if (effectTimer <=  millis()) {
    copy_led_array();
    int iCCW;
    idex++;
    if (idex > 2) {
      idex = 0;
    }
    switch (idex) {
      case 0:
        leds[0].r = thisval;
        leds[0].g = 0;
        leds[0].b = 0;
        break;
      case 1:
        leds[0].r = thisval;
        leds[0].g = thisval;
        leds[0].b = thisval;
        break;
      case 2:
        leds[0].r = 0;
        leds[0].g = 0;
        leds[0].b = thisval;
        break;
    }
    for (int i = 1; i < LED_COUNT; i++ ) {
      iCCW = adjacent_ccw(i);
      leds[i].r = ledsX[iCCW][0];
      leds[i].g = ledsX[iCCW][1];
      leds[i].b = ledsX[iCCW][2];
    }
    LEDS.show();
    effectTimer = millis() + thisdelay;
  }
  //  delay(thisdelay);
}

void ems_lightsSTROBE() {                  //-m26-EMERGENCY LIGHTS (STROBE LEFT/RIGHT)
  int thishue = 0;
  int thathue = (thishue + 160) % 255;
  if (!effectFrameState) {
    if (effectJ < 5) {
      if (effectTimer <= millis()) {
        if (!effectState) {
          for (int i = 0 ; i < TOP_INDEX; i++ ) {
            leds[i] = CHSV(thishue, thissat, thisval);
          }
        } else {
          setAll(0, 0, 0);
          effectJ++;
        }
        FastLED.show();
        effectState = !effectState;
        effectTimer = millis() + thisdelay;
      }
    } else {
      effectFrameState = true;
      effectTimer = millis() + thisdelay;
      effectJ = 0;
    }
  } else {
    if (effectJ < 5) {
      if (effectTimer <= millis()) {
        if (!effectState) {
          for (int i = TOP_INDEX ; i < LED_COUNT; i++ ) {
            leds[i] = CHSV(thathue, thissat, thisval);
          }
        } else {
          setAll(0, 0, 0);
          effectJ++;
        }
        FastLED.show();
        effectState = !effectState;
        effectTimer = millis() + thisdelay;
      }
    } else {
      effectFrameState = false;
      effectTimer = millis() + thisdelay;
      effectJ = 0;
    }
  }
}


//-------------------------------newKITT---------------------------------------
void rainbowCycle(int SpeedDelay) { //todo beauty code style
  static uint16_t rCycleJ;

  byte *c;
  uint16_t i;

  if (rCycleJ < 256 * 5) { // 5 cycles of all colors on wheel
    if (effectTimer <= millis()) {
      for (i = 0; i < LED_COUNT; i++) {
        c = Wheel(((i * 256 / LED_COUNT) + rCycleJ) & 255);
        setPixel(i, *c, *(c + 1), *(c + 2));
      }
      FastLED.show();
      rCycleJ++;
      effectTimer = millis() + SpeedDelay;
      //    delay(SpeedDelay);
    }
  }
  else {
    rCycleJ = 0;
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

//-------------------------------Strobe---------------------------------------
void Strobe(byte hue, byte sat, byte val, int StrobeCount, int FlashDelay, int EndPause) {
  if (!effectFrameState) {
    if (effectJ < StrobeCount) {
      if (effectTimer <= millis()) {
        if (!effectState) {
          one_color_all_HSV(hue, sat, val);
          //      delay(FlashDelay);
        } else {
          setAll(0, 0, 0);
          //      delay(FlashDelay);
          effectJ++;
        }
        FastLED.show();
        effectState = !effectState;

        effectTimer = millis() + FlashDelay;
      }
    } else {
      effectFrameState = true;
      effectTimer = millis() + EndPause;
    }
  } else {
    if (effectTimer <= millis()) {
      effectFrameState = false;
      effectJ = 0;
    }
  }
  //  delay(EndPause);
}
