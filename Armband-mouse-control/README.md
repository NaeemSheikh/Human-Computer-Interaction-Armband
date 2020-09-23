# Control Computer Mouse Cursor Using The Armband

__Tools Used:__

* Arduino Leonardo (other boards that have capability of using the Mouse library will work as well)
* HC-05
* Computer (of which the Leonardo board will be connected to and control the mouse of)

__Description:__

This program is used to control the Mouse Cursor of the PC that is connected to the arduino board with this program. 

In this application, the data sent from the Armband sensors is received by the bluetooth module and parsed by the leonardo board. 
The data is used as follows:
* EMG data (Making the hand into fist)--> Mouse CLICK.
* IMU Accelerometer x,y data --> Mouse cursor MOVE on screen.
