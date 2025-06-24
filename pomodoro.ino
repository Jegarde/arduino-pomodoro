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

// Macros
#define HAS_ANY_FLAGS(value, flags) ((value) & (flags))

// Component pins
#define BUZZER 4
#define BUTTON_1 9
#define BUTTON_2 12
#define BUTTON_3 13

#define SECOND 1000
#define MINUTE 60000

enum Modes {
  WORK_SETUP,
  BREAK_SETUP,
  CYCLE_SETUP,
  POMODORO_WORK,
  POMODORO_BREAK,
  POMODORO_FINISH
} mode = WORK_SETUP;

int buttonsPressed = 0;

int workMinutes = 25;
int breakMinutes = 5;
int workMinutesRemaining = workMinutes;
int breakMinutesRemaining = breakMinutes;

int cycles = 0;

unsigned long startMillis, currentMillis;
const unsigned int interval = 1000;  // time it takes to tick the clock

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); 

  // Heart chars
  /*
  for (int i = 0, n = 2; i < n; ++i) {
    lcd.createChar(i+2, CHARRAY_PULSING_HEART[i]);
  }
  */

  lcd.createChar(0, LEFT_ARROW);
  lcd.createChar(1, CONFIRM);
  lcd.createChar(2, RIGHT_ARROW);
  lcd.createChar(3, SKIP);
  lcd.createChar(4, CYCLE[0]);
  lcd.createChar(5, CYCLE[1]);

  // Components
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);

  // Initial print
  startMillis = millis();
  
  pomodoro_setup();
}

void pomodoro_setup() {
  lcd.home();
  lcd.print("Set Pomodoro");

  lcd.setCursor(13, 0);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));

  printTime(workMinutes);
}

void loop() {
  int buttons = oneShotButtonsPressed();
  bool minutePassed = countTime();

  switch (mode) {
    case WORK_SETUP:
      lcd.setCursor(12, 1);
      lcd.print("WORK");

      // LEFT ARROW
      if (buttons & BUTTON_1_PRESSED) {
        workMinutes -= 5;
        if (workMinutes <= 0)
          workMinutes = 5;
        printTime(workMinutes);
      }
      // RIGHT ARROW
      else if (buttons & BUTTON_3_PRESSED) {
        workMinutes += 5;
        if (workMinutes >= 120)  // 2 hours cap
          workMinutes = 120;
        printTime(workMinutes);
      }
      // CONFIRM
      else if (buttons & BUTTON_2_PRESSED) {
        mode = BREAK_SETUP;

        lcd.setCursor(11, 1);
        lcd.print("BREAK");
        printTime(breakMinutes);
      }
      workMinutesRemaining = workMinutes;
      break;

    case BREAK_SETUP:

      // LEFT ARROW
      if (buttons & BUTTON_1_PRESSED) {
        breakMinutes -= 5;
        if (breakMinutes <= 0)
          breakMinutes = 5;
        printTime(breakMinutes);
      }
      // RIGHT ARROW
      else if (buttons & BUTTON_3_PRESSED) {
        breakMinutes += 5;
        if (breakMinutes >= 120)  // 2 hours cap
          breakMinutes = 120;
        printTime(breakMinutes);
      }
      // CONFIRM
      else if (buttons & BUTTON_2_PRESSED) {
        mode = POMODORO_WORK;

        // Transition to work mode
        lcd.clear();
        lcd.home();
        lcd.print("Lock in!");
        lcd.setCursor(15, 0);
        lcd.write(byte(3));
        printTime(workMinutesRemaining);
      }
      breakMinutesRemaining = breakMinutes;
      break;

    case POMODORO_WORK:
      // SKIP
      if (buttons & BUTTON_3_PRESSED) {
        workMinutesRemaining = 0;
        minutePassed = true;
      }

      if (minutePassed) {
        workMinutesRemaining--;

        if (workMinutesRemaining <= 0) {
          workMinutesRemaining = workMinutes;

          // Cap cycles at 99
          if (cycles < 99) 
            cycles++;
        
          mode = POMODORO_BREAK;
          alarm();

          // Transition to break mode
          lcd.clear();
          lcd.home();
          lcd.print("Break time!");

          // Skip
          lcd.setCursor(15, 0);
          lcd.write(byte(3));

          // Cycles
          int col = cycles < 10 ? 13 : 12;
          lcd.setCursor(col, 1);
          lcd.write(byte(4));
          lcd.write(byte(5));
          lcd.print(cycles);

          printTime(breakMinutesRemaining);
        } else {
          printTime(workMinutesRemaining);
        }
      }
      break;

    case POMODORO_BREAK:
      // SKIP
      if (buttons & BUTTON_3_PRESSED) {
        breakMinutesRemaining = 0;
        minutePassed = true;
      }

      if (minutePassed) {
        breakMinutesRemaining--;

        if (breakMinutesRemaining <= 0) {
          breakMinutesRemaining = breakMinutes;
          mode = POMODORO_WORK;
          alarm();

          // Transition to work mode
          lcd.clear();
          lcd.home();
          lcd.print("Lock in!");
          lcd.setCursor(15, 0);
          lcd.write(byte(3));
          printTime(workMinutesRemaining);
        } else {
          printTime(breakMinutesRemaining);
        }
      }
      break;
  }
}

void alarm() {
  for (int i = 1; i <= 5; ++i) {
    tone(BUZZER, 250 * i, 200);
    delay(100);
  }
}

void clearTime() {
  lcd.setCursor(0, 1);
  lcd.print("        ");
}

// Prints time to col 0, row 1
void printTime(int minutes) {
  clearTime();
  lcd.setCursor(0, 1);
  lcd.print(minutes);
  lcd.print(" mins");
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
int oneShotButtonsPressed() {
  int flags = 0;

  int tonePitch = 0;

  if (digitalRead(BUTTON_1) == HIGH) {
    if ((buttonsPressed & BUTTON_1_PRESSED) == 0) {
      flags |= BUTTON_1_PRESSED;
      buttonsPressed |= BUTTON_1_PRESSED;
      tonePitch = 500;
    }
  } else {
    buttonsPressed = buttonsPressed & ~BUTTON_1_PRESSED;
  }
  if (digitalRead(BUTTON_2) == HIGH) {
    if ((buttonsPressed & BUTTON_2_PRESSED) == 0) {
      flags |= BUTTON_2_PRESSED;
      buttonsPressed |= BUTTON_2_PRESSED;
      tonePitch = 1000;
    }
  } else {
    buttonsPressed = buttonsPressed & ~BUTTON_2_PRESSED;
  }
  if (digitalRead(BUTTON_3) == HIGH) {
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


// Calculates the percentage of day completed
/*
int calculateDayCompleted() {
  const float minutesInDay = 24 * 60;
  float minutesPassed = time.hour * 60 + time.minute;

  // Round with one decimal
  return (int)round(minutesPassed / minutesInDay * 100);
}
*/