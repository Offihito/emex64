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

#ifndef EMEX64VM_DEVICE_8042_H
#define EMEX64VM_DEVICE_8042_H

#define EMEX64_8042_BASE    0x1FE00600
#define EMEX64_8042_SIZE    0x10

typedef struct emex64_core emex64_core_t;
typedef struct emex64_machine emex64_machine_t;

typedef struct {
    uint8_t status;
    uint8_t command_byte;
    uint8_t last_command;

    uint8_t kbd_buf[64];
    int kbd_head;
    int kbd_tail;

    uint8_t mouse_buf[64];
    int mouse_head;
    int mouse_tail;

    bool kbd_enabled;
    bool mouse_enabled;
    bool expecting_mouse_data;

    pthread_mutex_t lock;
    emex64_machine_t *machine;
} emex64_8042_t;

emex64_8042_t *emex64_8042_alloc(emex64_machine_t *machine);
void emex64_8042_dealloc(emex64_8042_t *dev);

/* for display backend */
void emex64_8042_send_keyboard(emex64_8042_t *dev, uint8_t scancode);
void emex64_8042_send_mouse(emex64_8042_t *dev, uint8_t byte);

uint64_t emex64_8042_read(emex64_core_t *core, void *device, uint64_t offset, int size);
void emex64_8042_write(emex64_core_t *core, void *device, uint64_t offset, uint64_t value, int size);

#endif /* EMEX64VM_DEVICE_8042_H */
