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
    if(unlikely(!emex64_mmu_access(core, addr, kEmex64MMUAccessRead, &addr)))
    {
        /* MMU wrote exception, no need to write it our selves */
        return;
    }

    /*
     * MMIO starts at 0x0040000000000000 while the physical
     * maximum memory size is 0x003FFFFFFFFFFFFF, that is so
     * MMIO doesnt sit in middle of the memory, which is better
     * for page memory management on the OS side and faster cuz
     * we don't have to look it up in MMIO on every memory
     * access.
     */
    if(addr >> 53)
    {
        emex64_mmio_region_t *mmio_region = emex64_mmio_find(core->machine->mmio_bus, addr);
        if(likely(mmio_region != NULL))
        {
            uint64_t offset = addr - mmio_region->base_addr;
            switch(action)
            {
                case kEmex64MemoryActionRead:
                    *value = mmio_region->read(core, mmio_region->device, offset, (int)size);
                    return;
                case kEmex64MemoryActionWrite:
                    mmio_region->write(core, mmio_region->device, offset, *value, (int)size);
                    return;
            }
        }
    }
    else
    {
        uint64_t *ptr = emex64_memory_access(core, addr, size);
        if(likely(ptr != NULL))
        {
            uint64_t mask = (size == 8) ? ~0ULL : (1ULL << (size * 8)) - 1;
            switch(action)
            {
                case kEmex64MemoryActionRead:
                    *value = *ptr & mask;
                    return;
                case kEmex64MemoryActionWrite:
                    /*
                     * preventing Kernel Text Read-Only Region
                     * writes.
                     */
                    if(unlikely(core->machine->memory->ktrr_size >= addr))
                    {
                        core->rl[kEmex64RegisterCR2] = kEmex64ExceptionKTRRViolation;
                        return;
                    }

                    *ptr = (*ptr & ~mask) | (*value & mask);
                    return;
            }
            return;
        }
    }

    core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
}
