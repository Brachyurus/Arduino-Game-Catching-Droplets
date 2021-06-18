//better random number generator version
//now with millis
//2021.06.18
#include <Adafruit_PCD8544.h>
#include <avr/power.h>
#include <avr/sleep.h>

// Adafruit_PCD8544(CLK,DIN,D/C,CE,RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 0, 1, 4);       //  for a finished build
//Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 10, 11, 4);     //   for a prototype with serial capabilities

const unsigned char heart[] PROGMEM = {
  B01101100,
  B11111110,
  B11111110,
  B11111110,
  B01111100,
  B00111000,
  B00010000
};

const unsigned char heartOutline[] PROGMEM = {
  B01101100,
  B10010010,
  B10000010,
  B10000010,
  B01000100,
  B00101000,
  B00010000
};

const unsigned char drop[] PROGMEM = {
  B00100000,
  B01010000,
  B10001000,
  B10001000,
  B10001000,
  B01110000
};

const unsigned char dropStart[] PROGMEM = {
  B10000010,
  B01000100,
  B01000100,
  B00101000,
  B00101000,
  B00010000
};

const unsigned char umbrella[] PROGMEM = {
  B00000100, B00000000,
  B00011111, B00000000,
  B01101000, B11000000,
  B10010010, B00100000,
};

const unsigned char lvl[] PROGMEM = {
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B01111000,
  B00000000,
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B00100000,
  B00000000,
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B01111000,
};

uint8_t button[2] = {3, 2};
uint8_t buttonCount = sizeof(button);
bool lastState[2];
uint8_t bounceDelay = 10;

uint8_t buzzer = 5;
uint8_t division = 2;
uint8_t cratePos = 1;
uint8_t cratePosCount = 4;

uint8_t circlePosCount = 5;
uint8_t circleStep = 5;
uint8_t circlePos[6] = {6, 6, 6, 6, 6, 6};

uint8_t trailCount = 4;
uint8_t trailStep = 19;
uint8_t failCount = 0;

uint16_t y = 500;
uint16_t cycleTime = y;
uint16_t score = 0;
uint8_t lives = 3;
bool scoreTaken;
bool liveLost;
uint8_t level = 1;

uint32_t prevFrame = 0;
uint32_t prevCheck = 0;
uint32_t prevLvlTime = 0;
uint16_t lvlChangeTime = 22000;

void setup() {
  clock_prescale_set (clock_div_2);
  display.begin();
  randomSeed(analogRead(A0));
  for (byte i = 0; i < buttonCount; i++) {
    pinMode(button[i], INPUT_PULLUP);
  }
  welcomeScreen();
}

void loop() {
  uint32_t currentMillis = millis();
  if (currentMillis - prevLvlTime > lvlChangeTime / division) {
    prevLvlTime = currentMillis;
    cycleTime = cycleTime * 0.95;
    level++;
    updateLevel();
  }
  if (currentMillis - prevFrame > cycleTime / division) {
    prevFrame = currentMillis;
    if (liveLost) {
      liveLost = false;
      holdScreen();
      for (byte i = 0; i < trailCount; i++) {
        circlePos[i] = 5;
      }
    }
    for (byte i = 0; i < trailCount; i++) {
      if (circlePos[i] <= circlePosCount) {
        circlePos[i]++;
      }
      if (circlePos[i] <= circlePosCount) {
        display.fillRect(6 + i * trailStep, 16, 5, 26, WHITE);
        display.drawBitmap(6 + i * trailStep, circlePos[i] * circleStep + 11, drop, 8, 6, BLACK);
      } else {
        display.fillRect(6 + i * trailStep, 16, 5, 26, WHITE);
      }
    }
    if (circlePos[0] < 5 || circlePos[1] < 5 || circlePos[2] < 5 || circlePos[3] < 5 || circlePos[4] < 5 || circlePos[5] < 5) {
      tone(buzzer, 880, 50 / division);
    }
    generateRandomNum();
    scoreTaken = false;
  }
  if (currentMillis - prevCheck > bounceDelay / division) {
    prevCheck = currentMillis;
    checkInputs();
    for (byte i = 0; i < trailCount; i++) {
      if (!scoreTaken) {
        if (circlePos[i] == circlePosCount) {
          liveLost = true;
          if (cratePos == i + 1) {
            score++;
            tone(buzzer, 1760, 50 / division);
            scoreTaken = true;
            liveLost = false;
            display.fillRect(6 + i * trailStep, 16, 5, 26, WHITE);
            updateScore();
          }
        }
      }
      display.display();
    }
  }
}

void checkInputs() {
  for (byte i = 0; i < buttonCount; i++) {
    bool state = digitalRead(button[i]);
    if (state != lastState[i]) {
      lastState[i] = state;
      if (state == LOW) {
        if (i == 0 && cratePos > 1) {
          cratePos--;
          updateCratePos();
        } else if (i == 1 && cratePos < cratePosCount) {
          cratePos++;
          updateCratePos();
        }
      }
    }
  }
}

