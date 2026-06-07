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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <emex64lib/support/diag.h>
#include <emex64lib/support/parser.h>

#include <emex64lib/asm/macro.h>
#include <emex64lib/asm/code.h>

typedef struct {
    const char *match;
    const char *replacement;
    /* TODO: do token replacement */
    //assembler_token_t *token;
    //uint64_t token_cnt;
} assembler_macro_t;

bool assembler_macro_expand(assembler_invocation_t *inv)
{
    /* count the amount of macros */
    uint64_t c = 0;
    for(uint64_t i = 0; i < inv->line_cnt; i++)
    {
        if(inv->line[i]->type == kAssemblerLineTypeMacroDefinition)
        {
            c++;
        }
    }

    /* allocating */
    assembler_macro_t *am = calloc(c, sizeof(assembler_macro_t));
    if(am == NULL)
    {
        diag_error(NULL, "something terrible has happened\n");
        return false;
    }

    /* adding stuff */
    c = 0;
    for(uint64_t i = 0; i < inv->line_cnt; i++)
    {
        if(inv->line[i]->type == kAssemblerLineTypeMacroDefinition)
        {
            am[c].match = inv->line[i]->token[1]->str;
            am[c].replacement = inv->line[i]->token[2]->str;
            c++;
        }
    }

    /* now replacing */
    for(uint64_t i = 0; i < inv->line_cnt; i++)
    {
        if(inv->line[i]->type != kAssemblerLineTypeMacroDefinition)
        {
            for(uint64_t a = 0; a < inv->line[i]->token_cnt; a++)
            {
                for(uint64_t b = 0; b < c; b++)
                {
                    if(strcmp(inv->line[i]->token[a]->str, am[b].match) == 0)
                    {
                        /* check */

                        char *copy_replacement = strdup(am[b].replacement);
                        if(copy_replacement == NULL)
                        {
                            diag_error(NULL, "something terrible has happened\n");
                            free(am);
                            return false;
                        }

                        free(inv->line[i]->token[a]->str);
                        inv->line[i]->token[a]->str = copy_replacement;
                        break;
                    }
                }
            }
        }
    }

    free(am);

    return true;
}
