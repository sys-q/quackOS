
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

// flanterm helper

void flantermHelperInit(struct flanterm_context* ft_context);

void flantermHelperWrite(char* str);

void printf(char* format, ...);