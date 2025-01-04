
#include <stdint.h>
#include <stdatomic.h>
#include <etc/acpi.h>

void spinlock_lock(atomic_flag *lock) {
    if(isEarly())
        return; 
    while(atomic_flag_test_and_set_explicit(lock,memory_order_acquire)) {
        asm volatile("pause");
    }
}

void spinlock_unlock(atomic_flag* lock) {
    if(isEarly())
        return; 
    atomic_flag_clear_explicit(lock,memory_order_acquire);
}