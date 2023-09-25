#include <math.h>

#include <math/vector.h>

void Vector2_Set(xVec2_t xVec, float fX, float fY) {
	xVec[0] = fX;
	xVec[1] = fY;
}
void Vector3_Set(xVec3_t xVec, float fX, float fY, float fZ) {
	xVec[0] = fX;
	xVec[1] = fY;
	xVec[2] = fZ;
}
void Vector4_Set(xVec4_t xVec, float fX, float fY, float fZ, float fW) {
	xVec[0] = fX;
	xVec[1] = fY;
	xVec[2] = fZ;
	xVec[3] = fW;
}

void Vector2_DivScalar(xVec2_t xVec, float fScalar, xVec2_t xRes) {
	xRes[0] = xVec[0] / fScalar;
	xRes[1] = xVec[1] / fScalar;
}
void Vector3_DivScalar(xVec3_t xVec, float fScalar, xVec3_t xRes) {
	xRes[0] = xVec[0] / fScalar;
	xRes[1] = xVec[1] / fScalar;
	xRes[2] = xVec[2] / fScalar;
}
void Vector4_DivScalar(xVec4_t xVec, float fScalar, xVec4_t xRes) {
	xRes[0] = xVec[0] / fScalar;
	xRes[1] = xVec[1] / fScalar;
	xRes[2] = xVec[2] / fScalar;
	xRes[3] = xVec[3] / fScalar;
}

void Vector3_Sub(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes) {
	xRes[0] = xVecA[0] - xVecB[0];
	xRes[1] = xVecA[1] - xVecB[1];
	xRes[2] = xVecA[2] - xVecB[2];
}

void Vector3_MulAdd(xVec4_t xVec, float fScalar, xVec4_t xRes) {
	xRes[0] += xVec[0] * fScalar;
	xRes[1] += xVec[1] * fScalar;
	xRes[2] += xVec[2] * fScalar;
	xRes[3] += xVec[3] * fScalar;
}

void Vector3_Scale(xVec3_t xVec, float fScale, xVec3_t xRes) {
	xRes[0] = xVec[0] * fScale;
	xRes[1] = xVec[1] * fScale;
	xRes[2] = xVec[2] * fScale;
}

void Vector3_Norm(xVec3_t xVec) {
	float fN = sqrtf(Vector3_Dot(xVec, xVec));

	if (fN == 0.0F) {
		xVec[0] = xVec[1] = xVec[2] = 0.0F;
	} else {
		Vector3_Scale(xVec, 1.0F / fN, xVec);
	}
}

float Vector3_Dot(xVec3_t xVecA, xVec3_t xVecB) {
	return xVecA[0] * xVecB[0] + xVecA[1] * xVecB[1] + xVecA[2] * xVecB[2];
}

void Vector3_Cross(xVec3_t xVecA, xVec3_t xVecB, xVec3_t xRes) {
	xRes[0] = xVecA[1] * xVecB[2] - xVecA[2] * xVecB[1];
	xRes[1] = xVecA[2] * xVecB[0] - xVecA[0] * xVecB[2];
	xRes[2] = xVecA[0] * xVecB[1] - xVecA[1] * xVecB[0];
}
