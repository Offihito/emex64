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

#ifndef EMEX64VM_INSTRUCTION_CTRL_H
#define EMEX64VM_INSTRUCTION_CTRL_H

#include <emex64lib/vm/core.h>

void emex64_op_b(emex64_core_t *core);
void emex64_op_cmp(emex64_core_t *core);
void emex64_op_be(emex64_core_t *core);
void emex64_op_bne(emex64_core_t *core);
void emex64_op_blt(emex64_core_t *core);
void emex64_op_bgt(emex64_core_t *core);
void emex64_op_ble(emex64_core_t *core);
void emex64_op_bge(emex64_core_t *core);
void emex64_op_bz(emex64_core_t *core);
void emex64_op_bnz(emex64_core_t *core);

void emex64_push(emex64_core_t *core, uint64_t value);
uint64_t emex64_pop(emex64_core_t *core);

void emex64_op_bl(emex64_core_t *core);
void emex64_op_ret(emex64_core_t *core);
void emex64_op_iret(emex64_core_t *core);

#endif /* EMEX64VM_INSTRUCTION_CTRL_H */
