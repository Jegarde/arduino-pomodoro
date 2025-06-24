void break_interface() {
    // Transition to break mode
    lcd.clear();
    lcd.home();
    lcd.print("Break time!");

    // Skip
    lcd.setCursor(13, 0);
    lcd.print(
      timer_hidden ? "X" : "O"
    );
    lcd.write(byte(
      paused ? CHAR_PAUSE : CHAR_RESUME
    ));
    lcd.write(byte(CHAR_SKIP));

    // Cycles
    int col = cycles < 10 ? 13 : 12;
    lcd.setCursor(col, 1);
    lcd.write(byte(CHAR_CYCLE_1));
    lcd.write(byte(CHAR_CYCLE_2));
    lcd.print(cycles);

    if (timer_hidden) {
      lcd.setCursor(0, 1);
      lcd.print("Focus!");
    } else {
      printTime(breakMinutesRemaining);
    }
}

void work_interface() {
    lcd.clear();
    lcd.home();
    lcd.print("Lock in!");
    lcd.setCursor(13, 0);
    lcd.print(
      timer_hidden ? "X" : "O"
    );
    lcd.write(byte(
      paused ? CHAR_PAUSE : CHAR_RESUME
    ));
    lcd.write(byte(CHAR_SKIP));

    if (timer_hidden) { 
      lcd.setCursor(0, 1);
      lcd.print("Focus!");
    } else {
      printTime(workMinutesRemaining);
    }
}

void setup_interface(int *minutes) {
  lcd.home();
  lcd.print("Set Pomodoro");

  lcd.setCursor(13, 0);
  lcd.write(byte(CHAR_LEFT_ARROW));
  lcd.write(byte(CHAR_CONFIRM));
  lcd.write(byte(CHAR_RIGHT_ARROW));

  printTime(*minutes);
}