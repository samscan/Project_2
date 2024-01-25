
#include "mbed.h"
#include "arm_book_lib.h"

#define TIME_INCREMENT_MS                       10
#define DEBOUNCE_BUTTON_TIME_MS                 40


typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
} buttonState_t;

int accumulatedDebounceButtonTime       = 0;
int pressed                            = 0;

DigitalIn driverSeat(D2);
DigitalIn ignition(D6);

AnalogIn headlightMode(A0);
AnalogIn lightLevel(A3);

DigitalOut leftHeadlight(D8);
DigitalOut rightHeadlight(D9);
DigitalOut lowBeams(D10);
DigitalOut highBeams(D11);
DigitalOut Engine(LED2);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

buttonState_t IgnitionButtonState;

void inputsInit();
void outputsInit();
void startEngine();
void stopEngine();
void debounceButtonInit();
bool debounceButtonUpdate();

int main() 
{
    inputsInit();
    outputsInit();
    while (true){
        startEngine();
        
        //stopEngine();
        delay(TIME_INCREMENT_MS);
    }
}

void inputsInit()
{
    driverSeat.mode(PullDown);
    ignition.mode(PullDown);
}

void outputsInit()
{
    leftHeadlight = OFF;
    rightHeadlight = OFF;
    lowBeams = OFF;
    highBeams = OFF;
}

void startEngine(){
    bool IgnitionButtonReleasedEvent = debounceButtonUpdate();
    if(driverSeat && !Engine && IgnitionButtonReleasedEvent){
        pressed += 1;
        Engine = ON;
    }
    
}

void stopEngine(){
    bool IgnitionButtonReleasedEvent = debounceButtonUpdate();
    if(Engine && IgnitionButtonReleasedEvent){
    
        Engine = OFF;
        pressed = 0;
    }

}

bool debounceButtonUpdate()
{
    bool ignitionReleasedEvent = false;
    switch( IgnitionButtonState ) {

    case BUTTON_UP:
        if( ignition == 1 ) {
            IgnitionButtonState = BUTTON_FALLING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_FALLING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( ignition == 1 ) {
                IgnitionButtonState = BUTTON_DOWN;
            } else {
                IgnitionButtonState = BUTTON_UP;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    case BUTTON_DOWN:
        if( ignition == 0 ) {
            IgnitionButtonState = BUTTON_RISING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_RISING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( ignition == 0 ) {
                IgnitionButtonState = BUTTON_UP;
                ignitionReleasedEvent = true;
                //yyyy
            } else {
                IgnitionButtonState = BUTTON_DOWN;
                //yyy
        
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    default:
        debounceButtonInit();
        break;
    }
    return ignitionReleasedEvent;
}

void debounceButtonInit()
{
    if( ignition == 1) {
        IgnitionButtonState = BUTTON_UP;
    } else {
        IgnitionButtonState = BUTTON_DOWN;
    }
}
