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
    const char *firmware_image_path = NULL;
    uint64_t memsize = 100 * 1024 * 1024;   /* standard is 100MB */

    /* parse arguments */
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--help") == 0)
        {
        usage:
            fprintf(stderr, "%s [options]\n", argv[0]);
            fprintf(stderr, "\t--help                   : showing help menu\n");
            fprintf(stderr, "\t--firmware <image path>  : providing firmware image\n");
            fprintf(stderr, "\t--memory <memory size>   : providing memory size in megabyte\n");
            return 1;
        }
        else if(strcmp(argv[i], "--firmware") == 0 && i + 1 < argc || strcmp(argv[i], "--bios") == 0 && i + 1 < argc)
        {
            firmware_image_path = argv[++i];
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
                return 1;
            }
        }
        else
        {
            diag_error(NULL, "unknown option '%s'\n", argv[i]);
            return 1;
        }
    }

    /* creating new emex64 virtual machine */
    emex64_machine_t *machine = emex64_machine_alloc(memsize);
    if(machine == NULL)
    {
        diag_error(NULL, "failed to allocated machine\n");
        return 1;
    }

    /*
     * load boot image, maybe use a dirty private
     * mapping and open the image it self as memory,
     * just size it.
     */
    if(firmware_image_path != NULL && !emex64_memory_load_image(machine->memory, firmware_image_path))
    {
        diag_error(NULL, "failed to load firmware image\n");
        return 1;
    }
    
    /* executing virtual machines 1st core TODO: Implement threading */
    emex64_core_execute(machine->core);

    /* deallocating machine */
    emex64_machine_dealloc(machine);

    return 0;
}
