#ifndef METEOR_MATH_MATRIX_H
#define METEOR_MATH_MATRIX_H

#include <math/common.h>

#define MAT4_ZERO { { 0.0F, 0.0F, 0.0F, 0.0F }, \
					{ 0.0F, 0.0F, 0.0F, 0.0F }, \
					{ 0.0F, 0.0F, 0.0F, 0.0F }, \
					{ 0.0F, 0.0F, 0.0F, 0.0F } }

#define MAT4_IDENTITY { { 1.0F, 0.0F, 0.0F, 0.0F }, \
						{ 0.0F, 1.0F, 0.0F, 0.0F }, \
						{ 0.0F, 0.0F, 1.0F, 0.0F }, \
						{ 0.0F, 0.0F, 0.0F, 1.0F } }

void Matrix4_GetPosition(xMat4_t xMat, xVec3_t xVec);
void Matrix4_GetScale(xMat4_t xMat, xVec3_t xVec);

void Matrix4_SetPosition(xMat4_t xMat, xVec3_t xVec);
void Matrix4_SetScale(xMat4_t xMat, xVec3_t xVec);

void Matrix4_Zero(xMat4_t xMat);
void Matrix4_Copy(xMat4_t xMat, xMat4_t xRes);
void Matrix4_Identity(xMat4_t xMat);

void Matrix4_Mul(xMat4_t xMatA, xMat4_t xMatB, xMat4_t xRes);

void Matrix4_Translate(xMat4_t xMat, xVec3_t xVec);

#endif
