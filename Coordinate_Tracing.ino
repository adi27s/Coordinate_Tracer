#include <Arduino.h>
#include <TimerOne.h>
#include <Servo.h>
#include <SoftwareSerial.h>
Servo servo_9;

#define ULTRASONIC_PIN_INPUT 3
#define ULTRASONIC_PIN_OUTPUT 18
#define ULTRASONIC_TIMER_US 50000
#define pi 3.14159265359

const unsigned int MAX_MESSAGE_LENGTH = 3;
int val = 1;
int global = 0;
int pos = 0;
unsigned int counter = 0;
volatile long ultrasonic_echo_start = 0;
volatile long ultrasonic_distance = 0;
float x1 = 0; // final coordinates
float y1 = 0;
float x = 0; //trace coordinates
float y = 0;
//String X;
//String Y;
//String Z;
float z = 9999;
int theta = 0;
float D = 0;
float l = 0;
float r = 0;

SoftwareSerial BTserial(0, 1); // RX, TX

void docount()
{
  counter++;
  Serial.println(counter);
}

void ultrasonicPulse() {

  digitalWrite(ULTRASONIC_PIN_OUTPUT, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_PIN_OUTPUT, LOW);
  ultrasonic_echo_start = micros();
}

void ultrasonicEcho() {
  if (ultrasonic_echo_start != 0) {
    ultrasonic_distance = (micros() - ultrasonic_echo_start) / 58;
    ultrasonic_echo_start = 0;
    //Serial.print("Distance: ");
    //  Serial.println(ultrasonic_distance-6);
    if ((ultrasonic_distance - 6 > 8) && (ultrasonic_distance - 6 < 20))
      global = 1;
    else
      global = 0;
  }
}

int degree(float xi, float yi, float xf, float yf)
{
  float m, n;
  m = ((yf - yi) / (xf - xi));
  // Serial.println("The value of m is : " + String(m));
  n = (atan2((yf - yi), (xf - xi))) * (180 / pi);
  // Serial.println("The value of n is : " + String(int(n)));
  return int(n);
}

float distance(float xi, float yi, float xf, float yf)
{
  float n;
  n = sqrt(sq(xf - xi) + sq(yf - yi)) ;
  return n;
}

void fwd(float c = 183)     // c = 183 for 50cm or 365 for 100cm
{
  counter = 0;
  Serial.print(c);
  while (counter <= c)
  {
    if (global == 1) {
      Serial.println("Fwd break activated");
      analogWrite(8, 0);
      analogWrite(7, 0);
      analogWrite(6, 0);
      analogWrite(5, 0);
      break;
    }
    analogWrite(8, 60);
    analogWrite(7, 0);
    analogWrite(6, 60);
    analogWrite(5, 0);
  }
  x = (x + (distance_calculator(counter) * cos(degToRad(theta))));
  y = (y + (distance_calculator(counter) * sin(degToRad(theta))));
  counter = 0;
}

int angle_calculator(int a)
{
  int b;
  b = (abs(theta - a) * 40) / 90;
  return b;  // number of counts required in return
}

float distance_calculator(float a)
{
  float b;
  b = a / 365;
  return b;    // distance covered
}

float degToRad(int q)
{
  float b = (q * pi) / 180;
  return b;
}

void rt(int e, int angle) // e is the angle reading calculated using anglecalculator function
{
  counter = 0;
  Serial.print(e);
  while (counter < e)
  {
    analogWrite(6, 0);
    analogWrite(5, 70);
    analogWrite(8, 70);
    analogWrite(7, 0);
  }
  theta = angle;
  counter = 0;
}

void lt(int f, int angle) // f is the angle reading calculated using anglecalculator function
{
  counter = 0;
  Serial.print(f);
  while (counter < f)
  {
    analogWrite(6, 70);
    analogWrite(5, 0);
    analogWrite(8, 0);
    analogWrite(7, 70);
  }
  theta = angle;
  Serial.println("shd print 45");
  Serial.println(theta);
  counter = 0;
}

float dist( float p)
{
  float h;
  h = p * 365;
  return h;
}

