//2021.06.07
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <avr/power.h>
#include <avr/sleep.h>

// Adafruit_PCD8544(CLK,DIN,D/C,CE,RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 0, 1, 4);

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

const unsigned char drop[] PROGMEM =  {
  B00100000,
  B01010000,
  B10001000,
  B10001000,
  B10001000,
  B01110000
};

const unsigned char dropStart[] PROGMEM = {
  B10001000,
  B10001000,
  B01010000,
  B01010000,
  B00100000
};

const unsigned char umbrella[] PROGMEM = {
  B00000100, B00000000,
  B00011111, B00000000,
  B01101000, B11000000,
  B10010010, B00100000,
};

const unsigned char lvl[]  PROGMEM  = {
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

byte button[] = {3, 2};
int buttonCount = sizeof(button);
bool lastState[2];
int bounceDelay = 6;
int buzzer = 5;
int division = 2;

int cratePos = 1;
int cratePosCount = 4;

int circlePosCount = 5;
int x = circlePosCount + 1;
int circleStep = 5;
int circlePos[6] = {x, x, x, x, x, x};

int trailCount = 4;
int trailStep = 19;
int randomNumber[6];
int number;

int y = 480;
int cycleTime = y;
int score = 0;
int lives = 3;
bool scoreTaken;
bool liveLost;

int cycleCount = 0;
int z = 50;
int maxCycleCount = z;
int level = 1;
int failCount = 0;


void setup() {
  display.begin();
  display.clearDisplay();
  display.display();

  clock_prescale_set (clock_div_2);

  for (int i = 0; i < buttonCount; i++) {
    pinMode(button[i], INPUT_PULLUP);
  }
  welcomeScreen();
}


void loop() {
  if (cycleCount >= maxCycleCount) {
    cycleCount = 0;
    cycleTime = cycleTime * 0.95;
    maxCycleCount = maxCycleCount * 1.05;
    level++;
    updateLevel();
  } else {
    cycleCount++;
  }
  for (int i = 0; i < trailCount; i++) {
    if (circlePos[i] < circlePosCount + 1) {
      circlePos[i]++;
    }
    if (circlePos[i] <= circlePosCount) {
      display.fillRect(5 + i * trailStep, 15, 5, 26, WHITE);
      display.drawBitmap(5 + i * trailStep, circlePos[i] * circleStep + 10, drop, 8, 6, BLACK);
      display.display();
    } else {
      display.fillRect(5 + i * trailStep, 15, 5, 26, WHITE);
      display.display();
    }
  }
  if (circlePos[0] < 5 || circlePos[1] < 5 || circlePos[2] < 5 || circlePos[3] < 5 || circlePos[4] < 5 || circlePos[5] < 5) {
    tone(buzzer, 880, 50 / division);
  }
  for (int j = 0; j < cycleTime / trailCount / bounceDelay / division; j++) {
    for (int i = 0; i < trailCount; i++) {
      checkInputs();
      if (!scoreTaken) {
        if (circlePos[i] == circlePosCount) {
          liveLost = true;
          if (cratePos == i + 1) {
            score++;
            tone(buzzer, 1760, 50 / division);
            scoreTaken = true;
            liveLost = false;
            display.fillRect(5 + i * trailStep, 15, 5, 26, WHITE);
            display.display();
            updateScore();
          }
        }
      }
      delay(bounceDelay);
    }
  }
  if (liveLost) {
    liveLost = false;
    holdScreen();
    for (int i = 0; i < trailCount; i++) {
      circlePos[i] = 5;
    }
    if (lives < 1) {
      gameOverScreen();
    }
  }
  scoreTaken = false;
  generateRandomNumber();
}


void checkInputs() {
  for (int i = 0; i < buttonCount; i++) {
    bool state = digitalRead(button[i]);
    if (state != lastState[i]) {
      lastState[i] = state;
      if (state == LOW) {
        if (i == 0) {
          if (cratePos > 1) {
            cratePos--;
            updateCratePos();
          }
        } else {
          if (cratePos < cratePosCount) {
            cratePos++;
            updateCratePos();
          }
        }
      }
    }
  }
}


void updateEverything() {
  updateCratePos();
  updateScore();
  updateLives();
  drawOutlines();
}


void updateCratePos() {
  int crateHeight = 41;
  if (cratePos == 1) {
    display.drawBitmap(2, crateHeight, umbrella, 11, 4, BLACK);
    display.drawBitmap(21, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(40, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(59, crateHeight, umbrella, 11, 4, WHITE);
    display.display();
  } else if (cratePos == 2) {
    display.drawBitmap(2, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(21, crateHeight, umbrella, 11, 4, BLACK);
    display.drawBitmap(40, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(59, crateHeight, umbrella, 11, 4, WHITE);
    display.display();
  } else if (cratePos == 3) {
    display.drawBitmap(2, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(21, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(40, crateHeight, umbrella, 11, 4, BLACK);
    display.drawBitmap(59, crateHeight, umbrella, 11, 4, WHITE);
    display.display();
  } else if (cratePos == 4) {
    display.drawBitmap(2, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(21, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(40, crateHeight, umbrella, 11, 4, WHITE);
    display.drawBitmap(59, crateHeight, umbrella, 11, 4, BLACK);
    display.display();
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
  display.display();
}


void updateLives() {
  int livesX = 48;
  display.fillRect(48, 0, 24, 7, WHITE);
  for (int i = 0; i < lives; i++) {
    display.drawBitmap(livesX + i * 8, 0, heart, 8, 7, BLACK);
  }
  for (int i = 0; i < 3; i++) {
    display.drawBitmap(livesX + i * 8, 0, heartOutline, 8, 7, BLACK);
  }
  display.display();
}


void drawOutlines() {
  display.drawRect(0, 9, 74, 38, BLACK);
  for (int i = 0; i < trailCount; i++) {
    display.drawBitmap(5 + i * trailStep, 10, dropStart, 8, 5, BLACK);
  }
  display.display();
}


void updateLevel() {
  int x = 77;
  display.drawBitmap(x, 9, lvl, 5, 18, BLACK);
  display.fillRect(x, 30, 5, 16, WHITE);
  if (level < 10) {
    display.setCursor(x, 30);
    display.print("0");
    display.setCursor(x, 39);
    display.print(level);
  } else {
    display.setCursor(x, 30);
    display.print(level / 10);
    display.setCursor(x, 39);
    display.print(level % 10);
  }
}


void generateRandomNumber() {
  int maxFails = 4;
  int randomVal = analogRead(A0);
  randomNumber[1] = random(1, 100);
  randomNumber[1] = randomNumber[1] * randomVal % 4;

  if (randomNumber[1] != randomNumber[2] && randomNumber[1] != randomNumber[3] && randomNumber[1] != randomNumber[4] && randomNumber[1] != randomNumber[5]) {
    circlePos[randomNumber[1]] = 0;
    failCount = 0;
  } else {
    if (failCount < maxFails) {
      failCount++;
    } else {
      generateRandomNumber();
    }
  }
  randomNumber[5] = randomNumber[4];
  randomNumber[4] = randomNumber[3];
  randomNumber[3] = randomNumber[2];
  randomNumber[2] = randomNumber[1];
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
}


void welcomeScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(13, 0);
  display.print("Rainy");
  display.setCursor(25, 16);
  display.print("Day");
  display.display();
  delay(1000);
  display.setCursor(3, 32);
  display.setTextSize(1);
  display.print("Press any key");
  display.setCursor(18, 41);
  display.print("to start");
  display.display();  
  goToSleep();

  display.clearDisplay();
  display.display();
  updateLevel();
  updateEverything();
}


void gameOverScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(18, 0);
  display.print("Game");
  display.setCursor(18, 16);
  display.print("Over");
  display.setCursor(10, 31);
  display.setTextSize(1);
  display.print("Your Score:");
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
  goToSleep();

  newGame();
  updateLevel();
  updateEverything();
}


void newGame() {
  lives = 3;
  score = 0;
  cratePos = 1;
  cycleCount = 0;
  cycleTime = y;
  level = 1;
  maxCycleCount = z;
  display.clearDisplay();
  display.display();
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
