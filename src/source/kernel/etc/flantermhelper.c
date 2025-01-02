
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <driverbase.h>
#include <stdint.h>
#include <stdarg.h>

struct flanterm_context* ft_ctx;

void flantermHelperInit(struct flanterm_context* ft_context) {
    ft_ctx = ft_context;
}

void flantermHelperWrite(char* str) {
    size_t size = strlen(str);
    flanterm_write(ft_ctx,str,size);
}

void flantermHelperWriteChar(char ch) {
    flanterm_write(ft_ctx,&ch,1);
}

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
                flantermHelperWrite(buffer);
            } else if (format[i] == 'c') {
                flantermHelperWriteChar(va_arg(args, char));
            } else if (format[i] == 's') {
                flantermHelperWrite(va_arg(args, char*));
            } else if (format[i] == 'p') {
                itoa(va_arg(args,uint64_t),buffer,16);
                buffer[255] = 0;
                flantermHelperWrite(buffer);
            } 
        } else {
            flantermHelperWriteChar(format[i]);
        }
        i++;
    }
    va_end(args);
}