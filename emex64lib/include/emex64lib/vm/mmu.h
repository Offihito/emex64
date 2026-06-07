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

#ifndef EMEX64VM_MMU_H
#define EMEX64VM_MMU_H

#include <stdbool.h>
#include <stdint.h>

#include <emex64lib/vm/core.h>

/* page table entry bit flags */
typedef enum: uint8_t {
    kEmex64MMUPTPresent =   0b00000001, /* marks a PTE or PXD as present */
    kEmex64MMUPTUser =      0b00000010, /* marks a PTE as user accessible, meaning user mode can access that page */
    kEmex64MMUPTDirty =     0b00000100, /* marks a PTE as dirty, writes on it cause a page fault TODO: to be implemented */
    kEmex64MMUPTRead =      0b00001000, /* marks a PTE as readable */
    kEmex64MMUPTWrite =     0b00010000, /* marks a PTE as writable (most MMU's don't have that, but this one does) */
    kEmex64MMUPTExec =      0b00100000, /* marks a PTE as executable (means the CPU core can fetch instructions from it and execute them) */
    kEmex64MMUPTAccessed =  0b01000000, /* marks a PTE as accessed (MMU sets this bit when this has been accessed) */
} kEmex64MMUPT;

/* access types */
typedef enum: uint8_t {
    kEmex64MMUAccessPageDirectory = 0b00000000, /* MARK: this is a internal private type, don't use it outside of mmu.c */
    kEmex64MMUAccessRead =          kEmex64MMUPTRead,
    kEmex64MMUAccessWrite =         kEmex64MMUPTWrite,
    kEmex64MMUAccessExec =          kEmex64MMUPTExec
} kEmex64MMUAccess;

/* page table enry masks */
#define EMEX64_MMU_MASK_FLAGS         0b0000000000000000000000000000000000000000000000000000000011111111
#define EMEX64_MMU_MASK_PFN           0b1111111111111111111111111111111111111111111111111111111100000000

bool emex64_mmu_access(emex64_core_t *core, uint64_t vaddr, kEmex64MMUAccess acc, uint64_t *paddr);

#endif /* EMEX64VM_MMU_H */
