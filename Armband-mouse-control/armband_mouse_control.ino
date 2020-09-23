/* 
  Program: Control Computer Mouse Cursor Using The Armband

  This program uses the Arduino Leonardo board which has a Mouse library that can be used to 
  control the mouse cursor of the PC using the board.
  Data is sent wirelessly from the HCI armband via HC-05 bluetooth module's at both boards. 
  We take the data received from the HCI armband, and use the data as follows:
  EMG data (Making the hand into fist)--> Mouse CLICK.
  IMU Accelerometer x,y data --> Mouse cursor MOVE on screen.
*/

#include <Mouse.h>

String data = "";
String emgAmpS = "", emgAmp2S = "", IMU_hS = "", IMU_vS = "", IMU_hRestS = "", IMU_vRestS = "";
int index0 =0, index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;
float IMU_h = 0, IMU_v = 0, IMU_hRest = 0, IMU_vRest = 0;
int xpos = 0, ypos = 0;
int emgAmp = 0, emgAmp2 = 0;

void setup() {
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  Serial.begin(38400);
  Serial1.begin(38400);
}

void loop() {
  if (Serial1.available() > 0) {
    data = Serial1.readStringUntil('|');
    Serial.println(data);
    //data = data.substring(0, data.length() - 1);
    //Serial.println(data);
    index0 = data.indexOf("~");
    index1 = data.indexOf("`");
    index2 = data.indexOf("<");
    index3 = data.indexOf(",");
    index4 = data.indexOf(">");
    index5 = data.indexOf(";");
    
    emgAmpS = data.substring(index0 + 1, index1);
    emgAmp = emgAmpS.toInt();
    IMU_vRestS = data.substring(index1 + 1, index2);
    IMU_vRest = IMU_vRestS.toFloat();
    IMU_vS = data.substring(index2 + 1, index3);
    IMU_v = IMU_vS.toFloat();
    IMU_hRestS = data.substring(index3 + 1, index4);
    IMU_hRest = IMU_hRestS.toFloat();
    IMU_hS = data.substring(index4 + 1, index5);
    IMU_h = IMU_hS.toFloat();    
    emgAmp2S = data.substring(index5+1, data.length());
    emgAmp2 = emgAmp2S.toInt();
  }

  if (digitalRead(2) == LOW) {

    Mouse.begin();

    //move cursor
    xpos = int(map(IMU_h, -100, 100, -128, 128));
    ypos = int(map(IMU_v, 100, -100, -128, 128));
    Mouse.move(xpos, ypos, 0);
    delay(100);
    
    //clickRight
    if(emgAmp2 == HIGH){
      Mouse.press(MOUSE_RIGHT);
      delay(100);
      Mouse.release(MOUSE_RIGHT);
      }
      

    //clickLeft
    if(emgAmp == HIGH){
      Mouse.press(MOUSE_LEFT);
      delay(100);
      Mouse.release(MOUSE_LEFT);
      }  
  }
  else {
    Mouse.end();
  }
}
