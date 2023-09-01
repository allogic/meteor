#ifndef METEOR_UNIFORM_H
#define METEOR_UNIFORM_H

#include <math/matrix.h>

typedef struct {
	xMat4_t xModel;
	xMat4_t xView;
	xMat4_t xProjection;
} xModelViewProjection_t;

#endif