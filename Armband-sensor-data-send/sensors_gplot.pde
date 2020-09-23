/* 
  Program: Graph Plotter For The Data From The HCI Armband 

  This Processing sketch uses the data from the Armband sensors, and plots them to 
  visually show what different movements and gestures of the hand and forearm output.

  The data from this program can be used to fine tune the thresholds for other 
  potential applications using this armband.
*/

import processing.serial.*;

Serial port;
float qx = 0, qy = 0, qz = 0;
float emg =0, IMU_vRest =0, IMU_hRest =0, IMU_v =0, IMU_h =0;
float x = 0, y1 = 0, y2 = 0, y3 = 0, y4 = 0, y5 =0;
String data = "";
int  index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;

void setup(){
  
  size(1500,1024);
  graph();

  port = new Serial(this, "COM8",9600);
  port.bufferUntil('|');
}

void draw(){
  if (x < width){
    //plot
    x++;
    
    stroke(255,50,50); 
    y1 = plot(emg, x, y1);
    stroke(50,255,50); 
    y2 = plot(IMU_vRest, x, y2);
    stroke(50,50,255); 
    y3 = plot(IMU_hRest, x, y3);
    stroke(255,0,0); 
    y4 = plot(IMU_v, x, y4);
    stroke(204,0,204); 
    y5 = plot(IMU_h, x, y5);
      
  }
  
  else{
    x = 0;
    stroke(50);
    strokeWeight(2);
    graph();
  }
}

void graph(){
    background(255);
  stroke(50);
  strokeWeight(2);
  for (int i =0; i <width; i= i+100){
     for (int j =0; j <height; j= j+100){
       line(i, j, i+100, j);
       line(i, j, i, j+100);
     }
  }
  stroke(100);
  strokeWeight(1);
  for (int i =0; i <width; i= i+20){
     for (int j =0; j <height; j= j+20){
       line(i, j, i+20, j);
       line(i, j, i, j+20);
     }
  } 
  strokeWeight(1.5);
}

float plot(float incr, float x, float y){
  float a = x;
  float b = y;
  
  incr = map(incr,-180,180,0,500);
  y = 500-incr;
  line (a,b,x,y);
  
  return y;
}

void serialEvent (Serial port) {
  data = port.readStringUntil('|');
  data = data.substring(0, data.length() - 1);
  
  index1 = data.indexOf("~");
  index2 = data.indexOf("`");
  index3 = data.indexOf("<");
  index4 = data.indexOf(",");
  index5 = data.indexOf(">");
  
  emg = float (data.substring(index1+1,index2));
  IMU_vRest = float (data.substring(index2+1, index3));
  IMU_v = float (data.substring(index3+1, index4));
  IMU_hRest = float (data.substring(index4+1, index5));
  IMU_h = float (data.substring(index5+1, data.length()));
}