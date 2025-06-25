/*
 The circuit:
 * LCD RS pin to digital pin 2
 * LCD Enable pin to digital pin 3
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 6
 * LCD D6 pin to digital pin 7
 * LCD D7 pin to digital pin 8
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * Piezo to digital pin 4
 * Button 1 to digital pin 9
 * Button 2 to digital pin 12
 * Button 3 to digital pin 13
 * 10K resistor:
 * ends to +5V and ground

wiper to LCD VO pin (pin 3)
*/

#include <LiquidCrystal.h>
#include "consts.h"

// LCD Pins
const int rs = 2, en = 10, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   

// Button flags
#define BUTTON_1_PRESSED (1 << 0)
#define BUTTON_2_PRESSED (1 << 1)
#define BUTTON_3_PRESSED (1 << 2)

// Component pins
#define BUZZER 4
#define BUTTON_1 9
#define BUTTON_2 12
#define BUTTON_3 13

// Minute in milliseconds
#define MINUTE 60000

// Inputs held down
uint8_t buttonsPressed = 0;

// Input in a single loop cycle
uint8_t buttons = 0;

// Configured timer durations
uint8_t workMinutes = 25;
uint8_t breakMinutes = 5;
uint8_t longBreakMinutes = 15;

// Timer minutes remaining
int8_t minutesRemaining = 0;

// How many pomodoro cycles have passed
uint8_t cycles = 0;

// If timer is paused
bool paused = false;

// If timer is hidden
bool timerHidden = false;

// Did a minute pass in loop?
bool minutePassed = false;

// For counting minutes
unsigned long startMillis, currentMillis;

// Custom chars
enum CUSTOM_CHAR : uint8_t {
    CHAR_LEFT_ARROW,
    CHAR_RIGHT_ARROW,
    CHAR_SKIP,
    CHAR_CYCLE_1,
    CHAR_CYCLE_2,
    CHAR_PAUSE,
    CHAR_RESUME,
    CHAR_CONFIRM
};

// Timer states
enum MODE : uint8_t {
  WORK_SETUP,
  BREAK_SETUP,
  LONG_BREAK_SETUP,
  POMODORO_WORK,
  POMODORO_BREAK
} mode = WORK_SETUP;

void setup() {
    // Let the fun begin
    Serial.begin(9600);
    lcd.begin(16, 2); 

    // Create characterse
    lcd.createChar(CHAR_LEFT_ARROW, LEFT_ARROW);
    lcd.createChar(CHAR_CONFIRM, CONFIRM);
    lcd.createChar(CHAR_RIGHT_ARROW, RIGHT_ARROW);
    lcd.createChar(CHAR_SKIP, SKIP);
    lcd.createChar(CHAR_CYCLE_1, CYCLE[0]);
    lcd.createChar(CHAR_CYCLE_2, CYCLE[1]);
    lcd.createChar(CHAR_PAUSE, PAUSE);
    lcd.createChar(CHAR_RESUME, RESUME);

    // Components
    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);
    pinMode(BUTTON_3, INPUT);

    // Initial print
    startMillis = millis();
    
    // Begin setup
    setupInterface();
}

