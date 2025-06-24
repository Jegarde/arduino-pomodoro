// Pauses with button 2
void pause_button() {
  if (buttons & BUTTON_2_PRESSED) {
      paused = !paused;
      lcd.setCursor(14, 0);
      lcd.write(byte(
        paused ? CHAR_PAUSE : CHAR_RESUME
      ));

      lcd.setCursor(10, 1);
      if (paused)
        lcd.print("PAUSED");
      else
        lcd.print("      ");
    }
}

// Skips with button 3
void skip_button() {
  if (buttons & BUTTON_3_PRESSED) {
      workMinutesRemaining = 0;
      minutePassed = true;
    }
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