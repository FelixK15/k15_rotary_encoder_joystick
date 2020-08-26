//FK: Since we're running on an arduino micro, we can enable the optimized interrupts
//    uncomment if it doesn't compile
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include <Joystick.h>

#define USE_SERIAL_DEBUG_OUTPUT 0

enum JoystickAxis : uint8_t
{
  JoystickAxis_X = 0u,
  JoystickAxis_Y,

  JoystickAxis_Count
};

struct JoystickSetup
{
      static const int buttonCount      = 0u;
      static const int hatSwitchCount   = 0u;
      static const int useXAxis         = 1u;
      static const int useYAxis         = 1u;
      static const int useZAxis         = 0u;
      static const int useRXAxis        = 0u;
      static const int useRYAxis        = 0u;
      static const int useRZAxis        = 0u;
      static const int useRudder        = 0u;
      static const int useThrottle      = 0u;
      static const int useAccelerator   = 0u;
      static const int useBreak         = 0u;
      static const int useSteering      = 0u;

      static const int axisRangeMin         = -64;
      static const int axisRangeMax         = 64;
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

struct RotaryEncoderJoystick
{
  RotaryEncoderJoystick( RotaryEncoderAxis* pEncoderAxisX, RotaryEncoderAxis* pEncoderAxisY ) :
    joystickDevice( JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
        JoystickSetup::buttonCount, JoystickSetup::hatSwitchCount,
        JoystickSetup::useXAxis, JoystickSetup::useYAxis, JoystickSetup::useZAxis,
        JoystickSetup::useRXAxis, JoystickSetup::useRYAxis, JoystickSetup::useRZAxis,
        JoystickSetup::useRudder, JoystickSetup::useThrottle, JoystickSetup::useAccelerator,
        JoystickSetup::useBreak, JoystickSetup::useSteering )
  {
    pRotaryEncoderAxis[ JoystickAxis_X ] = pEncoderAxisX;
    pRotaryEncoderAxis[ JoystickAxis_Y ] = pEncoderAxisY;

    joystickDevice.setXAxisRange( JoystickSetup::axisRangeMin, JoystickSetup::axisRangeMax );
    joystickDevice.setYAxisRange( JoystickSetup::axisRangeMin, JoystickSetup::axisRangeMax );

    const bool autoSendState = false;
    joystickDevice.begin( autoSendState );
  }

  void sendJoystickValues()
  {
    joystickDevice.setXAxis( pRotaryEncoderAxis[ JoystickAxis_X ]->relativeEncoderValue );
    joystickDevice.setYAxis( pRotaryEncoderAxis[ JoystickAxis_Y ]->relativeEncoderValue );
    joystickDevice.sendState();
    
    pRotaryEncoderAxis[ JoystickAxis_X ]->relativeEncoderValue = 0u;
    pRotaryEncoderAxis[ JoystickAxis_Y ]->relativeEncoderValue = 0u;
  }

  RotaryEncoderAxis*  pRotaryEncoderAxis[ JoystickAxis_Count ];
  Joystick_           joystickDevice;
};

RotaryEncoderJoystick rotaryEncoderJoystick( &encoderAxisPlayer1, &encoderAxisPlayer2 );

void setup() {
  #if USE_SERIAL_DEBUG_OUTPUT
    Serial.begin(9600);
  #endif
}

void loop() {
  for( uint8_t encoderIndex = 0u; encoderIndex < JoystickAxis_Count; ++encoderIndex )
  {
    RotaryEncoderAxis* pRotaryEncoderAxis = rotaryEncoderJoystick.pRotaryEncoderAxis[ encoderIndex ];
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

  rotaryEncoderJoystick.sendJoystickValues(); 
}
