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

#include "fixedPoint_c.h"

#ifdef __cplusplus
extern "C" {
#endif

int MIN_SHIFT2(FixedPoint *a, FixedPoint *b) {
	int min_shift ;
	int gtFlag ;

	gtFlag = (((a->shift - b->shift) + 255) / 256) * 255 ;
	min_shift = (gtFlag & b->shift) | ((~gtFlag) & a->shift) ;

	return min_shift ;
}

int MIN_SHIFT3(FixedPoint *a, FixedPoint *b, FixedPoint *c) {

	int min_shift ;
	int gtFlag ;

	gtFlag = (((a->shift - b->shift) + 255) / 256) * 255 ;
	min_shift = (gtFlag & b->shift) | ((~gtFlag) & a->shift) ;

	gtFlag = (((min_shift - c->shift) + 255) / 256) * 255 ;
	min_shift = (gtFlag & c->shift) | ((~gtFlag) & min_shift) ;

	return min_shift ;
}

// FP_ADD:		a + b -> d
FixedPoint *FP_ADD(FixedPoint *a, FixedPoint *b, FixedPoint *d) {
	int min_shift ;

	min_shift = MIN_SHIFT3(a, b, d) ;

	d->value = ((a->value >> (a->shift - min_shift)) + (b->value >> (b->shift - min_shift))) << (d->shift - min_shift) ;

	return d ;
}

// FP_ADD_I:	a + integer_b -> d
FixedPoint *FP_ADD_I(FixedPoint *a, int b, FixedPoint *d) {
	FixedPoint B ;

	B.shift = a->shift ;
	B.value = b << a->shift ;

	return FP_ADD(a, &B, d) ;
}

// FP_SUB:		a - b -> d
FixedPoint *FP_SUB(FixedPoint *a, FixedPoint *b, FixedPoint *d) {
	int min_shift ;

	min_shift = MIN_SHIFT3(a, b, d) ;

	d->value = ((a->value >> (a->shift - min_shift)) - (b->value >> (b->shift - min_shift))) << (d->shift - min_shift) ;

	return d ;
}

// FP_SUB_I:	a - integer_b -> d
FixedPoint *FP_SUB_I(FixedPoint *a, int b, FixedPoint *d) {
	FixedPoint B ;

	B.shift = a->shift ;
	B.value = b << a->shift ;

	return FP_SUB(a, &B, d) ;
}

// FP_MUL:		a * b -> d
FixedPoint *FP_MUL(FixedPoint *a, FixedPoint *b, FixedPoint *d) {
	int total_shift ;

	total_shift = a->shift + b->shift ;

	d->value = (a->value * b->value) >> (total_shift - d->shift) ;

	return d ;
}

// FP_MUL_I:	a * integer_b -> d
FixedPoint *FP_MUL_I(FixedPoint *a, int b, FixedPoint *d) {
	FixedPoint B ;

	B.shift = a->shift ;
	B.value = b << a->shift ;

	return FP_MUL(a, &B, d) ;
}

// FP_DIV:		a / b -> d ;
FixedPoint *FP_DIV(FixedPoint *a, FixedPoint *b, FixedPoint *d) {
	int min_shift ;

	min_shift = MIN_SHIFT3(a, b, d) ;

	d->value = (((a->value >> (a->shift - min_shift)) << b->shift) / b->value) << (d->shift - min_shift) ;

	return d ;
}

// FP_DIV_I:	a / integer_b -> d ;
FixedPoint *FP_DIV_I(FixedPoint *a, int b, FixedPoint *d) {
	FixedPoint B ;

	B.shift = a->shift ;
	B.value = b << a->shift ;

	return FP_DIV(a, &B, d) ;
}

// FP_SET:		integer_v -> d
FixedPoint *FP_SET(int v, FixedPoint *d) {

	d->value = v << d->shift ;

	return d ;
}

// FP_ROUND:	return rounded integer of the value of d
int FP_ROUND(FixedPoint *d) {

	LongInt ltFlag ;
	int mask ;
	LongInt absValue ;
	int value ;

	ltFlag = (d->value & (((LongInt) 1) << (sizeof(LongInt) * 8 - 1))) ;
	ltFlag >>= (sizeof(LongInt) * 8 - 1) ;
	mask = (int) ltFlag ;
	absValue = ((-d->value) & ltFlag) | (d->value & (~ltFlag)) ;
	value = (int) ((absValue + (((LongInt) 1) << (d->shift - 1))) >> d->shift) ;
	return value * (((-1) & mask) | ((1) & (~mask))) ;
}

// FP_ROUND_ABS: return rounded absolute integer of the value of d
int FP_ROUND_ABS(FixedPoint *d) {

	LongInt ltFlag ;
	LongInt absValue ;
	int value ;

	ltFlag = (d->value & (((LongInt) 1) << (sizeof(LongInt) * 8 - 1))) ;
	ltFlag >>= (sizeof(LongInt) * 8 - 1) ;
	absValue = ((-d->value) & ltFlag) | (d->value & (~ltFlag)) ;
	value = (int) ((absValue + (((LongInt) 1) << (d->shift - 1))) >> d->shift) ;
	return value ;
}

// FP_ABS:		ABS(a) -> d
FixedPoint *FP_ABS(FixedPoint *a, FixedPoint *d) {

	int min_shift ;
	LongInt ltFlag ;
	LongInt absValue ;

	min_shift = MIN_SHIFT2(a, d) ;
	ltFlag = (d->value & (((LongInt) 1) << (sizeof(LongInt) * 8 - 1))) ;
	ltFlag >>= (sizeof(LongInt) * 8 - 1) ;
	absValue = ((-a->value) & ltFlag) | (a->value & (~ltFlag)) ;
	d->value = 	(absValue >> (a->shift - min_shift)) << (d->shift - min_shift) ;
	return d ;
}

// FP_INIT:		Initialize the number of bits for binary fractions
FixedPoint *FP_INIT(FixedPoint *d, char shift) {

	d->value = (LongInt) 0 ;
	d->shift = shift ;

	return d ;
}

// FP_ZERO:		0 -> d
FixedPoint *FP_ZERO(FixedPoint *d) {

	d->value = 0 ;

	return d ;
}

// FP_MOV:		Move the value of a to d
FixedPoint *FP_MOV(FixedPoint *a, FixedPoint *d) {

	d->value = a->value ;
	d->shift = a->shift ;

	return d ;
}

// FP_CAST:		Assign the value of a to d, keeping the number of bits for binary fractions of both a and d unchanged 
FixedPoint *FP_CAST(FixedPoint *a, FixedPoint *d) {

	int min_shift ;

	min_shift = MIN_SHIFT2(a, d) ;

	d->value = (a->value >> (a->shift - min_shift)) << (d->shift - min_shift) ;

	return d ;
}

// FP_NEG:		-a -> d
FixedPoint *FP_NEG(FixedPoint *a, FixedPoint *d) {

	d->value = -a->value ;

	return d ;
}

// FP_MAT3x3_INIT:	Initialize the number of bits for binary fractions of all entries in matrix 'm' to 'shift'
MAT3x3_FP *FP_MAT3x3_INIT(MAT3x3_FP *m, char shift) {

	int i, j ;

	for(j = 0; j < 3; j++) {
		for(i = 0; i < 3; i++) {
			FP_INIT(&m->v[j][i], shift) ;
		}
	}

	return m ;
}

// FP_MAT3x3_ZERO:	Set all entries in matirx 'm' to zero
MAT3x3_FP *FP_MAT3x3_ZERO(MAT3x3_FP *m) {

	int i, j ;

	for(j = 0; j < 3; j++) {
		for(i = 0; i < 3; i++) {
			FP_SET(0, &m->v[j][i]) ;
		}
	}

	return m ;
}

// FP_MAT3x3_IDENTITY:	Set m to identity matrix
MAT3x3_FP *FP_MAT3x3_IDENTITY(MAT3x3_FP *m) {

	FP_MAT3x3_ZERO(m) ;
	FP_SET(1, &m->v[0][0]) ;
	FP_SET(1, &m->v[1][1]) ;
	FP_SET(1, &m->v[2][2]) ;

	return m ;
}

// FP_MAT3x3_ADD:	a (3 x 3) + b (3 x 3) -> d (3 x 3)
MAT3x3_FP *FP_MAT3x3_ADD(MAT3x3_FP *a, MAT3x3_FP *b, MAT3x3_FP *d) {

	int i, j ;

	for(j = 0; j < 3; j++) {
		for(i = 0; i < 3; i++) {
			FP_ADD(&a->v[j][i], &b->v[j][i], &d->v[j][i]) ;
		}
	}

	return d ;
}

// FP_MAT3x3_SUB:	a (3 x 3) - b (3 x 3) -> d (3 x 3)
MAT3x3_FP *FP_MAT3x3_SUB(MAT3x3_FP *a, MAT3x3_FP *b, MAT3x3_FP *d) {

	int i, j ;

	for(j = 0; j < 3; j++) {
		for(i = 0; i < 3; i++) {
			FP_SUB(&a->v[j][i], &b->v[j][i], &d->v[j][i]) ;
		}
	}

	return d ;
}

// FP_MAT3x3_MUL_V:	a (3 x 3) * v (3 x 1) -> r (3 x 1)
void FP_MAT3x3_MUL_V(MAT3x3_FP *a, FixedPoint *v, FixedPoint *r) {

	int i, k ;
	FixedPoint temp ;

	FP_INIT(&temp, v[0].shift) ;

	for(i = 0; i < 3; i++) {
		FP_SET(0, &r[i]) ;
		for(k = 0; k < 3; k++) {
			FP_ADD(&r[i], FP_MUL(&a->v[i][k], &v[k], &temp), &r[i]) ;
		}
	}
}

// FP_MAT3x3_MUL:	a (3 x 3) * b (3 x 3) -> c (3 x 1)
void FP_MAT3x3_MUL(MAT3x3_FP *a, MAT3x3_FP *b, MAT3x3_FP *c) {

	int i, j, k ;
	FixedPoint temp ;

	FP_INIT(&temp, c->v[0][0].shift) ;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			FP_SET(0, &c->v[i][j]) ;
			for(k = 0; k < 3; k++) {
				FP_ADD(&c->v[i][j], FP_MUL(&a->v[i][k], &b->v[k][j], &temp), &c->v[i][j]) ;
			}
		}
	}
}

