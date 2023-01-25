#include "TM1637.h"
//https://github.com/Seeed-Studio/Grove_4Digital_Display/blob/master/TM1637.h

#include "RTClib.h"
//https://github.com/adafruit/RTClib/blob/master/src/RTClib.h

#include "Unistep2.h"

TM1637 display1(3,2);//CLK,DIO
TM1637 display2(5,4);//CLK,DIO
RTC_DS3231 rtc;
DateTime nowt;
Unistep2 stepper(A0,A1,A2,A3,4096,1000);

int nowtime = 0;
int clktime = 630;

bool alarmswitch = 0;

void SerialClear() {
  while(Serial.available()) {
    Serial.read();
    delay(2);
  }
}

void show1(int num){

  Serial.print("nowtime : ");
  Serial.println(num);
  
  display1.display(3,num%10);
  num/=10;
  display1.display(2,num%10);
  num/=10;
  display1.display(1,num%10);
  num/=10;
  display1.display(0,num%10);

  display1._PointFlag = !display1._PointFlag;

  return;
}

void show2(int num){

  Serial.print("clktime : ");
  Serial.println(num);
  
  display2.display(3,num%10);
  num/=10;
  display2.display(2,num%10);
  num/=10;
  display2.display(1,num%10);
  num/=10;
  display2.display(0,num%10);

  display2._PointFlag = 1;

  return;
}

void alarm(){
  //多次旋轉，邊旋轉邊 showing time
  Serial.print("Ring!!!\nRing!!!\nRing!!!\n");
  int i=2;
  while(i){
    stepper.run();
    if ( stepper.stepsToGo() == 0 ){
      i--;
      stepper.move(4069);
    }
  }
  
  
}

void setup() {

  //Serial
  Serial.begin(9600);

  //TM1637 1
  display1.init();
  display1.set(7);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  display1._PointFlag = 1;
  
  //TM1637 2
  display2.init();
  display2.set(7);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  display2._PointFlag = 1;
  

  //test rtc
  if (! rtc.begin()) { 
    Serial.println("Couldn't find RTC");
    Serial.flush(); 
    abort();  
  }

  //alarmswitch
  pinMode(6,INPUT_PULLUP);

  //adjust time 8 down 9 up
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);

  //LED 13
  pinMode(13,OUTPUT);
}

void loop(){
  
  //resest rtc time
  //rtc.adjust(DateTime(2022,12,5,1,31,0));

  //alarm status
  alarmswitch = !digitalRead(6);

  //get now time
  nowt = rtc.now();

  //show time
  nowtime = int(nowt.hour())*100 + int(nowt.minute());
  show1(nowtime);
  
  if(alarmswitch){   
    show2(clktime);
    digitalWrite(13,HIGH); 
  }
  else{
    display2.clearDisplay();
    display2._PointFlag = 0;
    digitalWrite(13,LOW);
  }
    
  //if now = clk -> alarm
  if(nowtime == clktime && alarmswitch){
    alarm();
  }

  //control clktime
  if(digitalRead(10)){
    clktime--;
  }

  if(digitalRead(11)){
    clktime++;
  }
  
  //adjust clktime
  if(clktime%100 == 99){
    clktime-=40;
  }
  if(clktime<0){
    clktime+=2359;
  }
  if(clktime%100 == 60){
    clktime+=40;
  }
  if(clktime/100==24){
    clktime-=2400;
  }

  Serial.print("alarm status：");
  Serial.print(alarmswitch);
  Serial.print("\n\n");
  delay(250);

}
