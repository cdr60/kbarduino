#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C LCD(0x20,16,2);

#include <Wire.h>
#include <Adafruit_MCP23X17.h>
byte MCPADDR[4] = {0x21, 0x22, 0x23, 0x24};
Adafruit_MCP23X17 mcp[4];

const unsigned int MAX_MESSAGE_LENGTH = 3;
const long CHECKTIMER=250;
long last_check = 0;

struct pin_key {
    byte idmcp,position, alim, led_r, led_v;
    long keytime;
    boolean keypresent;
};

pin_key tab_pin_key[16];

const long KEYTIMEOUT=15000;

void setup_mcp()
{
  for (int i=0;i<16;i++)
  {
     tab_pin_key[i].idmcp=int(i/4);
     tab_pin_key[i].position=0+((i%4)*4);
     tab_pin_key[i].alim=1+((i%4)*4);
     tab_pin_key[i].led_r=2+((i%4)*4);
     tab_pin_key[i].led_v=3+((i%4)*4);
     tab_pin_key[i].keytime=0;
     tab_pin_key[i].keypresent=false;

    if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
    {
        mcp[tab_pin_key[i].idmcp].pinMode(tab_pin_key[i].position, INPUT_PULLUP);
        mcp[tab_pin_key[i].idmcp].pinMode(tab_pin_key[i].alim, OUTPUT);
        mcp[tab_pin_key[i].idmcp].pinMode(tab_pin_key[i].led_v, OUTPUT);
        mcp[tab_pin_key[i].idmcp].pinMode(tab_pin_key[i].led_r, OUTPUT);
        mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].alim, LOW);
        mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_r, LOW);
        mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v, LOW);
    }
  }
}

void setup_display()
{
   LCD.init(); // initialisation de l'afficheur
   LCD.backlight();
   LCD.setCursor(0, 0);
   LCD.print("KEYBOX");
   LCD.setCursor(8, 1);
   LCD.print("AEROINFO");
}



void check_mcp()
{
  LCD.setCursor(0, 0);
  LCD.print("MCP1     MCP2   ");
  LCD.setCursor(0, 1);
  LCD.print("MCP3     MCP4   ");
  
  for (byte i=0; i<4; i++)
  {
    byte x,y=0;
    if (i%2==0) x=5; else x=14;
    if (i>1) y=1; else y=0;
    LCD.setCursor(x,y);
    delay(250);
    if(!mcp[i].begin_I2C(MCPADDR[i])) 
    {
       LCD.print("KO");
       MCPADDR[i]=0x00;
    }
    else
    {
       LCD.print("OK ");
    }
  }
}

void check_red_led()
{
  for (int i=0;i<16;i++)
  {
    if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
    {
               mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_r,HIGH);
               delay(100);
               mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_r,LOW);
               delay(100);
    }

  }
}


void check_green_led()
{
  for (int i=0;i<16;i++)
  {
    if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
    {
               mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,HIGH);
               delay(100);
               mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,LOW);
               delay(100);
    }

  }
}

void check_electroaimant()
{
  for (int i=0;i<16;i++)
  {
    if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
    {
               mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].alim,HIGH);
               delay(100);
               mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,LOW);
               delay(100);
    }

  }
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(20);
  setup_display();
  delay(250);
  check_mcp();   /*MCP : qui est présent ?*/
  setup_mcp();   /* assignation tab_pin_key*/
  check_red_led();
  check_green_led();
}

void check_timeout(long timer)
{
  if ((millis()-last_check)>=timer)
  {
    for (int i=0;i<16;i++)
    {
      if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
      {
        tab_pin_key[i].keypresent=mcp[tab_pin_key[i].idmcp].digitalRead(tab_pin_key[i].position);
        if (!tab_pin_key[i].keypresent) 
        {
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].alim,LOW);
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,LOW);
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_r,HIGH);
        }
        if (((millis()-tab_pin_key[i].keytime)>KEYTIMEOUT) and (tab_pin_key[i].keytime>0))
        {
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].alim,LOW);
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,LOW);
          tab_pin_key[i].keytime=0;
        }
        if ((tab_pin_key[i].keypresent) and (tab_pin_key[i].keytime==0))
        {
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].alim,LOW);
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,LOW);
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_r,LOW);
        }
        if ((tab_pin_key[i].keypresent) and (tab_pin_key[i].keytime>0))
        {
          mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_r,LOW);
        }        
      }
    }
    last_check=millis();
  }
}

