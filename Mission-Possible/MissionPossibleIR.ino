/*
 * @author Dahlia Dry
 * V1
 * Hardware:
 * -Adafruit IR break beam sensor
 * -Atmega328P
 * -DC Motor 250mA
 * breaking of IR beam triggers revolution of DC motor
 * this reels in a string attached to a pair of scissors
 * thus cutting fishing line
 * Created for 2019 Florida Science Olympiad Mission Possible Event 
 */
#define recieverPin 2
#define motorPin 3
int sensorState = 0;
int lastState = 0;
int motorSpeed = 100; //0 to 255
int delayTime = 1000;
void setup(){
  pinMode(recieverPin, INPUT);
  pinMode(motorPin, OUTPUT);
  digitalWrite(recieverPin, HIGH);
  Serial.begin(9600);
}

void loop(){
  sensorState = digitalRead(recieverPin);
  if(sensorState == LOW){ //IR beam broken
     analogWrite(motorPin, motorSpeed);
     delay(delayTime);
     analogWrite(motorPin, 0);
  }
  if(sensorState && !lastState){
    Serial.println("unbroken");
  }
  if(!sensorState && lastState){
    Serial.println("broken");
  }
  lastState = sensorState;
}

