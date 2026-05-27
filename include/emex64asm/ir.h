
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

#ifndef EMEX64ASM_IR_H
#define EMEX64ASM_IR_H

#include <stdint.h>
#include <stddef.h>

#define LA64_EXPR_TYPE_INSTRUCTION  0b00000000
#define LA64_EXPR_TYPE_GLOBAL_LABEL 0b00000001
#define LA64_EXPR_TYPE_LOCAL_LABEL  0b00000010

#define LA64_LABEL_STR_MAX          512

typedef struct la64_operand {
    uint8_t coding;
    uint64_t value;
} la64_operand_t;

typedef struct la64_instr {
    uint8_t opcode;
    la64_operand_t operands[32];
} la64_instr_t;

typedef struct la64_label {
    char label_str[LA64_LABEL_STR_MAX];
} la64_label_t;

typedef struct la64_expr {
    uint8_t type;
    union {
        la64_instr_t instr;
        la64_label_t label;
    };
} la64_expr_t;

typedef struct la64_ir_program {
    size_t expr_cnt;
    la64_expr_t *expr;
} la64_ir_program_t;

la64_ir_program_t *la64_ir_program_alloc(void);
void la64_ir_program_dealloc(la64_ir_program_t *p);

#endif /* EMEX64ASM_IR_H */