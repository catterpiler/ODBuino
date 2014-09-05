/* 
   ODB 2 Vehicle Real Time Information
   @author papadkostas
   @email kostas.papadopulos@gmail.com
*/

#include <SoftwareSerial.h>
#define RxD 11
#define TxD 12
#define PowerPin 10
#define ResetPin 9
#define ModePin 8
SoftwareSerial BTComm(RxD,TxD);
boolean mode = false;
void setup(){
  /* Serial Comm Init */
  Serial.begin(9600);
  BTComm.begin(38400);
  /* HC-05 Control Pins Init */
  pinMode(PowerPin, OUTPUT);
  digitalWrite(PowerPin, HIGH);
  pinMode(ResetPin, OUTPUT);
  digitalWrite(ResetPin, HIGH);
  pinMode(ModePin, OUTPUT);
  digitalWrite(ModePin, LOW);
}

void loop(){
  ATmode();
  if(BTComm.available()){
    Serial.write(BTComm.read());
  }
  if(Serial.available()){
    BTComm.write(Serial.read());
  }
}
void ATmode(){
  if(mode==false){
    digitalWrite(ResetPin, LOW);
    digitalWrite(ModePin, HIGH);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    mode = true;
  }
}
void COMmode(){
  if(mode==true){
    digitalWrite(ModePin, LOW);
    digitalWrite(ResetPin, LOW);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    mode = false;
  }
}
