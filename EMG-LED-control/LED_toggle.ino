/* 
  Program: Toggle LED ON/OFF Using EMG

  This program uses the Arduino board to take the incoming EMG signals from the EMG amplifier 
  circuit, and use the EMG signal to control the LED power state.
  If the EMG signal is above the minimum set threshold, the LED power state is toggled
  between ON and OFF.
*/

#define EMG 0
#define LED 13

float th = 0;
float thHigh = 0;

int emgAmp = 0;

boolean ledState = false;

float threshold(){
    int sample = 0;
    
    while ( millis() <= 1000 ){
      th += analogRead(EMG);
      sample++; 
      }  
      
    th /= sample;
    Serial.print("Average rest EMG value is ");
    Serial.println(th);
    
    return th;
  }

void setup() {
  Serial.begin(115200);
  
  th = threshold();
  thHigh = 1024 * 0.12 + th;

  pinMode(LED, OUTPUT); 
  
}

void ledToggle(int emgAmp){
  if ( emgAmp >= thHigh){
      ledState = !ledState;
      digitalWrite(LED, ledState);
      delay (500);
    }
  }

void loop(){
  emgAmp = analogRead(EMG);
  Serial.println(emgAmp);
  delay(1);

  ledToggle(emgAmp);  
}
