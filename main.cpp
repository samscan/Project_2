
#include "mbed.h"
#include "arm_book_lib.h"

DigitalIn driverSeat(D2);
DigitalIn ignition(D6);

AnalogIn headlightMode(A0);
AnalogIn lightLevel(A3);

DigitalOut leftHeadlight(D8);
DigitalOut rightHeadlight(D9);
DigitalOut lowBeams(D10);
DigitalOut highBeams(D11);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

void inputsInit();
void outputsInit();

int main() 
{
    inputsInit();
    outputsInit();
    while (true) {
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