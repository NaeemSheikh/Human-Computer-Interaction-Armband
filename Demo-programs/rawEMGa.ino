int graphPin = 0;
int emgVal = 0;

void setup() {
  Serial.begin(115200);  
}

void loop(){
  emgVal = analogRead(graphPin);
  Serial.println(emgVal);
  //delay(1); 
}
