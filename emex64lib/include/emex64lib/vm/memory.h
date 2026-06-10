/*
 * MIT License
 *
 * Copyright (c) 2026 emexlab
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
#include <stddef.h>
#include <string.h>

#include <emex64lib/support/bitwalker.h>
#include <emex64lib/vm/core.h>

#define EMEX64_PAGE_SIZE 0x2000
#define EMEX64_PAGE_MASK (EMEX64_PAGE_SIZE - 1)
#define EMEX64_PAGE_ROUND_DOWN(x) ((x) & ~((EMEX64_PAGE_SIZE) - 1))
#define EMEX64_PAGE_ROUND_UP(x) (((x) + (EMEX64_PAGE_SIZE) - 1) & ~((EMEX64_PAGE_SIZE) - 1))
#define EMEX64_IN_PHYS_MEMORY(addr, access_size, mem_base, mem_size) (((uintptr_t)(addr) < (uintptr_t)(mem_size)) && ((uintptr_t)(addr) + (access_size) <= (uintptr_t)(mem_size)))
#define EMEX64_BYTES_TO_PAGE_BOUNDARY(addr) (EMEX64_PAGE_SIZE - ((uintptr_t)(addr) & EMEX64_PAGE_MASK))
#define EMEX64_CROSS_PAGE_OFFSET(addr, access_size) (((access_size) > EMEX64_BYTES_TO_PAGE_BOUNDARY(addr)) ? EMEX64_BYTES_TO_PAGE_BOUNDARY(addr) : 0)

#define EMEX64_MEMORY_WRITE_HELPER(mapping, offset, size, value)        \
    {                                                                   \
        uint64_t mask = (size == 8) ? ~0ULL : (1ULL << (size * 8)) - 1; \
        void *ptr = ((uint8_t*)mapping) + offset;                       \
        uint64_t raw = *(uint64_t *)ptr;                                \
        raw = (raw & ~mask) | (value & mask);                           \
        *(uint64_t *)ptr = raw;                                         \
    }

#define EMEX64_MEMORY_READ_HELPER(mapping, offset, size, out_value)     \
    {                                                                   \
        void *ptr = ((uint8_t*)mapping) + offset;                       \
        uint64_t raw = *(uint64_t *)ptr;                                \
        uint64_t mask = (size == 8) ? ~0ULL : (1ULL << (size * 8)) - 1; \
        out_value = raw & mask;                                         \
    }

typedef struct emex64_memory {
    uint8_t *memory;
    uint64_t memory_size;
    uint64_t ktrr_size;
    bool ktrr_locked;
} emex64_memory_t;

typedef enum: uint8_t {
    kEmex64MemoryActionRead,
    kEmex64MemoryActionWrite
} kEmex64MemoryAction;

emex64_memory_t *emex64_memory_alloc(uint64_t size);
void emex64_memory_dealloc(emex64_memory_t *memory);

bool emex64_memory_load_image(emex64_memory_t *memory, const char *image_path);

void emex64_memory_action(emex64_core_t *core, uint64_t addr, size_t size, uint64_t *value, kEmex64MemoryAction action);

#endif /* EMEX64VM_MEMORY_H */
