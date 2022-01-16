/*
 * Author: MIUZEI
 * IDE V1.6.9
 * Email:support@miuzeipro.com
 */
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int xpotPin = A0;
int ypotPin = A1;            
int bpotPin = 6; 
int xval=0;       //Initialize a variable
int yval=0;
int bval=0;

int c = 0;
int v = 0;
bool lost = false;

char o = 'O';
int obstacles_c[7] = {2, 5, 7 ,9 , 11, 13, 15};
int obstacles_v[7] = {0, 1, 0, 1,   0,  1,  0};

void setObstacles()
{
  for (int i = 0; i < 7; i++)
  {
    lcd.setCursor(obstacles_c[i], obstacles_v[i]) ;       
    lcd.print(o); 
  }
}

int calcC()
{
  if(yval < 403)
  {
    if (c < 15)
    {
      c = c+1;  
    }   
  }
  
  if(yval > 603)
  {
    if ( c > 0)
    {
      c = c - 1; 
    }  
  }
  
  return c;
}

int calcV()
{
  if(xval < 403)
  {
    if (v == 1 )
    {
      v = 0;  
    }   
  }
  
  if(xval > 603)
  {
    if ( v == 0)
    {
      v = 1; 
    }  
  }
  
  return v;
}

bool check()
{
  for (int i = 0; i < 7; i++)
  {
    if (c == obstacles_c[i] && v == obstacles_v[i])
    {
      lcd.clear();
      
      lcd.setCursor(3, 0) ;       
      lcd.print("Verloren");  
      lost = true;
      
      c = 0;
      v = 0;
      
      return lost;
    }
  }

  lcd.setCursor(c, v);
  lcd.print("%");

  return lost;
}

void checkWin()
{
  if (c == 15 && v == 1)
  {
    delay(500);
    lcd.clear();
      
    lcd.setCursor(3, 0) ;       
    lcd.print("Gewonnen");  
    delay(2000);
    
    c = 0;
    v = 0;    
  }
}

void setup()                                                                                     
{
  lcd.begin(16,2);   //Display Address
  pinMode(xpotPin,INPUT);
  pinMode(ypotPin,INPUT);
  pinMode(bpotPin,INPUT); 
  lcd.print("  Game ");
  lcd.setCursor(0,1);
  lcd.print("  is starting");
  delay(2000);
  lcd.clear();
}
void loop()
{ 
  if (lost)
  {
    delay(2000);
    lost = false;
    c = 0;
    v = 0;
    return;
  }
  
  xval = analogRead(xpotPin);
  yval = analogRead(ypotPin);   
     
  lcd.clear();
  
  setObstacles();
    
  c = calcC();
  v = calcV();
  
  check();

  checkWin();
    
  delay(500);
}
