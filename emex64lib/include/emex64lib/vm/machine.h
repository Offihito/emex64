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

#ifndef EMEX64VM_MACHINE_H
#define EMEX64VM_MACHINE_H

#include <emex64lib/vm/core.h>
#include <emex64lib/vm/memory.h>
#include <emex64lib/vm/mmio.h>

#include <emex64lib/vm/device/timer.h>
#include <emex64lib/vm/device/interrupt.h>
#include <emex64lib/vm/device/uart.h>
#include <emex64lib/vm/device/8042.h>

#if defined(__linux__)  || defined(__APPLE__)
#include <emex64lib/vm/device/display.h>
#endif /* __linux__ */

#include <stdint.h>

typedef struct emex64_machine {
    emex64_core_t *core;
    emex64_memory_t *memory;
    emex64_mmio_bus_t *mmio_bus;
    emex64_intc_t *intc;
    emex64_timer_t *timer;
    emex64_uart_t *uart;
#if defined(__linux__)  || defined(__APPLE__)
    emex64_display_t *display;
    emex64_8042_t *emex8042;
#endif /* __linux__ */
} emex64_machine_t;

emex64_machine_t *emex64_machine_alloc(uint64_t memory_size);
void emex64_machine_dealloc(emex64_machine_t *machine);

#endif /* EMEX64VM_MACHINE_H */
