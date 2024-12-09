#include <kernelapi.h>
#include <driverbase.h>
#include <stdint.h>
#include <stdarg.h>

void printf(char* format, ...) {
    va_list args;
    va_start(args, format);
    int i = 0;
    while (i < strlen(format)) {
        if (format[i] == '%') {
            i++;
            char buffer[256];
            if (format[i] == 'd' || format[i] == 'i') {
                itoa(va_arg(args, uint64_t), buffer,10);
                buffer[255] = 0;
                textPrintString(buffer);
            } else if (format[i] == 'c') {
                textPrintChar(va_arg(args, char));
            } else if (format[i] == 's') {
                textPrintString(va_arg(args, char*));
            } else if (format[i] == 'p') {
                itoa(va_arg(args,uint64_t),buffer,16);
                buffer[255] = 0;
                textPrintString(buffer);
            } 
        } else {
            textPrintChar(format[i]);
        }
        i++;
    }
    va_end(args);
}

void logPrintf(char* format, ...) {

    va_list args;
    va_start(args, format);

    printf("[ %d.%d.%d ] ",cmosHour(),cmosMinute(),cmosSecond());

    int i = 0;
    while (i < strlen(format)) {
        if (format[i] == '%') {
            i++;
            char buffer[256];
            if (format[i] == 'd' || format[i] == 'i') {
                itoa(va_arg(args, uint64_t), buffer,10);
                buffer[255] = 0;
                textPrintString(buffer);
            } else if (format[i] == 'c') {
                textPrintChar(va_arg(args, char));
            } else if (format[i] == 's') {
                textPrintString(va_arg(args, char*));
            } else if (format[i] == 'p') {
                itoa(va_arg(args,uint64_t),buffer,16);
                buffer[255] = 0;
                textPrintString(buffer);
            } 
        } else {
            textPrintChar(format[i]);
        }
        i++;
    }
    va_end(args);
    
}