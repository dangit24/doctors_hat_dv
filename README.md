# Doctor's Hat Celebration Arduino Project with Puzzle

![Doctor's Hat Celebration with Puzzle](images/doctor_hat_celebration_puzzle.jpg)

## Overview

This Arduino project is designed to celebrate the achievement of a doctor's degree by creating a decorative element for a doctor's hat. The hat features a rotating ellipse with eccentricity and ripple amplitude, accompanied by pixel animations and a [victory melody](https://gist.github.com/ataylor32/24f429e147d8b2a758d7). Additionally, the hat incorporates an interactive puzzle where two potentiometers must be turned to specific positions to create a circular ellipse with minimal ripple. The buzzer visualizes the potentiometer positions through audible beeps, changing in frequency and interval.

## Components

- ESP32 board [TTGO T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
- TFT display
- Buzzer
- Buttons (SOUND_OFF_PIN and BTN2_PIN)
- LEDs (LED1_PIN and LED2_PIN)
- Potentiometers (connected to pins 32 and 33)

## Features

1. **Rotating Ellipse:** The main visual element is a rotating ellipse with eccentricity and ripple amplitude, providing an animated and celebratory effect.

2. **Pixel Animation:** A set of pixels move around the display, creating a dynamic and lively atmosphere using the [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library.

3. **Victory Melody:** A victory melody plays when triggered, adding an audible element to the celebration.

4. **Interactive Puzzle:** The project includes a puzzle where two potentiometers must be turned to specific positions to create a circular ellipse with minimal ripple.

5. **Audible Feedback:** The buzzer provides audible feedback, changing its frequency and beep interval based on the positions of the potentiometers using the ESP32's ledc functions (PWM based).

6. **Interactive Buttons:** The project includes buttons for controlling sound and animation. The SOUND_OFF_PIN button toggles the sound, and the BTN2_PIN button toggles pixel animation.

7. **LED Blinking:** LEDs (LED1_PIN and LED2_PIN) blink in celebration after a victory.

## Setup and Usage

1. **Hardware Setup:** Connect the components as per the defined pins in the code. Ensure all connections are secure.

2. **Upload Code:** Use the [Arduino IDE](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) to upload the provided code to the ESP32 board.

3. **Power On:** Power on the ESP32 board, and the celebration will commence.

4. **Interact with Buttons:** Press the SOUND_OFF_PIN button to toggle sound, and press the BTN2_PIN button to toggle pixel animation.

5. **Solve the Puzzle:** Turn the potentiometers (connected to pins 32 and 33) to specific positions to create a circular ellipse with minimal ripple. Listen to the buzzer for audible feedback on your progress.

6. **Celebrate:** Enjoy the rotating ellipse, pixel animations, victory melody, and LED blinking as a fun and engaging decorative element for the doctor's hat.

## User_Setup.h Configuration

To ensure proper functionality with the TFT_eSPI library, the `User_Setup.h` file from the library must be modified with the following defines:

```c
#define ST7789_DRIVER
#define TFT_WIDTH  135
#define TFT_HEIGHT 240
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4
#define SPI_FREQUENCY  60000000
```

## Additional Notes

- The project includes comments within the code for better understanding of each section and functionality.

- Customize the victory melody or visual elements as desired for a personalized touch.

## Author

This project was created by github user [dangit24].

## License

This project is licensed under the [MIT License](LICENSE.md). Feel free to modify and share!
