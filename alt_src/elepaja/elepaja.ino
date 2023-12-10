#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_ADDR 0x3C
#define DP_ADDR 0x2F

// Define encoder and button pins
#define ENCODER_A 3
#define ENCODER_B 4
#define BUTTON_UP 12
#define BUTTON_DOWN 13

volatile int currentCLK = 0;
volatile int lastCLK = 0;
volatile int button1State = 0;
volatile int button2State = 0;

volatile int16_t frequency = 100;   // in MHz

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.display();
  delay(2000);
  
  lastCLK = digitalRead(ENCODER_B);

  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  
  //attachInterrupt(digitalPinToInterrupt(ENCODER_A), handleEncoder, CHANGE);
  
  //attachInterrupt(digitalPinToInterrupt(BUTTON_1), handleButton1, FALLING);
  //attachInterrupt(digitalPinToInterrupt(BUTTON_2), handleButton2, FALLING);
}

void loop() {

  int wiperValue = map(frequency, 90, 110, 0, 126);
  
  int buttonUpState = digitalRead(BUTTON_UP);
  int buttonDownState = digitalRead(BUTTON_DOWN);

  if (buttonUpState == HIGH) {
    frequency += 1;
  }
  if (buttonDownState == HIGH) {
    frequency -= 1;
  }

  handleEncoder();

  frequency = constrain(frequency, 90, 110);
  
  Wire.beginTransmission(DP_ADDR);
  Wire.write(wiperValue);
  Wire.endTransmission();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Freq: ");
  display.print(frequency);
  display.print(" MHz");
  display.display();
  
  delay(1);
}

void handleEncoder() {
  currentCLK = digitalRead(ENCODER_B);
  if (currentCLK != lastCLK && currentCLK == 1){
		if (digitalRead(ENCODER_A) == currentCLK) {
			frequency+=1;
		} 
    if (digitalRead(ENCODER_A) != currentCLK) {
			frequency-=1;
		}
  }
  lastCLK = currentCLK;
}

