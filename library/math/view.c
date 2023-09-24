#include <math/vector.h>
#include <math/view.h>

void View_LookAt(xVec3_t xEye, xVec3_t xCenter, xVec3_t xUp, xMat4_t xRes) {
	xVec3_t xF, xU, xS;

	Vector3_Sub(xCenter, xEye, xF);
	Vector3_Norm(xF);
	Vector3_Cross(xUp, xF, xS);
	Vector3_Norm(xS);
	Vector3_Cross(xF, xS, xU);

	xRes[0][0] = xS[0];
	xRes[0][1] = xU[0];
	xRes[0][2] = xF[0];
	xRes[1][0] = xS[1];
	xRes[1][1] = xU[1];
	xRes[1][2] = xF[1];
	xRes[2][0] = xS[2];
	xRes[2][1] = xU[2];
	xRes[2][2] = xF[2];
	xRes[3][0] = -Vector3_Dot(xS, xEye);
	xRes[3][1] = -Vector3_Dot(xU, xEye);
	xRes[3][2] = -Vector3_Dot(xF, xEye);
	xRes[0][3] = xRes[1][3] = xRes[2][3] = 0.0F;
	xRes[3][3] = 1.0F;
}
