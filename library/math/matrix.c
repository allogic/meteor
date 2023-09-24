#include <math/vector.h>
#include <math/matrix.h>

void Matrix4_GetPosition(xMat4_t xMat, xVec3_t xVec) {
	xVec[0] = xMat[0][0];
	xVec[1] = xMat[1][0];
	xVec[2] = xMat[2][0];
}

void Matrix4_GetScale(xMat4_t xMat, xVec3_t xVec) {
	xVec[0] = xMat[0][0];
	xVec[1] = xMat[1][1];
	xVec[2] = xMat[2][2];
}

void Matrix4_SetPosition(xMat4_t xMat, xVec3_t xVec) {
	xMat[3][0] = xVec[0];
	xMat[3][1] = xVec[1];
	xMat[3][2] = xVec[2];
}

void Matrix4_SetScale(xMat4_t xMat, xVec3_t xVec) {
	xMat[0][0] = xVec[0];
	xMat[1][1] = xVec[1];
	xMat[2][2] = xVec[2];
}

void Matrix4_Zero(xMat4_t xMat) {
	xMat[0][0] = xMat[0][1] = xMat[0][2] = xMat[0][3] = 0.0F;
	xMat[1][0] = xMat[1][1] = xMat[1][2] = xMat[1][3] = 0.0F;
	xMat[2][0] = xMat[2][1] = xMat[2][2] = xMat[2][3] = 0.0F;
	xMat[3][0] = xMat[3][1] = xMat[3][2] = xMat[3][3] = 0.0F;
}

void Matrix4_Identity(xMat4_t xMat) {
	xMat[0][0] = 1.0F; xMat[0][1] = 0.0F; xMat[0][2] = 0.0F; xMat[0][3] = 0.0F;
	xMat[1][0] = 0.0F; xMat[1][1] = 1.0F; xMat[1][2] = 0.0F; xMat[1][3] = 0.0F;
	xMat[2][0] = 0.0F; xMat[2][1] = 0.0F; xMat[2][2] = 1.0F; xMat[2][3] = 0.0F;
	xMat[3][0] = 0.0F; xMat[3][1] = 0.0F; xMat[3][2] = 0.0F; xMat[3][3] = 1.0F;
}

void Matrix4_Copy(xMat4_t xMat, xMat4_t xRes) {
	xRes[0][0] = xMat[0][0]; xRes[1][0] = xMat[1][0];
	xRes[0][1] = xMat[0][1]; xRes[1][1] = xMat[1][1];
	xRes[0][2] = xMat[0][2]; xRes[1][2] = xMat[1][2];
	xRes[0][3] = xMat[0][3]; xRes[1][3] = xMat[1][3];

	xRes[2][0] = xMat[2][0]; xRes[3][0] = xMat[3][0];
	xRes[2][1] = xMat[2][1]; xRes[3][1] = xMat[3][1];
	xRes[2][2] = xMat[2][2]; xRes[3][2] = xMat[3][2];
	xRes[2][3] = xMat[2][3]; xRes[3][3] = xMat[3][3];
}

void Matrix4_Mul(xMat4_t xMatA, xMat4_t xMatB, xMat4_t xRes) {
	float fA00 = xMatA[0][0], fA01 = xMatA[0][1], fA02 = xMatA[0][2], fA03 = xMatA[0][3];
	float fA10 = xMatA[1][0], fA11 = xMatA[1][1], fA12 = xMatA[1][2], fA13 = xMatA[1][3];
	float fA20 = xMatA[2][0], fA21 = xMatA[2][1], fA22 = xMatA[2][2], fA23 = xMatA[2][3];
	float fA30 = xMatA[3][0], fA31 = xMatA[3][1], fA32 = xMatA[3][2], fA33 = xMatA[3][3];

	float fB00 = xMatB[0][0], fB01 = xMatB[0][1], fB02 = xMatB[0][2], fB03 = xMatB[0][3];
	float fB10 = xMatB[1][0], fB11 = xMatB[1][1], fB12 = xMatB[1][2], fB13 = xMatB[1][3];
	float fB20 = xMatB[2][0], fB21 = xMatB[2][1], fB22 = xMatB[2][2], fB23 = xMatB[2][3];
	float fB30 = xMatB[3][0], fB31 = xMatB[3][1], fB32 = xMatB[3][2], fB33 = xMatB[3][3];

	xRes[0][0] = fA00 * fB00 + fA10 * fB01 + fA20 * fB02 + fA30 * fB03;
	xRes[0][1] = fA01 * fB00 + fA11 * fB01 + fA21 * fB02 + fA31 * fB03;
	xRes[0][2] = fA02 * fB00 + fA12 * fB01 + fA22 * fB02 + fA32 * fB03;
	xRes[0][3] = fA03 * fB00 + fA13 * fB01 + fA23 * fB02 + fA33 * fB03;
	xRes[1][0] = fA00 * fB10 + fA10 * fB11 + fA20 * fB12 + fA30 * fB13;
	xRes[1][1] = fA01 * fB10 + fA11 * fB11 + fA21 * fB12 + fA31 * fB13;
	xRes[1][2] = fA02 * fB10 + fA12 * fB11 + fA22 * fB12 + fA32 * fB13;
	xRes[1][3] = fA03 * fB10 + fA13 * fB11 + fA23 * fB12 + fA33 * fB13;
	xRes[2][0] = fA00 * fB20 + fA10 * fB21 + fA20 * fB22 + fA30 * fB23;
	xRes[2][1] = fA01 * fB20 + fA11 * fB21 + fA21 * fB22 + fA31 * fB23;
	xRes[2][2] = fA02 * fB20 + fA12 * fB21 + fA22 * fB22 + fA32 * fB23;
	xRes[2][3] = fA03 * fB20 + fA13 * fB21 + fA23 * fB22 + fA33 * fB23;
	xRes[3][0] = fA00 * fB30 + fA10 * fB31 + fA20 * fB32 + fA30 * fB33;
	xRes[3][1] = fA01 * fB30 + fA11 * fB31 + fA21 * fB32 + fA31 * fB33;
	xRes[3][2] = fA02 * fB30 + fA12 * fB31 + fA22 * fB32 + fA32 * fB33;
	xRes[3][3] = fA03 * fB30 + fA13 * fB31 + fA23 * fB32 + fA33 * fB33;
}

void Matrix4_Translate(xMat4_t xMat, xVec3_t xVec) {
	Vector3_MulAdd(xMat[0], xVec[0], xMat[3]);
	Vector3_MulAdd(xMat[1], xVec[1], xMat[3]);
	Vector3_MulAdd(xMat[2], xVec[2], xMat[3]);
}
