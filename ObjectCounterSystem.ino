#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Ultrasonic Sensor 1
#define TRIG1 2
#define ECHO1 3

// Ultrasonic Sensor 2
#define TRIG2 4
#define ECHO2 5

int count = 0;

// State Machine
// 0 = Waiting
// 1 = Sensor1 Triggered (Entry)
// 2 = Sensor2 Triggered (Exit)
// 3 = Wait until both sensors are free
int state = 0;

// ---------------- Distance Function ----------------
long distance(int trig, int echo) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);

  return duration * 0.034 / 2;
}

// ---------------- OLED Display ----------------
void oledDisplay() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(5,0);
  display.println("OBJECT");

  display.setCursor(5,20);
  display.println("COUNTER");

  display.setTextSize(2);
  display.setCursor(0,48);
  display.print("Count:");
  display.print(count);

  display.display();
}

// ---------------- Setup ----------------
void setup() {

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
    while(true);
  }

  display.clearDisplay();
  oledDisplay();
}

// ---------------- Main Loop ----------------
void loop() {

  long d1 = distance(TRIG1, ECHO1);
  long d2 = distance(TRIG2, ECHO2);

  // -------- Serial Monitor --------
  Serial.print("Sensor1: ");
  Serial.print(d1);
  Serial.print(" cm   ");

  Serial.print("Sensor2: ");
  Serial.print(d2);
  Serial.println(" cm");

  switch(state){

    // Waiting
    case 0:

      if(d1 < 8){
        state = 1;
      }

      else if(d2 < 8){
        state = 2;
      }

      break;

    // Entry
    case 1:

      if(d2 < 8){

        count++;

        oledDisplay();

        Serial.println(">>> Object Entered");
        Serial.print("Current Count = ");
        Serial.println(count);

        state = 3;
      }

      break;

    // Exit
    case 2:

      if(d1 < 8){

        if(count > 0)
          count--;

        oledDisplay();

        Serial.println("<<< Object Exited");
        Serial.print("Current Count = ");
        Serial.println(count);

        state = 3;
      }

      break;

    // Wait until object leaves both sensors
    case 3:

      if(d1 > 15 && d2 > 15){

        state = 0;
      }

      break;
  }

  delay(100);
}