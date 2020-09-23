/*
  Program: Test Program For Car Control Without HC-05

  To test the programs on the two boards, without HC-05 connected. 
  Both boards connected to same computer, this Processing sketch acts as 
  the gateway to send data from source Arduino board to the recipient board.
*/

import processing.serial.*;

Serial port;
Serial port2;

int xpos = 100, ypos = 100;
float emgAmp = 0, IMU_vRest =0, IMU_v = 0, IMU_hRest = 0, IMU_h = 0;
String data = "";
int index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;

boolean state = false;
int spd, dir;

void setup() {
  //size(200,200);
  port = new Serial(this, "COM8", 38400);
  port.bufferUntil('$');
  port2 = new Serial(this, "COM19", 38400);
  // Parameters go inside the parentheses when the object is constructed.
  
}

void draw() {
  println(data);
  if (data != null)
  port2.write(data);
}

void serialEvent (Serial port) {
  data = port.readStringUntil('$');
  
  data = data.substring(0, data.length() - 1);
 
}