void updateCratePos() {
  display.fillRect(3, 42, 68, 4, WHITE);
  if (cratePos == 1) {
    display.drawBitmap(3, 42, umbrella, 11, 4, BLACK);
  } else if (cratePos == 2) {
    display.drawBitmap(22, 42, umbrella, 11, 4, BLACK);
  } else if (cratePos == 3) {
    display.drawBitmap(41, 42, umbrella, 11, 4, BLACK);
  } else if (cratePos == 4) {
    display.drawBitmap(60, 42, umbrella, 11, 4, BLACK);
  }
}

void updateScore() {
  display.fillRect(0, 0, 38, 7, WHITE);
  display.setCursor(3, 0);
  if (score < 10) {
    display.print("   ");
    display.print(score);
  } else if (score < 100) {
    display.print("  ");
    display.print(score);
  } else if (score < 1000) {
    display.print(" ");
    display.print(score);
  } else {
    display.print(score);
  }
}

void updateLives() {
  display.fillRect(48, 0, 24, 7, WHITE);
  for (byte i = 0; i < lives; i++) {
    display.drawBitmap(48 + i * 8, 0, heart, 8, 7, BLACK);
  }
  for (byte i = 0; i < 3; i++) {
    display.drawBitmap(48 + i * 8, 0, heartOutline, 8, 7, BLACK);
  }
  display.display();
}

void drawOutlines() {
  display.drawRect(0, 9, 74, 39, BLACK);
  for (byte i = 0; i < trailCount; i++) {
    display.drawBitmap(5 + i * trailStep, 10, dropStart, 8, 6, BLACK);
  }
}

void updateLevel() {
  display.drawBitmap(77, 9, lvl, 5, 18, BLACK);
  display.fillRect(77, 30, 5, 16, WHITE);
  if (level < 10) {
    display.setCursor(77, 30);
    display.print("0");
    display.setCursor(77, 39);
    display.print(level);
  } else {
    display.setCursor(77, 30);
    display.print(level / 10);
    display.setCursor(77, 39);
    display.print(level % 10);
  }
}

void generateRandomNum() {
  uint8_t randomNum = random(0, 4);
  if (circlePos[randomNum] > 5) {
    circlePos[randomNum] = 0;
    failCount = 0;
  } else {
    if (failCount < 3) {
      failCount++;
    } else if (failCount > 100) {
      if (circlePos[0] > 5) {
        circlePos[0] = 0;
        failCount = 0;
      } else if (circlePos[1] > 5) {
        circlePos[1] = 0;
        failCount = 0;
      }
    } else {
      failCount++;
      generateRandomNum();
    }
  }
}

void holdScreen() {
  int blinkPeriod = 300 / division;
  tone(buzzer, 660);
  delay(100 / division);
  tone(buzzer, 440, 100 / division);
  for (int i = 0; i < 3; i++) {
    lives--;
    updateLives();
    delay(blinkPeriod);
    lives++;
    updateLives();
    delay(blinkPeriod);
  }
  lives--;
  updateLives();
  if (lives < 1) {
    gameOverScreen();
  }
  uint32_t  currentMillis = millis();
  prevFrame = currentMillis;
  prevCheck = currentMillis;
  prevLvlTime = currentMillis;
}

void welcomeScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(13, 0);
  display.print(F("Rainy"));
  display.setCursor(25, 16);
  display.print(F("Day"));
  display.display();
  delay(1500 / division);
  display.setCursor(3, 32);
  display.setTextSize(1);
  display.print(F("Press any key"));
  display.setCursor(18, 41);
  display.print(F("to start"));
  display.display();
  goToSleep();
  newGame();
}

void gameOverScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(18, 0);
  display.print(F("Game"));
  display.setCursor(18, 16);
  display.print(F("Over"));
  display.setCursor(10, 31);
  display.setTextSize(1);
  display.print(F("Your Score:"));
  display.setCursor(10, 40);
  display.print(score);
  display.display();
  tone(buzzer, 587);
  delay(200 / division);
  tone(buzzer, 733);
  delay(200 / division);
  tone(buzzer, 880);
  delay(200 / division);
  tone(buzzer, 1173);
  delay(400 / division);
  noTone(buzzer);
  delay(200 / division);
  goToSleep();
  newGame();
}

void newGame() {
  lives = 3;
  score = 0;
  cratePos = 1;
  cycleTime = y;
  level = 1;
  display.clearDisplay();
  updateCratePos();
  updateScore();
  drawOutlines();
  updateLevel();
  updateLives();
  uint32_t currentMillis = millis();
  prevFrame = currentMillis;
  prevCheck = currentMillis;
  prevLvlTime = currentMillis;
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);
  attachInterrupt(digitalPinToInterrupt(3), wakeUp, LOW);
  sleep_cpu();
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
  detachInterrupt(digitalPinToInterrupt(3));
}

void wakeUp() {}
