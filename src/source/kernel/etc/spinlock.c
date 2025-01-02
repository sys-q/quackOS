
#include <stdint.h>
#include <stdatomic.h>
#include <kernelapi.h>

void spinlock_lock(atomic_flag *lock) {
    while(atomic_flag_test_and_set_explicit(lock,memory_order_acquire)) {
        asm volatile("pause");
    }
}

void spinlock_unlock(atomic_flag* lock) {
    atomic_flag_clear_explicit(lock,memory_order_acquire);
}