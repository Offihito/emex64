/*
 * MIT License
 *
 * Copyright (c) 2024 cr4zyengineer
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

#ifndef LAUTILS_OBJECT_H
#define LAUTILS_OBJECT_H

#include <stdint.h>

#define LAO_MAGIC        0xFADEBACE

#define LAO_VERSION_NEWEST      0x0 /* TODO: create support check macros for the version field */
#define LAO_VERSION_OLDEST      0x0

#define LAO_ARCH_NONE           0x0
#define LAO_ARCH_LA16           0x1 /* unsupported yet */
#define LAO_ARCH_LA32           0x2 /* unsupported yet */
#define LAO_ARCH_LA64           0x3

#define LAO_ABI_NONE            0x0
#define LAO_ABI_BOOTIMG         LAO_ABI_NONE /* bootimg means without ABI, there is no ABI as there is no proper operating system yet */

#define LAO_TYPE_NONE           0b000000000
#define LAO_TYPE_OBJECT         0b000000001
#define LAO_TYPE_EXECUTABLE     0b000000010

#define LAO_BASE_HEADER_MAGIC_VALID(header) (header->magic == LAO_MAGIC)
#define LAO_BASE_HEADER_VERSION_VALID(header) (header->version == LAO_VERSION_NEWEST)
#define LAO_BASE_HEADER_ARCH_VALIC(header) (header->arch == LAO_ARCH_LA64)
#define LAO_BASE_HEADER_ABI_VALID(header) (header->abi == LAO_ABI_BOOTIMG)
#define LAO_BASE_HEADER_TYPE_VALID(header) (header->type == LAO_TYPE_OBJECT || header->type == LAO_TYPE_EXECUTABLE) /* once library support is there we can make truly use out of the bitmask */

#define LAO_BASE_HEADER_VALID(header) (LAO_BASE_HEADER_MAGIC_VALID(header) && LAO_BASE_HEADER_VERSION_VALID(header) && LAO_BASE_HEADER_ARCH_VALID(header) && LAO_BASE_HEADER_ABI_VALID(header))

typedef struct __attribute__((packed)) lao_base_header {
    uint32_t magic;
    uint8_t version;
    uint8_t arch;
    uint8_t abi;
    uint8_t type;
} lao_base_header_t;

typedef struct __attribute__((packed)) lao_header64 {
    uint64_t string_table_offset;
    uint64_t symbol_table_offset;
    uint64_t section_table_offset;
    uint64_t start_offset;
} lao_header64_t;

#endif /* LAUTILS_OBJECT_H */

