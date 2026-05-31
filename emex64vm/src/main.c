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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <emex64lib/support/bitwalker.h>
#include <emex64lib/support/diag.h>
#include <emex64lib/support/parser.h>

#include <emex64lib/vm/machine.h>
#include <emex64lib/vm/device/display.h>

int main(int argc, char *argv[])
{
    int opt;
    const char *bios_image = NULL;
    uint64_t memsize = 100 * 1024 * 1024;   /* standard is 100MB */

    /* parse arguments */
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--help") == 0)
        {
            printf("%s [options]\n", argv[0]);
            printf("\t--help                 : showing help menu\n");
            printf("\t--bios <image path>    : providing bios image\n");
            printf("\t--memory <memory size> : providing memory size in megabyte\n");
            return 0;
        }
        else if(strcmp(argv[i], "--bios") == 0 && i + 1 < argc)
        {
            bios_image = argv[++i];
        }
        else if(strcmp(argv[i], "--memory") == 0 && i + 1 < argc)
        {
            parser_return_t pr = parse_value_from_string(argv[++i]);
            if(pr.type == emexParserValueTypeNumber)
            {
                memsize = pr.value * 1024 * 1024;
            }
            else
            {
                diag_error(NULL, "illegal value type used\n", argv[i]);
            }
        }
        else
        {
            diag_error(NULL, "unknown option '%s'\n", argv[i]);
        }
    }

    if(bios_image == NULL)
    {
        diag_error(NULL, "no bios image provided\n");
    }

    /* creating new la16 virtual machine */
    la64_machine_t *machine = la64_machine_alloc(memsize);
    if(machine == NULL)
    {
        diag_error(NULL, "failed to allocated machine\n");
    }

    /*
     * load boot image, maybe use a dirty private
     * mapping and open the image it self as memory,
     * just size it.
     */
    if(!la64_memory_load_image(machine->memory, bios_image))
    {
        goto usage;
    }

    /*
     * getting entry point of boot image of virtual machine
     * and setting program pointer of first core to it
     */
    bitwalker_t bw;
    bitwalker_init_read(&bw, machine->memory->memory, 8, BW_LITTLE_ENDIAN);
    machine->core->rl[LA64_REGISTER_PC] = bitwalker_read(&bw, 64);
    machine->core->rl[LA64_REGISTER_SP] = machine->memory->memory_size - 8;
    machine->core->rl[LA64_REGISTER_CR0] = LA64_ELEVATION_SECURE_MONITOR;

    /* executing virtual machines 1st core TODO: Implement threading */
    la64_core_execute(machine->core);

    /* deallocating machine */
    la64_machine_dealloc(machine);

    return 0;

usage:
    printf("%s <boot image>\n", (argv == NULL || argv[0] == NULL) ? "(nil)" : argv[0]);
    return 1;
}
