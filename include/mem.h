#ifndef MEM_H
#define MEM_H 1

#include <stdint.h>

typedef struct {
    void* base_addr;
    uint32_t size;
} mem_info_t;

#endif /* mem_h */
