/*
================================================================================

    File........... NES gamepads to PS2
    Purpose........ Connect 2 NES gamepads and PS/2 Keyboard to Sizif XXS PS/2
    Author......... Peter A. Ukhov
    E-mail......... ukhov79 @ gmail.com
    Started........ 11/19/2023
    Finished....... not yet (waiting for all components to assembly)
    Updated........ --/--/----

================================================================================
   Notes
================================================================================

Now just keyboard repetier.
It doesn't work yet!

*/

//===============================================================================
//  Connactions
//===============================================================================

// Gamepads pins
#define PULT1_DATA_PIN 17
#define PULT1_LATCH_PIN 16
#define PULT1_CLOCK_PIN 15
#define PULT2_DATA_PIN 6
#define PULT2_LATCH_PIN 7
#define PULT2_CLOCK_PIN 8

// Emulator pins to connect into retro PC
#define SIZIF_KEYBOARD_DATA_PIN 5
#define SIZIF_KEYBOARD_CLOCK_PIN 3

// PS2 Keyboard pins to connect into Keyboard
#define PS2_KEYBOARD_DATA_PIN 4
#define PS2_KEYBOARD_CLOCK_PIN 2

// Time countdown for delay in polling cycles of gamepads
#define TICK 2

//===============================================================================
//  Gamepads cpp
//===============================================================================

struct pult_cfg {
  int data;
  int latch;
  int clock;
};

pult_cfg pult_1 = {
  .data = PULT1_DATA_PIN,
  .latch = PULT1_LATCH_PIN,
  .clock = PULT1_CLOCK_PIN
};

pult_cfg pult_2 = {
  .data = PULT2_DATA_PIN,
  .latch = PULT2_LATCH_PIN,
  .clock = PULT2_CLOCK_PIN
};

void init_joystick(pult_cfg& cfg) {
  pinMode(cfg.data, INPUT_PULLUP);
  pinMode(cfg.clock, OUTPUT);
  pinMode(cfg.latch, OUTPUT);
  digitalWrite(cfg.clock, HIGH);
}

int get_keys_state_joystick(pult_cfg& cfg) {
  digitalWrite(cfg.latch, HIGH);
  delayMicroseconds(TICK);
  digitalWrite(cfg.latch, LOW);

  int keys_state = 0;

  for (int i = 0; i < 8; ++i) {
    delayMicroseconds(TICK);
    digitalWrite(cfg.clock, LOW);

    keys_state <<= 1;
    keys_state += digitalRead(cfg.data);

    delayMicroseconds(TICK);
    digitalWrite(cfg.clock, HIGH);
  }

  return keys_state;
}

// The order of shifting for the buttons:
// 0 - A, 1 - B, 2 - Select, 3 - Start, 4 - Up, 5 - Down, 6 - Left, 7 - Right   
// Codes for 
byte keys_pult_1_c[] = {0x70, 0x43, 0x29, 0x5A, 0x75, 0x72, 0x6B, 0x74};
// {'a', 'b', 'c', 'd', 'e', 's', 'd', 'f'};
byte keys_pult_2_c[] = {0x70, 0x43, 0x29, 0x5A, 0x75, 0x72, 0x6B, 0x74};
// {'k', 'h', 'j', 'u', 'y', 'i', 'n', 'm'};
// TODO: Read from EEPROM, will be configured, change keycodes

//===============================================================================
//  libraries:
//  To Read keys from PS/2 keyboard 
//  https://github.com/PaulStoffregen/PS2Keyboard
//  To Emulate PS/2 Keyboard
//  https://github.com/Harvie/ps2dev
//===============================================================================

#include <EEPROM.h>
#include <PS2Keyboard.h>    // Read a PS/2 keyboard
#include <ps2dev.h>         // Emulate a PS/2 device

PS2Keyboard readkeyboard;
PS2dev keyboard(SIZIF_KEYBOARD_CLOCK_PIN, SIZIF_KEYBOARD_DATA_PIN);

//===============================================================================
//  Keyboard emulator keys - emul.cpp
//===============================================================================

int enabled = 0; // pseudo variable for state of "keyboard"
String smbls="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`-=\\[];',./ ";
byte codes[49]={0x1C,0x32,0x21,0x23,0x24,0x2B,0x34,0x33,0x43,0x3B,0x42,0x4B,0x3A,0x31,0x44,0x4D,0x15,0x2D,0x1B,0x2C,0x3C,0x2A,0x1D,0x22,0x35,
0x1A,0x45,0x16,0x1E,0x26,0x25,0x2E,0x36,0x3D,0x3E,0x46,0x0E,0x4E,0x55,0x5D,0x54,0x5B,0x4C,0x52,0x41,0x49,0x4A,0x29};

void SendString(String str) {
  //str=(str.toUpperCase());
  str.toUpperCase();
  int i=0;
  int n=0;//subarrays index
  while (str[i]!=0) {
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


//===============================================================================
//  Setup
//===============================================================================

void setup() {
  // send the keyboard start up to SIZIF XXS
  while(keyboard.write(0xAA)!=0);
  delay(10);

  // init for PS/2 keyboard
  readkeyboard.begin(PS2_KEYBOARD_DATA_PIN, PS2_KEYBOARD_CLOCK_PIN, PS2Keymap_US);
  delay(10);

  // to test PS/2 Keyboard
  // Serial.begin(9600);
  // Keyboard.begin();

  // init_joystick(pult_1);
  // init_joystick(pult_2); 
}

//===============================================================================
//  loop
//===============================================================================

void loop() {

  unsigned char c;
  //if host device wants to send a command:
  if( (digitalRead(SIZIF_KEYBOARD_CLOCK_PIN)==LOW) || (digitalRead(SIZIF_KEYBOARD_DATA_PIN) == LOW)) {
    while(keyboard.read(&c)) ;
    keyboardcommand(c);
  }
  else{ 
    //send keypresses accordingly using scancodes

    if (readkeyboard.available()) {
      char key = readkeyboard.read();
      SendString(String(key));
    }

  }



  // Just print something
  //keyboard.write(0x0D); //send another Tab
  //keyboard.write(0xF0);  //tab break
  //keyboard.write(0x0D);  //...

  // Test and print Gamepads
  /*
  int pult_1_keys = get_keys_state_joystick(pult_1);
  int pult_2_keys = get_keys_state_joystick(pult_2);
  
  for (int i = 0; i < 8; i++) {
      if (!(pult_1_keys & (1 << i))) {
        keyboard.write(keys_pult_1_c[i]);
        delay(20);
      } else {
        keyboard.write(0xF0);
        delay(20);
        keyboard.write(keys_pult_1_c[i]);
        delay(20);
      }
  }


  for (int i = 0; i < 8; i++) {
      if (!(pult_2_keys & (1 << i))) {
        keyboard.write(keys_pult_2_c[i]);
        delay(20);
      } else {
        keyboard.write(0xF0);
        delay(20);
        keyboard.write(keys_pult_2_c[i]);
        delay(20);
      }
    }
  */

}