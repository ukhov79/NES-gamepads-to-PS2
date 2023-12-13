#include <PS2KeyAdvanced.h>

/* Keyboard constants  Change to suit your Arduino
   define pins used for data and clock from keyboard */
#define DATAPIN 4
#define IRQPIN  2

uint16_t c;

PS2KeyAdvanced keyboard;


void setup( )
{
// Configure the keyboard library
keyboard.begin( DATAPIN, IRQPIN );
Serial.begin( 9600 );
Serial.println( "PS2 Advanced Key Simple Test:" );
}


void loop( )
{
if( keyboard.available() )
  {
  // read the next key
  
  c = keyboard.read( );
  if( c > 0 )
    {
    Serial.print( "Value " );
    Serial.print( c, HEX );
    Serial.print( " - Status Bits " );
    Serial.print( c >> 8, HEX );
    Serial.print( "  Code " );
    Serial.println( c & 0xFF, HEX );
    }
  }
}