/*
 * MIT License
 *
 * Copyright (c) 2024 emexlab
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef EMEX64VM_MEMORY_H
#define EMEX64VM_MEMORY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <emex64lib/vm/core.h>

#define LA64_PAGE_SIZE 0x2000
#define LA64_PAGE_MASK (LA64_PAGE_SIZE - 1)
#define LA64_PAGE_ROUND_DOWN(x) ((x) & ~((LA64_PAGE_SIZE) - 1))
#define LA64_PAGE_ROUND_UP(x) (((x) + (LA64_PAGE_SIZE) - 1) & ~((LA64_PAGE_SIZE) - 1))
#define LA64_IN_PHYS_MEMORY(addr, access_size, mem_base, mem_size) (((uintptr_t)(addr) < (uintptr_t)(mem_size)) && ((uintptr_t)(addr) + (access_size) <= (uintptr_t)(mem_size)))
#define LA64_BYTES_TO_PAGE_BOUNDARY(addr) (LA64_PAGE_SIZE - ((uintptr_t)(addr) & LA64_PAGE_MASK))
#define LA64_CROSS_PAGE_OFFSET(addr, access_size) (((access_size) > LA64_BYTES_TO_PAGE_BOUNDARY(addr)) ? LA64_BYTES_TO_PAGE_BOUNDARY(addr) : 0)

#define LA64_MEMORY_WRITE_HELPER(mapping, offset, size, value)  \
    switch(size)                                                \
    {                                                           \
        case 1: /* 8 bit */                                     \
            *(uint8_t*)(((uint8_t*)mapping) + offset) = value;  \
        case 2: /* 16 bit */                                    \
            *(uint16_t*)(((uint8_t*)mapping) + offset) = value; \
        case 4: /* 32 bit */                                    \
            *(uint32_t*)(((uint8_t*)mapping) + offset) = value; \
        case 8: /* 64 bit */                                    \
            *(uint64_t*)(((uint8_t*)mapping) + offset) = value; \
    }

#define LA64_MEMORY_READ_HELPER(mapping, offset, size, out_value)       \
    switch(size)                                                        \
    {                                                                   \
        case 1:                                                         \
            (out_value) = *(uint8_t*)(((uint8_t*)mapping) + offset);    \
            break;                                                      \
        case 2:                                                         \
            (out_value) = *(uint16_t*)(((uint8_t*)mapping) + offset);   \
            break;                                                      \
        case 4:                                                         \
            (out_value) = *(uint32_t*)(((uint8_t*)mapping) + offset);   \
            break;                                                      \
        case 8:                                                         \
            (out_value) = *(uint64_t*)(((uint8_t*)mapping) + offset);   \
            break;                                                      \
    }

typedef struct la64_memory {
    uint8_t *memory;
    uint64_t memory_size;
} la64_memory_t;

la64_memory_t *la64_memory_alloc(uint64_t size);
void la64_memory_dealloc(la64_memory_t *memory);

bool la64_memory_load_image(la64_memory_t *memory, const char *image_path);

void *la64_memory_access(la64_core_t *core, uint64_t addr, size_t size);
bool la64_memory_read(la64_core_t *core, uint64_t addr, size_t size, uint64_t *value);
bool la64_memory_write(la64_core_t *core, uint64_t addr, uint64_t value, size_t size);

#endif /* EMEX64VM_MEMORY_H */
