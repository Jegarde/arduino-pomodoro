void timerInterface() {
    // Transition to break mode
    lcd.clear();
    lcd.home();

    // Change title based on mode
    lcd.print(
        mode == POMODORO_WORK ? "Lock in!" : "Break!"
    );

    // Skip
    lcd.setCursor(13, 0);
    lcd.print(
        timerHidden ? "X" : "O"
    );
    lcd.write(byte(
        paused ? CHAR_PAUSE : CHAR_RESUME
    ));
    lcd.write(byte(CHAR_SKIP));

    // Cycles
    uint8_t col = cycles < 10 ? 9 : 10;
    lcd.setCursor(col, 0);
    lcd.write(byte(CHAR_CYCLE_1));
    lcd.write(byte(CHAR_CYCLE_2));
    lcd.print(cycles);

    if (timerHidden) {
        lcd.setCursor(0, 1);
        lcd.print("Focus!");
    } else {
        printTime(minutesRemaining);
    }
}

void setupInterface() {
    lcd.home();
    lcd.print("Set Pomodoro");

    lcd.setCursor(13, 0);
    lcd.write(byte(CHAR_LEFT_ARROW));
    lcd.write(byte(CHAR_CONFIRM));
    lcd.write(byte(CHAR_RIGHT_ARROW));

    uint8_t minutes = 0;
    switch (mode) {
        case WORK_SETUP:
            minutes = workMinutes;
            break;
        case BREAK_SETUP:
            minutes = breakMinutes;
            break;  
        case LONG_BREAK_SETUP:
            minutes = longBreakMinutes;
            break;  
    }
    printTime(minutes);
}