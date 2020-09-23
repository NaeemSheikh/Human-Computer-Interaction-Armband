#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

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

//program variables
float IMU_h = 0, IMU_v = 0, IMU_hRest = 0, IMU_vRest = 0, IMU_UP = 0, IMU_DWN = 0, IMU_LFT = 0, IMU_RT = 0;
int c_IMU = 0;
int route = 9;
#define LED 13

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);
  while (!Serial); // wait for Leonardo enumeration, others continue immediately

  // initialize device
  //Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  //Serial.println(F("Testing device connections..."));
  //Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  delay(500);

  // load and configure the DMP
  //Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    //Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    //Serial.print(F("DMP Initialization failed (code "));
    //Serial.print(devStatus);
    //Serial.println(F(")"));
  }
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
}

void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {

  }

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    //Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;
    // display Euler angles in degrees
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
          //Serial.println("Move into UP position:");

        }
        break;
      case 2:

        IMU_UP += ypr[1] * 180 / M_PI;

        c_IMU++;
        if (c_IMU == 499) {
          IMU_UP /= c_IMU;
          route++;

          // calibrate DOWN value
          c_IMU = 0;
          digitalWrite(LED, HIGH);
          //Serial.println("Move into DOWN position:");

        }
        break;
      case 3:
        IMU_DWN += ypr[1] * 180 / M_PI;

        c_IMU++;
        if (c_IMU == 499) {
          IMU_DWN /= c_IMU;

          // calibrate LEFT value
          c_IMU = 0;
          digitalWrite(LED, LOW);
          //Serial.println("Move into LEFT position:");

          route++;
        }
        break;
      case 4:

        IMU_LFT += ypr[2] * 180 / M_PI;

        c_IMU++;

        if (c_IMU == 499) {
          IMU_LFT /= c_IMU;

          // calibrate RIGHT value
          c_IMU = 0;
          digitalWrite(LED, HIGH);
          //Serial.println("Move into RIGHT position:");

          route++;
        }
        break;
      case 5:


        IMU_RT += ypr[2] * 180 / M_PI;

        c_IMU++;

        if (c_IMU == 499) {
          IMU_RT /= c_IMU;

          //Serial.println("CALIBRATING DONE");
          route++;
        }
        break;

      default:
        IMU_h = ypr[2] * 180 / M_PI;
        IMU_v = ypr[1] * 180 / M_PI;
        delay(5);

        // SEND value in form ( IMU_vRest, IMU_UP, IMU_DWN, IMU_v, IMU_hRest, IMU_LFT, IMU_RT, IMU_h )
        Serial.print(IMU_vRest);
        Serial.print("~");
        Serial.print(IMU_UP);
        Serial.print("<");
        Serial.print(IMU_DWN);
        Serial.print(">");
        Serial.print(IMU_v);
        Serial.print("/");
        Serial.print(IMU_hRest);
        Serial.print(";");
        Serial.print(IMU_LFT);
        Serial.print(":");
        Serial.print(IMU_RT);
        Serial.print("'");
        Serial.print(IMU_h);
        Serial.print("|");
        break;
    }
  }
}