void setup() {

  Serial.begin(9600);
  BTserial.begin(9600);

  servo_9.attach(9);
  pinMode(ULTRASONIC_PIN_INPUT, INPUT);
  pinMode(ULTRASONIC_PIN_OUTPUT, OUTPUT);

  Timer1.initialize(ULTRASONIC_TIMER_US);
  Timer1.attachInterrupt(ultrasonicPulse);
  attachInterrupt(digitalPinToInterrupt(ULTRASONIC_PIN_INPUT), ultrasonicEcho, FALLING); //FALLING
  attachInterrupt(digitalPinToInterrupt(2), docount, RISING); //RISING
}

void loop() {
  if ((x1 == 0) && (y1 == 0)) {
    //Check to see if anything is available in the serial receive buffer
    while (Serial.available() > 0)
    {
      //Create a place to hold the incoming message
      static char message[MAX_MESSAGE_LENGTH];
      static unsigned int message_pos = 0;

      //Read the next available byte in the serial receive buffer
      char inByte = Serial.read();

      //Message coming in (check not terminating character) and guard for over message size
      if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
      {
        //Add the incoming byte to our message
        message[message_pos] = inByte;
        message_pos++;
      }
      //Full message received...
      else
      {
        //Add null character to string
        message[message_pos] = '\0';

        //Print the message (or do other things)
        x1 = float((message[0]) - '0');
        y1 = float((message[1]) - '0');

        Serial.println("Coordinates : " + String(x1) + "|" + String(y1));

        //Reset for the next message
        message_pos = 0;
      }
    }
  }
  else
  {
    if (global == 1)
    {
      for (pos = 90; pos <= 210; pos++)
      {
        Serial.println("Pos = " + String(pos));
        servo_9.write(pos);
      }
      // CHECK WITH ULTRASONIC SENSOR L ------------------------------------------------------------------<
      l = ultrasonic_distance - 6;

      for (pos = 210; pos >= 90; pos--)
      {
        Serial.println("Pos = " + String(pos));
        servo_9.write(pos);
      }
      for (pos = 90; pos >= 0; pos--)
      {
        Serial.println("Pos = " + String(pos));
        servo_9.write(pos);
      }
      // CHECK WITH ULTRASONIC SENSOR R ------------------------------------------------------------------<
      r = ultrasonic_distance - 6;

      for (pos = 0; pos <= 90; pos++)
      {
        Serial.println("Pos = " + String(pos));
        servo_9.write(pos);
      }

      if (l < 25)
      {
        lt(angle_calculator(40), theta + 40);
        fwd(146);
      }
      else if (r < 25)
      {
        rt(angle_calculator(40), theta - 40);
        fwd(146);
      }
      else
      {
        lt(angle_calculator(40), theta + 40);
        fwd(146);
      }
    }
    else
    {
      if (theta != degree(x, y, x1, y1))
      {
        if ( theta < (degree(x, y, x1, y1) - 2))
        {
          Serial.println("The value of theta is :");
          Serial.println(theta);
          int row;
          row = degree(x, y, x1, y1) - theta;
          lt(angle_calculator(row), degree(x, y, x1, y1));
        }
        if (theta > (degree(x, y, x1, y1) + 2))
        {
          Serial.println("The value of theta is :");
          Serial.println(theta);
          int row;
          row = theta - degree(x, y, x1, y1);
          rt(angle_calculator(row), degree(x, y, x1, y1));
        }
      }
      Serial.print("Current angle : ");
      Serial.println(theta);
      Serial.print("X coordinate : ");
      Serial.println(x);
      Serial.print("Y coordinate : ");
      Serial.println(y);
      D = distance(x, y, x1, y1);
      if (D >= 0.5)
        fwd();
      else if (D < 0.5)
      {
        float w = dist(D);
        fwd(w);
        Serial.println("Destination Reached");
        analogWrite(8, 0);
        analogWrite(7, 0);
        analogWrite(6, 0);
        analogWrite(5, 0);
        delay(50);
        exit(0);

      }
    }
  }
}
