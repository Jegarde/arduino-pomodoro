// Pauses with button 2
// Returns true if pressed
void pause_button(void interface_func()) {
  if (buttons & BUTTON_2_PRESSED) {
      paused = !paused;
      lcd.setCursor(14, 0);
      lcd.write(byte(
        paused ? CHAR_PAUSE : CHAR_RESUME
      ));

      lcd.setCursor(10, 1);
      if (paused)
        lcd.print("PAUSED");
      else  {
        interface_func();
      }

      return true;
    }
    return false;
}

// Skips with button 3
// Returns true if pressed
bool skip_button(int *minutes) {
  if (buttons & BUTTON_3_PRESSED) {
      *minutes = 0;
      minutePassed = true;
      return true;
  }
  return false;
}

// Hides timer with button 1
// Returns true if pressed
bool visibility_button(void interface_func()) {
  if (buttons & BUTTON_1_PRESSED) {
      timer_hidden = !timer_hidden;
      if (timer_hidden) {
        lcd.setCursor(0, 1);
        lcd.print("Focus!  ");
        lcd.setCursor(13, 0);
        lcd.print("X");
      } else {
        interface_func();
        lcd.setCursor(13, 0);
        lcd.print("O");
      }
      return true;
  }
  return false;
}

// Time set buttons
void set_time_buttons(int* outputMinutes) {
      // LEFT ARROW
      if (buttons & BUTTON_1_PRESSED) {
        *outputMinutes -= 5;
        if (*outputMinutes <= 0)
          *outputMinutes = 5;
        printTime(*outputMinutes);
      }
      // RIGHT ARROW
      else if (buttons & BUTTON_3_PRESSED) {
        *outputMinutes += 5;
        if (*outputMinutes >= 120)  // 2 hours cap
          *outputMinutes = 120;
        printTime(*outputMinutes);
      }
}