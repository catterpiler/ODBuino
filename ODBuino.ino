/**************************************************************
  * ODB-II ECU Wireless BT Real Time Information System
  * @author  Kostas Papadopulos
  * @email   kostas.papadopulos@gmail.com
  * @version v1.1
  *************************************************************
  * ODB-II Module Address 00:0D:18:00:00:01(000D,18,000001)
  **************************************************************
  * If you are running this sketch for the first time uncomment
  * the next line #define FirstRun
  **************************************************************
  */
#include <SoftwareSerial.h>
//#define FirstRun
#define StatusInput 4
#define PowerPin 10
#define ResetPin 9
#define ModePin 8
#define LCDled 3
#define RxD 11
#define TxD 12

#include <Wire.h>
#include <BH1750FVI.h>
#include <EasyScheduler.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

boolean BT_state;
boolean BT_Mode=0;                                            
boolean BT_Error_Flag;

volatile int pages = 5;
volatile int lastpage = 1;
volatile int currentpage = 1;
uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t degree[8] = {0x1c,0x14,0x1c,0x0,0x0,0x0,0x0};

char ODBbuffer[20];
int SensorValue=0;
uint16_t Light_Intensity=0;
int rpm=0;
Schedular BTcheck;
Schedular LightTask;
Schedular DisplayTask;

BH1750FVI LightSensor;
SoftwareSerial BTComm(RxD,TxD);
LiquidCrystal_I2C lcd(0x27,20,4);

void setup(){
  IOinit();
  BTcheck.start();
  LightTask.start();
  DisplayTask.start();
  LightSensor.begin();
  LightSensor.SetAddress(Device_Address_L);
  LightSensor.SetMode(Continuous_H_resolution_Mode);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.createChar(0, check);
  lcd.createChar(1, cross);
  lcd.createChar(2, degree);
  Serial.begin(9600);
  BTComm.begin(38400);
}

void loop(){
                                
  BTcheck.check(BTstatus,500);
  DisplayTask.check(Display,200);
  LightTask.check(LCDbacklight,1000);
  if(BTComm.available()){
    Serial.write(BTComm.read());
  }
  if(Serial.available()){
    BTComm.write(Serial.read());
  }
}

void BTmode(boolean mode){
  switch(mode){
  case 0:
    digitalWrite(ResetPin, LOW);
    digitalWrite(ModePin, HIGH);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    BT_Mode=0;
    break;
  case 1:
    digitalWrite(ModePin, LOW);
    digitalWrite(ResetPin, LOW);
    delay(100);
    digitalWrite(ResetPin, HIGH);
    BT_Mode=1;
    break;
  }
}

void BTstatus(){
  boolean state=digitalRead(StatusInput);
  if(state==HIGH)
    BT_state=true;
  else{
    BT_state=false;
    currentpage = 1;
  }
}

void BTconnect(){
  BTmode(0);
  BTComm.write("AT+RESET\r\n");
  delay(1000);
    #ifdef FirstRun
    BTComm.write("AT+ORGL\r\n");
    delay(500);
    BTComm.write("AT+ROLE=1\r\n");
    delay(500);
    BTComm.write("AT+CMODE=0\r\n");
    delay(500);
    BTComm.write("AT+BIND=000D,18,000001\r\n");
    delay(500);
    #endif
  BTComm.write("AT+INIT\r\n");
  delay(1000);
  BTComm.write("AT+PAIR=000D,18,000001,20\r\n");
  delay(1000);
  BTComm.write("AT+LINK=000D,18,000001\r\n");
  delay(1000);
  BTmode(1);
}

boolean ODBcmd(char *odb_cmd){
  int i=0;
  char recvChar;
  boolean prompt=false;
  while(!prompt){
    BTComm.print(odb_cmd);
    BTComm.print("\r");
    while(BTComm.available() <= 0);
    while((BTComm.available()>0) && (!prompt)){
      recvChar = BTComm.read();
      if(odb_cmd=="010C1"){
        ODBbuffer[i]=recvChar;
        i++;
      }
      if(recvChar==62)
        return prompt=true;
    }
  }
}

int readRPM(){
  boolean prompt=false,valid=false;  
  char recvChar;
  char bufin[15];
  int i=0;
  BTComm.print("010C1");
  BTComm.print("\r");
  while(BTComm.available() <= 0);
  Serial.println("");
  while((BTComm.available()>0) && (!prompt)){
    recvChar = BTComm.read();
    Serial.print("Serial available");
    Serial.print("i=");
    Serial.println(i);
    if((i<15)&&(!(recvChar==32))){
      bufin[i]=recvChar;
      Serial.print(bufin[i]);
      i=i+1;
    }
    if(recvChar==62)
      prompt=true;
  }
  if((bufin[6]=='4') && (bufin[7]=='1') && (bufin[8]=='0') && (bufin[9]=='C')){
    valid=true;
  }
  else{
    valid=false;
  }
  if(valid){
    rpm=0;                                                                                            
    for (i=10;i<14;i++){                              //in that 4 chars of bufin array which is the RPM value
      if ((bufin[i]>='A') && (bufin[i]<='F')){        //if char is between 'A' and 'F'
        bufin[i]-=55;                                 //'A' is int 65 minus 55 gives 10 which is int value for hex A
      }    
      if ((bufin[i]>='0') && (bufin[i]<='9')){        //if char is between '0' and '9'
        bufin[i]-=48;                                 //'0' is int 48 minus 48 gives 0 same as hex
      }  
      rpm=(rpm << 4) | (bufin[i] & 0xf);              //shift left rpm 4 bits and add the 4 bits of new char 
    }
      rpm=rpm >> 2;                                     //finaly shift right rpm 2 bits, rpm=rpm/4
  }
  if(!valid){
    rpm=-1;
  }
  return rpm;
}

void IOinit(){
  pinMode(LCDled,OUTPUT);
  analogWrite(LCDled, 255);
                                 
  pinMode(PowerPin, OUTPUT);
  digitalWrite(PowerPin, HIGH);
  pinMode(ResetPin, OUTPUT);
  digitalWrite(ResetPin, HIGH);
  pinMode(ModePin, OUTPUT);
  digitalWrite(ModePin, LOW);
  pinMode(StatusInput, INPUT);
}

void LCDbacklight(){
  Light_Intensity=LightSensor.GetLightIntensity();
  delay(50);
  SensorValue=map(Light_Intensity,0,1023,20,255);
  SensorValue=constrain(SensorValue,0,255);
  analogWrite(LCDled,SensorValue);
}
