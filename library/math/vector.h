#ifndef METEOR_MATH_VECTOR_H
#define METEOR_MATH_VECTOR_H

#include <math/common.h>

#define VEC2_ZERO { 0.0F, 0.0F }
#define VEC3_ZERO { 0.0F, 0.0F, 0.0F }
#define VEC4_ZERO { 0.0F, 0.0F, 0.0F, 0.0F }

void Vector2_Set(xVec2_t xVec, float fX, float fY);
void Vector3_Set(xVec3_t xVec, float fX, float fY, float fZ);
void Vector4_Set(xVec4_t xVec, float fX, float fY, float fZ, float fW);

void Vector2_DivScalar(xVec2_t xVec, float fScalar, xVec2_t xRes);
void Vector3_DivScalar(xVec3_t xVec, float fScalar, xVec3_t xRes);
void Vector4_DivScalar(xVec4_t xVec, float fScalar, xVec4_t xRes);

void Vector3_Sub(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes);
void Vector3_MulAdd(xVec4_t xVec, float fScalar, xVec4_t xRes);
void Vector3_Scale(xVec3_t xVec, float fScale, xVec3_t xRes);

void Vector3_Norm(xVec3_t xVec);

float Vector3_Dot(xVec3_t xVecA, xVec3_t xVecB);
void Vector3_Cross(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes);

#endif
