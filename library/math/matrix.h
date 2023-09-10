#ifndef METEOR_MATRIX_H
#define METEOR_MATRIX_H

#include <math/mathtypes.h>

#define MAT4_ZERO { { 0.0F, 0.0F, 0.0F, 0.0F }, \
					{ 0.0F, 0.0F, 0.0F, 0.0F }, \
					{ 0.0F, 0.0F, 0.0F, 0.0F }, \
					{ 0.0F, 0.0F, 0.0F, 0.0F } }

#define MAT4_IDENTITY { { 1.0F, 0.0F, 0.0F, 0.0F }, \
						{ 0.0F, 1.0F, 0.0F, 0.0F }, \
						{ 0.0F, 0.0F, 1.0F, 0.0F }, \
						{ 0.0F, 0.0F, 0.0F, 1.0F } }

void Matrix_GetPosition(xMat4_t xMat, xVec3_t xVec);

void Matrix_SetPosition(xMat4_t xMat, xVec3_t xVec);

void Matrix_Zero(xMat4_t xMat);
void Matrix_Copy(xMat4_t xMat, xMat4_t xRes);
void Matrix_Identity(xMat4_t xMat);

void Matrix_Mul(xMat4_t xMatA, xMat4_t xMatB, xMat4_t xRes);

void Matrix_Translate(xMat4_t xMat, xVec3_t xVec);

#endif
