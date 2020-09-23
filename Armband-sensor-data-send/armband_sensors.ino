/* 
  Program: HCI Armband To Control Devices Using Forearm Movements & Gestures

  This program uses the sensors attached to this board to collect data which is then used
  to control other devices (like a robot car) which are programmed to use the data sent 
  from the armband to act a certain way.
  This program sends the EMG amplitude, and the relevant data from the MPU6050 IMU.
*/

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#define LED 13
#define EMG 0

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

//IMU variables
float IMU_h = 0, IMU_v = 0, IMU_hRest = 0, IMU_vRest = 0;
int c_IMU = 0, route = 1;
float th = 0, thHigh = 0;

//EMG variables
int emgAmp = 0;
boolean emgState = false;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}


void setup() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(9600);
  while (!Serial);

  mpu.initialize();
  delay(500);
  devStatus = mpu.dmpInitialize();

  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {}

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

}


void loop() {
  if (!dmpReady) return;
  while (!mpuInterrupt && fifoCount < packetSize) {}

  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  fifoCount = mpu.getFIFOCount();
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
  }
  else if (mpuIntStatus & 0x02) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    switch (route) {
      case 1:
        IMU_vRest += ypr[1] * 180 / M_PI;
        IMU_hRest += ypr[2] * 180 / M_PI;

        c_IMU++;

        if (c_IMU == 499) {
          IMU_hRest /= c_IMU;
          IMU_vRest /= c_IMU;
          route++;
          // calibrate UP value
          c_IMU = 0;
          digitalWrite(LED, LOW);
        }
        break;
      default:
        emgAmp = analogRead(EMG);
        //Serial.println(emgAmp);
        //emgState = Toggle(emgAmp);

        IMU_v = ypr[1] * 180 / M_PI;
        IMU_h = ypr[2] * 180 / M_PI;
        //delay(5);

        // SEND value in form ( ~emgState`IMU_vRest<IMU_v,IMU_hRest>IMU_h| )
        Serial.print("~"); Serial.print(emgAmp);//Serial.print(emgState);
        Serial.print("`"); Serial.print(IMU_vRest);
        Serial.print("<"); Serial.print(IMU_v);
        Serial.print(","); Serial.print(IMU_hRest);
        Serial.print(">"); Serial.print(IMU_h);
        Serial.print("|");
        delay(100);
        break;
    }
  }
}
