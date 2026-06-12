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
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <emex64lib/support/diag.h>

#include <emex64lib/asm/invocation.h>
#include <emex64lib/asm/code.h>
#include <emex64lib/asm/label.h>
#include <emex64lib/asm/emit.h>
#include <emex64lib/asm/section.h>
#include <emex64lib/asm/macro.h>
#include <emex64lib/asm/elf.h>

assembler_invocation_t *assembler_invocation_alloc(assembler_options_t *options)
{
    /* apply warning_error local thread variable */
    warning_error = options->warning_error;

    int fd = open(assembler_options_get_output_path(options), O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(fd < 0)
    {
        return NULL;
    }

    assembler_invocation_t *inv = calloc(1, sizeof(assembler_invocation_t));
    if(inv == NULL)
    {
        close(fd);
        return NULL;
    }

    inv->fdwalker = malloc(sizeof(fdwalker_t));
    if(inv->fdwalker == NULL)
    {
        free(inv);
        close(fd);
        return NULL;
    }

    fdwalker_init(inv->fdwalker, fd, BW_LITTLE_ENDIAN);
    fdwalker_seek(inv->fdwalker, 10, 0);

    inv->data_section_start = UINT64_MAX;
    inv->data_section_end = UINT64_MAX;
    inv->bss_section_start = UINT64_MAX;
    inv->bss_section_size = 0;

    inv->options = options;

    return inv;
}

void assembler_invocation_dealloc(assembler_invocation_t *inv)
{
    /* todo: this must be redone from scratch */
    diag_warn(NULL, "deallocation of assembler invocation is not implemented in this version of emex64lib\n");
}

bool assembler_invocation_emit(assembler_invocation_t *inv,
                               int filec,
                               char **filev)
{
    if(filec <= 0)
    {
        diag_error(NULL, "no input files provided\n");
        return false;
    }

    if(!assembler_code_preparse(inv, (const char **)filev, filec) ||
       !assembler_macro_expand(inv) ||
       !assembler_code_parse(inv) ||
       !assembler_label_prealloc(inv) ||
       !assembler_section_parse(inv) ||
       !assembler_emit(inv) ||
       !assembler_elf_emit(inv))
    {
        return false;
    }
    
    return true;
}