void loop() {
    // Check for one shot input from any of the 3 buttons
    buttons = oneShotButtonsPressed();
    
    // Don't count time if paused
    if (!paused) minutePassed = countTime();

    switch (mode) {
      case WORK_SETUP:
          lcd.setCursor(12, 1);
          lcd.print("WORK");

          // LEFT, RIGHT ARROW BUTTONS
          setTimeButtons(&workMinutes);

          // CONFIRM
          if (buttons & BUTTON_2_PRESSED) {
            // Switch over to break setup
            mode = BREAK_SETUP;
            lcd.setCursor(11, 1);
            lcd.print("BREAK");
            printTime(breakMinutes);
          }
          minutesRemaining = workMinutes;
          break;

      case BREAK_SETUP:
        // LEFT, RIGHT ARROW BUTTONS
        setTimeButtons(&breakMinutes);
        // CONFIRM
        if (buttons & BUTTON_2_PRESSED) {
            // Switch over to long break setup
            mode = LONG_BREAK_SETUP;
            lcd.setCursor(7, 1);
            lcd.print("LONGBREAK");
            printTime(longBreakMinutes);
        }

        break;

      case LONG_BREAK_SETUP:
        // LEFT, RIGHT ARROW BUTTONS
        setTimeButtons(&longBreakMinutes);
        // CONFIRM
        if (buttons & BUTTON_2_PRESSED) {
            // Switch over to work timer
            mode = POMODORO_WORK;

            // Transition to pomodoro timer
            timerInterface();
        }
        break;

      case POMODORO_WORK:
      case POMODORO_BREAK:
        // SKIP
        skipButton();
        // PAUSE BUTTON
        pauseButton();
        // VISIBILITY BUTTON
        visibilityButton();

        // Check if a minute passed
        if (minutePassed) {
            minutesRemaining--;

            // Check if timer ran out
            if (minutesRemaining <= 0) {
                // Ring the alarm
                alarm();

                // Toggle between work and break mode
                mode = mode == POMODORO_WORK ? POMODORO_BREAK : POMODORO_WORK;

                // Add a cycle. Cap at 99.
                if (cycles < 99 && mode == POMODORO_BREAK) 
                  cycles++;

                // Update timer
                if (mode == POMODORO_BREAK) {
                    // After every 4 cycles, do long break
                    minutesRemaining = 
                        cycles % 4 == 0 ? 
                        longBreakMinutes : breakMinutes;
                } else {
                    minutesRemaining = workMinutes;
                }

                // Refresh interface
                timerInterface();
            } else {
                // Timer hasn't run out yet, so update it
                if (!timerHidden) {
                    printTime(minutesRemaining);
                }
            }
        }
        break;
    }
}


// Plays a cute alarm with piezo
void alarm() {
  for (uint8_t i = 1; i <= 5; ++i) {
    tone(BUZZER, 250 * i, 200);
    delay(100);
  }
  tone(BUZZER, 1250, 500);
}

// Wipes the timer text
void clearTimerText() {
  lcd.setCursor(0, 1);
  lcd.print("       ");
}

// Prints time to col 0, row 1
void printTime(int minutes) {
  clearTimerText();
  lcd.setCursor(0, 1);
  lcd.print(minutes);
  lcd.print(" min");
}

// Counts time.
// Returns true if interval passed
bool countTime() {
  currentMillis = millis();
  if (currentMillis - startMillis >= MINUTE) {
    startMillis = currentMillis;
    return true;
  }
  return false;
}

// single fire buttons presses
uint8_t oneShotButtonsPressed() {
  uint8_t flags = 0;
  uint16_t tonePitch = 0;

  if (digitalRead(BUTTON_1) == LOW) {
    if ((buttonsPressed & BUTTON_1_PRESSED) == 0) {
      flags |= BUTTON_1_PRESSED;
      buttonsPressed |= BUTTON_1_PRESSED;
      tonePitch = 500;
    }
  } else {
    buttonsPressed = buttonsPressed & ~BUTTON_1_PRESSED;
  }
  if (digitalRead(BUTTON_2) == LOW) {
    if ((buttonsPressed & BUTTON_2_PRESSED) == 0) {
      flags |= BUTTON_2_PRESSED;
      buttonsPressed |= BUTTON_2_PRESSED;
      tonePitch = 1000;
    }
  } else {
    buttonsPressed = buttonsPressed & ~BUTTON_2_PRESSED;
  }
  if (digitalRead(BUTTON_3) == LOW) {
    if ((buttonsPressed & BUTTON_3_PRESSED) == 0) {
      flags |= BUTTON_3_PRESSED;
      buttonsPressed |= BUTTON_3_PRESSED;
      tonePitch = 750;
    }
  } else {
    buttonsPressed = buttonsPressed & ~BUTTON_3_PRESSED;
  }

  // Play tone if button pressed
  if (flags != 0) {
    tone(BUZZER, tonePitch, 100);
  }

  return flags;
}