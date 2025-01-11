#include <stdint.h>

void spinlock_lock(uint8_t* lock);

void spinlock_unlock(uint8_t* lock);