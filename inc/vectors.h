#ifndef _VECTORS_H_
#define _VECTORS_H_
#pragma once

#include <math.h>

typedef float Flt;
typedef Flt Vec[3];

Flt dist (const Vec);
Flt dist (const Vec &, const Vec &);
void vecNormalize(Vec);
void vecSub(const Vec &, const Vec &, Vec);

#endif