void print_debug_key()
{
  for (int i=0;i<16;i++)
  {
    if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
    {
      if (i%4==0)
      {
        Serial.print(tab_pin_key[i].idmcp);
        Serial.print(" ");
        Serial.print(MCPADDR[tab_pin_key[i].idmcp],HEX);
        Serial.print(" pinposition ");
        Serial.print(tab_pin_key[i].position);
        Serial.print(" value ");
        Serial.println(tab_pin_key[i].keypresent=mcp[tab_pin_key[i].idmcp].digitalRead(tab_pin_key[i].position));
      }
      tab_pin_key[i].keypresent=mcp[tab_pin_key[i].idmcp].digitalRead(tab_pin_key[i].position);
      Serial.print("key ");
      Serial.print(i,DEC);
      Serial.print(" MCP ");
      Serial.print(MCPADDR[tab_pin_key[i].idmcp],HEX);
      Serial.print(" PIN ");
      Serial.print(tab_pin_key[i].position,DEC);
      Serial.print(" VALUE ");
      Serial.println(tab_pin_key[i].keypresent);
    }
  }
}



void free_key(byte i)
{
  if ((i<16) and (i>=0))
  {
    if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
    {
      mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].alim,HIGH);
      mcp[tab_pin_key[i].idmcp].digitalWrite(tab_pin_key[i].led_v,HIGH);
      tab_pin_key[i].keytime=millis();
    }
  }
}

byte powbyte(byte x, byte y)
{
  byte val=x;
  for(byte z=0;z<=y;z++)
  {
    if(z==0)
      val=1;
    else
      val=val*x;
  }
  return val;
}

void send_statut(byte com1,byte com2)
{
  //print_debug_key();
    //demande de statut
    byte b1=0;
    byte b2=0;
    byte b3=0;
    for (byte i=0;i<16;i++)
    {
      tab_pin_key[i].keypresent=false;
      if (MCPADDR[tab_pin_key[i].idmcp]!=0x00)
          tab_pin_key[i].keypresent=mcp[tab_pin_key[i].idmcp].digitalRead(tab_pin_key[i].position);
      if (tab_pin_key[i].keypresent==true)
      {
        if ((i>=0) and (i<8)) b1=b1+powbyte(2,7-i);
        if ((i>=8) and (i<16)) b2=b2+powbyte(2,15-i);
      }
    }
    Serial.write(com1);
    Serial.write(com2);
    Serial.write(b1);
    Serial.write(b2);
    Serial.write(b3);
}


void send_param(byte com1,byte com2)
{
    Serial.write(com1);
    Serial.write(com2);
    Serial.write(byte(KEYTIMEOUT/1000));
    Serial.write(byte(8));
    Serial.write(byte(7));
    Serial.write(byte(2));
    Serial.write(byte(60));
}

void loop() 
{
  // put your main code here, to run repeatedly:
  check_timeout(CHECKTIMER);

  static byte message[MAX_MESSAGE_LENGTH];
  if (Serial.available() > 0)
   {
    // read the incoming bytes:
    int rlen = Serial.readBytes(message, MAX_MESSAGE_LENGTH);  
    if (rlen==MAX_MESSAGE_LENGTH)
    {
          //Analyse du message
          if ((message[0] == 0x10) and (message[1] == 0x02) and (message[2] == 0x00))
          {
              send_statut(message[0],message[1]);
              
          }
          else if ((message[0] == 0x10) and (message[1] == 0x01) and (message[2] >= 0) and (message[2] < 16))
          {
            free_key(message[2]);
            send_statut(message[0],message[1]);
          }
          else if ((message[0] == 0x10) and (message[1] == 0xFF) and (message[2] == 0x00))
          {
            //demande paramètres
            send_param(message[0],message[1]);
          }     
    }
    //Vider le buffer
    else while (Serial.available()>0) Serial.read();
 }
 
}  
