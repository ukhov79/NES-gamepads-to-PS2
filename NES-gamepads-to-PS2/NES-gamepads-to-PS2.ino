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

WARNING - NOT TESTED YET!!!
waiting for all components to assembly

*/

//===============================================================================
//  Constants
//===============================================================================

// Gamepads pins
#define PULT1_DATA_PIN 17
#define PULT1_LATCH_PIN 16
#define PULT1_CLOCK_PIN 15
#define PULT2_DATA_PIN 6
#define PULT2_LATCH_PIN 7
#define PULT2_CLOCK_PIN 8

// Emulator pins to connect into retro PC
#define SIZIF_KEYBOARD_DATA_PIN 2
#define SIZIF_KEYBOARD_CLOCK_PIN 3

// PS2 Keyboard pins to connect into Keyboard
#define PS2_KEYBOARD_DATA_PIN 4
#define PS2_KEYBOARD_CLOCK_PIN 5

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

void init_joystick(pult_cfg& cfg)
{
  pinMode(cfg.data, INPUT_PULLUP);
  pinMode(cfg.clock, OUTPUT);
  pinMode(cfg.latch, OUTPUT);
  digitalWrite(cfg.clock, HIGH);
}

int get_keys_state_joystick(pult_cfg& cfg)
{
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
byte keys_pult_1_c[] = {0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};
// {'a', 'b', 'c', 'd', 'e', 's', 'd', 'f'};
byte keys_pult_2_c[] = {0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};
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

//===============================================================================
//  Setup
//===============================================================================

PS2Keyboard readkeyboard;
PS2dev keyboard(SIZIF_KEYBOARD_CLOCK_PIN, SIZIF_KEYBOARD_DATA_PIN);

void setup()
{
  // Keyboard.begin();
  init_joystick(pult_1);
  init_joystick(pult_2);

  // init for PS/2 keyboard
  readkeyboard.begin(PS2_KEYBOARD_DATA_PIN, PS2_KEYBOARD_CLOCK_PIN, PS2Keymap_US);

  // send the keyboard start up to SIZIF XXS
  while(keyboard.write(0xAA)!=0);
  
  // to test PS/2 Keyboard
  Serial.begin(9600);
}

//===============================================================================
//  loop
//===============================================================================

void loop()
{

  // Just read something
  if (readkeyboard.available()) {
    char c = readkeyboard.read();
    Serial.print(c);
  }

  // Just print something
  keyboard.write(0x0D); //send another Tab
  keyboard.write(0xF0);  //tab break
  keyboard.write(0x0D);  //...

  // Test and print Gamepads
  int pult_1_keys = get_keys_state_joystick(pult_1);
  int pult_2_keys = get_keys_state_joystick(pult_2);
  
  if(pult_1_keys == 255)
    Serial.println("Pult 1 Error");
  else {
    Serial.print("Pult 1 = ");
    Serial.println(pult_1_keys);

    for (int i = 0; i < 8; i++) {
      if (!(pult_1_keys & (1 << i))) {
        keyboard.write(keys_pult_1_c[i]);
      } else {
        keyboard.write(0xF0);
        keyboard.write(keys_pult_1_c[i]);
      }
    }
  }

  if(pult_2_keys == 255)
    Serial.println("Pult 2 Error");
  else {
    Serial.print("Pult 2 = ");
    Serial.println(pult_2_keys);

    for (int i = 0; i < 8; i++) {
      if (!(pult_2_keys & (1 << i))) {
        keyboard.write(keys_pult_2_c[i]);
      } else {
        keyboard.write(0xF0);
        keyboard.write(keys_pult_2_c[i]);
      }
    }
  }

  delay(2000);
}