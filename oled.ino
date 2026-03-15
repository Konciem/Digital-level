#include <Adafruit_SH110X.h>
#include <splash.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <MPU6050.h>

// --- KONFIGURACJA EKRANU ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SH1106G oled = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- KONFIGURACJA MPU ---
MPU6050 mpu;

// --- PINY PRZYCISKÓW ---
#define BTN_HOLD_PIN 16
#define BTN_ZERO_PIN 17
#define BTN_AXIS_PIN 18
#define BTN_BAT_PIN  19
#define BTN_RSET_PIN 20

// --- PINY DIOD LED ---
#define LED_RED_NEG    12
#define LED_YEL_NEG    11
#define LED_GREEN      15
#define LED_YEL_POS    14
#define LED_RED_POS    13

// --- PIN BATERII ---
#define BAT_ADC_PIN 29 

// --- ZMIENNE STANU ---
bool isFrozen = false;          
bool showBatteryScreen = false;
int  currentAxis = 0;  
float pitchOffset = 0.0;        
float rollOffset = 0.0;

// Zmienne baterii
int batPercentage = 100;
float batVoltage = 4.2;

struct Button { uint8_t pin; bool lastState; unsigned long lastPressTime; };
Button buttons[] = {
  {BTN_HOLD_PIN, HIGH, 0}, {BTN_ZERO_PIN, HIGH, 0}, {BTN_AXIS_PIN, HIGH, 0},
  {BTN_BAT_PIN,  HIGH, 0}, {BTN_RSET_PIN, HIGH, 0}
};
const int numButtons = 5;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  analogReadResolution(12);

  for (int i = 0; i < numButtons; i++) pinMode(buttons[i].pin, INPUT_PULLUP);
  
  pinMode(LED_RED_NEG, OUTPUT); pinMode(LED_YEL_NEG, OUTPUT);
  pinMode(LED_GREEN, OUTPUT); pinMode(LED_YEL_POS, OUTPUT); pinMode(LED_RED_POS, OUTPUT);

  if (!oled.begin(OLED_ADDR, true)) { while (1) delay(100); }
  
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SH110X_WHITE);
  oled.setCursor(10, 20);
  oled.println("Startuje...");
  oled.println("Tryb: BOKI"); 
  oled.display();
  
  delay(500);
  mpu.initialize();
  mpu.setSleepEnabled(false);
  
  if (!mpu.testConnection()) {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("Blad MPU6050!");
    oled.display();
    while (1) delay(1000);
  }
}

int checkButtons() {
  int pressedBtnIndex = -1;
  unsigned long now = millis();
  for (int i = 0; i < numButtons; i++) {
    bool state = digitalRead(buttons[i].pin);
    if (buttons[i].lastState == HIGH && state == LOW) {
      if (now - buttons[i].lastPressTime > 200) { 
        buttons[i].lastPressTime = now;
        pressedBtnIndex = i;
      }
    }
    buttons[i].lastState = state;
  }
  return pressedBtnIndex;
}

float constrainAngle(float a) {
  if (a > 180) a -= 360; if (a < -180) a += 360; return a;
}

void updateLEDs(float value) {
  if (showBatteryScreen) {
     digitalWrite(LED_RED_NEG, LOW); digitalWrite(LED_YEL_NEG, LOW);
     digitalWrite(LED_GREEN, LOW); digitalWrite(LED_YEL_POS, LOW); digitalWrite(LED_RED_POS, LOW);
     return;
  }
  digitalWrite(LED_RED_NEG, LOW); digitalWrite(LED_YEL_NEG, LOW);
  digitalWrite(LED_GREEN,   LOW); digitalWrite(LED_YEL_POS, LOW); digitalWrite(LED_RED_POS, LOW);

  if (value > -2.0 && value < 2.0) digitalWrite(LED_GREEN, HIGH);
  else if (value >= 2.0 && value < 10.0) digitalWrite(LED_YEL_POS, HIGH);
  else if (value >= 10.0) digitalWrite(LED_RED_POS, HIGH);
  else if (value <= -2.0 && value > -10.0) digitalWrite(LED_YEL_NEG, HIGH);
  else if (value <= -10.0) digitalWrite(LED_RED_NEG, HIGH);
}

void readBattery() {
  int rawADC = analogRead(BAT_ADC_PIN);
  batVoltage = (rawADC / 4095.0) * 3.3 * 3.0;
  int pct = (int)((batVoltage - 3.0) / (4.2 - 3.0) * 100.0);
  if (pct > 100) pct = 100; if (pct < 0) pct = 0;
  batPercentage = pct;
}

void drawLargeBatteryScreen() {
  oled.clearDisplay();
  oled.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);
  oled.setCursor(30, 6); oled.setTextSize(1); oled.print("STAN BATERII");
  oled.drawRect(20, 25, 80, 24, SH110X_WHITE); oled.fillRect(100, 30, 4, 14, SH110X_WHITE);
  int fillW = map(batPercentage, 0, 100, 0, 76);
  if (fillW > 0) oled.fillRect(22, 27, fillW, 20, SH110X_WHITE);
  oled.setCursor(10, 54); oled.print(batVoltage, 2); oled.print("V");
  oled.setCursor(80, 54); oled.print(batPercentage); oled.print("%");
  oled.display();
}

