/* 
   ODB-II Vehicle ECU Wireless BT Real Time Information System
   @author papadkostas
   @email kostas.papadopulos@gmail.com
   
   ######################
   ODB-II Module Address 00:0D:18:00:00:01(000D,18,000001)
*/

#include <Wire.h>
#include <BH1750FVI.h>
#include <EasyScheduler.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define RxD 11
#define TxD 12
#define PowerPin 10
#define ResetPin 9
#define ModePin 8
#define LCDled 3

Schedular Light;
BH1750FVI LightSensor;
SoftwareSerial BTComm(RxD,TxD);
LiquidCrystal_I2C lcd(0x27,20,4);

boolean bt_error_flag;
boolean bt_mode=false;
int SensorValue=0;
uint16_t Light_Intensity=0;

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
  ATmode();
  BTconnect();
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
  if(bt_mode==false){
    digitalWrite(ResetPin, LOW);
    digitalWrite(ModePin, HIGH);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    bt_mode = true;
  }
}
void COMmode(){
  if(bt_mode==true){
    digitalWrite(ModePin, LOW);
    digitalWrite(ResetPin, LOW);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    bt_mode = false;
  }
}

void BTconnect(){
  BTComm.write("AT+RESET\r\n");
  delay(200);
  BTComm.write("AT+ORGL\r\n");
  BTComm.write("AT+CMODE=0\r\n");
  BTComm.write("AT+AT+BIND=2222,66,9D1C4B\r\n");
  BTComm.write("AT+INIT\r\n");
  delay(1000);
  BTComm.write("AT+PAIR=2222,66,9D1C4B,20\r\n");
  delay(1000);
  BTComm.write("AT+LINK=2222,66,9D1C4B\r\n");
  delay(1000);
}

void ATcmd(char *command){
  char recvChar,str[2];
  int retries,i;
}
void BH1750check(){
  Light_Intensity=LightSensor.GetLightIntensity();
  delay(50);
  SensorValue=map(Light_Intensity,0,1023,5,255);
  analogWrite(LCDled,SensorValue);
}
