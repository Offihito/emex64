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
     * sake of god.
     */
    emex64_memory_t *memory = malloc(sizeof(emex64_memory_t));
    if(memory == NULL)
    {
        return NULL;
    }

    /* allocate raw memory (using mmap for larger sizes, better than heap in this case) */
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
     * opening bios image with RO(read-only)
     * access, which is because we don't
     * write to it and we shall not write
     * to it.
     */
    int fd = open(image_path, O_RDONLY);
    if(fd == -1)
    {
        diag_error(NULL, "failed to open boot image at path \"%s\"\n", image_path);
        return false;
    }

    /* gather size of bios image */
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
        diag_error(NULL, "boot image is too large\n");
        return false;
    }

    /*
     * overmap the memory with the file in a dirty way tehe ^^
     * meaning that when ever the vm writes to this memory
     * it will become writable as the OS then copies the memory
     * to a writable page, this is much faster as usually those
     * pages aren't written to anyways, especially after the
     * assembler will start to emit a _linker_start symbol for
     * which does all the setup, like it will set the .bss pointers
     * as they usually shouldn't be part of the image anyways.
     * 
     * and with ABI it will be handled by the OS dynamic linker.
     * 
     * wen eta object file format object.h being done.
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

void *emex64_memory_access(emex64_core_t *core,
                           uint64_t addr,
                           size_t size)
{
    assert(size != 0);

    uint64_t addr_end = addr + size;
    if(addr >= addr_end || core->machine->memory->memory_size < addr_end)
    {
        /* attempt to access memory is OOB. */
        return NULL;
    }

    return &(core->machine->memory->memory[addr]);
}

void emex64_memory_read(emex64_core_t *core,
                        uint64_t addr,
                        size_t size,
                        uint64_t *value)
{
    if(unlikely(!emex64_mmu_access(core, addr, kEmex64MMUAccessRead, &addr)))
    {
        /* MMU wrote exception */
        return;
    }

    /* MMIO devices exist ^^ */
    emex64_mmio_region_t *mmio = emex64_mmio_find(core->machine->mmio_bus, addr);
    if(mmio != NULL)
    {
        *value = unlikely(mmio->read != NULL)  ? mmio->read(core, mmio->device, addr - mmio->base_addr, (int)size) : 0;
        return;
    }

    void *ptr = emex64_memory_access(core, addr, size);
    if(ptr == NULL)
    {
        core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
        return;
    }

    switch(size)
    {
        case 1:
            *value = *(uint8_t *)ptr;
            return;
        case 2:
            *value = *(uint16_t *)ptr;
            return;
        case 4:
            *value = *(uint32_t *)ptr;
            return;
        case 8:
            *value = *(uint64_t *)ptr;
            return;
        default:
            core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
            return;
    }
}

void emex64_memory_write(emex64_core_t *core,
                         uint64_t addr,
                         uint64_t value,
                         size_t size)
{
    if(unlikely(!emex64_mmu_access(core, addr, kEmex64MMUAccessWrite, &addr)))
    {
        /* MMU wrote exception */
        return;
    }

    /* checking against KTRR */
    if(unlikely(core->machine->memory->ktrr_size >= addr))
    {
        core->rl[kEmex64RegisterCR2] = kEmex64ExceptionKTRRViolation;
        return;
    }

    /* MMIO devices exist ^^ */
    emex64_mmio_region_t *mmio = emex64_mmio_find(core->machine->mmio_bus, addr);
    if(mmio != NULL)
    {
        if(unlikely(mmio->write != NULL))
        {
            mmio->write(core, mmio->device, addr - mmio->base_addr, value, (int)size);
        }
        return;
    }

    void *ptr = emex64_memory_access(core, addr, size);
    if(unlikely(ptr == NULL))
    {
        core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
        return;
    }

    switch(size)
    {
        case 1:
            *(uint8_t *)ptr = (uint8_t)value;
            return;
        case 2:
            *(uint16_t *)ptr = (uint16_t)value;
            return;
        case 4:
            *(uint32_t *)ptr = (uint32_t)value;
            return;
        case 8:
            *(uint64_t *)ptr = value;
            return;
        default:
            core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
            return;
    }
}
