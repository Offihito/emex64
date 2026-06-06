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

#ifndef EMEX64_KEYBOARD_H
#define EMEX64_KEYBOARD_H

#include <stdint.h>

typedef enum: uint8_t {
    kEmexKeyPhysEsc,
    kEmexKeyPhysF1,
    kEmexKeyPhysF2,
    kEmexKeyPhysF3,
    kEmexKeyPhysF4,
    kEmexKeyPhysF5,
    kEmexKeyPhysF6,
    kEmexKeyPhysF7,
    kEmexKeyPhysF8,
    kEmexKeyPhysF9,
    kEmexKeyPhysF10,
    kEmexKeyPhysF11,
    kEmexKeyPhysF12,
    kEmexKeyPhysGrave,
    kEmexKeyPhys1,
    kEmexKeyPhys2,
    kEmexKeyPhys3,
    kEmexKeyPhys4,
    kEmexKeyPhys5,
    kEmexKeyPhys6,
    kEmexKeyPhys7,
    kEmexKeyPhys8,
    kEmexKeyPhys9,
    kEmexKeyPhys0,
    kEmexKeyPhysMinus,
    kEmexKeyPhysEqual,
    kEmexKeyPhysBackspace,
    kEmexKeyPhysTab,
    kEmexKeyPhysQ,
    kEmexKeyPhysW,
    kEmexKeyPhysE,
    kEmexKeyPhysR,
    kEmexKeyPhysT,
    kEmexKeyPhysY,
    kEmexKeyPhysU,
    kEmexKeyPhysI,
    kEmexKeyPhysO,
    kEmexKeyPhysP,
    kEmexKeyPhysLeftBracket,
    kEmexKeyPhysRightBracket,
    kEmexKeyPhysBackslash,
    kEmexKeyPhysCapsLock,
    kEmexKeyPhysA,
    kEmexKeyPhysS,
    kEmexKeyPhysD,
    kEmexKeyPhysF,
    kEmexKeyPhysG,
    kEmexKeyPhysH,
    kEmexKeyPhysJ,
    kEmexKeyPhysK,
    kEmexKeyPhysL,
    kEmexKeyPhysSemicolon,
    kEmexKeyPhysQuote,
    kEmexKeyPhysEnter,
    kEmexKeyPhysLeftShift,
    kEmexKeyPhysZ,
    kEmexKeyPhysX,
    kEmexKeyPhysC,
    kEmexKeyPhysV,
    kEmexKeyPhysB,
    kEmexKeyPhysN,
    kEmexKeyPhysM,
    kEmexKeyPhysComma,
    kEmexKeyPhysPeriod,
    kEmexKeyPhysSlash,
    kEmexKeyPhysRightShift,
    kEmexKeyPhysLeftCtrl,
    kEmexKeyPhysLeftGUI,
    kEmexKeyPhysLeftAlt,
    kEmexKeyPhysSpace,
    kEmexKeyPhysRightAlt,
    kEmexKeyPhysRightGUI,
    kEmexKeyPhysMenu,
    kEmexKeyPhysRightCtrl,
    kEmexKeyPhysInsert,
    kEmexKeyPhysDelete,
    kEmexKeyPhysHome,
    kEmexKeyPhysEnd,
    kEmexKeyPhysPageUp,
    kEmexKeyPhysPageDown,
    kEmexKeyPhysArrowUp,
    kEmexKeyPhysArrowLeft,
    kEmexKeyPhysArrowDown,
    kEmexKeyPhysArrowRight,
    kEmexKeyPhysNumLock,
    kEmexKeyPhysNumpadDivide,
    kEmexKeyPhysNumpadMultiply,
    kEmexKeyPhysNumpadMinus,
    kEmexKeyPhysNumpadPlus,
    kEmexKeyPhysNumpadEnter,
    kEmexKeyPhysNumpad1,
    kEmexKeyPhysNumpad2,
    kEmexKeyPhysNumpad3,
    kEmexKeyPhysNumpad4,
    kEmexKeyPhysNumpad5,
    kEmexKeyPhysNumpad6,
    kEmexKeyPhysNumpad7,
    kEmexKeyPhysNumpad8,
    kEmexKeyPhysNumpad9,
    kEmexKeyPhysNumpad0,
    kEmexKeyPhysNumpadDot,
    kEmexKeyPhysPrintScreen,
    kEmexKeyPhysScrollLock,
    kEmexKeyPhysPause,
    kEmexKeyPhysUnknown
} kEmexKeyPhys;

#endif /* EMEX64_KEYBOARD_H */
