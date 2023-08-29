#ifndef MATRIX_H
#define MATRIX_H

#define MAT4_IDENTITY { { 1.0F, 0.0F, 0.0F, 0.0F }, \
						{ 0.0F, 1.0F, 0.0F, 0.0F }, \
						{ 0.0F, 0.0F, 1.0F, 0.0F }, \
						{ 0.0F, 0.0F, 0.0F, 1.0F } }

typedef float xMat4_t[4][4];

void Matrix_Mul(xMat4_t xMat1, xMat4_t xMat2, xMat4_t xMatProduct);

#endif
