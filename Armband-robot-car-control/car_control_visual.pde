/*
  Program: Test Program For Using Armband data To Control A Robot Car

  This Processing sketch is an alternative way to collect the data from the 
  Armband and use this data to move a visual representation of a car (rectangle)  
  in a 2D world (the window). 

  The data is used as follows:

  * EMG data (Making the hand into fist)--> Toggles the car between ON and STANDBY state. 
    - In the STANDBY state, the car will ignore other incoming data, and wait for the 
      EMG amplitude to be large enough to trigger the switch to ON state. 
    - In the ON state, the robot car will take IMU data to move the car.
    
  * IMU data --> The MPU6050 data is used to control the movement of the robot car. 
    - This means the data from the armband is responsible for the speed and the direction
      the robot car moves in.
*/

import processing.serial.*;

Serial port;

int xpos = 100, ypos = 100;
float emgAmp = 0, IMU_vRest =0, IMU_v = 0, IMU_hRest = 0, IMU_h = 0;
String data = "";
int index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;

boolean state = false;
int spd, dir;

void setup() {
  size(200,200);
  port = new Serial(this, "COM8",9600);
  port.bufferUntil('|');
  // Parameters go inside the parentheses when the object is constructed.
  
}

void draw() {
  background(125);
  
  fill(0,255,0);
  noStroke();
  rect(100,100,100,height);
   display(xpos,ypos,20,20);
  state = emgCheck(emgAmp, state);
  //println(state);
  if (state == true){
  
  spd = chkSpd(IMU_v, IMU_vRest);
  dir = chkDir(IMU_h);
  }
  else {
  spd = 0;
  }
}
boolean emgCheck(float emgAmp, boolean state){
if (emgAmp  > 10 ){
  state = !state;
  //background(random(0,255));
}
else{}
  
  return state;
}

void display(int xpos,int ypos, int cWidth, int cLength) {
    stroke(0);
    fill(0);
    drive(spd);
    rectMode(CENTER);
    rect(xpos,ypos,cWidth,cLength);
  }
  
  int chkSpd(float Y, float Ya){
    int yspeed=0;
    if (Y >= 0){
    yspeed = int(map(Y, Ya, 100,0, 10));
    }
    else if(Y < 0){
    yspeed = int(map(Y, -100, Ya, -10, 0));
    }
    return yspeed;
  }
  
  int chkDir(float X){
    
    xpos = int(map(X, -100,100,width,0));
    return xpos;
  }

  void drive(int yspeed) {
    ypos += yspeed;
    if (ypos > height){
      ypos = 0;
    }
    else if (ypos < 0)
    ypos = height;
  }

void serialEvent (Serial port) {
  data = port.readStringUntil('|');
  println(data);
  data = data.substring(0, data.length() - 1);
  
  index1 = data.indexOf("~");
  index2 = data.indexOf("`");
  index3 = data.indexOf("<");
  index4 = data.indexOf(",");
  index5 = data.indexOf(">");
  
  emgAmp = float (data.substring(index1+1, index2));
  IMU_vRest = float (data.substring(index2+1, index3));
  IMU_v = float (data.substring(index3+1, index4));
  IMU_hRest = float (data.substring(index4+1, index5));
  IMU_h = float (data.substring(index5+1,data.length()));
   
}