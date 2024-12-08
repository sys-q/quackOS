#include <variables.h>
#include <kernelapi.h>
#include <driverbase.h>

void osMain() {
    textSetFG(0xFFFF00);
    textPrintString("Welcome to quackOS ! \n");
    cli();
    while(true) {hlt();}
}