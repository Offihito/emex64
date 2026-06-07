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
#include <string.h>
#include <unistd.h>

#include <emex64lib/support/diag.h>

#include <emex64lib/asm/invocation.h>
#include <emex64lib/asm/code.h>
#include <emex64lib/asm/label.h>
#include <emex64lib/asm/emit.h>
#include <emex64lib/asm/section.h>
#include <emex64lib/asm/macro.h>

int main(int argc, char *argv[])
{
    int opt;
    const char *output_path = NULL;
    int file_count = 0;
    char **files = calloc(argc, sizeof(char *));

    /* invocation settings */
    const char *start_entry_name = "_start";
    bool page_align = true;
    bool absolute_addr_align = true;
    bool warning_deprecated = true;
    bool offset_branch = true;

    /* macros passed to assembler arguments */
    uint64_t macro_cnt = 0;
    assembler_macro_definition_t *macro = NULL;

    /* parse arguments */
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            output_path = argv[++i];
        }
        else if(strncmp(argv[i], "-f", 2) == 0)
        {
            const char *flag;
            if(argv[i][2] != '\0')
            {
                flag = argv[i] + 2;
            }
            else if(i + 1 < argc)
            {
                flag = argv[++i];
            }
            else
            {
                diag_error(NULL, "missing argument to '-f'\n");
                goto failed;
            }

            if(strcmp(flag, "page_align") == 0)
            {
                page_align = true;
            }
            else if(strcmp(flag, "no_page_align") == 0)
            {
                page_align = false;
            }
            else if(strcmp(flag, "absolute_addr_align") == 0)
            {
                absolute_addr_align = true;
            }
            else if(strcmp(flag, "no_absolute_addr_align") == 0)
            {
                absolute_addr_align = false;
            }
            else if(strcmp(flag, "offset_branch") == 0)
            {
                offset_branch = true;
            }
            else if(strcmp(flag, "no_offset_branch") == 0)
            {
                offset_branch = false;
            }
            else
            {
                diag_error(NULL, "unknown feature flag '%s'\n", flag);
                goto failed;
            }
        }
        else if(strncmp(argv[i], "-W", 2) == 0)
        {
            const char *flag;
            if(argv[i][2] != '\0')
            {
                flag = argv[i] + 2;
            }
            else if(i + 1 < argc)
            {
                flag = argv[++i];
            }
            else
            {
                diag_error(NULL, "missing argument to '-W'\n");
                goto failed;
            }

            if(strcmp(flag, "error") == 0)
            {
                warning_error = true;
            }
            else if(strcmp(flag, "no_error") == 0)
            {
                warning_error = false;
            }
            else if(strcmp(flag, "deprecated") == 0)
            {
                warning_deprecated = true;
            }
            else if(strcmp(flag, "no_deprecated") == 0)
            {
                warning_deprecated = false;
            }
            else
            {
                diag_error(NULL, "unknown warning flag '%s'\n", flag);
                goto failed;
            }
        }
        else if(strncmp(argv[i], "-e", 2) == 0)
        {
            const char *flag;
            if(argv[i][2] != '\0')
            {
                flag = argv[i] + 2;
            }
            else if(i + 1 < argc)
            {
                flag = argv[++i];
            }
            else
            {
                diag_error(NULL, "missing argument to '-e'\n");
                goto failed;
            }

            start_entry_name = flag;
        }
        else if(strncmp(argv[i], "-D", 2) == 0)
        {
            const char *flag;
            if(argv[i][2] != '\0')
            {
                flag = argv[i] + 2;
            }
            else if(i + 1 < argc)
            {
                flag = argv[++i];
            }
            else
            {
                diag_error(NULL, "missing argument to '-D'\n");
                goto failed;
            }

            const char *eq = strchr(flag, '=');
            char *match = NULL;
            char *value = NULL;
            if(!eq)
            {
                match = strdup(flag);
                size_t len = strlen(match);
                if(len > 0 && (match[len-1] == '"' || match[len-1] == '\''))
                {
                    match[len-1] = '\0';
                }
                value = "1";
            }

            size_t name_len = eq - flag;
            match = malloc(name_len + 1);
            memcpy(match, flag, name_len);
            match[name_len] = '\0';

            const char *v = eq + 1;
            char quote = 0;

            if(*v == '"' || *v == '\'')
            {
                quote = *v;
                v++;
            }

            const char *end = v;
            while (*end && *end != quote && *end != '\0') end++;

            size_t val_len = end - v;
            value = malloc(val_len + 1);
            memcpy(value, v, val_len);
            value[val_len] = '\0';

            uint64_t macro_slot = macro_cnt++;
            if(macro == NULL)
            {
                macro = calloc(macro_cnt, sizeof(assembler_macro_definition_t));
            }
            else
            {
                macro = realloc(macro, macro_cnt * sizeof(assembler_macro_definition_t));
            }

            macro[macro_slot].match = match;
            macro[macro_slot].value = value;
        }
        else if(argv[i][0] != '-')
        {
            files[file_count++] = strdup(argv[i]);
        }
        else
        {
            diag_error(NULL, "unknown option '%s'\n", argv[i]);
            goto failed;
        }
    }

    /* checking for output path */
    if(!output_path)
    {
        diag_warn(NULL, "no output binary specified, falling back to a.out\n");
        output_path = "a.out";
    }

    assembler_options_t options = assembler_options_default();
    options.page_align = page_align;
    options.absolute_addr_align = absolute_addr_align;
    options.offset_branch = offset_branch;
    options.start_entry_name = start_entry_name;
    options.warning_error = warning_error;
    options.warning_deprecated = warning_deprecated;

    /* allocating compiler invocation */
    assembler_invocation_t *inv = assembler_invocation_alloc_with_options(output_path, options);
    if(inv == NULL)
    {
        diag_error(NULL, "something went terribly wrong\n");
        goto failed;
    }

    inv->definition = macro;
    inv->definition_cnt = macro_cnt;

    bool succeeded = assembler_invocation_emit(inv, file_count, files);
    for(int i = 0; i < file_count; i++)
    {
        free(files[i]);
    }
    free(files);
    if(!succeeded)
    {
        goto failed;
    }

    return 0;

failed:
    unlink(output_path);
    return 1;
}
