//	===========================================================
//
//	Copyright 2006-2008 by CyberView Inc. All rights reserved.
//
//	It is prohibited to disclose or redistribute any portion
//	of the code in this file without prior permission from
//	CyberView Inc.
//
//	===========================================================
//
// fixedPoint.c/fixedPoint.h - Module that provides functions for Fixed Point Arithmetics


#ifndef FIXEDPOINT_C_H
#define FIXEDPOINT_C_H

#ifdef WIN32
typedef __int64 LongInt ;
#else
typedef int LongInt ;
#endif

typedef struct {
	LongInt value ;
	char shift ;
} FixedPoint ;

typedef struct {
	FixedPoint v[3][3] ;
} MAT3x3_FP ;

#ifdef __cplusplus
extern "C" {
#endif

// FP_ADD:		a + b -> d
FixedPoint *FP_ADD(FixedPoint *a, FixedPoint *b, FixedPoint *d) ;

// FP_ADD_I:	a + integer_b -> d
FixedPoint *FP_ADD_I(FixedPoint *a, int b, FixedPoint *d) ;

// FP_SUB:		a - b -> d
FixedPoint *FP_SUB(FixedPoint *a, FixedPoint *b, FixedPoint *d) ;

// FP_SUB_I:	a - integer_b -> d
FixedPoint *FP_SUB_I(FixedPoint *a, int b, FixedPoint *d) ;

// FP_MUL:		a * b -> d
FixedPoint *FP_MUL(FixedPoint *a, FixedPoint *b, FixedPoint *d) ;

// FP_MUL_I:	a * integer_b -> d
FixedPoint *FP_MUL_I(FixedPoint *a, int b, FixedPoint *d) ;

// FP_DIV:		a / b -> d ;
FixedPoint *FP_DIV(FixedPoint *a, FixedPoint *b, FixedPoint *d) ;

// FP_DIV_I:	a / integer_b -> d ;
FixedPoint *FP_DIV_I(FixedPoint *a, int b, FixedPoint *d) ;

// FP_SET:		integer_v -> d
FixedPoint *FP_SET(int v, FixedPoint *d) ;

// FP_ROUND:	return rounded integer of the value of d
int  FP_ROUND(FixedPoint *d) ;

// FP_ROUND_ABS: return rounded absolute integer of the value of d
int FP_ROUND_ABS(FixedPoint *d) ;

// FP_INIT:		Initialize the number of bits for binary fractions
FixedPoint *FP_INIT(FixedPoint *d, char shift) ;

// FP_ZERO:		0 -> d
FixedPoint *FP_ZERO(FixedPoint *d) ;

// FP_CAST:		Assign the value of a to d, keeping the number of bits for binary fractions of both a and d unchanged 
FixedPoint *FP_CAST(FixedPoint *a, FixedPoint *d) ;

// FP_NEG:		-a -> d
FixedPoint *FP_NEG(FixedPoint *a, FixedPoint *d) ;

// FP_MOV:		Move the value of a to d
FixedPoint *FP_MOV(FixedPoint *a, FixedPoint *d) ;

// FP_ABS:		ABS(a) -> d
FixedPoint *FP_ABS(FixedPoint *a, FixedPoint *d) ;

// Fixed Point Matrix Operations

// FP_MAT3x3_MUL_V:	a (3 x 3) * v (3 x 1) -> r (3 x 1)
void FP_MAT3x3_MUL_V(MAT3x3_FP *a, FixedPoint *v, FixedPoint *r) ;

// FP_MAT3x3_MUL:	a (3 x 3) * b (3 x 3) -> c (3 x 1)
void FP_MAT3x3_MUL(MAT3x3_FP *a, MAT3x3_FP *b, MAT3x3_FP *c) ;

// FP_MAT3x3_INV:	inverse(a) -> b, return the determinant of a
FixedPoint FP_MAT3x3_INV(MAT3x3_FP *a, MAT3x3_FP *b) ;

// FP_MAT3x3_INIT:	Initialize the number of bits for binary fractions of all entries in matrix 'm' to 'shift'
MAT3x3_FP *FP_MAT3x3_INIT(MAT3x3_FP *m, char shift) ;

// FP_MAT3x3_ZERO:	Set all entries in matirx 'm' to zero
MAT3x3_FP *FP_MAT3x3_ZERO(MAT3x3_FP *m) ;

// FP_MAT3x3_IDENTITY:	Set m to identity matrix
MAT3x3_FP *FP_MAT3x3_IDENTITY(MAT3x3_FP *m) ;

// FP_MAT3x3_ADD:	a (3 x 3) + b (3 x 3) -> d (3 x 3)
MAT3x3_FP *FP_MAT3x3_ADD(MAT3x3_FP *a, MAT3x3_FP *b, MAT3x3_FP *d) ;

// FP_MAT3x3_SUB:	a (3 x 3) - b (3 x 3) -> d (3 x 3)
MAT3x3_FP *FP_MAT3x3_SUB(MAT3x3_FP *a, MAT3x3_FP *b, MAT3x3_FP *d) ;

#ifdef __cplusplus
}
#endif

#endif

