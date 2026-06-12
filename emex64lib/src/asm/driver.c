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
    driver->input_path_count = 0;
    driver->input_path = calloc(argc, sizeof(char *));

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
        else if(strncmp(argv[i], "-Wl,", 4) == 0)
        {
            const char *p = argv[i] + 4;
            if(*p == '\0')
            {
                diag_error(NULL, "missing argument to '-Wl,'\n");
                return false;
            }

            while(*p != '\0')
            {
                const char *comma = strchr(p, ',');
                size_t len = comma ? (size_t)(comma - p) : strlen(p);

                char *arg = malloc(len + 1);
                memcpy(arg, p, len);
                arg[len] = '\0';

                driver->linker_flags = realloc(driver->linker_flags, (driver->linker_flags_cnt + 1) * sizeof(char *));
                driver->linker_flags[driver->linker_flags_cnt++] = arg;

                p = comma ? comma + 1 : p + len;
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
        else if(strncmp(argv[i], "-v", 2) == 0)
        {
            driver->verbose = true;
        }
        else if(argv[i][0] != '-')
        {
            driver->input_path[driver->input_path_count++] = strdup(argv[i]);
        }
        else
        {
            diag_error(NULL, "unknown option '%s'\n", argv[i]);
            return false;
        }
    }

    if(driver->output_path == NULL)
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

    if(driver->verbose)
    {
        fprintf(stderr, "---- driver ----\n");
        fprintf(stderr, "page_align: %d\n", driver->page_align);
        fprintf(stderr, "warning_error: %d\n", driver->warning_error);
        fprintf(stderr, "warning_deprecated: %d\n", driver->warning_deprecated);
        fprintf(stderr, "emit_object: %d\n", driver->emit_object);
        fprintf(stderr, "verbose: %d\n", driver->verbose);
        fprintf(stderr, "output_path: %s\n", driver->output_path);

        fprintf(stderr, "input_path[%d]: { ", driver->input_path_count);
        for(int i = 0; i < driver->input_path_count; i++)
        {
            if(i != 0)
            {
                fprintf(stderr, ", ");
            }
            fprintf(stderr, "%s ", driver->input_path[i]);
        }
        fprintf(stderr, "}\n");

        fprintf(stderr, "inc_dirs[%zu]: { ", driver->inc_dir_cnt);
        for(size_t i = 0; i < driver->inc_dir_cnt; i++)
        {
            if(i != 0)
            {
                fprintf(stderr, ", ");
            }
            fprintf(stderr, "%s ", driver->inc_dirs[i]);
        }
        fprintf(stderr, "}\n");

        fprintf(stderr, "macro[%llu]: { ", driver->macro_cnt);
        for(uint64_t i = 0; i < driver->macro_cnt; i++)
        {
            if(i != 0)
            {
                fprintf(stderr, ", ");
            }
            fprintf(stderr, "(match='%s' | replacement='%s') ", driver->macro[i].match, driver->macro[i].value);
        }
        fprintf(stderr, "}\n");

        fprintf(stderr, "linker_flags[%d]: { ", driver->linker_flags_cnt);
        for(int i = 0; i < driver->linker_flags_cnt; i++)
        {
            if(i != 0)
            {
                fprintf(stderr, ", ");
            }
            fprintf(stderr, "%s ", driver->linker_flags[i]);
        }
        fprintf(stderr, "}\n");

        /*
    struct assembler_job *prev;
    struct assembler_job *next;
        */

        driver->job = assembler_job_alloc(NULL, kAssemblerJobTypeAssembler, "emex64asm", argv, argc);
        assembler_job_alloc(driver->job, kAssemblerJobTypeLinker, "emex64ld", argv, argc);

        fprintf(stderr, "jobs: {");
        assembler_job_t *job = driver->job;
        if(job != NULL)
        {
            fprintf(stderr, "\n");
        }
        while(job != NULL)
        {
            fprintf(stderr, "\t{\n");
            fprintf(stderr, "\t\ttype: %d\n", job->type);
            fprintf(stderr, "\t\tcommand: %s\n", job->command);
            fprintf(stderr, "\t\targv[%d]: { ", job->argc);
            for(int i = 0; i < job->argc; i++)
            {
                if(i != 0)
                {
                    fprintf(stderr, ", ");
                }
                fprintf(stderr, "%s ", job->argv[i]);
            }
            fprintf(stderr, "}\n");
            fprintf(stderr, "\t\tprev: %p\n ", (void*)job->prev);
            fprintf(stderr, "\t\tnext: %p\n ", (void*)job->next);
            fprintf(stderr, "\t}");
            fprintf(stderr, "\n");

            job = job->next;
        }
        fprintf(stderr, "}\n");
    }

    driver->job = NULL;
    return driver;
}

void assembler_driver_dealloc(assembler_driver_t *driver)
{
    free(driver);
}
