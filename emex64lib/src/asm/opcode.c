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

#include <emex64lib/asm/opcode.h>
#include <emex64lib/asm/emit.h>

const opcode_entry_t opcode_table[] = {
    /* core operations */
    { .name = "hlt",    .opcode = kEmex64OpcodeHLT,         .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "nop",    .opcode = kEmex64OpcodeNOP,         .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },

    /* data operations */
    { .name = "mov",    .opcode = kEmex64OpcodeMOV,         .minargs = 2, .maxargs = 2,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "swp",    .opcode = kEmex64OpcodeSWP,         .minargs = 2, .maxargs = 2,  .argmask = 0b11000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "swpz",   .opcode = kEmex64OpcodeSWPZ,        .minargs = 2, .maxargs = 2,  .argmask = 0b11000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "push",   .opcode = kEmex64OpcodePUSH,        .minargs = 1, .maxargs = 32, .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "pop",    .opcode = kEmex64OpcodePOP,         .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ldb",    .opcode = kEmex64OpcodeLDB,         .minargs = 2, .maxargs = 2,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ldw",    .opcode = kEmex64OpcodeLDW,         .minargs = 2, .maxargs = 2,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ldd",    .opcode = kEmex64OpcodeLDD,         .minargs = 2, .maxargs = 2,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ldq",    .opcode = kEmex64OpcodeLDQ,         .minargs = 2, .maxargs = 2,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "stb",    .opcode = kEmex64OpcodeSTB,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "stw",    .opcode = kEmex64OpcodeSTW,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "std",    .opcode = kEmex64OpcodeSTD,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "stq",    .opcode = kEmex64OpcodeSTQ,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },

    /* alu operations */
    { .name = "add",    .opcode = kEmex64OpcodeADD,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "sub",    .opcode = kEmex64OpcodeSUB,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "mul",    .opcode = kEmex64OpcodeMUL,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "div",    .opcode = kEmex64OpcodeDIV,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "idiv",   .opcode = kEmex64OpcodeIDIV,        .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "mod",    .opcode = kEmex64OpcodeMOD,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "not",    .opcode = kEmex64OpcodeNOT,         .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "neg",    .opcode = kEmex64OpcodeNEG,         .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "and",    .opcode = kEmex64OpcodeAND,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "or",     .opcode = kEmex64OpcodeOR,          .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "xor",    .opcode = kEmex64OpcodeXOR,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "shr",    .opcode = kEmex64OpcodeSHR,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "shl",    .opcode = kEmex64OpcodeSHL,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "sar",    .opcode = kEmex64OpcodeSAR,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ror",    .opcode = kEmex64OpcodeROR,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "rol",    .opcode = kEmex64OpcodeROL,         .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "pdep",   .opcode = kEmex64OpcodePDEP,        .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "pext",   .opcode = kEmex64OpcodePEXT,        .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bswapw", .opcode = kEmex64OpcodeBSWAPW,      .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bswapd", .opcode = kEmex64OpcodeBSWAPD,      .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bswapq", .opcode = kEmex64OpcodeBSWAPQ,      .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "inc",    .opcode = kEmex64OpcodeINC,         .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "dec",    .opcode = kEmex64OpcodeDEC,         .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111, .dnstr = NULL, .handler = assembler_emit_instruction_generic },

    /* contol flow operations */
    { .name = "b",      .opcode = kEmex64OpcodeB,           .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "cmp",    .opcode = kEmex64OpcodeCMP,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "be",     .opcode = kEmex64OpcodeBE,          .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bne",    .opcode = kEmex64OpcodeBNE,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "blt",    .opcode = kEmex64OpcodeBLT,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bgt",    .opcode = kEmex64OpcodeBGT,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ble",    .opcode = kEmex64OpcodeBLE,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bge",    .opcode = kEmex64OpcodeBGE,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bz",     .opcode = kEmex64OpcodeBZ,          .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bnz",    .opcode = kEmex64OpcodeBNZ,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "blw",    .opcode = kEmex64OpcodeBLW,         .minargs = 1, .maxargs = 32, .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "wret",   .opcode = kEmex64OpcodeWRET,        .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "iret",   .opcode = kEmex64OpcodeIRET,        .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "bl",     .opcode = kEmex64OpcodeBL,          .minargs = 1, .maxargs = 32, .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },
    { .name = "ret",    .opcode = kEmex64OpcodeRET,         .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000, .dnstr = NULL, .handler = assembler_emit_instruction_generic },

    /* compatibility stubs for older code */
    { .name = "jmp",    .opcode = kEmex64OpcodeB,           .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"b\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "je",     .opcode = kEmex64OpcodeBE,          .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"be\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jne",    .opcode = kEmex64OpcodeBNE,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"bne\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jlt",    .opcode = kEmex64OpcodeBLT,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"blt\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jgt",    .opcode = kEmex64OpcodeBGT,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"bgt\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jle",    .opcode = kEmex64OpcodeBLE,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"ble\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jge",    .opcode = kEmex64OpcodeBGE,         .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"bge\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jz",     .opcode = kEmex64OpcodeBZ,          .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"bz\" instead", .handler = assembler_emit_instruction_generic },
    { .name = "jnz",    .opcode = kEmex64OpcodeBNZ,         .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000, .dnstr = "use \"bnz\" instead", .handler = assembler_emit_instruction_generic },

    /* pseudo opcodes */
    { .name = "clr",    .opcode = kEmex64OpcodeHLT,         .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111, .dnstr = NULL, .handler = assembler_emit_instruction_clr },
};

const opcode_entry_t *opcode_from_string(const char *name)
{
    /* null pointer check */
    if(name == NULL)
    {
        return NULL;
    }

    /* iterating through table */
    for(unsigned char opcode = 0x00; opcode < (sizeof(opcode_table) / sizeof(opcode_table[0])); opcode++)
    {
        /* check if opcode name matches */
        if(strcmp(opcode_table[opcode].name, name) == 0)
        {
            return &opcode_table[opcode];
        }
    }

    /* shouldnt happen if code is correct */
    return NULL;
}

bool opcode_arg_accepts_reg_only(const opcode_entry_t *opce,
                                 uint8_t arg)
{
    /* null pointer check */
    if(opce == NULL)
    {
        return false;
    }

    /* lol how tiny that operation is */
    return (opce->argmask & (1u << (31 - arg))) != 0;
}
