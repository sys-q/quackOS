
#include <cpu/data.h>
#include <memory/heap.h>
#include <driverbase.h>

cpu_data_t* createData() {
    cpu_data_t* data = (cpu_data_t*)kmalloc(sizeof(cpu_data_t));
    wrmsr64(0xC0000101,(uint64_t)data);
    return data;
}

cpu_data_t* fetchData() {
    return (cpu_data_t*)rdmsr64(0xC0000101);
}