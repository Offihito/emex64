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

#include <emex64lib/support/likely.h>

#include <emex64lib/vm/mmu.h>
#include <emex64lib/vm/core.h>
#include <emex64lib/vm/machine.h>

typedef struct emex64_mmu_entry_lookup {
    bool fail;
    uint64_t *pte;
} emex64_mmu_entry_lookup_t;

static inline emex64_mmu_entry_lookup_t emex64_mmu_lookup_pte(emex64_core_t *core,
                                                              uint64_t pt_addr,
                                                              uint16_t idx)
{
    /*
     * bounds check pt_addr and check if it
     * can be even a table.
     */
    pt_addr = EMEX64_PAGE_ROUND_DOWN(pt_addr);
    if(unlikely(!EMEX64_IN_PHYS_MEMORY(pt_addr, EMEX64_PAGE_SIZE, core->machine->memory->memory, core->machine->memory->memory_size)))
    {
        return (emex64_mmu_entry_lookup_t){ .fail = true, .pte = NULL };
    }

    /* now access the table and check its entry too */
    uint64_t *pt = (uint64_t*)&core->machine->memory->memory[pt_addr];
    uint64_t *pte = &pt[idx];

    if(unlikely(!((*pte & EMEX64_MMU_MASK_FLAGS) & kEmex64MMUPTPresent)))
    {
        return (emex64_mmu_entry_lookup_t){ .fail = true, .pte = NULL };
    }

    return (emex64_mmu_entry_lookup_t){ .fail = false, .pte = pte };
}

static inline bool emex64_mmu_access_pxd(emex64_core_t *core,
                                         uint64_t pt_addr,
                                         uint16_t pxd_idx,
                                         kEmex64MMUAccess acc,
                                         uint64_t *oaddr)
{
    emex64_mmu_entry_lookup_t lookup = emex64_mmu_lookup_pte(core, pt_addr, pxd_idx);
    if(unlikely(lookup.fail))
    {
        return false;
    }

    uint64_t mmu_flags = 0;
    if(acc != kEmex64MMUAccessPageDirectory)
    {
        uint8_t checkflg = acc;

        /*
         * if CR0 is user then we need to add user
         * check too, otherwise the user program will
         * be able to access kernel memory.
         */
        if(core->rl[kEmex64RegisterCR0] < kEmex64ElevationLevelKernel)
        {
            checkflg |= kEmex64MMUPTUser;
        }

        /* initial flag check */
        mmu_flags = (*(lookup.pte) & EMEX64_MMU_MASK_FLAGS);
        if(unlikely((mmu_flags & checkflg) != checkflg))
        {
            return false;
        }
    }

    uint64_t pfn = (*(lookup.pte) & EMEX64_MMU_MASK_PFN) >> 8;
    uint64_t physaddr = EMEX64_PAGE_ROUND_DOWN(pfn << 13);
    if(unlikely(!EMEX64_IN_PHYS_MEMORY(physaddr, EMEX64_PAGE_SIZE, core->machine->memory->memory, core->machine->memory->memory_size)))
    {
        return false;
    }

    switch(acc)
    {
        case kEmex64MMUAccessPageDirectory:
            /* not a normal page access */
            break;
        case kEmex64MMUAccessWrite:
            mmu_flags |= kEmex64MMUPTDirty;
            /* fallthrough */
        case kEmex64MMUAccessRead:
        case kEmex64MMUAccessExec:
            mmu_flags |= kEmex64MMUPTAccessed;
            *(lookup.pte) = (*(lookup.pte) & ~EMEX64_MMU_MASK_FLAGS) | mmu_flags;
    }

    *oaddr = physaddr;

    return true;
}

bool emex64_mmu_access(emex64_core_t *core,
                       uint64_t vaddr,
                       kEmex64MMUAccess acc,
                       uint64_t *paddr)
{
    /* vaddr cannot be bigger than 53bits */
    if(unlikely(vaddr >> 53))
    {
        /* not a valid address to begin with */
        core->rl[kEmex64RegisterCR2] = kEmex64ExceptionBadAccess;
        return false;
    }

    /* get pfn of control register */
    uint64_t cr_pte = core->rl[kEmex64RegisterCR4];
    uint64_t cr_pfn = (cr_pte & EMEX64_MMU_MASK_PFN) >> 8;

    /* precalculating all indexes */
    uint16_t pgd_index = (vaddr >> 43) & 0x3FF;     /* 10 bits for each level index  */
    uint16_t pud_index = (vaddr >> 33) & 0x3FF;
    uint16_t pmd_index = (vaddr >> 23) & 0x3FF;
    uint16_t pte_index = (vaddr >> 13) & 0x3FF;
    uint16_t offset = vaddr & 0x1FFF;               /* 13bit offset (addressing within a page) */

    /*
     * getting page global directory from physical frame number
     * stored in the 5th level (yk the control register x3).
     */
    uint64_t pgd_addr = cr_pfn << 13;

    /* still unknown page directory addresses */
    uint64_t pud_addr, pmd_addr, pte_addr, physaddr;

    /* now access each table */
    if(!emex64_mmu_access_pxd(core, pgd_addr, pgd_index, kEmex64MMUAccessPageDirectory, &pud_addr) ||
       !emex64_mmu_access_pxd(core, pud_addr, pud_index, kEmex64MMUAccessPageDirectory, &pmd_addr) ||
       !emex64_mmu_access_pxd(core, pmd_addr, pmd_index, kEmex64MMUAccessPageDirectory, &pte_addr) ||
       !emex64_mmu_access_pxd(core, pte_addr, pte_index, acc, &physaddr))
    {
        core->rl[kEmex64RegisterCR2] = kEmex64ExceptionPageFault;
        return false;
    }

    *paddr = physaddr + offset;

    return true;
}
