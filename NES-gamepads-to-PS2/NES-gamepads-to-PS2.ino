/*
================================================================================

    File........... NES gamepads to PS2
    Purpose........ Connect 2 NES gamepads and PS/2 Keyboard to Sizif XXS PS/2
    Author......... Peter A. Ukhov
    E-mail......... ukhov79 @ gmail.com
    Started........ 11/19/2023
    Finished....... --/--/----
    Updated........ --/--/----

================================================================================
   Notes
================================================================================
*/

//===============================================================================
//  Constants
//===============================================================================
struct pult_cfg {
  int data;
  int latch;
  int clock;
};

pult_cfg pult_1 = {
  .data = 12,
  .latch = 10,
  .clock = 13
};

pult_cfg pult_2 = {
  .data = 4,
  .latch = 5,
  .clock = 6
};

const int TICK = 2;

static const int keys_pult_1_c[] = {'a', 'b', 'c', 'd', 'e', 's', 'd', 'f'};
static const int keys_pult_2_c[] = {'k', 'h', 'j', 'u', 'y', 'i', 'n', 'm'};
//===============================================================================
//  libraries
//===============================================================================
//#include "FidPS2/FidPS2Keyboard.h"
//#include "FidPS2/FidPS2Host.h"




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

void setup()
{
  // Keyboard.begin();
  init_joystick(pult_1);
  init_joystick(pult_2);
 // fid_ps2kb_init(1, 2);
  Serial.begin(9600);


}

void loop()
{
  int pult_1_keys = get_keys_state_joystick(pult_1);
  int pult_2_keys = get_keys_state_joystick(pult_2);
  
  if(pult_1_keys == 255)
    Serial.println("Pult 1 Error");
  else {
    Serial.print("Pult 1 = ");
    Serial.println(pult_1_keys);
  }

  if(pult_2_keys == 255)
    Serial.println("Pult 2 Error");
  else {
    Serial.print("Pult 2 = ");
    Serial.println(pult_2_keys);
  }

 // bool fid_ps2kb_read(uint8_t* b);

  /*
  for (int i = 0; i < 8; i++) {
    if (!(pult_1_keys & (1 << i))) {
      Keyboard.press(keys_pult_1_c[i]);
    } else {
      Keyboard.release(keys_pult_1_c[i]);
    }

    if (!(pult_2_keys & (1 << i))) {
      Keyboard.press(keys_pult_2_c[i]);
    } else {
      Keyboard.release(keys_pult_2_c[i]);
    }
  }
  */

  delay(2000);
}