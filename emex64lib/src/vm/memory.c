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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <emex64lib/support/diag.h>
#include <emex64lib/support/likely.h>

#include <emex64lib/vm/memory.h>
#include <emex64lib/vm/core.h>
#include <emex64lib/vm/machine.h>
#include <emex64lib/vm/mmio.h>
#include <emex64lib/vm/mmu.h>

emex64_memory_t *emex64_memory_alloc(uint64_t size)
{
    /*
     * allocating random access memory, which
     * must be aligned to page size for the
     * sake of god. And because it makes sense
     * lol.
     */
    emex64_memory_t *memory = malloc(sizeof(emex64_memory_t));
    if(memory == NULL)
    {
        return NULL;
    }

    /* allocate raw memory (using mmap for larger sizes, better than malloc in this case) */
    memory->memory_size = EMEX64_PAGE_ROUND_UP(size);
    memory->memory = mmap(NULL, memory->memory_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(memory->memory == MAP_FAILED)
    {
        free(memory);
        return NULL;
    }

    return memory;
}

void emex64_memory_dealloc(emex64_memory_t *memory)
{
    munmap(memory->memory, memory->memory_size);
    free(memory);
}

bool emex64_memory_load_image(emex64_memory_t *memory,
                              const char *image_path)
{
    /*
     * opening firmware image with RO(read-only)
     * access, which is because it is more efficient,
     * atleast I think that x3.
     */
    int fd = open(image_path, O_RDONLY);
    if(fd == -1)
    {
        diag_error(NULL, "failed to open firmware image at path \"%s\"\n", image_path);
        return false;
    }

    struct stat image_stat;
    if(fstat(fd, &image_stat) != 0)
    {
        close(fd);
        diag_error(NULL, "failed to gather size of file at path \"%s\"\n", image_path);
        return false;
    }

    size_t image_size = image_stat.st_size;
    if(image_size > memory->memory_size)
    {
        close(fd);
        diag_error(NULL, "firmware image is too large\n");
        return false;
    }

    /*
     * overmap the memory with the file in a dirty way tehe ^^
     * meaning that when ever the vm writes to this memory
     * it will become writable as the OS then copies the memory
     * to a writable page, this is much faster than copying it
     * our selves.
     */
    void *mapped = mmap(memory->memory, image_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd, 0);
    close(fd);
    if(mapped == MAP_FAILED)
    {
        diag_error(NULL, "mapping boot image failed\n");
        return false;
    }

    return true;
}

void emex64_memory_action(emex64_core_t *core,
                          uint64_t addr,
                          size_t size,
                          uint64_t *value,
                          kEmex64MemoryAction action)
{
    if(unlikely(core->rl[kEmex64RegisterCR2] == kEmex64ExceptionBadAccess || core->rl[kEmex64RegisterCR2] == kEmex64ExceptionKTRRViolation) && !core->in_interrupt)
    {
        return;
    }

    uint64_t paddr;
    
    /*
     * MMIO starts at 0x0020000000000000 while the physical
     * maximum memory size is 0x001FFFFFFFFFFFFF, that is so
     * MMIO doesnt sit in middle of the memory, which is better
     * for page memory management on the OS side and faster cuz
     * we don't have to look it up in MMIO on every memory
     * access.
     */
    if(addr >> 53)
    {
        uint64_t cr_pte = core->rl[kEmex64RegisterCR4];
        if(((cr_pte & EMEX64_MMU_MASK_FLAGS) & kEmex64MMUPTPresent) && !core->in_interrupt)
        {
            goto bad_access;
        }

        paddr = addr;

        emex64_mmio_region_t *mmio_region = emex64_mmio_find(core->machine->mmio_bus, paddr);
        if(likely(mmio_region != NULL))
        {
            uint64_t offset = paddr - mmio_region->base_addr;
            switch(action)
            {
                case kEmex64MemoryActionRead:
                    *value = mmio_region->read(core, mmio_region->device, offset, (int)size);
                    return;
                case kEmex64MemoryActionWrite:
                    mmio_region->write(core, mmio_region->device, offset, *value, (int)size);
                    return;
                default:
                    goto bad_access;
            }
        }
    }

    uint64_t page_end = (addr & ~EMEX64_PAGE_MASK) + EMEX64_PAGE_SIZE;
    size_t lo_size = (size_t)(page_end - addr);

    /*
     * find out if paging is enabled, if not write vaddr to paddr,
     * because that means paddr is vaddr because virtual addressing
     * is already off.
     *
     * we read it as if it was a 5th level entry, but its just a
     * control register.. for simplicity we do that hahaha.
     */
    uint64_t cr_pte = core->rl[kEmex64RegisterCR4];
    if(!((cr_pte & EMEX64_MMU_MASK_FLAGS) & kEmex64MMUPTPresent) || core->in_interrupt)
    {
        /* incase paging is disabled */
        paddr = addr;
        goto skip_to_rw;
    }

    if(unlikely(!emex64_mmu_access(core, addr, kEmex64MMUAccessRead, &paddr)))
    {
        /* MMU wrote exception */
        return;
    }

    if(lo_size < size)
    {
        size_t hi_size = size - lo_size;
        uint64_t hi_shift = lo_size * 8;
        uint64_t lo_val, hi_val, lo_mask;

        switch(action)
        {
            case kEmex64MemoryActionExecute:
            case kEmex64MemoryActionRead:
                lo_val = 0;
                hi_val = 0;
                emex64_memory_action(core, addr, lo_size, &lo_val, action);
                emex64_memory_action(core, page_end, hi_size, &hi_val, action);
                *value = lo_val | (hi_val << hi_shift);
                return;
            case kEmex64MemoryActionWrite:
                lo_mask = (lo_size == 8) ? ~0ULL : (1ULL << hi_shift) - 1;
                lo_val = *value & lo_mask;
                hi_val = *value >> hi_shift;
                emex64_memory_action(core, addr, lo_size, &lo_val, action);
                emex64_memory_action(core, page_end, hi_size, &hi_val, action);
                return;
        }
        return;
    }

skip_to_rw:
    if(likely(emex64_memory_access(core, paddr, size)))
    {
        uint64_t *ptr  = (uint64_t *)(core->machine->memory->memory + paddr);
        uint64_t mask = (size == 8) ? ~0ULL : (1ULL << (size * 8)) - 1;
        switch(action)
        {
            case kEmex64MemoryActionExecute:
            case kEmex64MemoryActionRead:
                *value = *ptr & mask;
                return;
            case kEmex64MemoryActionWrite:
                if(unlikely(core->machine->memory->ktrr_size >= paddr))
                {
                    core->rl[kEmex64RegisterCR2] = kEmex64ExceptionKTRRViolation;
                    return;
                }
                *ptr = (*ptr & ~mask) | (*value & mask);
                return;
        }
    }

bad_access:
    core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
}
