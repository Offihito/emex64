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

#ifndef EMEX64ASM_ELF_H
#define EMEX64ASM_ELF_H

#include <stdint.h>
#include <stdbool.h>

#include <emex64lib/asm/invocation.h>

#define EM_EMEX64   0x0E64

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASS64      2
#define ELFDATA2LSB     1

#define EV_CURRENT      1

#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2

#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_NOBITS      8
#define SHF_WRITE       (1 << 0)
#define SHF_ALLOC       (1 << 1)
#define SHF_EXECINSTR   (1 << 2)
#define STB_LOCAL       0
#define STB_GLOBAL      1
#define STB_WEAK        2

#define STT_NOTYPE      0
#define STT_OBJECT      1
#define STT_FUNC        2
#define STT_SECTION     3
#define STT_FILE        4

#define STV_DEFAULT     0

#define SHN_UNDEF       0
#define SHN_ABS         0xFFF1

#define R_EMEX64_ABS64  1

#define EI_NIDENT       16

typedef struct {
    uint8_t     e_ident[EI_NIDENT];
    uint16_t    e_type;
    uint16_t    e_machine;
    uint32_t    e_version;
    uint64_t    e_entry;
    uint64_t    e_phoff;
    uint64_t    e_shoff;
    uint32_t    e_flags;
    uint16_t    e_ehsize;
    uint16_t    e_phentsize;
    uint16_t    e_phnum;
    uint16_t    e_shentsize;
    uint16_t    e_shnum;
    uint16_t    e_shstrndx;
} __attribute__((packed)) Emex64_Ehdr;

typedef struct {
    uint32_t    sh_name;
    uint32_t    sh_type;
    uint64_t    sh_flags;
    uint64_t    sh_addr;
    uint64_t    sh_offset;
    uint64_t    sh_size;
    uint32_t    sh_link;
    uint32_t    sh_info;
    uint64_t    sh_addralign;
    uint64_t    sh_entsize;
} __attribute__((packed)) Emex64_Shdr;

typedef struct {
    uint32_t    st_name;
    uint8_t     st_info;
    uint8_t     st_other;
    uint16_t    st_shndx;
    uint64_t    st_value;
    uint64_t    st_size;
} __attribute__((packed)) Emex64_Sym;

typedef struct {
    uint64_t    r_offset;
    uint64_t    r_info;
    int64_t     r_addend;
} __attribute__((packed)) Emex64_Rela;

#define EMEX64_ELF32_R_SYM(i)      ((i) >> 32)
#define EMEX64_ELF32_R_TYPE(i)     ((i) & 0xFFFFFFFF)
#define EMEX64_ELF64_R_INFO(s,t)   (((uint64_t)(s) << 32) | (uint32_t)(t))

static inline void emex64_elf_fill_ident(uint8_t ident[EI_NIDENT])
{
    ident[0] = ELFMAG0;
    ident[1] = ELFMAG1;
    ident[2] = ELFMAG2;
    ident[3] = ELFMAG3;
    ident[4] = ELFCLASS64;
    ident[5] = ELFDATA2LSB;
    ident[6] = EV_CURRENT;
    ident[7] = 0;
    for(int i = 8; i < EI_NIDENT; i++)
    {
        ident[i] = 0;
    }
}

#define EMEX64_SYM_INFO(bind, type) (((bind) << 4) | ((type) & 0xf))

bool assembler_elf_emit(struct assembler_invocation *inv);

#endif /* EMEX64ASM_ELF_H */
