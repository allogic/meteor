#ifndef VECTOR_H
#define VECTOR_H

#include <math/types.h>

#define VEC2_ZERO { 0.0F, 0.0F }
#define VEC3_ZERO { 0.0F, 0.0F, 0.0F }
#define VEC4_ZERO { 0.0F, 0.0F, 0.0F, 0.0F }

void Vector_Sub(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes);
void Vector_MulAdd(xVec4_t xVec, float fScalar, xVec4_t xRes);
void Vector_Scale(xVec3_t xVec, float fScale, xVec3_t xRes);

void Vector_Norm(xVec3_t xVec);

float Vector_Dot(xVec3_t xVecA, xVec3_t xVecB);
void Vector_Cross(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes);

#endif
