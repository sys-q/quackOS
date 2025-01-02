#include <stdint.h>
#include <kernelapi.h>

uint64_t next;
uint64_t dnext;
uint64_t randMagic(uint64_t seed) {
    seed = (seed *1103515245+12345) & 0x7fffffff;
    return seed;
}

uint64_t randNum(uint64_t min, uint64_t max) {
    int seconds = cmosSecond();
    int minutes = cmosMinute();
    int hours = cmosHour();
    uint64_t seed = seconds + minutes + hours + (next*dnext);
    next++;
    dnext+=(seconds*minutes);
    uint64_t random_seed = randMagic(seed);
    uint64_t random_number = min + random_seed % (max - min +1);
    return random_number;
}