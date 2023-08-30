#include <math.h>

#include <math/vector.h>

void Vector_Sub(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes) {
	xRes[0] = xVecA[0] - xVecB[0];
	xRes[1] = xVecA[1] - xVecB[1];
	xRes[2] = xVecA[2] - xVecB[2];
}

void Vector_MulAdd(xVec4_t xVec, float fScalar, xVec4_t xRes) {
	xRes[0] = xVec[0] * fScalar;
	xRes[1] = xVec[1] * fScalar;
	xRes[2] = xVec[2] * fScalar;
	xRes[3] = xVec[3] * fScalar;
}

void Vector_Scale(xVec3_t xVec, float fScale, xVec3_t xRes) {
	xRes[0] = xVec[0] * fScale;
	xRes[1] = xVec[1] * fScale;
	xRes[2] = xVec[2] * fScale;
}

void Vector_Norm(xVec3_t xVec) {
	float fN = sqrtf(Vector_Dot(xVec, xVec));

	if (fN == 0.0F) {
		xVec[0] = xVec[1] = xVec[2] = 0.0F;
	} else {
		Vector_Scale(xVec, 1.0F / fN, xVec);
	}
}

float Vector_Dot(xVec3_t xVecA, xVec3_t xVecB) {
	return xVecA[0] * xVecB[0] + xVecA[1] * xVecB[1] + xVecA[2] * xVecB[2];
}

void Vector_Cross(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes) {
	xRes[0] = xVecA[1] * xVecB[2] - xVecA[2] * xVecB[1];
	xRes[1] = xVecA[2] * xVecB[0] - xVecA[0] * xVecB[2];
	xRes[2] = xVecA[0] * xVecB[1] - xVecA[1] * xVecB[0];
}
