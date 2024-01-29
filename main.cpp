
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

typedef enum {
    ON_SELECTED, 
    OFF_SELECTED,
    AUTO_SELECTED
} headlightSelection_t;

typedef enum {
    DAYLIGHT,
    DUSK,
    BETWEEN
} lightLevelState_t;

DigitalIn driverSeat(D2);
DigitalIn ignition(D6);

AnalogIn headlightMode(A0);
AnalogIn lightLevel(A3);

DigitalOut leftLowBeam(D8);
DigitalOut leftHighBeam(D9);
DigitalOut rightLowBeam(D10);
DigitalOut rightHighBeam(D11);
DigitalOut engine(LED2);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

buttonState_t ignitionButtonState;
headlightSelection_t headlightState;
lightLevelState_t lightLevelState;

int accumulatedDebounceButtonTime = 0;
int ignitionPressedDebounceTime = 0;
int accumulatedHeadlightDelay = 0;

void inputsInit();
void outputsInit();
void engineUpdate();
void checkStartEngine();
void checkStopEngine();
void debounceButtonInit();
bool debounceButtonUpdate();
void headlightUpdate();
void headlightON();
void headlightOFF();
void headlightAUTO();
void lightLevelUpdate();

int main() 
{
    inputsInit();
    outputsInit();
    while (true){
        engineUpdate();
        headlightUpdate();
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
    leftLowBeam = OFF;
    rightLowBeam = OFF;
    leftHighBeam = OFF;
    rightHighBeam = OFF;
    engine = OFF;
}

void engineUpdate() {
    if(!engine) {
        checkStartEngine();
    }
    else {
        checkStopEngine();
    }
    ignitionPressedDebounceTime = ignitionPressedDebounceTime + TIME_INCREMENT_MS;
}

void checkStartEngine(){
    bool ignitionButtonReleasedEvent = debounceButtonUpdate();
    if(driverSeat && ignitionButtonReleasedEvent && ignitionPressedDebounceTime >= DEBOUNCE_BUTTON_TIME_MS){

        engine = ON;
        ignitionPressedDebounceTime = 0;
    }
    
}

void checkStopEngine(){
    bool ignitionButtonReleasedEvent = debounceButtonUpdate();
    if(ignitionButtonReleasedEvent && ignitionPressedDebounceTime >= DEBOUNCE_BUTTON_TIME_MS){
        engine = OFF;
        ignitionPressedDebounceTime = 0;
    }
}

bool debounceButtonUpdate()
{
    bool ignitionReleasedEvent = false;
    switch( ignitionButtonState ) {
    
    case BUTTON_UP:
        if( ignition == 1 ) {
            ignitionButtonState = BUTTON_FALLING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_FALLING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( ignition == 1 ) {
                ignitionButtonState = BUTTON_DOWN;
            } else {
                ignitionButtonState = BUTTON_UP;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    case BUTTON_DOWN:
        if( ignition == 0 ) {
            ignitionButtonState = BUTTON_RISING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_RISING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( ignition == 0 ) {
                ignitionButtonState = BUTTON_UP;
                ignitionReleasedEvent = true;
                uartUsb.write( "yes\r\n", 5);
            } else {
                ignitionButtonState = BUTTON_DOWN;
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
        ignitionButtonState = BUTTON_UP;
    } else {
        ignitionButtonState = BUTTON_DOWN;
    }
}

void headlightUpdate() 
{
    if(engine) {
        float headlightModeValue = headlightMode.read();
        if(headlightModeValue <= 0.333) {
            headlightState = ON_SELECTED;
            headlightON();
        }
        if(0.333 < headlightModeValue && headlightModeValue <= 0.667) {
            headlightState = OFF_SELECTED;
            headlightOFF();
        }
        if (0.667 < headlightModeValue) {
            headlightState = AUTO_SELECTED;
            headlightAUTO();
        }
    }
    else {
        headlightState = OFF_SELECTED;
        headlightOFF();
    }
}

void headlightON()
{
    leftLowBeam = ON;
    rightLowBeam = ON;
    leftHighBeam = OFF;
    rightHighBeam = OFF; 
}

void headlightOFF()
{
    leftLowBeam = OFF;
    rightLowBeam = OFF;
    leftHighBeam = OFF;
    rightHighBeam = OFF;
}

void headlightAUTO() 
{
    lightLevelUpdate();
}

void lightLevelUpdate()
{
    float lightLevelValue = lightLevel.read();
    if(lightLevelValue <= 0.333) {
        if(accumulatedHeadlightDelay >= 1000) {
            headlightON();
        }
        accumulatedHeadlightDelay = accumulatedHeadlightDelay + TIME_INCREMENT_MS;
    }
    if(0.333 < lightLevelValue && lightLevelValue <= 0.667) {
        accumulatedHeadlightDelay = 0;
    }
    if(0.667 < lightLevelValue) {
        if(accumulatedHeadlightDelay >= 2000) {
            headlightOFF();
        }
        accumulatedHeadlightDelay = accumulatedHeadlightDelay + TIME_INCREMENT_MS;
    }
}
