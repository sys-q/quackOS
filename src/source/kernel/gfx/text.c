#include <kernelapi.h>
#include <driverbase.h>
#include <stdarg.h>
#include <fonts.h>

uint32_t maxCharsX;
uint32_t maxCharsY;
uint32_t charX;
uint32_t charY;
uint32_t charBackGround;
uint32_t charForeGround;
uint32_t textScreenX = 0;
uint32_t textScreenY = 0;
uint8_t fontSizeX = 8;
uint8_t fontSizeY = 15;

void textDrawChar(int xOffset, int yOffset, int color, char ch) {
    int firstByteIdx = ch * 16;
    bool doDrawBuffer;
    for (int by = 0; by < 16; by++) { 
        for (int bi = 0; bi < 8; bi++) {
            doDrawBuffer = (font8x15[firstByteIdx + by] >> (7 - bi)) & 1;
            if(doDrawBuffer) 
                gopPaint(xOffset + bi, yOffset + by, color);
            else {
                if(!gopGetFastState()){
                    gopPaint(xOffset + bi, yOffset + by, charBackGround);
                }
            }
        }
    }
}

void textDrawString(char* String,int X,int Y,int Color){
    int TX = X;
    for(int i=0;i<strlen(String);i++){
        textDrawChar(TX,Y,Color,String[i]);
        TX += 8;
    }
}

void textPrintChar(char key) {
    if(key == '\n') {
        if(charY == maxCharsY - 1 ) {
            textClearTextScreen();
        }
        else{charY++;charX=0;textScreenX = 0;textScreenY += fontSizeY;}
        
        return;
    }
    textDrawChar(textScreenX,textScreenY,charForeGround,key);
    if(charX == maxCharsX - 1) {
            if(charY == maxCharsY) {
                textClearTextScreen();
                charX=0;textScreenX = 0;
            }
            else{charY++;charX=0;textScreenX = 0;textScreenY += fontSizeY;}
            
    }
    else {charX++;textScreenX += fontSizeX;}
}

void textPutChar(int x,int y,char key) {
    textClearChar(x,y);
    textDrawChar(x*fontSizeX,y*fontSizeY,charForeGround,key);
}

void kernelInitTextMode() {
    maxCharsX = gopGetWidth() / fontSizeX;
    maxCharsY = (gopGetHeight() / fontSizeY) - 1;
    charForeGround = 0xFFFFFFFFFF;
    charBackGround = 0;
}

void textClearChar(int x, int y){
    x = x * fontSizeX;
    y = y * fontSizeY;
    for (int by = 0; by < 16; by++) { 
        for (int bi = 0; bi < 8; bi++) {
            gopPaint(x + bi, y + by, charBackGround);
        }
    }
}

void textPrintString(char* String) {
    for(int i = 0;i < strlen(String);i++) {
        textPrintChar(String[i]);
    }
}

void textClearTextScreen() {
    gopClear(charBackGround);
    charY = 0;charX = 0;textScreenY=0;textScreenX=0;
}

void textSetBG(uint32_t color) {
    charBackGround = color;
}

void textSetFG(uint32_t color) {
    charForeGround = color;
}

uint32_t textGetFG() {
    return charForeGround;
}

uint32_t textGetBG() {
    return charBackGround;
}