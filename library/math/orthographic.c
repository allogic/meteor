#include <math/matrix.h>

#include <math/orthographic.h>

void Orthographic_Projection(float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ, xMat4_t xRes) {
  float fRl, fTb, fFn;

  Matrix4_Zero(xRes);

  fRl = 1.0F / (fRight - fLeft);
  fTb = 1.0F / (fTop - fBottom);
  fFn = -1.0F / (fFarZ - fNearZ);

  xRes[0][0] = 2.0F * fRl;
  xRes[1][1] = 2.0F * fTb;
  xRes[2][2] = -fFn;
  xRes[3][0] = -(fRight + fLeft) * fRl;
  xRes[3][1] = -(fTop + fBottom) * fTb;
  xRes[3][2] = fNearZ * fFn;
  xRes[3][3] = 1.0F;
}
