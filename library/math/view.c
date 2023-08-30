#include <math/vector.h>
#include <math/view.h>

void View_LookAt(xVec3_t xEye, xVec3_t xCenter, xVec3_t xUp, xMat4_t xRes) {
	xVec3_t xF, xU, xS;

	Vector_Sub(xCenter, xEye, xF);
	Vector_Norm(xF);
	Vector_Cross(xUp, xF, xS);
	Vector_Norm(xS);
	Vector_Cross(xF, xS, xU);

	xRes[0][0] = xS[0];
	xRes[0][1] = xU[0];
	xRes[0][2] = xF[0];
	xRes[1][0] = xS[1];
	xRes[1][1] = xU[1];
	xRes[1][2] = xF[1];
	xRes[2][0] = xS[2];
	xRes[2][1] = xU[2];
	xRes[2][2] = xF[2];
	xRes[3][0] = -Vector_Dot(xS, xEye);
	xRes[3][1] = -Vector_Dot(xU, xEye);
	xRes[3][2] = -Vector_Dot(xF, xEye);
	xRes[0][3] = xRes[1][3] = xRes[2][3] = 0.0f;
	xRes[3][3] = 1.0f;
}
