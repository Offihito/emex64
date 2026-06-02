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

#ifndef EMEX64C_ASTUNIT_H
#define EMEX64C_ASTUNIT_H

#include <stdint.h>
#include <stddef.h>

enum ASTNodeKind: uint8_t {
    /* structures */
    ASTNodeKindTranslationUnit,
    ASTNodeKindFunctionDeclaration,
    ASTNodeKindFunctionDefinition,
    ASTNodeKindStructDeclaration,

    /* statements */
    ASTNodeKindCompoundStatement,
    ASTNodeKindExpressionStatement,
    ASTNodeKindReturnStatement,
    ASTNodeKindIfStatement,
    ASTNodeKindWhileStatement,
    ASTNodeKindForStatement,
    ASTNodeKindBreakStatement,
    ASTNodeKindContinueStatement,

    /* declarations */
    ASTNodeKindVariableDeclaration,
    ASTNodeKindParameterDeclaration,

    /* leaf expressions */
    ASTNodeKindBinaryExpression,
    ASTNodeKindUnaryExpression,
    ASTNodeKindAssignExpression,
    ASTNodeKindFunctionCall,
    ASTNodeKindMemberAccess,
    ASTNodeKindArrayIndex,
    ASTNodeKindCastExpression,
};

/* characters that are in between expressions */
enum OpKind: uint8_t {
    OpKindAdd,
    OpKindSub,
    OpKindMul,
    OpKindDiv,
    OpKindEqual,
    OpKindNotEqual,
    OpKindLessThan,
    OpKindGreaterThan
};

/* characters that are right before literals */
enum UnaryOpKind: uint8_t {
    UnaryOpKindMinus,
    UnaryOpKindNot,
    UnaryOpKindBitwiseNot,
    UnaryOpKindDereference,
    UnaryOpKindReference
};

/* structure access */
enum AccessKind: uint8_t {
    AccessKindDot,
    AccessKindArrow
} AccessKind;

struct ASTNode {
    enum ASTNodeKind kind;

    union {
        /* structures */

        /*
         * the entire c file that needs to be translated
         * down to ASM.
         */
        struct {
            struct ASTNode** declarations;
            size_t declaration_count;
        } translationUnit;

        /*
         * function declarations are the function it self,
         * like..
         * 
         * int foo()
         * {
         *     return 0;
         * }
         * 
         */
        struct {
            char* name;
            //struct DataType* type;
            struct ASTNode** parameters; 
            size_t parameter_count;
        } functionDeclaration;

        /*
         * function definitions are...
         * like..
         * 
         * int foo();
         * 
         */
        struct {
            char* name;
            //struct DataType* type;
            struct ASTNode** parameters;
            size_t parameter_count;
            struct ASTNode** body;       
            size_t body_count;
        } functionDefinition;

        /* bruh, its a declaration of a structure */
        struct {
            char* name;
            struct ASTNode** members;       /* hopefully all variable declaration nodes lol */
            size_t member_count;
        } structDeclaration;

        /* statements */

        struct {
            struct ASTNode** body;
            size_t body_count;
        } compoundStatement;

        struct ASTNodeExpressionStatement {
            struct ASTNode* expression;
        } expressionStatement;

        struct ASTNodeExpressionStatement returnStatement;

        struct {
            struct ASTNode* condition;      /* hopefully an expression */
            struct ASTNode* then_branch;    /* what to execute if condition(yk.. the expression) is met (usually an compound statement) */
            struct ASTNode* else_branch;    /* what to execute if condition is not met (null if not existing) (usually an compound statement) */
        } ifStatement;

        struct {
            struct ASTNode* condition;      /* hopefully an expression */
            struct ASTNode* body;           /* hopefully an (usually a compound statement) */
        } whileStatement;

        struct {
            struct ASTNode* init;
            struct ASTNode* condition;
            struct ASTNode* increment;
            struct ASTNode* body;
        } forStatement;

        /* declarations */

        struct {
            char* name;
            //struct DataType* type;
            struct ASTNode* init;
        } variableDeclaration;

        struct {
            char* name;
            //struct DataType* type;
        } parameterDeclaration;

        /* leaf expressions */

        struct {
            enum OpKind kind;
            struct ASTNode* left;
            struct ASTNode* right;
        } binaryExpression;

        struct {
            enum UnaryOpKind op;
            struct ASTNode* operand;
        } unaryExpression;

        struct {
            struct ASTNode* left;   /* destination */
            struct ASTNode* right;  /* expression */
        } assignExpression;

        struct {
            char* callee;
            struct ASTNode** arguments;
            size_t argument_count;
        } functionCall;

        struct {
            struct ASTNode* object;
            char* member_name;
            enum AccessKind kind;
        } memberAccess;

        struct {
            struct ASTNode* array;
            struct ASTNode* index;
        } arrayIndex;

        struct {
            //struct Type* target_type; I'm so so lazyyyy, data types are kinda hard to think about rn
            struct ASTNode* operand;
        } castExpression;
    };
};

#endif /* EMEX64C_ASTUNIT_H */
