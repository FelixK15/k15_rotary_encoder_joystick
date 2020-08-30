//FK: Since we're running on an arduino micro, we can enable the optimized interrupts
//    uncomment if it doesn't compile
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <Mouse.h>

#define USE_SERIAL_DEBUG_OUTPUT 0

enum MouseAxis : uint8_t
{
  MouseAxis_X = 0u,
  MouseAxis_Y,

  MouseAxis_Count
};

struct RotaryEncoderAxis
{
  RotaryEncoderAxis( const int* pPins ) : 
    encoder( pPins[0], pPins[1] )
  {  
    absoluteEncoderValue = 0;
    relativeEncoderValue = 0;
  }

  Encoder     encoder;
  int         absoluteEncoderValue;
  char        relativeEncoderValue;
};

struct RotaryEncoderMouse
{
  RotaryEncoderMouse( RotaryEncoderAxis* pEncoderAxisX, RotaryEncoderAxis* pEncoderAxisY )
  {
    pRotaryEncoderAxis[ MouseAxis_X ] = pEncoderAxisX;
    pRotaryEncoderAxis[ MouseAxis_Y ] = pEncoderAxisY;

    Mouse.begin();
  }

  void updateMouseAxisValues()
  {
    if( pRotaryEncoderAxis[ MouseAxis_X ]->relativeEncoderValue == 0 &&
        pRotaryEncoderAxis[ MouseAxis_Y ]->relativeEncoderValue == 0 )
    {
      //FK: Nothing to do here
      return;
    }
    
    const char mouseWheel = 0;
    Mouse.move( pRotaryEncoderAxis[ MouseAxis_X ]->relativeEncoderValue, pRotaryEncoderAxis[ MouseAxis_Y ]->relativeEncoderValue, mouseWheel );
    
    pRotaryEncoderAxis[ MouseAxis_X ]->relativeEncoderValue = 0u;
    pRotaryEncoderAxis[ MouseAxis_Y ]->relativeEncoderValue = 0u;
  }

  RotaryEncoderAxis*  pRotaryEncoderAxis[ MouseAxis_Count ];
};

const int encoderPinsPlayer1[] = { 2, 3 };
const int encoderPinsPlayer2[] = { 0, 1 };

RotaryEncoderAxis encoderAxisPlayer1( encoderPinsPlayer1 );
RotaryEncoderAxis encoderAxisPlayer2( encoderPinsPlayer2 );

RotaryEncoderMouse rotaryEncoderMouse( &encoderAxisPlayer1, &encoderAxisPlayer2 );

char clampIntToChar(int value)
{
  return ( value < -127 ? -127 : value > 128 ? 128 : value );
}

void setup() {
  #if USE_SERIAL_DEBUG_OUTPUT
    Serial.begin(9600);
  #endif
}

void loop() {
  const float axisScale = 1.0f;

  for( uint8_t encoderIndex = 0u; encoderIndex < MouseAxis_Count; ++encoderIndex )
  {
    RotaryEncoderAxis* pRotaryEncoderAxis = rotaryEncoderMouse.pRotaryEncoderAxis[ encoderIndex ];
    const int newEncoderValue = ( int )( axisScale * (float)pRotaryEncoderAxis->encoder.read() );
    const int encoderValueDelta = ( newEncoderValue - pRotaryEncoderAxis->absoluteEncoderValue );

    #if USE_SERIAL_DEBUG_OUTPUT
    {
      Serial.print("Detected joystick axis movement for axis: ");
      if (encoderIndex == JoystickAxis_X)
      {
        Serial.print("X Axis - ");
      }
      else
      {
        Serial.print("Y Axis - ");
      }

      Serial.print("value: ");
      Serial.print(newEncoderValue);
      Serial.print(" delta: ");
      Serial.print(encoderValueDelta);
      Serial.print("\n");
    }
    #endif

    pRotaryEncoderAxis->relativeEncoderValue = clampIntToChar( encoderValueDelta );
    pRotaryEncoderAxis->absoluteEncoderValue = newEncoderValue;
  }

  rotaryEncoderMouse.updateMouseAxisValues(); 
}
