/* 
  Program: Control Robot Car Using The Armband

  This program uses the data received by the attached Arduino board to control the movement 
  speed and direction of the robot car.
  Data is sent wirelessly from the HCI armband via HC-05 bluetooth module's at both boards. 
  We take the data received from the HCI armband, and use the data as follows:

  * EMG data (Making the hand into fist)--> Toggles the car between ON and STANDBY state. 
    - In the STANDBY state, the car will ignore other incoming data, and wait for the 
      EMG amplitude to be large enough to trigger the switch to ON state. 
    - In the ON state, the robot car will take IMU data to move the car.
    
  * IMU data --> The MPU6050 data is used to control the movement of the robot car. 
    - This means the data from the armband is responsible for the speed and the direction
      the robot car moves in.
*/

//L293 Connection
const int motorA1  = 7;  // Pin  2 of L293
const int motorA2  = 8;  // Pin  7 of L293
const int motorB1  = 10; // Pin 10 of L293
const int motorB2  = 9;  // Pin 14 of L293
const int enableA = 6; // Pin 1 of L293
const int enableB = 11; // Pin 9 of L293

//Bluetooth (HC-06 JY-MCU) State pin on pin 2 of Arduino
const int BTState = 2;

//Useful Variables
int i = 0;
int j = 0;
int vSpeed = 200;   // Default speed, from 0 to 255

//Data read variables
String data = "";
String IMU_hS = "", IMU_vS = "", IMU_hRestS = "", IMU_vRestS = "";
int index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;
boolean emgState = false, pEmgState = false, motorState = false;
float IMU_h = 0, IMU_v = 0, IMU_hRest = 0, IMU_vRest = 0;

void setup() {
  // Set pins as outputs:
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);

  pinMode(BTState, INPUT);
  
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

}

void loop() {
  //Stop car when connection lost or bluetooth disconnected
  if (digitalRead(BTState) == LOW) {
    motorState = false;
  }

  else if (emgState != pEmgState) {
    //toggle motor state
    motorState != motorState;
    delay(1000);
  }
  
  else {
    /***********************Set Speed***********************/
    if (IMU_v > IMU_vRest) {
      if (IMU_v <= 100)
        vSpeed = int(map(IMU_v, IMU_vRest, 100, 0, 255));
      else vSpeed = 255;
    }
    
    else if (IMU_v < IMU_vRest) {
      if (IMU_v > -100)
        vSpeed = int(map(IMU_v, -100, IMU_vRest, 0, 255));
      else vSpeed = 255;
    }
    
    /*********************Set Direction*********************/
    if (IMU_h <= (IMU_hRest + 36) && IMU_h > (IMU_hRest - 36)) {
      analogWrite(enableA, vSpeed); analogWrite(enableB, vSpeed);
      if (IMU_v > IMU_vRest) {
        // FWD movement
        digitalWrite(motorA1, motorState); digitalWrite(motorA2, LOW);
        digitalWrite(motorB1, motorState); digitalWrite(motorB2, LOW);
      }
      else if (IMU_v < IMU_vRest) {
        // BKWD movement
        digitalWrite(motorA1, LOW); digitalWrite(motorA2, motorState);
        digitalWrite(motorB1, LOW); digitalWrite(motorB2, motorState);
      }
    }

    else if (IMU_h <= (IMU_hRest - 36)) {
      // RT movement
      analogWrite(enableA, vSpeed); analogWrite(enableB, vSpeed);
      digitalWrite(motorA1, LOW); digitalWrite(motorA2, motorState);
      digitalWrite(motorB1, motorState); digitalWrite(motorB2, LOW);
    }

    else if (IMU_h > (IMU_hRest + 36)) {
      // LFT movement
      analogWrite(enableA, vSpeed); analogWrite(enableB, vSpeed);
      digitalWrite(motorA1, motorState); digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, LOW); digitalWrite(motorB2, motorState);
    }
  }
}

void serialEvent () {
  data = Serial.readStringUntil('|');
  data = data.substring(0, data.length() - 1);

  index1 = data.indexOf("~");
  index2 = data.indexOf("`");
  index3 = data.indexOf("<");
  index4 = data.indexOf(",");
  index5 = data.indexOf(">");

  pEmgState = emgState;
  emgState = boolean (data.substring(index1+1, index2));
  
  IMU_vRestS = data.substring(index2+1, index3);
  IMU_vRest = IMU_vRestS.toFloat();
  IMU_vS = data.substring(index3+1, index4);
  IMU_v = IMU_vS.toFloat();
  IMU_hRestS = data.substring(index4+1, index5);
  IMU_hRest = IMU_hRestS.toFloat();
  IMU_hS = data.substring(index5+1, data.length());
  IMU_h = IMU_hS.toFloat();

}



