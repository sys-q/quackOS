#include <stdint.h>
#include <lock/spinlock.h>

void spinlock_lock(uint8_t* lock) {
    while(__sync_lock_test_and_set(lock,1));
}

void spinlock_unlock(uint8_t* lock) {
    __sync_lock_release(lock);
    *lock = 0;
}