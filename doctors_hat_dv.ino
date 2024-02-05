#include <TFT_eSPI.h>  // Hardware-specific library
#include <SPI.h>

#define BUZZER_PIN 25
#define BUZZER_CHANNEL 0
#define SOUND_OFF_PIN 35
#define BTN2_PIN 0
#define LED1_PIN 26
#define LED2_PIN 27
#define LED_BLINK_INTERVAL_MS 200

// Define maximum number of pixels
#define MAX_PIXELS 100

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite txtSprite = TFT_eSprite(&tft);

bool sound_on = true;
bool btn2_state = true;
bool melody_playing = false;
int num_victory_played = 0;
int num_victories = 0;

long ms_last_beep1 = 0;
long ms_last_beep2 = 0;

float target_freq = 880.0f;
float pot1_target_pu = 0.6f;
float pot2_target_pu = 0.4f;
long ms_in_target_range = 0;
long ms_flashing_started = 0;
int num_btn_pressed = 0;

// Structure to represent a pixel
struct Pixel {
  float x;
  float y;
  float velocityX;
  float velocityY;
};

// Array to store the pixels
Pixel pixels[MAX_PIXELS];

void resetPixel(int index) {
  pixels[index].x = TFT_WIDTH / 2;
  pixels[index].y = TFT_HEIGHT / 2;
  pixels[index].velocityX = random(-100.0, 100.0) / 10.0;
  pixels[index].velocityY = random(-100.0, 100.0) / 10.0;
}

void updatePixels() {
  for (int i = 0; i < MAX_PIXELS; i++) {
    pixels[i].x += pixels[i].velocityX;
    pixels[i].y += pixels[i].velocityY;

    // Check if pixel is out of bounds, reset if necessary
    if (pixels[i].x < 0 || pixels[i].x >= TFT_WIDTH || pixels[i].y < 0 || pixels[i].y >= TFT_HEIGHT) {
      resetPixel(i);
    }
  }

  // Randomly reset pixels at a certain rate
  if (random(100) < 5) {
    int index = int(random(MAX_PIXELS));
    resetPixel(index);
  }
}

void drawPixels() {
  for (int i = 0; i < MAX_PIXELS; i++) {
    sprite.fillCircle(pixels[i].x, pixels[i].y, 1, TFT_WHITE);
  }
}

void beep(int freq, int ms_dur) {
  if (sound_on)
    ledcWriteTone(BUZZER_CHANNEL, freq);
  else
    ledcWriteTone(BUZZER_CHANNEL, 0);
  delay(ms_dur);
}

void play_victory_melody() {
  beep(130, 100);
  beep(262, 100);
  beep(330, 100);
  beep(392, 100);
  beep(523, 100);
  beep(660, 100);
  beep(784, 300);
  beep(660, 300);
  beep(146, 100);
  beep(262, 100);
  beep(311, 100);
  beep(415, 100);
  beep(523, 100);
  beep(622, 100);
  beep(831, 300);
  beep(622, 300);
  beep(155, 100);
  beep(294, 100);
  beep(349, 100);
  beep(466, 100);
  beep(588, 100);
  beep(699, 100);
  beep(933, 300);
  beep(933, 100);
  beep(933, 100);
  beep(933, 100);
  beep(1047, 400);
  beep(0, 1);
}

void play_victory_melody_task(void *pvParameter) {
  melody_playing = true;
  play_victory_melody();
  delay(1000);
  play_victory_melody();
  melody_playing = false;
  num_victory_played++;
  sound_on = false;
  vTaskDelete(NULL);
}

void victory() {
  num_victories++;
  xTaskCreate(&play_victory_melody_task,
              "vict_task", 2048, NULL, 5, NULL);
}

