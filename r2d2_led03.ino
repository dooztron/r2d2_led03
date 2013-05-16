//D:Mega its digital #20(light Red)
//C:Mega its digital #21(White&Black)
//V:Blue
//G:Red


//Blue = 5V
//Red = SDI
//Green = CKI
//Black = GND

#include <Servo.h>      // include the servo library

Servo servoMotor1;
Servo servoMotor2;

char servoPin1 = 5;
char servoPin2 = 4;

int pos1 = 10;
int pos2 = 10;
int led = 13;

unsigned long nextTime = 1500;

boolean servoHigh = false;

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_8x8matrix matrix01 = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix02 = Adafruit_8x8matrix();
int SDI = 2; //Red wire (not the red 5V wire!)
int CKI = 3; //Green wire
int ledPin = 13; //On board LED

#define STRIP_LENGTH 6 //32 LEDs on this strip
long strip_colors[STRIP_LENGTH];

void setup(){
  Serial.begin(9600);
  delay(1000);
  Serial.println("setup begin");
  Serial.write(servoPin1);
  delay(100);
  servoMotor1.attach(servoPin1);
  delay(100);
  servoMotor1.write(pos1);
  delay(100);
  
  Serial.write(servoPin2);
  delay(100);
  servoMotor2.attach(servoPin2);
  delay(100);
  servoMotor2.write(pos2);
  delay(100);
  pinMode(led, OUTPUT);
  matrix01.begin(0x73);  // pass in the address
  delay(100);
  matrix02.begin(0x70);
  delay(100);
  pinMode(SDI, OUTPUT);
  pinMode(CKI, OUTPUT);
  pinMode(ledPin, OUTPUT);
  delay(100);
  //Clear out the array
  for(int x = 0 ; x < STRIP_LENGTH ; x++)
    strip_colors[x] = 0;

  randomSeed(analogRead(0));

  //Serial.begin(9600);
 Serial.println("setup end");
}

static uint8_t PROGMEM
smile_bmp[] =
{ 
  B11111111,
  B11111111,
  B10100101,
  B11111111,
  B10100101,
  B11111111,
  B01000010,
  B11111111 }
,
neutral_bmp[] =
{ 
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100 }
,
frown_bmp[] =
{ 
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100 };



void loop(){
  Serial.print("loop at: ");
  Serial.println(millis());
  //Pre-fill the color array with known values
  strip_colors[0] = 0xFF0000; //Bright Red
  strip_colors[1] = 0x00FF00; //Bright Green
  strip_colors[2] = 0x0000FF; //Bright Blue
  strip_colors[3] = 0x010000; //Faint red
  strip_colors[4] = 0x800000; //1/2 red (0x80 = 128 out of 256)
  post_frame(); //Push the current color frame to the strip
  delay(500);
  
  addRandom();
  delay(50);
  post_frame();
  delay(50);
  moveServo();
  delay(50);
//  updateMatrix();
//  delay(50);

}

void moveServo(void){
  Serial.print("moveServo ");
  Serial.print(" at: ");
  Serial.println(millis());
  Serial.print("-- nexttime is currently: ");
  Serial.println(nextTime);
  unsigned long currTime = millis();
  
  if (currTime > nextTime){
    Serial.println("------ hit timer -----");
    if(!servoHigh){
      Serial.println("servoHigh == false!");
      pos1 = random(40,90);
      pos2 = random(40,90);
      servoMotor1.write((int)pos1);
      delay(50);
      servoMotor2.write((int)pos2);
      delay(50);
      servoHigh = true;
      digitalWrite(led, HIGH);
    }
    else{
      Serial.println("servoHigh == true!");
      pos1 = random(90,140);
      pos2 = random(90,140);
      servoMotor1.write((int)pos1);
      delay(50);
      servoMotor2.write((int)pos2);
      delay(50);
      servoHigh = false;
      digitalWrite(led, LOW);
    }
    nextTime = millis() +3000;
  }
  Serial.println(pos1, pos2);  
}

void updateMatrix(){
  Serial.print("-- nexttime is currently: ");
  Serial.println(nextTime);
  unsigned long currTime = millis();
  
  if (currTime > nextTime){
    Serial.println("------ hit timer -----");
      matrix01.clear();
      matrix02.clear();
      delay(50);
      matrix01.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
      matrix02.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
      matrix01.writeDisplay();
      matrix02.writeDisplay();
  }
  else{
      matrix01.clear();
      matrix02.clear();
      delay(50);
      matrix01.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
      matrix02.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
      matrix01.writeDisplay();
      matrix02.writeDisplay();
  }
    nextTime = millis() +3000;
    matrix01.setRotation(0);
    matrix02.setRotation(0);
}

void addRandom(void) {
  Serial.println("addRandom");
  int x;
  
  //First, shuffle all the current colors down one spot on the strip
  for(x = (STRIP_LENGTH - 1) ; x > 0 ; x--)
    strip_colors[x] = strip_colors[x - 1];
    
  //Now form a new RGB color
  long new_color = 0;
  for(x = 0 ; x < 3 ; x++){
    new_color <<= 8;
    new_color |= random(0xFF); //Give me a number from 0 to 0xFF
    //new_color &= 0xFFFFF0; //Force the random number to just the upper brightness levels. It sort of works.
  }
  
  strip_colors[0] = new_color; //Add the new random color to the strip
}

//Takes the current strip color array and pushes it out
void post_frame (void) {
  Serial.println("post_frame");
  //Each LED requires 24 bits of data
  //MSB: R7, R6, R5..., G7, G6..., B7, B6... B0 
  //Once the 24 bits have been delivered, the IC immediately relays these bits to its neighbor
  //Pulling the clock low for 500us or more causes the IC to post the data.

  for(int LED_number = 0 ; LED_number < STRIP_LENGTH ; LED_number++) {
    long this_led_color = strip_colors[LED_number]; //24 bits of color data

    for(byte color_bit = 23 ; color_bit != 255 ; color_bit--) {
      //Feed color bit 23 first (red data MSB)
      
      digitalWrite(CKI, LOW); //Only change data when clock is low
      
      long mask = 1L << color_bit;
      //The 1'L' forces the 1 to start as a 32 bit number, otherwise it defaults to 16-bit.
      
      if(this_led_color & mask) 
        digitalWrite(SDI, HIGH);
      else
        digitalWrite(SDI, LOW);
  
      digitalWrite(CKI, HIGH); //Data is latched when clock goes high
    }
  }

  //Pull clock low to put strip into reset/post mode
  digitalWrite(CKI, LOW);
  delayMicroseconds(500); //Wait for 500us to go into reset
}
