#include <stdatomic.h>

void spinlock_lock(atomic_flag *lock);

void spinlock_unlock(atomic_flag* lock);