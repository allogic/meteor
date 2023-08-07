#ifndef MACROS_H
#define MACROS_H

#define UNUSED(X) ((void)X)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define ARRAY_LENGTH(X) (sizeof(X) / sizeof(X[0]))

#endif