// FP_MAT3x3_INV:	inverse(a) -> b, return the determinant of a
FixedPoint FP_MAT3x3_INV(MAT3x3_FP *a, MAT3x3_FP *b) {

	FixedPoint det ;
	FixedPoint temp[6] ;
	int zeroThreshold ;

	FP_INIT(&det, a->v[0][0].shift) ;
	FP_INIT(&temp[0], a->v[0][0].shift) ;
	FP_INIT(&temp[1], a->v[0][0].shift) ;
	FP_INIT(&temp[2], a->v[0][0].shift) ;
	FP_INIT(&temp[3], a->v[0][0].shift) ;
	FP_INIT(&temp[4], a->v[0][0].shift) ;
	FP_INIT(&temp[5], a->v[0][0].shift) ;

	FP_MUL(FP_MUL(&a->v[0][0], &a->v[1][1], &temp[0]), &a->v[2][2], &temp[0]) ;
	FP_MUL(FP_MUL(&a->v[0][1], &a->v[1][2], &temp[1]), &a->v[2][0], &temp[1]) ;
	FP_MUL(FP_MUL(&a->v[0][2], &a->v[2][1], &temp[2]), &a->v[1][0], &temp[2]) ;
	FP_MUL(FP_MUL(&a->v[0][2], &a->v[1][1], &temp[3]), &a->v[2][0], &temp[3]) ;
	FP_MUL(FP_MUL(&a->v[1][2], &a->v[2][1], &temp[4]), &a->v[0][0], &temp[4]) ;
	FP_MUL(FP_MUL(&a->v[2][2], &a->v[1][0], &temp[5]), &a->v[0][1], &temp[5]) ;

	FP_SET(0, &det) ;

	FP_ADD(&det, &temp[0], &det) ;
	FP_ADD(&det, &temp[1], &det) ;
	FP_ADD(&det, &temp[2], &det) ;
	FP_SUB(&det, &temp[3], &det) ;
	FP_SUB(&det, &temp[4], &det) ;
	FP_SUB(&det, &temp[5], &det) ;

	if(det.shift <= 16) {
		zeroThreshold = 32 >> (16 - det.shift) ;
	} else {
		zeroThreshold = 32 << (det.shift - 16) ;
	}
	if(det.value >= -zeroThreshold && det.value <= zeroThreshold) {
		det.value = 0 ;

		return det ;
	}

	FP_MUL(&a->v[1][1], &a->v[2][2], &temp[0]) ;
	FP_MUL(&a->v[1][2], &a->v[2][1], &temp[1]) ;
	FP_SUB(&temp[0], &temp[1], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[0][0]) ;

	FP_MUL(&a->v[1][0], &a->v[2][2], &temp[0]) ;
	FP_MUL(&a->v[1][2], &a->v[2][0], &temp[1]) ;
	FP_SUB(&temp[1], &temp[0], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[1][0]) ;

	FP_MUL(&a->v[1][0], &a->v[2][1], &temp[0]) ;
	FP_MUL(&a->v[1][1], &a->v[2][0], &temp[1]) ;
	FP_SUB(&temp[0], &temp[1], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[2][0]) ;

	FP_MUL(&a->v[0][1], &a->v[2][2], &temp[0]) ;
	FP_MUL(&a->v[0][2], &a->v[2][1], &temp[1]) ;
	FP_SUB(&temp[1], &temp[0], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[0][1]) ;

	FP_MUL(&a->v[0][0], &a->v[2][2], &temp[0]) ;
	FP_MUL(&a->v[0][2], &a->v[2][0], &temp[1]) ;
	FP_SUB(&temp[0], &temp[1], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[1][1]) ;

	FP_MUL(&a->v[0][0], &a->v[2][1], &temp[0]) ;
	FP_MUL(&a->v[0][1], &a->v[2][0], &temp[1]) ;
	FP_SUB(&temp[1], &temp[0], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[2][1]) ;

	FP_MUL(&a->v[0][1], &a->v[1][2], &temp[0]) ;
	FP_MUL(&a->v[0][2], &a->v[1][1], &temp[1]) ;
	FP_SUB(&temp[0], &temp[1], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[0][2]) ;

	FP_MUL(&a->v[0][0], &a->v[1][2], &temp[0]) ;
	FP_MUL(&a->v[0][2], &a->v[1][0], &temp[1]) ;
	FP_SUB(&temp[1], &temp[0], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[1][2]) ;

	FP_MUL(&a->v[0][0], &a->v[1][1], &temp[0]) ;
	FP_MUL(&a->v[0][1], &a->v[1][0], &temp[1]) ;
	FP_SUB(&temp[0], &temp[1], &temp[2]) ;
	FP_DIV(&temp[2], &det, &b->v[2][2]) ;

	return det ;
}

#ifdef __cplusplus
}
#endif
