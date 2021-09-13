#include <LiquidCrystal.h>
#include <Servo.h>
Servo myservo;  // create servo object to control a servo

LiquidCrystal lcd(13,12,11,10,9,8) ;


int gas=6;
int FLAME = 7;
int stove= 5;
int stove_control= 3;

int pos = 0;    //servo variable

                      //LOAD CELL
#define DT A0
#define SCK A1
#define sw 2
 
long sample=0;
float val=0;
long count=0;
 
unsigned long readCount(void)
{
unsigned long Count;
unsigned char i;
pinMode(DT, OUTPUT);
digitalWrite(DT,HIGH);
digitalWrite(SCK,LOW);
Count=0;
pinMode(DT, INPUT);
while(digitalRead(DT));
for (i=0;i<24;i++)
{
digitalWrite(SCK,HIGH);
Count=Count<<1;
digitalWrite(SCK,LOW);
if(digitalRead(DT))
Count++;
}
digitalWrite(SCK,HIGH);
Count=Count^0x800000;
digitalWrite(SCK,LOW);
return(Count);
}
                          //LOAD CELL CALLIBRATION
void calibrate()
{
lcd.clear();
lcd.print("Calibrating...");
lcd.setCursor(0,1);
lcd.print("Please Wait...");
for(int i=0;i<100;i++)
{
count=readCount();
sample+=count;
}
sample/=100;
lcd.clear();
lcd.print("Put 1kg weight");
count=0;
while(count<1000)
{
count=readCount();
count=sample-count;
}
lcd.clear();
lcd.print("Please Wait....");
delay(1000);
for(int i=0;i<100;i++)
{
count=readCount();
val+=sample-count;
}
val=val/100.0;
val=val/100.0; //KILOGRAM MEASUREMENT
lcd.clear();
}

void setup()
{
  Serial.begin(9600);
  pinMode(stove, INPUT);               //stove status 
  pinMode(FLAME, INPUT);               //FLAME input pin
  pinMode(gas, INPUT);                 //GAS input pin
  pinMode(stove_control, OUTPUT);      //stove relay control 
   
  lcd.begin(20,4);
  lcd.setCursor(0,0);
  lcd.print("Sensor Activation");
  delay(500);
  lcd.clear();
  lcd.setCursor(10,2);
  lcd.print("READY!!!");
  delay(500);
  lcd.clear();
  delay(1000);
                            //LOAD CELL CALLIBRATION
  pinMode(SCK, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  lcd.setCursor(0,0);
  lcd.print("wEIGHT CALLIBRATION");
  calibrate();

  myservo.attach(4);    // attaches the servo on pin 4 to the servo object
 }

void loop()
{
                                          //LOAD CELL LPG WEIGHT MEASUREMENT
  count= readCount();
  int w=(((count-sample)/val)-2*((count-sample)/val));
  lcd.setCursor(0,0);
  lcd.print("Measured Weight");
  lcd.setCursor(0,1);
  lcd.print(w);
  lcd.print("Kg ");
  delay(1000);
  if(digitalRead(sw)==0)
  {
    val=0;
    sample=0;
    w=0;
    count=0;
    calibrate();
  }
  if(w<2)                           //LPG REFILL CONDITION WHERE REFIL NOTIFICATION IS SENT WHEN LPG WEIGHT DROPS BELOW 2 Kilogram
  {
    lcd.setCursor(0,2);
    lcd.print("LOW LPG");
    delay(500);
    lcd.setCursor(0,3);
    lcd.print("Refil soon...");
    delay(1000);
  }
  
  if(digitalRead(stove) == HIGH)   // Checking the stove status
  {
    lcd.setCursor(0,0);
    lcd.print("Stove is on");
  }
  else
  {
    lcd.clear();
  }
  
  

  if(digitalRead(FLAME) == HIGH)          // If surrounding of stove catches fire
  {
    lcd.setCursor(0,1);
    lcd.print("Danger, Flame Detected!");
    digitalWrite(stove_control, HIGH);
    lcd.setCursor(0,2);
    lcd.print("Stove turned off");
    delay(100);
    for (pos = 0; pos <= 180; pos += 5)   //LPG SUPPLY IS TURNED OFF VIA SERVO ACTION ON REGULATOR
    {  
         myservo.write(pos);               
          delay(10); 
    }
    while(1)            //INFINITE LOOP TO STOP FURTHER EXECUTION UNTILL USER MANUALLY RESETS THE DEVICE
    {
      
    }
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("Happy Cooking!");
  }
  
delay(100);

if(digitalRead(gas) == HIGH)               //Reading data for surrounding smoke or LPG leak
  {
    lcd.setCursor(0,2);                     //THIS CONDITION IS APPLICABLE FOR BOTH LPG LEAK OR BURNING OF FOOD/MILK
    lcd.print("Food Burn or LPG leak!");
    digitalWrite(stove_control, HIGH);
    lcd.setCursor(0,3);
    lcd.print("Stove turned off");
     for (pos = 0; pos <= 180; pos += 5)   //LPG SUPPLY IS TURNED OFF VIA SERVO ACTION ON REGULATOR
      {  
         myservo.write(pos);               
          delay(10); 
      }
     while(1)            //INFINITE LOOP TO STOP FURTHER EXECUTION UNTILL USER MANUALLY RESETS THE DEVICE
      {
      
      }
  }
  else
  {
    lcd.setCursor(0,2);
    lcd.print("Happy Cooking!");
  }
delay(500);
lcd.clear();
 
}



 
