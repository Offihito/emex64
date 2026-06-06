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

#include <emex64lib/vm/machine.h>
#include <emex64lib/vm/core.h>
#include <emex64lib/vm/device/8042.h>
#include <string.h>
#include <pthread.h>

static void update_status(emex64_8042_t *dev)
{
    bool has_data = (dev->kbd_head != dev->kbd_tail) || (dev->mouse_head != dev->mouse_tail);
    if(has_data)
    {
        dev->status |= 0x01;
    }
    else
    {
        dev->status &= ~0x01;
    }
}

emex64_8042_t *emex64_8042_alloc(emex64_machine_t *machine)
{
    emex64_8042_t *dev = calloc(1, sizeof(emex64_8042_t));
    if(!dev)
    {
        return NULL;
    }

    pthread_mutex_init(&dev->lock, NULL);
    dev->machine = machine;
    dev->kbd_enabled = true;
    dev->mouse_enabled = true;
    dev->status = 0x10;

    if(!emex64_mmio_register(machine->mmio_bus, EMEX64_8042_BASE,
                             EMEX64_8042_SIZE, dev,
                             emex64_8042_read, emex64_8042_write))
    {
        free(dev);
        return NULL;
    }

    return dev;
}

void emex64_8042_dealloc(emex64_8042_t *dev)
{
    pthread_mutex_destroy(&dev->lock);
    free(dev);
}

static void try_raise_8042_irq(emex64_8042_t *dev)
{
    bool has_data = (dev->kbd_head != dev->kbd_tail) || (dev->mouse_head != dev->mouse_tail);

    if(has_data)
    {
        dev->status |= 0x01;
        emex64_raise_interrupt(dev->machine, EMEX64_IRQ_8042);
    }
    else
    {
        dev->status &= ~0x01;
    }
}

void emex64_8042_send_keyboard(emex64_8042_t *dev, uint8_t scancode)
{
    pthread_mutex_lock(&dev->lock);

    int next = (dev->kbd_tail + 1) % 64;
    if(next != dev->kbd_head)
    {
        dev->kbd_buf[dev->kbd_tail] = scancode;
        dev->kbd_tail = next;
        try_raise_8042_irq(dev);
    }

    pthread_mutex_unlock(&dev->lock);
}

void emex64_8042_send_mouse(emex64_8042_t *dev, uint8_t byte)
{
    pthread_mutex_lock(&dev->lock);

    int next = (dev->mouse_tail + 1) % 64;
    if(next != dev->mouse_head)
    {
        dev->mouse_buf[dev->mouse_tail] = byte;
        dev->mouse_tail = next;
        try_raise_8042_irq(dev);
    }

    pthread_mutex_unlock(&dev->lock);
}

uint64_t emex64_8042_read(emex64_core_t *core,
                          void *device,
                          uint64_t offset,
                          int size)
{
    emex64_8042_t *dev = device;
    uint64_t val = 0;

    pthread_mutex_lock(&dev->lock);

    if(offset == 0x00)
    {
        if(dev->kbd_head != dev->kbd_tail)
        {
            val = dev->kbd_buf[dev->kbd_head];
            dev->kbd_head = (dev->kbd_head + 1) % 64;
        } 
        else if(dev->mouse_head != dev->mouse_tail)
        {
            val = dev->mouse_buf[dev->mouse_head];
            dev->mouse_head = (dev->mouse_head + 1) % 64;
        }
        update_status(dev);
    } 
    else if(offset == 0x08)
    {
        val = dev->status;
    }

    pthread_mutex_unlock(&dev->lock);
    return val;
}

void emex64_8042_write(emex64_core_t *core,
                       void *device,
                       uint64_t offset,
                       uint64_t value,
                       int size)
{
    emex64_8042_t *dev = device;

    pthread_mutex_lock(&dev->lock);

    if(offset == 0x00)
    {
        if(dev->last_command == 0x60)
        {
            dev->command_byte = value;
        }
        dev->last_command = 0;
    } 
    else if(offset == 0x08)
    {
        dev->last_command = value;

        switch(value)
        {
            case 0x20:
                break;
            case 0x60:
                break;
            case 0xA7:
                dev->mouse_enabled = false;
                break;
            case 0xA8:
                dev->mouse_enabled = true;
                break;
            case 0xAD:
                dev->kbd_enabled  = false;
                break;
            case 0xAE:
                dev->kbd_enabled  = true; 
                break;
            case 0xD4:
                dev->expecting_mouse_data = true;
                break;
        }
    }

    pthread_mutex_unlock(&dev->lock);
}
