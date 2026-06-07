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
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <emex64lib/support/file.h>

emex_file_t *emex_file_alloc(const char *path)
{
    emex_file_t *f = malloc(sizeof(emex_file_t));
    if(f == NULL)
    {
        return NULL;
    }

    /*
     * resolving the true paths is important
     * so errors can reveal the actual file
     * locations.
     */
    f->path = malloc(PATH_MAX);
    if(realpath(path, f->path) == NULL)
    {
        free(f);
        return NULL;
    }

    /* setting standard values */
    f->len = 0;
    f->code = MAP_FAILED;

    return f;
}

void emex_file_dealloc(emex_file_t *f)
{
    emex_file_close(f);
    free(f->path);
    free(f);
}

bool emex_file_open(emex_file_t *f)
{
    if(f->code != MAP_FAILED)
    {
        emex_file_close(f);
    }

    /* initial open */
    int fd = open(f->path, O_RDONLY);
    if(fd < 0)
    {
        return false;
    }

    /* initially mapping assembly file */
    struct stat fdstat;
    if(fstat(fd, &fdstat) < 0)
    {
        close(fd);
        return false;
    }

    f->len = fdstat.st_size;
    f->code = mmap(NULL, f->len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if(f->code == MAP_FAILED)
    {
        close(fd);
        return false;
    }

    close(fd);

    return true;
}

void emex_file_close(emex_file_t *f)
{
    if(f->code != MAP_FAILED)
    {
        munmap(f->code, f->len);
        f->code = MAP_FAILED;
        f->len = 0;
    }
}
