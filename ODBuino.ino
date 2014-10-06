/* 
   ODB-II Vehicle ECU Wireless BT Real Time Information System
   @author papadkostas
   @email kostas.papadopulos@gmail.com
   
   ######################
   ODB-II Module Address 00:0D:18:00:00:01(000D,18,000001)
*/
#define SerialMonitor
#include <Wire.h>
#include <BH1750FVI.h>
#include <EasyScheduler.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define StatusInput 4
#define StatusLed 13
#define PowerPin 10
#define ResetPin 9
#define ModePin 8
#define LCDled 3
#define RxD 11
#define TxD 12
Schedular Light;
BH1750FVI LightSensor;
SoftwareSerial BTComm(RxD,TxD);
LiquidCrystal_I2C lcd(0x27,20,4);

boolean bt_mode=0;
boolean bt_status;
boolean bt_error_flag;
int SensorValue=0;
uint16_t Light_Intensity=0;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup(){
  /* Serial Comm Init */
  Serial.begin(9600);
  BTComm.begin(38400);
  Light.start();
  LightSensor.begin();
  LightSensor.SetAddress(Device_Address_L);
  LightSensor.SetMode(Continuous_H_resolution_Mode);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.print("ODB-II SCAN TOOL");
  pinMode(LCDled,OUTPUT);
  analogWrite(LCDled, 255);
  /* HC-05 Control Pins Init */
  pinMode(PowerPin, OUTPUT);
  digitalWrite(PowerPin, HIGH);
  pinMode(ResetPin, OUTPUT);
  digitalWrite(ResetPin, HIGH);
  pinMode(ModePin, OUTPUT);
  digitalWrite(ModePin, LOW);
  pinMode(StatusLed, OUTPUT);
  digitalWrite(StatusLed, LOW);
  pinMode(StatusInput, INPUT);
  BTconnect();
  //ATmode();
}

void loop(){
  Light.check(BH1750check,2000);
  if(BTComm.available()){
    Serial.write(BTComm.read());
  }
  if(Serial.available()){
    BTComm.write(Serial.read());
  }
}
void ATmode(){
  if(bt_mode==0){
    digitalWrite(ResetPin, LOW);
    digitalWrite(ModePin, HIGH);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    bt_mode=1;
  }
}
void COMmode(){
  if(bt_mode==1){
    digitalWrite(ModePin, LOW);
    digitalWrite(ResetPin, LOW);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    bt_mode=0;
  }
}

boolean BTstatus(){
  bt_status=digitalRead(StatusInput);
  return bt_status;
}

void BTconnect(){
  ATmode();
  BTComm.write("AT+RESET\r\n");
  BTComm.write("AT+ORGL\r\n");
  BTComm.write("AT+ROLE=1\r\n");
  BTComm.write("AT+CMODE=1\r\n");
  BTComm.write("AT+BIND=000D,18,000001\r\n");
  BTComm.write("AT+INIT\r\n");
  BTComm.write("AT+PAIR=000D,18,000001,20\r\n");
  BTComm.write("AT+LINK=000D,18,000001\r\n");
}

void BH1750check(){
  Light_Intensity=LightSensor.GetLightIntensity();
  delay(50);
  SensorValue=map(Light_Intensity,0,1023,20,255);
  SensorValue=constrain(SensorValue,0,255);
  analogWrite(LCDled,SensorValue);
}
