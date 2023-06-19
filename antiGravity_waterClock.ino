#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
#include <DS1302.h>


#define NUMPIXELS 24
#define PIN 11

#define POT_BRIGHTNESS 14
#define POT_DELAY 15


#define MAX_BRIGHTNESS 250
#define MIN_BRIGHTNESS 0

#define MAX_DELAY 1000
#define MIN_DELAY 100


#define BLINK_TIME 500

DS1302 rtc(2, 3, 4);
// LiquidCrystal_I2C lcd(2,3,4,5,6,7);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int s,m,h,a,d,state,state1,state2,dg,cnt,dt,mo;
char months[13] = {' ','1','2','3','4','5','6','7','8','9','O','N','D'};
int l[13] = {0,31,29,31,30,31,30,31,31,30,31,30,31};

byte bar1[8] = {B11100,B11110,B11110,B11110, 
                B11110,B11110,B11110,B11100};
byte bar2[8] = {B00111,B01111,B01111,B01111,
                B01111,B01111,B01111,B00111};
byte bar3[8] = {B11111,B11111,B00000,B00000,
                B00000,B00000,B11111,B11111};
byte bar4[8] = {B11110,B11100,B00000,B00000,
                B00000,B00000,B11000,B11100};
byte bar5[8] = {B01111,B00111,B00000,B00000,
                B00000,B00000,B00011,B00111};
byte bar6[8] = {B00000,B00000,B00000,B00000,
                B00000,B00000,B11111,B11111};
byte bar7[8] = {B00000,B00000,B00000,B00000,
                B00000,B00000,B00111,B01111};
byte bar8[8] = {B11111,B11111,B00000,B00000,
                B00000,B00000,B00000,B00000};


Adafruit_NeoPixel Neo = Adafruit_NeoPixel(NUMPIXELS,PIN, NEO_GRB + NEO_KHZ800);


unsigned long prevNeoMS = 0;
unsigned long prevClockMS = 0;


void setup()
{
  Serial.begin(9600);
  
  Neo.begin();
  SetupClock();
  
  prevNeoMS = millis();
  prevClockMS = millis();
}


const uint32_t ON = Neo.Color(255,255,255);
const uint32_t OFF = Neo.Color(0,0,0);

int val_brightness = 0;
int val_delay = 0;


void loop()
{
  NeoLoop();
  ClockLoop();
}


void SetupClock()
{
  rtc.halt(false);
  if(!strcmp(rtc.getDateStr(), "01.01.2000"))
    rtc.writeProtect(false);
  else
    rtc.writeProtect(true);
  // 직접 초기화
  rtc.setTime(18, 0, 0);
  rtc.setDOW(MONDAY);
  rtc.setDate(19, 6, 2023);

  lcd.init();

  lcd.createChar(1,bar1);
  lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);
  lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);
  lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);
  lcd.createChar(8,bar8);

  state=1,state1=1,state2=1;
  // sets the LCD's rows and colums:
  lcd.begin(16, 2);

  Time t = rtc.getTime();
  s=t.sec, m=t.min, h=t.hour, dt=t.date, mo=t.mon;
}

bool bClockBlink = false;