void drawLevelUI(float pitch, float roll) {
  oled.clearDisplay();
  oled.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);

  oled.setCursor(4, 4); oled.setTextSize(1);
  if (isFrozen) oled.print("[HOLD] "); else oled.print("RUN    ");

  float displayVal = (currentAxis == 0) ? pitch : roll;
  oled.setCursor(4, 25); oled.setTextSize(2);
  oled.print(currentAxis == 0 ? "P:" : "R:"); 
  oled.print(displayVal, 1); 
  oled.print((char)247);

  oled.setTextSize(1); oled.setCursor(4, 50);
  oled.print(currentAxis == 0 ? "Roll: " : "Pitch: "); 
  oled.print(currentAxis == 0 ? roll : pitch, 1);
  
  if (pitchOffset != 0 || rollOffset != 0) { 
    oled.setCursor(80, 50); oled.print("(ZERO)"); 
  }

  int barX = 110; int barY = 16; int barH = 40; int barW = 10;
  oled.drawRect(barX, barY, barW, barH, SH110X_WHITE);
  int midY = barY + barH / 2; oled.drawLine(barX, midY, barX + barW, midY, SH110X_WHITE); 
  
  float clampedVal = constrain(displayVal, -45, 45);
  int dotY = midY - (int)(clampedVal * (barH / 2) / 45.0);
  oled.fillCircle(barX + barW/2, dotY, 3, SH110X_WHITE);
  
  oled.display();
}

void loop() {
  static unsigned long lastBatCheck = 0;
  if (millis() - lastBatCheck > 1000) { readBattery(); lastBatCheck = millis(); }

  int pressed = checkButtons();
  if (pressed != -1) {
    switch(pressed) {
      case 0: isFrozen = !isFrozen; break; // HOLD
      
      case 1: if (!isFrozen && !showBatteryScreen) { // ZERO (TARA)
          int16_t ax, ay, az, gx, gy, gz; 
          mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
          float axf = ax / 16384.0; float ayf = ay / 16384.0; float azf = az / 16384.0;
          
          // --- LOGIKA ZEROWANIA (BOKI JAKO PITCH) ---
          float v_Z = ayf; // PION (Oś Y czujnika)
          
          // ZAMIANA OSI DLA ZEROWANIA:
          float v_Y = axf; // Teraz PITCH to Oś X (Boki)
          float v_X = azf; // Teraz ROLL to Oś Z (Przód/Tył)

          pitchOffset = atan2(v_Y, sqrt(v_X * v_X + v_Z * v_Z)) * 180.0 / PI;
          rollOffset  = atan2(v_X, sqrt(v_Y * v_Y + v_Z * v_Z)) * 180.0 / PI;
        } break;
        
      case 2: currentAxis = !currentAxis; break; // Zmiana Osi
      case 3: showBatteryScreen = !showBatteryScreen; break; // Bateria
      case 4: pitchOffset = 0; rollOffset = 0; break; // RESET
    }
  }

  if (showBatteryScreen) {
    drawLargeBatteryScreen();
    updateLEDs(0); 
    delay(50); 
    return;
  }

  static float displayPitch = 0; 
  static float displayRoll = 0;
  static float smoothPitch = 0;
  static float smoothRoll = 0;
  float filterFactor = 0.1; 

  if (!isFrozen) {
    int16_t ax, ay, az, gx, gy, gz; 
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float axf = ax / 16384.0; float ayf = ay / 16384.0; float azf = az / 16384.0;
    
    float virtual_Z = ayf;  
    float virtual_Y = axf;  
    float virtual_X = azf;  

    // Obliczamy PITCH (Teraz reaguje na lewo/prawo)
    float rawPitch = atan2(virtual_Y, sqrt(virtual_X * virtual_X + virtual_Z * virtual_Z)) * 180.0 / PI;

    // Obliczamy ROLL (Teraz reaguje na przód/tył)
    float rawRoll  = atan2(virtual_X, sqrt(virtual_Y * virtual_Y + virtual_Z * virtual_Z)) * 180.0 / PI;
    
    // Filtracja
    smoothPitch = (rawPitch * filterFactor) + (smoothPitch * (1.0 - filterFactor));
    smoothRoll  = (rawRoll  * filterFactor) + (smoothRoll  * (1.0 - filterFactor));

    displayPitch = constrainAngle(smoothPitch - pitchOffset);
    displayRoll  = constrainAngle(smoothRoll  - rollOffset);
  }

  float activeValue = (currentAxis == 0) ? displayPitch : displayRoll;
  updateLEDs(activeValue);
  drawLevelUI(displayPitch, displayRoll);
  
  delay(20);
}