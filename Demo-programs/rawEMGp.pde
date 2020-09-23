import processing.serial.*;

Serial port;
float incr = 0;
float x = 0, y = 0;

void setup(){
  
  size(1000,500);
  
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
  stroke(255,50,50);  
  strokeWeight(1.5);
  port = new Serial(this, "COM8",115200);
  port.bufferUntil('\n');
}

void draw(){
  // background(brightness);
  if (x < width){
    //plot
    x++;
    y = plot(incr, x, y);
    
  }
  
  else{
    saveFrame("rawEMG1-###.jpg");
    x = 0;
    stroke(50);
    strokeWeight(2);
    background(255);
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
    stroke(255,50,50);
  strokeWeight(1.5);
    }
}

float plot(float incr, float x, float y){
  float a = x;
  float b = y;
  
  incr = map(incr,0,1024,0,500);
  y = 500-incr;
  line (a,b,x,y);
  
  return y;
}

void serialEvent (Serial port) {
  incr = float (port.readStringUntil('\n'));
}