void ClockLoop()
{
  unsigned long currClockMS = millis();	

  if (digitalRead(8)&&state==1) cnt++, state=0, cnt=cnt%5;
  else if(!digitalRead(8)&&state==0) state=1;
    
  if (digitalRead(9)&&state1==1) dg=1, state1=0;
  else if(!digitalRead(9)&&state1==0) state1=1;

  if(digitalRead(10)&&state2==1) dg=-1, state2=0;
  else if(!digitalRead(10)&state2==0) state2=1;
  
  switch(cnt)
  {
    case 2:
      m=m+dg, dg=0;  
      if(m>59) m=0;
      if(m<0) m=59;
      break;
    case 1:
      h=h+dg, dg=0;
      if(h>23) h=h-24;
      if(h<0) h=23;
      break;
    case 3:
      dt=dt+dg, dg=0;
      if(dt>l[mo]) dt=l[mo];
      if(dt<1) dt=1;
      break;
    case 4:
      mo=mo+dg, dg=0;
      if(mo>12) mo=1;
      if(mo<1) mo=12;
      if(dt>l[mo])
      {
        dt=1, mo++;
        mo=(1+(mo-1)%12);
      }
      break;
  }

  if(s>59)
  {
    s=0, m++;
    if(m>59)
      {
      m=0, h++;
      if(h>23)
          {
        h=0, dt++;
        if(dt>l[mo]) dt=1, mo++;
          if(mo>12) mo=1;
      } 
    }
  }

  d=(h)%10, printNumber(d, 3);
  d=(h)/10, printNumber(d, 0);

  d=m%10, printNumber(d, 10);
  d=m/10, printNumber(d, 7);

  lcd.setCursor(14, 0);
  lcd.print(s/10), lcd.print(s%10);
      
  lcd.setCursor(13, 1);
  lcd.print(months[mo]), lcd.print(dt/10), lcd.print(dt%10);
  if(cnt==0)
  {
      //s++;
      //lcd.setCursor(6, 0);
      //lcd.print(" ");
      //lcd.setCursor(6, 1);
      //lcd.print(" ");
      //lcd.setCursor(13,0);
      //lcd.print(" ");
      //delay(500);
      //lcd.setCursor(6, 0);
      //lcd.print(".");
      //lcd.setCursor(6, 1);
      //lcd.print(".");
      //lcd.setCursor(13,0);
      //lcd.print(":");
      //delay(500);
    
      if(currClockMS - prevClockMS >= BLINK_TIME)
      {
        s++;
        prevClockMS = currClockMS;
        if(bClockBlink)
        {
          bClockBlink = false;
          lcd.setCursor(6, 0);
          lcd.print(" ");
          lcd.setCursor(6, 1);
          lcd.print(" ");
          lcd.setCursor(13,0);
          lcd.print(" ");
        }
        else
        {
          bClockBlink = true;
          lcd.setCursor(6, 0);
          lcd.print(".");
          lcd.setCursor(6, 1);
          lcd.print(".");
          lcd.setCursor(13,0);
          lcd.print(":");
        }
      }
  }
}

void custom0(int col)
{ // uses segments to build the number 0
  lcd.setCursor(col, 0); 
  lcd.write(2); lcd.write(8); lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(2); lcd.write(6); lcd.write(1);
}
void custom1(int col)
{
  lcd.setCursor(col,0);
  lcd.write(32); lcd.write(32); lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(32); lcd.write(32); lcd.write(1);
}
void custom2(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5); lcd.write(3); lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2); lcd.write(6); lcd.write(6);
}
void custom3(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5); lcd.write(3); lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(7); lcd.write(6); lcd.write(1); 
}
void custom4(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2); lcd.write(6); lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32); lcd.write(32); lcd.write(1);
}
void custom5(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2); lcd.write(3); lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(7); lcd.write(6); lcd.write(1);
}
void custom6(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2); lcd.write(3); lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(2); lcd.write(6); lcd.write(1);
}
void custom7(int col)
{
  lcd.setCursor(col+0,0);
  lcd.write(8); lcd.write(8); lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32); lcd.write(32); lcd.write(1);
}
void custom8(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2); lcd.write(3); lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(2); lcd.write(6); lcd.write(1);
}
void custom9(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2); lcd.write(3); lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(7); lcd.write(6); lcd.write(1);
}
void printNumber(int value, int col) 
{
  	if (value == 0) custom0(col);
  	if (value == 1) custom1(col);
  	if (value == 2) custom2(col);
  	if (value == 3) custom3(col);
  	if (value == 4) custom4(col);
  	if (value == 5) custom5(col);
  	if (value == 6) custom6(col);
    if (value == 7) custom7(col);
  	if (value == 8) custom8(col);
    if (value == 9) custom9(col);
}  

bool bNeoOn = false;

void NeoLoop()
{
  unsigned long currNeoMS = millis();

  val_brightness = analogRead(POT_BRIGHTNESS);
  // max를 최대 밝기인 255로 설정시 LED 두개만 켜지는 잣버그 발생 
  val_brightness = map(val_brightness, 0, 1023, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

  val_delay = analogRead(POT_DELAY);
  val_delay = map(val_delay, 0, 1023, MIN_DELAY, MAX_DELAY);

  PrintValueAll();

  Neo.setBrightness(val_brightness);

//Neo.fill(ON, 0, NUMPIXELS);
  //Neo.show();
  //delay(val_delay);
  //Neo.clear();
  //Neo.show();
  //delay(val_delay);
  
  if(currNeoMS - prevNeoMS >= val_delay)
  {
      prevNeoMS = currNeoMS;
    if(bNeoOn)
      {
          bNeoOn = false;
          Neo.clear();
      }
      else
      {
          bNeoOn = true;
          Neo.fill(ON, 0, NUMPIXELS);
      }
      Neo.show();
  }
}

void PrintValueAll()
{
  Serial.print("Brightness : ");
  Serial.print(map(val_brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, 0, 100));
  Serial.println("%");

  Serial.print("Delay : ");
  Serial.print(val_delay / 1000.0);
  Serial.println("s");
}