void beep_task(void *pvParameter) {
  printf("Hello world!\n");
  while (1) {
    if (!melody_playing) {
      if (sound_on) {
        float pot1_pu = analogRead(32) / 4095.0f;
        float pot2_pu = analogRead(33) / 4095.0f;
        float diff_pot1_pu = pot1_target_pu - pot1_pu;
        float diff_pot2_pu = pot2_target_pu - pot2_pu;
        if (millis() - ms_last_beep1 > abs(2000 * diff_pot1_pu)) {
          ms_last_beep1 = millis();
          ledcWriteTone(BUZZER_CHANNEL, 440.0f - abs(diff_pot1_pu * 440.0f));
          delay(100);
        }
        if (millis() - ms_last_beep2 > abs(2000 * diff_pot2_pu)) {
          ms_last_beep2 = millis();
          ledcWriteTone(BUZZER_CHANNEL, 440.0f - abs(diff_pot2_pu * 440.0f));
          delay(100);
        }
        ledcWriteTone(BUZZER_CHANNEL, 0);
      } else {
        ledcWriteTone(BUZZER_CHANNEL, 0);
        delay(100);
      }
    } else {
      delay(100);
    }
  }
}

void setup() {
  pinMode(SOUND_OFF_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pot1_target_pu = 0.5 + random(50, 150) / 800.0f;
  pot2_target_pu = 0.5 + random(50, 150) / 800.0f;
  // Initialize TFT display
  tft.begin();
  //tft.setRotation(0);  // Adjust the rotation if needed
  printf("\n%d %d\n", tft.width(), tft.height());
  // Create sprite with the same dimensions as the display
  sprite.setColorDepth(16);
  sprite.createSprite(tft.width(), tft.height());
  txtSprite.createSprite(300, 100);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  // Initialize pixels
  for (int i = 0; i < MAX_PIXELS; i++) {
    resetPixel(i);
  }

  xTaskCreatePinnedToCore(&beep_task, "beep_task", 2048, NULL, 5, NULL, 0);
}

void drawText() {
  // Set text properties
  txtSprite.fillSprite(TFT_TRANSPARENT);
  txtSprite.setTextSize(2);
  txtSprite.setTextColor(TFT_YELLOW);
  txtSprite.setTextFont(4);
  txtSprite.setTextWrap(false);
  // Scroll the text horizontally
  static int textPosition = tft.width();  // Starting position beyond the right side
  txtSprite.setCursor(40, 15);
  txtSprite.print("DOKTOR!");

  // Adjust the text position for scrolling
  textPosition -= 2;

  // Reset the position when text disappears on the left
  if (textPosition < -txtSprite.width()) {
    textPosition = txtSprite.width();
  }
}

long ms_last_flash = 0;
int last_flash_color = TFT_BLACK;
void drawEllipseWithRipples(float eccentricity, float rippleAmplitude, unsigned long currentTime) {
  long ms1 = millis();
  // Clear the screen
  if (num_victories > 0) {
    if (millis() - ms_last_flash > 500) {
      if (ms_last_flash == 0) ms_flashing_started = millis();
      ms_last_flash = millis();
      if (last_flash_color != TFT_BLACK) {
        last_flash_color = TFT_BLACK;
      } else {
        last_flash_color = TFT_GREEN;
      }
    }
    if (millis() - ms_flashing_started > 60000) {
      last_flash_color = TFT_BLACK;
    }
  } else {
    last_flash_color = TFT_BLACK;
  }

  sprite.fillScreen(last_flash_color);

  // Calculate center of the screen
  int centerX = sprite.width() / 2;
  int centerY = sprite.height() / 2;

  // Set ellipse dimensions
  int ellipseWidth = 120;                           // Set the width of the ellipse
  int ellipseHeight = ellipseWidth * eccentricity;  // Calculate height based on eccentricity

  // Draw centered horizontal line
  sprite.drawFastHLine(0, centerY, sprite.width(), TFT_RED);
  sprite.drawFastHLine(0, centerY + 1, sprite.width(), TFT_RED);
  sprite.drawFastHLine(0, centerY - 1, sprite.width(), TFT_RED);

  // Draw centered vertical line
  sprite.drawFastVLine(centerX, 0, sprite.height(), TFT_BLUE);
  sprite.drawFastVLine(centerX + 1, 0, sprite.height(), TFT_BLUE);
  sprite.drawFastVLine(centerX - 1, 0, sprite.height(), TFT_BLUE);

  // Draw rotating ellipse outline with ripples
  for (float angle = 0; angle < 360; angle += 0.3) {
    float radian = (angle + currentTime * 0.01) * PI / 180.0;
    float radian2 = (angle)*PI / 180.0;
    int x = centerX + (ellipseWidth / 2 + rippleAmplitude * sin(radian2 * 10)) * cos(radian);
    int y = centerY + (ellipseHeight / 2 + rippleAmplitude * sin(radian2 * 10)) * sin(radian);
    sprite.fillCircle(x, y, 2, TFT_WHITE);
  }

  //Draw unity circle as a hint
  if (!sound_on)
    for (float angle = 0; angle < 360; angle += 10) {
      float radian = (angle)*PI / 180.0;
      int x = centerX + (ellipseWidth / 2) * cos(radian);
      int y = centerY + (ellipseWidth / 2) * sin(radian);
      sprite.drawPixel(x, y, TFT_GREEN);
    }

  if (num_victories > 0 && btn2_state) {
    // Update and draw pixels
    updatePixels();
    drawPixels();
    drawText();
    txtSprite.pushRotated(&sprite, millis() / 20.0, TFT_TRANSPARENT);
  }

  if (!sound_on) sprite.fillCircle(2, 2, 2, TFT_RED);
  // Push the sprite to the display

  sprite.pushSprite(0, 0);
  long ms2 = millis();
  printf("[%d] %d\n", millis(), ms2 - ms1);  //Performance analysis
}

bool last_sound_off_btn_state = 1;
bool last_btn2_state = 1;
long ms_last_led_blink = 0;
bool last_led_state = false;

void loop() {
  // Get current time in milliseconds
  unsigned long currentTime = millis() % 1000000; //Workaround for calculations in drawEllipseWithRipples slowing down beyond this time value
  float pot1_pu = 0;
  float pot2_pu = 0;
  for (int i = 0; i < 100; i++) {
    pot1_pu += analogRead(32) / 4095.0f;
    pot2_pu += analogRead(33) / 4095.0f;
  }
  pot1_pu /= 100.0;
  pot2_pu /= 100.0;
  float diff_pot1_pu = pot1_target_pu - pot1_pu;
  float diff_pot2_pu = pot2_target_pu - pot2_pu;
  //draw rotating ellipse with eccentricity and ripple amplitude
  drawEllipseWithRipples(diff_pot1_pu * 4.0f - 1.0f, diff_pot2_pu * 100.0, currentTime);

  // Check if both potentiometers are within a certain range of the target values
  if ((abs(diff_pot1_pu) < 0.02) && (abs(diff_pot2_pu) < 0.05)) {

    // If entering the target range for the first time, record the current time
    if (ms_in_target_range == 0) ms_in_target_range = millis();

    // Check if victory conditions are met and play victory once after 3 seconds
    if (num_victory_played == 0 && num_victories == 0 && (millis() - ms_in_target_range > 3000))
      victory();

  } else {
    // Reset the time if not in the target range
    ms_in_target_range = 0;
  }

  // Check if the sound-off button is pressed and update sound status
  if (!digitalRead(SOUND_OFF_PIN) && last_sound_off_btn_state) {
    sound_on = !sound_on;
    num_btn_pressed++;
  }

  // Check if button 2 is pressed and update its state
  if (!digitalRead(BTN2_PIN) && last_btn2_state) {
    btn2_state = !btn2_state;
  }

  // Store the current state of buttons for the next iteration
  last_sound_off_btn_state = digitalRead(SOUND_OFF_PIN);
  last_btn2_state = digitalRead(BTN2_PIN);

  // If there are victories, alternate LED states every 500 milliseconds
  if (num_victories > 0) {
    if (millis() - ms_last_led_blink > LED_BLINK_INTERVAL_MS) {
      ms_last_led_blink = millis();
      last_led_state = !last_led_state;
      digitalWrite(LED1_PIN, last_led_state);
      digitalWrite(LED2_PIN, !last_led_state);
    }
  } else {
    // Turn off LEDs if there are no victories
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
  }
}
