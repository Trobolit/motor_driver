#include <elapsedMillis.h>
#include <Servo.h>

#define BAUDRATE 57600 // Serial baudrate
#define SIZE 6 // Size of incoming data in bytes

// DC motor pins
#define ML 5     //M1 Speed Control
#define MR 6     //M2 Speed Control

Servo motorL;
Servo motorR;

// Serial data in will be 6 bytes: SOM, L_dir, L_speed, R_dir, R_speed, EOM
// where: L_dir = 0 meanst stop, 1 forward and 2 rev, same for R
//        speed is a byte from 0-200 determining throttle position (201 - 255 are reserved.)
// special bytes:
// SOM (start of message): 250, EOM (end of message): 251

// Byte array to hold incoming serial data
byte bytes[SIZE];
short motors[SIZE-2];

// Timer for communications problems check
elapsedMillis timer0;

void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  blinks(5, 100);

  // Initialize DC motor pins
  pinMode(ML, OUTPUT);
  pinMode(MR, OUTPUT);
  motorL.attach(ML);
  motorR.attach(MR);

  for (int i = 0; i < SIZE-2; i++) {
    motors[i] = 0;
  }

  // Initialize serial
  Serial.begin(BAUDRATE);
  // Serial.println("COMMUNICATION ESTABLISHED !!\n");
}

void loop(void)
{
  // Clear bytes array
  for (int i = 0; i < SIZE; i++) {
    bytes[i] = 0;
  }

  // Check for connection problems, if nothing in for more than .5 secs stop motors
  while (Serial.available() < SIZE) {
    if (timer0 > 500) {
      timer0 = 0;
      motorL.writeMicroseconds(1500);
      motorR.writeMicroseconds(1500);
    }

    delay(10);
  }

  // We now have a full message read, search for SOM while keeping led lit.
  digitalWrite(LED_BUILTIN, HIGH);
  while(Serial.read() != 250); // Wait for SOM
  bytes[0] = 250;
  digitalWrite(LED_BUILTIN, LOW);
  
  // Fill bytes array with serial data
  while (Serial.available() >= SIZE-1) { 
    for (int i = 1; i < SIZE; i++) {
      bytes[i] = Serial.read();
    }
    //blinks(1,50);
  }

  //  0 meanst stop, 1 forward and 2 rev
  // SOM, L_dir, L_speed, R_dir, R_speed, EOM

  // If serial data looks ok, fill data array
  if (bytes[SIZE - 1] == 251) {
      //digitalWrite(LED_BUILTIN, HIGH);
      if (bytes[1] == 0) {
        motors[0] = 0;
        motors[1] = 0;
        
      } else if (bytes[1] == 1) {
        motors[0] = 1;
        motors[1] = round(2.5*bytes[2]);
      } else {
        motors[0] = -1;
        motors[1] = round(2.5*bytes[2]);
      }

      if (bytes[3] == 0) {
        motors[2] = 0;
        motors[3] = 0;
        
      } else if (bytes[3] == 1) {
        motors[2] = 1;
        motors[3] = round(2.5*bytes[4]);
      } else {
        motors[2] = -1;
        motors[3] = round(2.5*bytes[4]);
      }
       
      
      motorL.writeMicroseconds(1500+motors[0]*motors[1]);
      motorR.writeMicroseconds(1500+motors[2]*motors[3]);
      //digitalWrite(LED_BUILTIN, LOW);
      //blinks(3,50);

  } else {
    //blinks(2,50);
    digitalWrite(LED_BUILTIN, HIGH);
    motorL.writeMicroseconds(1500);
    motorR.writeMicroseconds(1500);
    digitalWrite(LED_BUILTIN, LOW);
  }



  timer0 = 0;
  //delay(10);
}


void blinks(int n, int d) {
  for(int i=0;i<n;i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(d);
      digitalWrite(LED_BUILTIN, LOW);
      delay(d);
  }
}

