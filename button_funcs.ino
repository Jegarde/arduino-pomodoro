// Pauses with button 2
// Returns true if pressed
void pauseButton() {
    if ((buttons & BUTTON_2_PRESSED) == 0) {
        return false;
    }
        
    paused = !paused;
    lcd.setCursor(14, 0);
    lcd.write(byte(
      paused ? CHAR_PAUSE : CHAR_RESUME
    ));

    lcd.setCursor(10, 1);
    if (paused) {
        lcd.print("PAUSED");
    } else {
        timerInterface();
    }

    return true;
}

// Skips with button 3
// Returns true if pressed
bool skipButton() {
    // Check if button 3 was pressed
    if ((buttons & BUTTON_3_PRESSED) == 0) {
        return false;
    }
        
    // Set minutes remaining to 0 to skip
    minutesRemaining = 1; 
    minutePassed = true;
    return true;
}

// Hides timer with button 1
// Returns true if pressed
bool visibilityButton() {
    // Check if button 1 was pressed
    if ((buttons & BUTTON_1_PRESSED) == 0) {
        return false;
    }
        
    // Toggle timer visiblity
    timerHidden = !timerHidden;
    if (timerHidden) {
      // Overwrite timer to hide it
      lcd.setCursor(0, 1);
      lcd.print("Focus!  ");
    } else {
      // Refresh interface
      timerInterface();
    }

    // Update visibility indicator
    lcd.setCursor(13, 0);
    lcd.print(
      timerHidden ? "X" : "O"
    );
    return true;
}

// Time set buttons
void setTimeButtons(uint8_t* outputMinutes) {
    uint8_t change = 5, min = 5, max = 30;

    // LEFT ARROW, reduce time
    if (buttons & BUTTON_1_PRESSED) {
      // Check that we stay within min bounds
      *outputMinutes -= change;
      if (*outputMinutes < min)
        *outputMinutes = max;
      printTime(*outputMinutes);
    }
    // RIGHT ARROW, increase time
    else if (buttons & BUTTON_3_PRESSED) {
      *outputMinutes += change;
      // Check that we stay within max bounds
      if (*outputMinutes > max)
        *outputMinutes = min;
      printTime(*outputMinutes);
    }
}