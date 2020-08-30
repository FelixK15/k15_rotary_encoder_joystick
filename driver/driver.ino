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
  int         relativeEncoderValue;
};

const int encoderPinsPlayer1[] = { 2, 3 };
const int encoderPinsPlayer2[] = { 0, 1 };

RotaryEncoderAxis encoderAxisPlayer1( encoderPinsPlayer1 );
RotaryEncoderAxis encoderAxisPlayer2( encoderPinsPlayer2 );

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
    const char mouseWheel = 0;
    Mouse.move( pRotaryEncoderAxis[ MouseAxis_Y ]->relativeEncoderValue, pRotaryEncoderAxis[ MouseAxis_Y ]->relativeEncoderValue, mouseWheel );
    
    pRotaryEncoderAxis[ MouseAxis_X ]->relativeEncoderValue = 0u;
    pRotaryEncoderAxis[ MouseAxis_Y ]->relativeEncoderValue = 0u;
  }

  RotaryEncoderAxis*  pRotaryEncoderAxis[ MouseAxis_Count ];
};

RotaryEncoderMouse rotaryEncoderMouse( &encoderAxisPlayer1, &encoderAxisPlayer2 );

void setup() {
  #if USE_SERIAL_DEBUG_OUTPUT
    Serial.begin(9600);
  #endif
}

void loop() {
  for( uint8_t encoderIndex = 0u; encoderIndex < MouseAxis_Count; ++encoderIndex )
  {
    RotaryEncoderAxis* pRotaryEncoderAxis = rotaryEncoderMouse.pRotaryEncoderAxis[ encoderIndex ];
    const int newEncoderValue = pRotaryEncoderAxis->encoder.read();
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

    pRotaryEncoderAxis->relativeEncoderValue = encoderValueDelta;
    pRotaryEncoderAxis->absoluteEncoderValue = newEncoderValue;
  }

  rotaryEncoderMouse.updateMouseAxisValues(); 
}
