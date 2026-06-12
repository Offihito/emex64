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

#include <emex64lib/support/diag.h>

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

bool assembler_driver_predrive(assembler_driver_t *driver,
                               const char **argv,
                               int argc)
{
    driver->page_align = true;
    driver->warning_error = false;
    driver->warning_deprecated = true;

    driver->output_path = NULL;
    driver->file_count = 0;
    driver->files = calloc(argc, sizeof(char *));

    driver->inc_dir_cnt = 0;
    driver->inc_dirs = NULL;

    driver->macro_cnt = 0;
    driver->macro = NULL;

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            driver->output_path = argv[++i];
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
                return false;
            }

            if(strcmp(flag, "page_align") == 0)
            {
                driver->page_align = true;
            }
            else if(strcmp(flag, "no_page_align") == 0)
            {
                driver->page_align = false;
            }
            else
            {
                diag_error(NULL, "unknown feature flag '%s'\n", flag);
                return false;
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
                return false;
            }

            if(strcmp(flag, "error") == 0)
            {
                driver->warning_error = true;
            }
            else if(strcmp(flag, "no_error") == 0)
            {
                driver->warning_error = false;
            }
            else if(strcmp(flag, "deprecated") == 0)
            {
                driver->warning_deprecated = true;
            }
            else if(strcmp(flag, "no_deprecated") == 0)
            {
                driver->warning_deprecated = false;
            }
            else
            {
                diag_error(NULL, "unknown warning flag '%s'\n", flag);
                return false;
            }
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
                return false;
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

            uint64_t macro_slot = driver->macro_cnt++;
            if(driver->macro == NULL)
            {
                driver->macro = calloc(driver->macro_cnt, sizeof(assembler_macro_definition_t));
            }
            else
            {
                driver->macro = realloc(driver->macro, driver->macro_cnt * sizeof(assembler_macro_definition_t));
            }

            driver->macro[macro_slot].match = match;
            driver->macro[macro_slot].value = value;
        }
        else if(strncmp(argv[i], "-I", 2) == 0)
        {
            const char *dir;
            if(argv[i][2] != '\0')
            {
                dir = argv[i] + 2;
            }
            else if(i + 1 < argc)
            {
                dir = argv[++i];
            }
            else
            {
                diag_error(NULL, "missing argument to '-I'\n");
                return false;
            }
            driver->inc_dirs = realloc(driver->inc_dirs, (driver->inc_dir_cnt + 1) * sizeof(char*));
            driver->inc_dirs[driver->inc_dir_cnt++] = strdup(dir);
        }
        else if(strncmp(argv[i], "-c", 2) == 0)
        {
            driver->emit_object = true;
        }
        else if(argv[i][0] != '-')
        {
            driver->files[driver->file_count++] = strdup(argv[i]);
        }
        else
        {
            diag_error(NULL, "unknown option '%s'\n", argv[i]);
            return false;
        }
    }

    if(driver->output_path != NULL)
    {
        diag_warn(NULL, "no output path provided, falling back to 'a.out'\n");
        driver->output_path = "a.out";
    }

    return true;
}

assembler_driver_t *assembler_driver_alloc(const char **argv,
                                           int argc)
{
    assembler_driver_t *driver = calloc(1, sizeof(assembler_driver_t));
    if(driver == NULL)
    {
        return NULL;
    }

    if(!assembler_driver_predrive(driver, argv, argc))
    {
        free(driver);
        return NULL;
    }

    driver->job = NULL;
    return driver;
}

void assembler_driver_dealloc(assembler_driver_t *driver)
{
    free(driver);
}
