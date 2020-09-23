# Control A Robotic Car Using The Armband

__Tools Used:__

* Arduino Uno
* HC-05
* Robot Car setup (the motors, power supply, H-bridge, etc.)

__Description:__

In this application, the data sent from the Armband sensors is received by the bluetooth module and parsed by the recipient arduino board and used to control the robot car.  
The data is used as follows:
* EMG data (Making the hand into fist)--> Toggles the robot car between ON and STANDBY state. In the STANDBY state, the car will ignore other incoming data, and wait for the EMG amplitude to be large enough to trigger the switch to ON state. In the ON state, the robot car will take IMU data to move the car.
* IMU data --> The MPU6050 dat is used to control the movement of the robot car. This means the data from the armband is responsible for the speed and the direction the robot car moves in.
