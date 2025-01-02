
#include <stdint.h>
#include <scheduling/process.h>
#include <fthelper.h>

void processSchedule(interrupt_context_t* ctx) {
    printf("Received scheduling\n");
}