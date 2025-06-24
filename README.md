# Arduino Pomodoro Timer

Demonstration: https://youtu.be/Fk8wHDysHaA

## Features
- Set work time from 5 minutes to 2 hours.
- Set break time from 5 minutes to 2 hours.
- Skipping the timer.
- Pausing the timer.
- Hiding the timer.
- Piezo alarm when timer runs out.

## What & why
This is a fun little beginner project I put together with my [Arduino Starter Pack](https://store.arduino.cc/products/arduino-starter-kit-multi-language). I didn't focus on hyper-optimizing the code or organizing it to my best ability, I just wanted to put out a simple project.

I have struggled with getting started on personal projects, and if I do get started, finishing it is another monumental task. By taking it easy, I was able to put this out and have fun while doing it. :)

It's projects like these that teach me to organize and write better code, so my future code will be better and cleaner, even if I don't try as hard. After all, I'm not that familiar with C and Arduino's workflow yet.

## What I learned
- Bit masks and bit manipulation
- Using a piezo and a liquid crystal display.
- Arduino IDE tabs.

## Issue I ran to with piezos and liquid crystal displays, and how I fixed it
Using Arduino's `tone()` function to make sound with the piezo caused my lcd display to corrupt. It had something to do with electromagnetic interference. Better explained here: https://forum.arduino.cc/t/please-help-circuit-with-lcd-screen-and-piezo/849076

What finally fixed it was grounding the piezo with a 220 OHM resistor!
