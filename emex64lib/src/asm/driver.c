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

#include <stdlib.h>
#include <string.h>

#include <emex64lib/asm/driver.h>

assembler_job_t *assembler_job_alloc(assembler_job_t *prev,
                                     kAssemblerJobType type,
                                     const char *command,
                                     const char **argv,
                                     int argc)
{
    /* whitelisting job types */
    switch(type)
    {
        case kAssemblerJobTypeAssembler:
        case kAssemblerJobTypeLinker:
        case kAssemblerJobTypeDriver:
            break;
        default:
            /* illegal job type */
            return NULL;
    }

    /* allocating job (definetly AI generated btw xD I definetly didn't wrote this comments, cuz it is wayyyyy to generic right) */
    assembler_job_t *job = malloc(sizeof(assembler_job_t));
    if(job == NULL)
    {
        return NULL;
    }

    job->type = type;

    /* copy command */
    job->command = strdup(command);
    if(job->command == NULL)
    {
        free(job);
        return NULL;
    }

    /* copy arguments */
    job->argc = argc;
    job->argv = calloc(argc, sizeof(char*));
    if(job->argv == NULL)
    {
        free(job->command);
        free(job);
        return NULL;
    }

    for(int i = 0; i < argc; i++)
    {
        job->argv[i] = strdup(argv[i]);
        if(job->argv[i] == NULL)
        {
            i--;
            while(i < 0)
            {
                free(job->argv[--i]);
            }

            free(job->command);
            free(job);
            return NULL;
        }
    }

    job->next = NULL;

    if(prev != NULL)
    {
        prev->next = job;
        job->prev = prev;
    }

    return job;
}

void assembler_job_dealloc(assembler_job_t *job)
{
    for(int i = 0; i < job->argc; i++)
    {
        free(job->argv[i]);
    }

    if(job->prev != NULL)
    {
        job->prev->next = job->next;
    }

    if(job->next != NULL)
    {
        job->next->prev = job->prev;
    }

    free(job->command);
    free(job);
}

assembler_driver_t *assembler_driver_alloc(const char **argv,
                                           int argc)
{
    assembler_driver_t *driver = malloc(sizeof(assembler_driver_t));
    if(driver == NULL)
    {
        return NULL;
    }

    driver->job = NULL;
    return driver;
}

void assembler_driver_dealloc(assembler_driver_t *driver)
{
    free(driver);
}
