#include <ps2dev.h> // to emulate a PS/2 device
//#include "String.h"

int enabled = 0; // pseudo variable for state of "keyboard"
int CLKpin = 3;
int DATApin = 5;
PS2dev keyboard(CLKpin, DATApin); // PS2dev object (2:data, 3:clock)
int todel=1;

String smbls="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`-=\\[];',./ ";
byte codes[49]={0x1C,0x32,0x21,0x23,0x24,0x2B,0x34,0x33,0x43,0x3B,0x42,0x4B,0x3A,0x31,0x44,0x4D,0x15,0x2D,0x1B,0x2C,0x3C,0x2A,0x1D,0x22,0x35,
0x1A,0x45,0x16,0x1E,0x26,0x25,0x2E,0x36,0x3D,0x3E,0x46,0x0E,0x4E,0x55,0x5D,0x54,0x5B,0x4C,0x52,0x41,0x49,0x4A,0x29};

void SendString(String str)
{
  //str=(str.toUpperCase());
  str.toUpperCase();
  int i=0;
  int n=0;//subarrays index
  while (str[i]!=0)
  {
    while ((str[i]!=smbls[n]) && smbls[n]!=0) n++;
    keyboard.write(codes[n]); 
    delay(20);
    keyboard.write(0xF0); // send smbl
    delay(20);
    keyboard.write(codes[n]); 
    delay(20);
    n=0;
    i++;
  }
 
};

void ack() {
  //acknowledge commands
  while(keyboard.write(0xFA));
}

int keyboardcommand(int command) {
  unsigned char val;
  switch (command) {
  case 0xFF: //reset
    ack();
    //the while loop lets us wait for the host to be ready
    while(keyboard.write(0xAA)!=0);
    break;
  case 0xFE: //resend
    ack();
    break;
  case 0xF6: //set defaults
    //enter stream mode
    ack();
    break;
  case 0xF5: //disable data reporting
    //FM
    enabled = 0;
    ack();
    break;
  case 0xF4: //enable data reporting
    //FM
    enabled = 1;
    ack();
    break;
  case 0xF3: //set typematic rate
    ack();
    keyboard.read(&val); //do nothing with the rate
    ack();
    break;
  case 0xF2: //get device id
    ack();
    keyboard.write(0xAB);
    keyboard.write(0x83);
    break;
  case 0xF0: //set scan code set
    ack();
    keyboard.read(&val); //do nothing with the rate
    ack();
    break;
  case 0xEE: //echo
    //ack();
    keyboard.write(0xEE);
    break;
  case 0xED: //set/reset LEDs
    ack();
    keyboard.read(&val); //do nothing with the rate
    Serial.println('Val = '+val);
    ack();
    break;
  }
}

void setup() {
  // send the keyboard start up
  while(keyboard.write(0xAA)!=0);
  delay(10);
}

void loop() {
  unsigned char c;
  //if host device wants to send a command:
  if( (digitalRead(CLKpin)==LOW) || (digitalRead(DATApin) == LOW)) {
    while(keyboard.read(&c)) ;
    keyboardcommand(c);
  }
  else{ //send keypresses accordingly using scancodes
  // secancodes: http://www.computer-engineering.org/ps2keyboard/scancodes2.html
  
  delay(10000);
  if (todel==1)
    {
    SendString("J 'Just test keyboard emulator!'      40"); 
    todel=1;
    } // wait 10 second
  }
}